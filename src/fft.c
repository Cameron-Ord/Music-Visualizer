#include "audio.h"
#include "threads.h"
#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

void fft_func(float in[], size_t stride, float _Complex out[], size_t n) {
  // 0 1 2 3 4 5 6
  // n should be greater than 0
  assert(n > 0);
  if (n == 1) {
    out[0] = in[0];
    return;
  }

  fft_func(in, stride * 2, out, n / 2);
  fft_func(in + stride, stride * 2, out + n / 2, n / 2);
  // v = o*x
  // out = e - o*x e + o*x e e| e + o*x o - o*x o o

  for (size_t k = 0; k < n / 2; ++k) {
    float t          = (float)k / n;
    float _Complex v = cexpf(-2 * I * M_PI * t) * out[k + n / 2];
    float _Complex e = out[k];
    out[k]           = e + v;
    out[k + n / 2]   = e - v;
  }
} /*fft_func*/

void fft_push(FourierTransform* FT, SongState* SS, int channels, int bytes) {
  if (channels == 2) {
    memcpy(FT->fft_buffers->fft_in, SS->audio_data->buffer + SS->audio_data->audio_pos, bytes);
  }
} /*fft_push*/

void generate_visual(FourierTransform* FT, ThreadWrapper* TW) {
  float*          out_log         = FT->fft_buffers->out_log;
  float*          smoothed        = FT->fft_buffers->smoothed;
  float*          combined_window = FT->fft_buffers->combined_window;
  float _Complex* out_raw         = FT->fft_buffers->out_raw;

  create_hann_window(FT, TW);
  fft_func(combined_window, 1, out_raw, N);
  apply_amp(N / 2, FT, TW);
} /*generate_visual*/

void create_hann_window(FourierTransform* FT, ThreadWrapper* TW) {
  f32*        fft_in = FT->fft_buffers->fft_in;
  int         cores  = TW->cores;
  HannThread* hann   = TW->hann;
  int         chunk  = N / cores;

  for (int i = 0; i < cores; i++) {
    hann[i].start = (i * chunk);
    hann[i].end   = (i == cores - 1) ? N : (i + 1) * chunk;
    memset(hann[i].win, 0, sizeof(f32) * N);
    memcpy(hann[i].tmp, fft_in, sizeof(f32) * DOUBLE_N);
    resume_thread(&TW->hann[i].cond, &TW->hann[i].mutex, &TW->hann[i].paused);
  }

  f32* combined_window = FT->fft_buffers->combined_window;
  int  m               = 0;
  int  m_increm        = 0;

  for (int i = 0; i < cores; i++) {
    while (TRUE) {
      if (hann[i].cycle_complete) {
        break;
      }
    }
    m = hann[i].end;
    memcpy(combined_window + m_increm, hann[i].win, sizeof(f32) * m);
    m_increm += m;

    hann[i].cycle_complete = FALSE;
  }
}

void low_pass(float* input, int size, float cutoff, int SR) {
  /*Just a simple attenuation, don't feel like complicating this*/
  float nyquist    = (float)SR / 2.0f;
  int   cutoff_bin = (int)((cutoff / nyquist) * size);
  for (int i = 0; i < cutoff_bin; ++i) {
    input[i] *= 0.75;
  }
} /*low_pass*/

void apply_amp(int size, FourierTransform* FT, ThreadWrapper* TW) {

  FTransformBuffers* ftbuf  = FT->fft_buffers;
  FTransformData*    ftdata = FT->fft_data;
  int                cores  = TW->cores;
  LogThread*         log    = TW->log;
  int                chunk  = size / cores;

  for (int i = 0; i < cores; i++) {
    log[i].start = (i == 0) ? (i * chunk) + 1.0f : (i * chunk);
    log[i].end   = (i == cores - 1) ? size : (i + 1) * chunk;
    log[i].m     = 0;
    memset(log[i].tmp_proc, 0, sizeof(f32) * (N / 2));
    memcpy(log[i].tmp, ftbuf->out_raw, sizeof(f32c) * N);
    resume_thread(&TW->log[i].cond, &TW->log[i].mutex, &TW->log[i].paused);
  }

  f32*   processed = ftbuf->processed;
  f32*   smoothed  = ftbuf->smoothed;
  size_t m         = 0;
  int    m_increm  = 0;
  f32    max_ampl  = 1.0f;

  for (int i = 0; i < cores; i++) {
    // Wait until the worker is finished
    while (TRUE) {
      if (log[i].cycle_complete) {
        break;
      }
    }

    m = log[i].m;
    memcpy(processed + m_increm, log[i].tmp_proc, sizeof(f32) * m);
    m_increm += m;

    if (log[i].max_ampl > max_ampl)
      max_ampl = log[i].max_ampl;

    log[i].cycle_complete = FALSE;
  }

  m = m_increm;

  for (size_t i = 0; i < m; ++i) {
    processed[i] /= max_ampl;
    smoothed[i] = smoothed[i] + (processed[i] - smoothed[i]) * 7 * (1.0 / FPS);
  }

  FT->fft_data->output_len = m;

} /*apply_amp*/

float amp(float _Complex z) {
  float a = fabsf(crealf(z));
  float b = fabsf(cimagf(z));
  return logf(a * a + b * b);
} /*amp*/
