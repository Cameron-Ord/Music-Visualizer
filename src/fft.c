#include "audio.h"
#include "audiodefs.h"
#include "main.h"
size_t bit_reverse(size_t index, size_t log2n) {
  size_t reversed = 0;
  for (size_t i = 0; i < log2n; i++) {
    reversed <<= 1;
    reversed |= (index & 1);
    index >>= 1;
  }
  return reversed;
}

void zero_fft(FFTBuffers *bufs, FFTData *f_data) {
  memset(bufs->extracted, 0, sizeof(float) * M_BUF_SIZE);
  memset(bufs->fft_in, 0, sizeof(float) * M_BUF_SIZE);
  memset(bufs->out_raw, 0, sizeof(Float_Complex) * M_BUF_SIZE);
  memset(bufs->phases, 0, sizeof(float) * M_BUF_SIZE);
  memset(bufs->processed_phases, 0, sizeof(float) * M_BUF_SIZE);
  memset(bufs->processed_samples, 0, sizeof(float) * M_BUF_SIZE);
  memset(bufs->smear, 0, sizeof(float) * M_BUF_SIZE);
  memset(bufs->smoothed, 0, sizeof(float) * M_BUF_SIZE);
  memset(bufs->windowed, 0, sizeof(float) * M_BUF_SIZE);
  memset(bufs->sm_phases, 0, sizeof(float) * M_BUF_SIZE);

  f_data->max_ampl = 1.0;
  f_data->max_phase = 1.0;
  f_data->cell_width = 0;
  f_data->output_len = 0;
}

// https://www.geeksforgeeks.org/iterative-fast-fourier-transformation-polynomial-multiplication/
void iter_fft(float *in, Float_Complex *out, size_t size) {

  for (size_t i = 0; i < size; i++) {
    int rev_index = bit_reverse(i, log2(size));
    out[i] = cfromreal(in[rev_index]);
  }

  const Float_Complex iota = cfromimag(1.0f);
  for (int stage = 1; stage <= log2(size); ++stage) {
    size_t sub_arr_size = 1 << stage; // 2^stage
    size_t half_sub_arr = sub_arr_size >> 1;
    Float_Complex twiddle = cfromreal(1.0f);

    Float_Complex step = cexpf(iota * ((float)M_PI / half_sub_arr));
    for (size_t j = 0; j < half_sub_arr; j++) {
      for (size_t k = j; k < size; k += sub_arr_size) {
        Float_Complex t = mulcc(twiddle, out[k + half_sub_arr]);
        Float_Complex u = out[k];

        out[k] = addcc(u, t);
        out[k + half_sub_arr] = subcc(u, t);
      }
      twiddle = mulcc(twiddle, step);
    }
  }
}

// https://github.com/tsoding/musializer/blob/master/src/plug.c#L268
void recursive_fft(float *in, size_t stride, Float_Complex *out, size_t n) {
  if (n == 1) {
    out[0] = cfromreal(in[0]);
    return;
  }

  size_t half_n = n / 2;

  recursive_fft(in, stride * 2, out, half_n);
  recursive_fft(in + stride, stride * 2, out + half_n, half_n);
  // v = o*x
  // out = e - o*x e + o*x e e| e + o*x o - o*x o o
  for (size_t k = 0; k < n / 2; ++k) {
    float t = (float)k / n;
    Float_Complex v = mulcc(cexpf(cfromimag(-2 * M_PI * t)), out[k + n / 2]);
    Float_Complex e = out[k];
    out[k] = addcc(e, v);
    out[k + n / 2] = subcc(e, v);
  }

} /*fft_func*/

// Probably the most expensive thing here.
void extract_frequencies(FFTBuffers *bufs) {
  for (int i = 0; i < HALF_BUFF_SIZE; ++i) {
    const size_t left = i * 2;
    const size_t right = i * 2 + 1;

    float real = 0.0f;
    float imag = 0.0f;

    // Logf is applied later so we just square it instead of sqrt

    real = crealf(bufs->out_raw[left]);
    imag = cimagf(bufs->out_raw[left]);

    bufs->extracted[left] = (real * real + imag * imag);
    bufs->phases[left] = atan2f(imag, real);

    real = crealf(bufs->out_raw[right]);
    imag = cimagf(bufs->out_raw[right]);

    bufs->extracted[right] = (real * real + imag * imag);
    bufs->phases[right] = atan2f(imag, real);
  }
}

static float interpolate(float base, float interpolated, int coeff) {
  return (base - interpolated) * coeff * (1.0 / vis.target_frames);
}

