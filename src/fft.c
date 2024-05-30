#include "macro.h"
#include "music_visualizer.h"
#include <assert.h>
#include <complex.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
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

void generate_visual(FourierTransform* FT, SongState* SS) {
  float*          out_log         = FT->fft_buffers->out_log;
  float*          smoothed        = FT->fft_buffers->smoothed;
  float*          combined_window = FT->fft_buffers->combined_window;
  float _Complex* out_raw         = FT->fft_buffers->out_raw;

  create_hann_window(FT);
  fft_func(combined_window, 1, out_raw, N);
  apply_amp(N / 2, FT);
} /*generate_visual*/

void create_hann_window(FourierTransform* FT) {
  f32* fft_in = FT->fft_buffers->fft_in;

  pthread_t  threads[FT->cpu_cores];
  HannThread hann_thread[FT->cpu_cores];
  int        chunk = N / FT->cpu_cores;

  for (int t = 0; t < FT->cpu_cores; ++t) {
    hann_thread[t].FT    = FT;
    hann_thread[t].start = t * chunk;

    memcpy(hann_thread[t].tmp, fft_in, sizeof(f32) * DOUBLE_N);

    int end = (t == FT->cpu_cores - 1) ? N : (t + 1) * chunk;

    hann_thread[t].end = end;

    pthread_create(&threads[t], NULL, hann_window_worker, &hann_thread[t]);
  }

  for (int t = 0; t < FT->cpu_cores; ++t) {
    pthread_join(threads[t], NULL);
  }
}

void* hann_window_worker(void* arg) {
  HannThread* _FT = (HannThread*)arg;

  float* combined_window = _FT->FT->fft_buffers->combined_window;
  float* fft_in          = _FT->FT->fft_buffers->fft_in;

  f32 Nf     = (float)N;
  f32 TWO_PI = 2.0f * M_PI;

  for (int i = _FT->start; i < _FT->end; ++i) {
    // hann window to reduce spectral leakage before passing it to FFT
    // Summing left and right channels
    f32 sum            = _FT->tmp[i * 2] + _FT->tmp[i * 2 + 1];
    combined_window[i] = sum / 2;

    float t    = (float)i / (Nf - 1);
    float hann = 0.5 - 0.5 * cosf(TWO_PI * t);

    combined_window[i] *= hann;
  }

  return NULL;
}

void low_pass(float* input, int size, float cutoff, int SR) {
  /*Just a simple attenuation, don't feel like complicating this*/
  float nyquist    = (float)SR / 2.0f;
  int   cutoff_bin = (int)((cutoff / nyquist) * size);
  for (int i = 0; i < cutoff_bin; ++i) {
    input[i] *= 0.75;
  }
} /*low_pass*/

void apply_amp(int size, FourierTransform* FT) {

  pthread_t threads[FT->cpu_cores];
  LogThread log_thread[FT->cpu_cores];
  int       chunk = size / FT->cpu_cores;

  for (int t = 0; t < FT->cpu_cores; ++t) {
    log_thread[t].FT       = FT;
    f32 start              = (t == 0) ? (t * chunk) + 1.0f : (t * chunk);
    log_thread[t].start    = start;
    log_thread[t].m        = 0;
    log_thread[t].max_ampl = 1.0f;
    int end                = (t == FT->cpu_cores - 1) ? size : (t + 1) * chunk;
    log_thread[t].end      = end;
    pthread_create(&threads[t], NULL, log_worker, &log_thread[t]);
  }

  FTransformBuffers* ftbuf  = FT->fft_buffers;
  FTransformData*    ftdata = FT->fft_data;

  f32* smoothed = ftbuf->smoothed;
  f32* out_log  = ftbuf->out_log;

  size_t tmp_m    = 0;
  int    increm   = 0;
  f32    max_ampl = 1.0f;

  for (int t = 0; t < FT->cpu_cores; ++t) {
    pthread_join(threads[t], NULL);
    tmp_m = log_thread[t].m;
    memcpy(out_log + increm, log_thread[t].tmp, sizeof(float) * tmp_m);
    increm += tmp_m;
    if (log_thread[t].max_ampl > max_ampl) {
      max_ampl = log_thread[t].max_ampl;
    }
  }

  size_t m = (size_t)increm;

  for (size_t i = 0; i < m; ++i) {
    out_log[i] /= max_ampl;
    smoothed[i] = smoothed[i] + (out_log[i] - smoothed[i]) * 8 * (1.0 / FPS);
  }

  ftdata->output_len = m;

} /*apply_amp*/

void* log_worker(void* arg) {
  LogThread* _FT = (LogThread*)arg;

  float step = 1.06f;

  FTransformBuffers* ftbuf  = _FT->FT->fft_buffers;
  FTransformData*    ftdata = _FT->FT->fft_data;

  memset(_FT->tmp, 0, sizeof(f32) * N);

  for (float f = _FT->start; (size_t)f < _FT->end; f = ceilf(f * step)) {
    float fs = ceilf(f * step);
    float a  = 0.0f;
    for (size_t q = (size_t)f; q < _FT->end && q < (size_t)fs; ++q) {
      float b = 0.0f;

      b = amp(ftbuf->out_raw[q]);

      if (b > a)
        a = b;
    }
    if (_FT->max_ampl < a) {
      _FT->max_ampl = a;
    }
    _FT->tmp[_FT->m++] = a;
  }

  return NULL;
}

float amp(float _Complex z) {
  float a = fabsf(crealf(z));
  float b = fabsf(cimagf(z));
  return logf(a * a + b * b);
} /*amp*/
