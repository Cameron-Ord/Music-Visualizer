#include "audio.h"
#include "macro.h"
#include <assert.h>
#include <complex.h>
#include <math.h>

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

void generate_visual(FourierTransform* FT, ThreadWrapper* TW, int SR) {
  float*          smoothed        = FT->fft_buffers->smoothed;
  float*          combined_window = FT->fft_buffers->combined_window;
  float _Complex* out_raw         = FT->fft_buffers->out_raw;

  create_hann_window(FT, TW);
  fft_func(combined_window, 1, out_raw, N);
  squash_to_log(N / 2, FT, TW);
} /*generate_visual*/

void create_hann_window(FourierTransform* FT, ThreadWrapper* TW) {
  float* combined_window = FT->fft_buffers->combined_window;

  f32* fft_in = FT->fft_buffers->fft_in;

  for (int i = 0; i < N; ++i) {
    f32 sum            = fft_in[i * 2] + fft_in[i * 2 + 1];
    combined_window[i] = sum;
    // hann window to reduce spectral leakage before passing it to FFT
    float Nf   = (float)N;
    float t    = (float)i / (Nf - 1);
    float hann = 0.5 - 0.5 * cosf(2 * M_PI * t);

    combined_window[i] *= hann;
  }
}

void low_pass(float* input, int size, float cutoff, int SR) {
  /*Just a simple attenuation, don't feel like complicating this*/
  float nyquist    = (float)SR / 2.0f;
  int   cutoff_bin = (int)((cutoff / nyquist) * size);
  for (int i = 0; i < cutoff_bin; ++i) {
    input[i] *= 0.5;
  }
} /*low_pass*/

void squash_to_log(int size, FourierTransform* FT, ThreadWrapper* TW) {

  FTransformBuffers* ftbuf  = FT->fft_buffers;
  FTransformData*    ftdata = FT->fft_data;

  float  step     = 1.06f;
  float  lowf     = 1.0f;
  size_t m        = 0;
  float  max_ampl = 1.0f;

  for (float f = lowf; (size_t)f < size; f = ceilf(f * step)) {
    float fs = ceilf(f * step);
    float a  = 0.0f;
    for (size_t q = (size_t)f; q < size && q < (size_t)fs; ++q) {
      float b = amp(ftbuf->out_raw[q]);
      if (b > a)
        a = b;
    }
    if (max_ampl < a) {
      max_ampl = a;
    }
    ftbuf->processed[m++] = a;
  }

  f32* processed = ftbuf->processed;
  f32* smoothed  = ftbuf->smoothed;

  for (size_t i = 0; i < m; ++i) {
    processed[i] /= max_ampl;
    smoothed[i] = smoothed[i] + (processed[i] - smoothed[i]) * 8 * (1.0 / FPS);
  }

  FT->fft_data->output_len = m;

} /*apply_amp*/

float amp(float _Complex z) {
  float a = fabsf(crealf(z));
  float b = fabsf(cimagf(z));
  return logf(a * a + b * b);
} /*amp*/