void linear_mapping(FFTBuffers *bufs, FFTData *data) {
  float *ph = bufs->processed_phases;
  float *ps = bufs->processed_samples;
  float *sm_s = bufs->smoothed;
  float *sm_p = bufs->sm_phases;
  float *smr = bufs->smear;

  for (size_t i = 0; i < data->output_len; ++i) {
    ph[i] /= data->max_phase;
    ps[i] /= data->max_ampl;

    // interpolated phases
    sm_p[i] += interpolate(ph[i], sm_p[i], vis.smoothing);
    // interpolated audio amplitudes
    sm_s[i] += interpolate(ps[i], sm_s[i], vis.smoothing);
    // interpolated smear frames (of the audio amplitudes)
    smr[i] += interpolate(sm_s[i], smr[i], vis.smearing);
  }
}

float amp(float z) {
  if (z == 0.0f) {
    return 0.0f;
  }
  return logf(z);
}

// Bass, Lower Midrange, Midrange, Upper Midrange, Presence
const float low_cutoffs[] = {60.0f, 250.0f, 500.0f, 2000.0, 4000.0f};
const float high_cutoffs[] = {250.0f, 500.0f, 2000.0f, 4000.0f, 6000.0f};

void freq_bin_algo(int sr, float *extracted) {
  float freq_bin_size = (float)sr / M_BUF_SIZE;
  const size_t buf_size = sizeof(low_cutoffs) / sizeof(low_cutoffs[0]);
  float bin_sums[buf_size];

  int low_bin;
  int high_bin;

  for (size_t filter_index = 0; filter_index < buf_size; filter_index++) {
    low_bin = low_cutoffs[filter_index] / freq_bin_size;
    high_bin = high_cutoffs[filter_index] / freq_bin_size;

    bin_sums[filter_index] = 0.0f;

    for (int i = low_bin; i < high_bin; i++) {
      bin_sums[filter_index] += extracted[i];
    }

    // average it out.
    const int iter_count = high_bin - low_bin;
    bin_sums[filter_index] /= iter_count;
  }

  float max_bin_value = bin_sums[0];
  float min_bin_value = bin_sums[0];

  int max_bin_index = 0;
  int min_bin_index = 0;

  for (size_t filter_index = 0; filter_index < buf_size; filter_index++) {
    if (bin_sums[filter_index] > max_bin_value) {
      max_bin_value = bin_sums[filter_index];
      max_bin_index = filter_index;
      continue;
    }

    if (bin_sums[filter_index] < min_bin_value) {
      min_bin_value = bin_sums[filter_index];
      min_bin_index = filter_index;
    }
  }

  low_bin = low_cutoffs[max_bin_index] / freq_bin_size;
  high_bin = high_cutoffs[max_bin_index] / freq_bin_size;

  for (int i = low_bin; i < high_bin; i++) {
    extracted[i] *= 1.5;
  }

  low_bin = low_cutoffs[min_bin_index] / freq_bin_size;
  high_bin = high_cutoffs[min_bin_index] / freq_bin_size;

  for (int i = low_bin; i < high_bin; i++) {
    extracted[i] *= 0.75;
  }
}

void squash_to_log(FFTBuffers *bufs, FFTData *data) {
  float step = 1.06f;
  float lowf = 1.0f;
  size_t m = 0;
  size_t y = 0;

  data->max_ampl = 1.0f;
  data->max_phase = 1.0f;

  for (float f = lowf; (size_t)f < HALF_BUFF_SIZE; f = ceilf(f * step)) {
    float fs = ceilf(f * step);
    float a = 0.0f;
    float p = 0.0f;

    for (size_t q = (size_t)f; q < HALF_BUFF_SIZE && q < (size_t)fs; ++q) {
      float b = amp(bufs->extracted[q]);
      if (b > a) {
        a = b;
      }

      float ph = bufs->phases[q];
      if (ph > p) {
        p = ph;
      }
    }

    if (data->max_ampl < a) {
      data->max_ampl = a;
    }

    if (data->max_phase < p) {
      data->max_phase = p;
    }

    bufs->processed_samples[m++] = a;
    bufs->processed_phases[y++] = p;
  }

  data->output_len = m;
}

void hamming_window(float *in, const float *hamming_values, float *windowed) {
  /*Iterate for the size of a single channel*/
  for (int i = 0; i < HALF_BUFF_SIZE; ++i) {
    int left = i * 2;
    int right = i * 2 + 1;

    windowed[left] = in[left] * hamming_values[left];
    windowed[right] = in[right] * hamming_values[right];
  }
}

void calculate_window(float *hamming_values) {
  for (int i = 0; i < M_BUF_SIZE; ++i) {
    float Nf = (float)M_BUF_SIZE;
    float t = (float)i / (Nf - 1);
    hamming_values[i] = 0.54 - 0.46 * cosf(2 * M_PI * t);
  }
}
