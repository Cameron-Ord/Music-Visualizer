#include "../inc/audio.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FFTData data = {0};
FFTBuffers bufs = {0};
float *in_buffer = NULL;

const FFTData *get_data(void) { return &data; }
const FFTBuffers *get_bufs(void) { return &bufs; }

static void iter_fft(float *in, float *coeffs, Compf *out, size_t size);
static void squash_to_log(void);
static void linear(const int *smear_c, const int *smooth_c, const int *tframes);

static inline Compf c_from_real(const float real) {
  Compf _complex;
  _complex.real = real;
  _complex.imag = 0.0;
  return _complex;
}

static inline Compf c_from_imag(const float imag) {
  Compf _complex;
  _complex.real = 0.0;
  _complex.imag = imag;
  return _complex;
}

static inline Compf compf_expf(const Compf *c) {
  Compf res;
  float exp_real = expf(c->real);
  res.real = exp_real * cosf(c->imag);
  res.imag = exp_real * sinf(c->imag);
  return res;
}

static inline Compf compf_subtract(const Compf *a, const Compf *b) {
  Compf sub;
  sub.real = a->real - b->real;
  sub.imag = a->imag - b->imag;
  return sub;
}

static inline Compf compf_add(const Compf *a, const Compf *b) {
  Compf add;
  add.real = a->real + b->real;
  add.imag = a->imag + b->imag;
  return add;
}

static inline Compf compf_mult(const Compf *a, const Compf *b) {
  Compf mult;
  mult.real = a->real * b->real - a->imag * b->imag;
  mult.imag = a->real * b->imag + a->imag * b->real;
  return mult;
}

// Not used
//  static inline Compf compf_divide(const Compf *a, const Compf *b) {
//  Compf divide;
//  float denom = b->real * b->real + b->imag * b->imag;
//  divide.real = (a->real * b->real + a->imag * b->imag) / denom;
//  divide.imag = (a->imag * b->real - a->real * b->imag) / denom;
//  return divide;
// }

static inline Compf compf_step(const size_t *half_len, const Compf *iota) {
  Compf step;
  float theta = (float)M_PI / *half_len;

  step.real = iota->real * theta;
  step.imag = iota->imag * theta;

  step = compf_expf(&step);
  return step;
}

static int access_clamp(const int access) {
  if (access > 1) {
    return 1;
  }

  if (access < 0) {
    return 0;
  }

  return access;
}

static void fft_push(const uint32_t *pos, float *buffer, const size_t bytes) {
  if (buffer) {
    data.buffer_access = !data.buffer_access;
    in_buffer = bufs.fft_in[access_clamp(!data.buffer_access)];
    memcpy(in_buffer, buffer + *pos, bytes);
  }
}

static void do_fft(const int *smear_v, const int *smooth_v,
                   const int *tframes) {
  if (!in_buffer) {
    return;
  }

  float tmp[M_BUF_SIZE];
  memcpy(tmp, in_buffer, sizeof(float) * M_BUF_SIZE);
  iter_fft(tmp, data.hamming_values, bufs.out_raw, M_BUF_SIZE);
  squash_to_log();
  linear(smear_v, smooth_v, tframes);
}

void _do_fft(const int *smear_v, const int *smooth_v, const int *tframes) {
  do_fft(smear_v, smooth_v, tframes);
}

static void zero_fft(void) {
  memset(bufs.extracted, 0, sizeof(float) * M_BUF_SIZE);
  memset(bufs.fft_in[0], 0, sizeof(float) * M_BUF_SIZE);
  memset(bufs.fft_in[1], 0, sizeof(float) * M_BUF_SIZE);
  memset(bufs.out_raw, 0, sizeof(Compf) * M_BUF_SIZE);
  memset(bufs.processed_samples, 0, sizeof(float) * M_BUF_SIZE);
  memset(bufs.smear, 0, sizeof(float) * M_BUF_SIZE);
  memset(bufs.smoothed, 0, sizeof(float) * M_BUF_SIZE);

  data.max_ampl = 1.0;
  data.cell_width = 0;
  data.output_len = 0;
}

static size_t bit_reverse(size_t index, size_t log2n) {
  size_t reversed = 0;
  for (size_t i = 0; i < log2n; i++) {
    reversed <<= 1;
    reversed |= (index & 1);
    index >>= 1;
  }
  return reversed;
}

// https://www.geeksforgeeks.org/iterative-fast-fourier-transformation-polynomial-multiplication/
static float window(const float in, const float coeff) { return in * coeff; }
static float get_freq(const Compf *c) {
  return (c->real * c->real + c->imag * c->imag);
}

static float interpolate(float base, float interpolated, int coeff,
                         const int tframes) {
  return (base - interpolated) * coeff * (1.0 / tframes);
}

static float amp(float z) {
  if (z == 0.0f) {
    return 0.0f;
  }
  return logf(z);
}

void _fft_push(const uint32_t *pos, float *buffer, const size_t bytes) {
  fft_push(pos, buffer, bytes);
}

void _zero_fft(void) { zero_fft(); }

static void iter_fft(float *in, float *coeffs, Compf *out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    int rev_index = bit_reverse(i, log2(size));
    out[i] = c_from_real(window(in[rev_index], coeffs[rev_index]));
  }

  const Compf iota = c_from_imag(1.0f);
  for (int stage = 1; stage <= log2(size); ++stage) {
    size_t sub_arr_size = 1 << stage; // 2^stage
    size_t half_sub_arr = sub_arr_size >> 1;
    Compf twiddle = c_from_real(1.0f);

    Compf step = compf_step(&half_sub_arr, &iota);
    for (size_t j = 0; j < half_sub_arr; j++) {
      for (size_t k = j; k < size; k += sub_arr_size) {
        Compf t = compf_mult(&twiddle, &out[k + half_sub_arr]);
        Compf u = out[k];

        out[k] = compf_add(&u, &t);
        out[k + half_sub_arr] = compf_subtract(&u, &t);
      }
      twiddle = compf_mult(&twiddle, &step);
    }
  }
}

static void linear(const int *smear_v, const int *smooth_v,
                   const int *tframes) {
  for (size_t i = 0; i < data.output_len; ++i) {
    const float n = bufs.processed_samples[i] / data.max_ampl;
    // interpolated audio amplitudes
    bufs.smoothed[i] += interpolate(n, bufs.smoothed[i], *smooth_v, *tframes);
    // interpolated smear frames (of the audio amplitudes)
    bufs.smear[i] +=
        interpolate(bufs.smoothed[i], bufs.smear[i], *smear_v, *tframes);
  }
}

static void squash_to_log(void) {
  float step = 1.06f;
  float lowf = 1.0f;
  size_t m = 0;

  data.max_ampl = 1.0f;
  for (float f = lowf; (size_t)f < HALF_BUFF_SIZE; f = ceilf(f * step)) {
    float fs = ceilf(f * step);
    float a = 0.0f;

    for (size_t q = (size_t)f; q < HALF_BUFF_SIZE && q < (size_t)fs; ++q) {
      float b = amp(get_freq(&bufs.out_raw[q]));
      if (b > a) {
        a = b;
      }
    }

    if (data.max_ampl < a) {
      data.max_ampl = a;
    }

    bufs.processed_samples[m++] = a;
  }

  data.output_len = m;
}

void calculate_window(void) {
  for (int i = 0; i < M_BUF_SIZE; ++i) {
    float Nf = (float)M_BUF_SIZE;
    float t = (float)i / (Nf - 1);
    data.hamming_values[i] = 0.54 - 0.46 * cosf(2 * M_PI * t);
  }
}
