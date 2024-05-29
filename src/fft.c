#include "macro.h"
#include "music_visualizer.h"
#include <assert.h>
#include <complex.h>
#include <math.h>
#include <string.h>

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
  // Two channels when combined  is 8192 but seperate them and we get - 4096 : 4096
  if (channels == 2) {
    int samples = bytes / (channels * sizeof(float));
    // memcpying the left side of the buffer into a seperate left buffer. This works because we are
    // only copying 4096 * 4
    memcpy(FT->fft_buffers->in_left, SS->audio_data->buffer + SS->audio_data->audio_pos,
           samples * sizeof(float));
    // same idea, but we advance the audio position by the amount of samples, effectively nabbing
    // the 2nd half
    memcpy(FT->fft_buffers->in_right, SS->audio_data->buffer + SS->audio_data->audio_pos + samples,
           samples * sizeof(float));
  }
} /*fft_push*/

void generate_visual(FourierTransform* FT, SongState* SS) {
  int             DS              = FT->fft_data->DS_AMOUNT;
  float*          out_log         = FT->fft_buffers->out_log;
  float*          smoothed        = FT->fft_buffers->smoothed;
  float*          processed       = FT->fft_buffers->processed;
  float*          in_right        = FT->fft_buffers->in_right;
  float*          in_left         = FT->fft_buffers->in_left;
  float*          combined_window = FT->fft_buffers->combined_window;
  float _Complex* out_raw         = FT->fft_buffers->out_raw;

  create_hann_window(DS, FT);
  fft_func(combined_window, 1, out_raw, N);
  get_log(out_raw, out_log, N);
  low_pass(out_log, N, 5000.0f, SS->audio_data->sr);
  FT->fft_data->output_len = apply_amp(HALF_BUFF, processed, out_log, smoothed);
} /*generate_visual*/

void create_hann_window(int DS, FourierTransform* FT) {
  float* in_right        = FT->fft_buffers->in_right;
  float* in_left         = FT->fft_buffers->in_left;
  float* combined_window = FT->fft_buffers->combined_window;

  for (int i = 0; i < N; ++i) {
    f32 sum = in_left[i] + in_right[i];
    combined_window[i] = sum / 2;
    // hann window to reduce spectral leakage before passing it to FFT
    float Nf = (float)N;
    float t    = (float)i / (Nf - 1);
    float hann = 0.5 - 0.5 * cosf(2 * M_PI * t);

    combined_window[i] *= hann;
  }
}

void get_log(float _Complex* out_raw, float* out_log, int len) {
  for (int q = 0; q < len; ++q) {
    if (crealf(out_raw[q]) != 0.0 || cimagf(out_raw[q]) != 0.0) {
      out_log[q] = amp(out_raw[q]);
    }
  }
} /*get_log*/

void low_pass(float* input, int size, float cutoff, int SR) {
  /*Just a simple attenuation, don't feel like complicating this*/
  float nyquist    = (float)SR / 2.0f;
  int   cutoff_bin = (int)((cutoff / nyquist) * size);
  for (int i = 0; i < cutoff_bin; ++i) {
    input[i] *= 0.75;
  }
} /*low_pass*/

int apply_amp(int size, float* out_processed, float* out, float* out_smoothed) {
  float  step     = 1.06f;
  float  lowf     = 1.0f;
  size_t m        = 0;
  float  max_ampl = 1.0f;

  for (float f = lowf; (size_t)f < size; f = ceilf(f * step)) {
    float fs = ceilf(f * step);
    float a  = 0.0f;
    for (size_t q = (size_t)f; q < size && q < (size_t)fs; ++q) {
      float b = out[q];
      if (b > a)
        a = b;
    }
    if (max_ampl < a){
      max_ampl = a;
    }
    out_processed[m++] = a;
  }

  for (size_t i = 0; i < m; ++i) {
    out_processed[i] /= max_ampl;
    out_smoothed[i] = out_smoothed[i] + (out_processed[i] - out_smoothed[i]) * 7 * (1.0 / FPS);
  }

  return (int)m;
} /*apply_amp*/

float amp(float _Complex z) {
  float a = fabsf(crealf(z));
  float b = fabsf(cimagf(z));
  return logf(a * a + b * b);
} /*amp*/
