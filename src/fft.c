#include "../inc/audio.h"
#include "../inc/macro.h"
#include "../inc/threads.h"
#include <complex.h>
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

void
fft_func(f32* in, size_t stride, f32c* out, size_t n) {
  // assert(n > 0);
  if (n == 1) {
    out[0] = in[0];
    return;
  }

  size_t half_n = n / 2;

  fft_func(in, stride * 2, out, half_n);
  fft_func(in + stride, stride * 2, out + half_n, half_n);
  // v = o*x
  // out = e - o*x e + o*x e e| e + o*x o - o*x o o
  for (size_t k = 0; k < n / 2; ++k) {
    float t         = (float)k / n;
    f32c  v         = cexpf(-2 * I * M_PI * t) * out[k + half_n];
    f32c  e         = out[k];
    out[k]          = e + v;
    out[k + half_n] = e - v;
  }

} /*fft_func*/

void
fft_push(FourierTransform* FT, SongState* SS, int channels, int bytes) {
  if (channels == 2) {
    u32  audio_pos = SS->audio_data->audio_pos;
    f32* in_buf    = FT->fft_buffers->fft_in;
    f32* aud_buf   = SS->audio_data->buffer;

    memcpy(in_buf, aud_buf + audio_pos, bytes);
  }
} /*fft_push*/

void
generate_visual(FourierTransform* FT) {
  f32*  in_cpy  = FT->fft_buffers->in_cpy;
  f32c* out_raw = FT->fft_buffers->out_raw;

  calc_hann_window_threads(FT);
  // create_hann_window(FT);
  fft_func(in_cpy, 1, out_raw, (size_t)DOUBLE_BUFF);
  squash_to_log((size_t)(DOUBLE_BUFF / 2), FT);
  apply_smoothing(FT);

} /*generate_visual*/

/*This function is used when there is no multithreading. Currently it is not set up to fallback to this if
 * there is an issue with the multithreading, so I have to implement that*/
void
create_hann_window(FourierTransform* FT) {
  f32* in_cpy = FT->fft_buffers->in_cpy;
  f32* in     = FT->fft_buffers->fft_in;

  /*Iterate for the size of a single channel*/
  for (int i = 0; i < DOUBLE_BUFF; ++i) {
    float Nf = (float)DOUBLE_BUFF;
    float t  = (float)i / (Nf - 1);
    /*Calculate the hann window*/
    float hann = 0.5 - 0.5 * cosf(2 * M_PI * t);

    in[i] *= hann;

    /*Sum the channels and divide by 2*/
    in_cpy[i] = in[i];
  }
}

void
squash_to_log(size_t size, FourierTransform* FT) {

  f32c* out_raw   = FT->fft_buffers->out_raw;
  f32*  processed = FT->fft_buffers->processed;

  float  step     = 1.06f;
  float  lowf     = 1.0f;
  size_t m        = 0;
  float  max_ampl = 1.0f;

  for (float f = lowf; (size_t)f < size; f = ceilf(f * step)) {
    float fs = ceilf(f * step);
    float a  = 0.0f;
    for (size_t q = (size_t)f; q < size && q < (size_t)fs; ++q) {
      float b = amp(out_raw[q]);
      if (b > a) {
        a = b;
      }
    }
    if (max_ampl < a) {
      max_ampl = a;
    }
    processed[m++] = a;
  }

  FT->fft_data->max_ampl   = max_ampl;
  FT->fft_data->output_len = m;
}

float
amp(float _Complex z) {
  float a = fabsf(crealf(z));
  float b = fabsf(cimagf(z));
  return logf(a * a + b * b);
} /*amp*/

void
low_pass(float* input, int size, float cutoff, int SR) {
  /*Just a simple attenuation, don't feel like complicating this. Not currently in use*/
  float nyquist    = (float)SR / 2.0f;
  int   cutoff_bin = (int)((cutoff / nyquist) * size);
  for (int i = 0; i < cutoff_bin; ++i) {
    input[i] *= 0.75;
  }
} /*low_pass*/

void
apply_smoothing(FourierTransform* FT) {
  FTransformBuffers* ftbuf  = FT->fft_buffers;
  FTransformData*    ftdata = FT->fft_data;

  f32* processed = ftbuf->processed;
  f32* smoothed  = ftbuf->smoothed;

  /*Linear smoothing*/
  for (size_t i = 0; i < ftdata->output_len; ++i) {
    processed[i] /= ftdata->max_ampl;
    smoothed[i] = smoothed[i] + (processed[i] - smoothed[i]) * 6 * (1.0 / FPS);
  }
}
