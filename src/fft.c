#include "../inc/audio.h"
#include "../inc/macro.h"
#include <complex.h>
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

void
fft_func(f32* in, size_t stride, f32c* out, size_t n) {

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
    f32* in_buf    = FT->fft_buffers->in_ptr;
    f32* aud_buf   = SS->audio_data->buffer;

    memcpy(in_buf, aud_buf + audio_pos, bytes);
  }
} /*fft_push*/

void
generate_visual(FTransformData* data, FTransformBuffers* bufs, int SR) {
  const size_t half_size = (size_t)BUFF_SIZE / 2;

  memcpy(bufs->in_cpy_prim, bufs->in_ptr, sizeof(f32) * DOUBLE_BUFF);

  swap_buffers(bufs->in_ptr, bufs->fft_in_prim, bufs->fft_in_sec);

  hamming_window(bufs->in_cpy_prim);

  fft_func(bufs->in_cpy_prim, 1, bufs->out_raw_prim, (size_t)BUFF_SIZE);

  squash_to_log(half_size, bufs->out_raw_prim, bufs->processed_prim, &data->max_ampl, &data->output_len, SR);

  apply_smoothing(data->output_len, data->max_ampl, bufs->processed_prim, bufs->smoothed_prim);

} /*generate_visual*/

/*This function is used when there is no multithreading. Currently it is not set up to fallback to this if
 * there is an issue with the multithreading, so I have to implement that*/
void
hamming_window(f32* in_cpy) {
  /*Iterate for the size of a single channel*/
  for (int i = 0; i < BUFF_SIZE; ++i) {
    f32 left  = in_cpy[i * 2];
    f32 right = in_cpy[i * 2 + 1];

    float Nf = (float)BUFF_SIZE;
    float t  = (float)i / (Nf - 1);
    /*Calculate the hamming window*/
    float hamm = 0.54 - 0.46 * cosf(2 * M_PI * t);

    left *= hamm;
    right *= hamm;

    in_cpy[i] = (left + right) / 2;
  }
}

void
squash_to_log(size_t size, f32c* raw, f32* proc, f32* max_ampl, size_t* len, int SR) {

  f32 delta_f = (f32)SR / size;
  // f32 bin_low  = (f32)(1000.0f / delta_f);
  // f32 bin_high = (f32)(5000.0f / delta_f);

  float  step = 1.06f;
  float  lowf = 1.0f;
  size_t m    = 0;
  *max_ampl   = 1.0f;

  for (float f = lowf; (size_t)f < size; f = ceilf(f * step)) {
    float fs = ceilf(f * step);
    float a  = 0.0f;

    for (size_t q = (size_t)f; q < size && q < (size_t)fs; ++q) {
      float b = amp(raw[q]);
      if (b > a) {
        a = b;
      }
    }

    if (*max_ampl < a) {
      *max_ampl = a;
    }

    proc[m++] = a;
  }

  *len = m;
}

float
amp(f32c z) {
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
    input[i] *= 0.5;
  }
} /*low_pass*/

void
apply_smoothing(size_t len, f32 max_ampl, f32* processed, f32* smoothed) {
  /*Linear smoothing*/

  for (size_t i = 0; i < len; ++i) {
    processed[i] /= max_ampl;
    smoothed[i] = smoothed[i] + (processed[i] - smoothed[i]) * 7 * (1.0 / FPS);
  }
}
