#include "../include/fft.hpp"
#include "../include/threads.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

size_t bit_reverse(size_t index, size_t log2n);

FourierTransform::FourierTransform() {
  data.max_ampl = 1.0;
  data.output_len = 0;

  settings.smoothing_amount = 7;
  settings.smearing_amount = 5;

  for (size_t i = 0;
       i < sizeof(settings.filter_coeffs) / sizeof(settings.filter_coeffs[0]);
       i++) {
    settings.filter_coeffs[i] = 1.0f;
  }

  low_cutoffs = {60.0f, 250.0f, 2000.0f};
  high_cutoffs = {250.0f, 2000.0f, 5000.0f};

  memset(data.hamming_values, 0, sizeof(float) * BUFF_SIZE);
  memset(bufs.in_cpy, 0, DOUBLE_BUFF * sizeof(float));
  memset(bufs.pre_raw, 0, BUFF_SIZE * sizeof(float));
  memset(bufs.phases, 0, BUFF_SIZE * sizeof(float));
  memset(bufs.extracted, 0, BUFF_SIZE * sizeof(float));
  memset(bufs.processed, 0, HALF_BUFF * sizeof(float));
  memset(bufs.processed_phases, 0, HALF_BUFF * sizeof(float));
  memset(bufs.smoothed, 0, HALF_BUFF * sizeof(float));
  memset(bufs.smear, 0, HALF_BUFF * sizeof(float));

  for (size_t i = 0; i < BUFF_SIZE; i++) {
    bufs.out_raw[i] = std::complex<float>(0.0f, 0.0f);
  }

  for (size_t i = 0; i < BUFF_SIZE; i++) {
    bufs.post_raw[i] = std::complex<float>(0.0f, 0.0f);
  }

  calculate_window();
}

void FourierTransform::set_audio_data_ptr(AudioDataContainer *a) { a_data = a; }

void FourierTransform::set_filter_coeff(size_t i, float amount) {
  settings.filter_coeffs[i] = amount;
}

void FourierTransform::set_smear(int amount) {
  settings.smearing_amount = amount;
}

void FourierTransform::set_smoothing(int amount) {
  settings.smoothing_amount = amount;
}

const FFTSettings *FourierTransform::get_settings() { return &settings; }

FData *FourierTransform::get_data() { return &data; }
FBuffers *FourierTransform::get_bufs() { return &bufs; }

void recursive_fft(float *in, size_t stride, std::complex<float> *out,
                   size_t n) {
  if (n == 1) {
    out[0] = in[0];
    return;
  }

  size_t half_n = n / 2;

  recursive_fft(in, stride * 2, out, half_n);
  recursive_fft(in + stride, stride * 2, out + half_n, half_n);
  // v = o*x
  // out = e - o*x e + o*x e e| e + o*x o - o*x o o
  for (size_t k = 0; k < n / 2; ++k) {
    float t = (float)k / n;
    std::complex<float> v =
        std::exp(std::complex<float>(0, -2.0f * M_PI * t)) * out[k + half_n];
    std::complex<float> e = out[k];
    out[k] = e + v;
    out[k + half_n] = e - v;
  }

} /*fft_func*/

size_t bit_reverse(size_t index, size_t log2n) {
  size_t reversed = 0;
  for (size_t i = 0; i < log2n; i++) {
    reversed <<= 1;
    reversed |= (index & 1);
    index >>= 1;
  }
  return reversed;
}

// https://www.geeksforgeeks.org/iterative-fast-fourier-transformation-polynomial-multiplication/

void iterative_fft(float *in, std::complex<float> *out, size_t size) {

  for (size_t i = 0; i < size; i++) {
    int rev_index = bit_reverse(i, log2(size));
    out[i] = std::complex<float>(in[rev_index], 0.0);
  }

  const std::complex<float> iota(0.0f, 1.0f);
  for (int stage = 1; stage <= log2(size); ++stage) {
    size_t sub_arr_size = 1 << stage; // 2^stage
    size_t half_sub_arr = sub_arr_size >> 1;
    std::complex<float> twiddle(1.0f, 0.0f);

    std::complex<float> step =
        std::exp(iota * (static_cast<float>(M_PI) / half_sub_arr));
    for (size_t j = 0; j < half_sub_arr; j++) {
      for (size_t k = j; k < size; k += sub_arr_size) {
        std::complex<float> t = twiddle * out[k + half_sub_arr];
        std::complex<float> u = out[k];

        out[k] = u + t;
        out[k + half_sub_arr] = u - t;
      }
      twiddle *= step;
    }
  }
}

void fft_push(uint32_t pos, float *ffn_in_buf, float *audio_data_buffer,
              int bytes) {
  memcpy(ffn_in_buf, audio_data_buffer + pos, bytes);
}

void FourierTransform::generate_visual() {
  memcpy(bufs.in_cpy, a_data->fft_in, sizeof(float) * DOUBLE_BUFF);
  hamming_window();
  iterative_fft(bufs.pre_raw, bufs.out_raw, BUFF_SIZE);
  extract_frequencies();
  multi_band_stop(a_data->SR);
  freq_bin_algo(a_data->SR);
  squash_to_log(HALF_BUFF);
  visual_refine();
} /*generate_visual*/

void FourierTransform::freq_bin_algo(int SR) {
  float freq_bin_size = static_cast<float>(SR) / BUFF_SIZE;
  const size_t buf_size = 3;
  float bin_sums[buf_size];

  int low_bin;
  int high_bin;

  for (size_t filter_index = 0; filter_index < buf_size; filter_index++) {
    low_bin = low_cutoffs[filter_index] / freq_bin_size;
    high_bin = high_cutoffs[filter_index] / freq_bin_size;

    bin_sums[filter_index] = 0.0f;

    for (int i = low_bin; i < high_bin; i++) {
      bin_sums[filter_index] += bufs.extracted[i];
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
    bufs.extracted[i] *= 1.25;
  }

  low_bin = low_cutoffs[min_bin_index] / freq_bin_size;
  high_bin = high_cutoffs[min_bin_index] / freq_bin_size;

  for (int i = low_bin; i < high_bin; i++) {
    bufs.extracted[i] *= 0.75;
  }
}

void FourierTransform::calculate_window() {
  for (int i = 0; i < BUFF_SIZE; ++i) {
    float Nf = (float)BUFF_SIZE;
    float t = (float)i / (Nf - 1);
    data.hamming_values[i] = 0.54 - 0.46 * cosf(2 * M_PI * t);
  }
}

void FourierTransform::hamming_window() {
  /*Iterate for the size of a single channel*/
  for (int i = 0; i < BUFF_SIZE; ++i) {
    int left = i * 2;
    int right = i * 2 + 1;

    float summed = (bufs.in_cpy[left] + bufs.in_cpy[right]) / 2;
    bufs.pre_raw[i] = summed;
    bufs.pre_raw[i] *= data.hamming_values[i];
  }
}

// Y[n]=X[n]−0.90⋅X[n−1]
// void FourierTransform::pre_emphasis() {
//  for (int i = BUFF_SIZE - 1; i > 0; --i) {
//        float *input = &bufs.pre_raw[i];
//      const float last_input = bufs.pre_raw[i - 1];
//     *input = *input - settings.filter_alpha * last_input;
//}
//}

void FourierTransform::extract_frequencies() {
  for (int i = 0; i < BUFF_SIZE; ++i) {
    float real = bufs.out_raw[i].real();
    float imag = bufs.out_raw[i].imag();
    bufs.extracted[i] = sqrt(real * real + imag * imag);
    bufs.phases[i] = atan2(imag, real);
  }
}

void FourierTransform::multi_band_stop(int SR) {
  float freq_bin_size = static_cast<float>(SR) / BUFF_SIZE;
  for (size_t filter_index = 0; filter_index < low_cutoffs.size();
       ++filter_index) {

    // Just skip if its 1.0f since it doesnt actually change anything;
    if (settings.filter_coeffs[filter_index] == 1.0f) {
      continue;
    }

    int low_bin = low_cutoffs[filter_index] / freq_bin_size;
    int high_bin = high_cutoffs[filter_index] / freq_bin_size;
    for (int i = low_bin; i < high_bin; i++) {
      bufs.extracted[i] *= settings.filter_coeffs[filter_index];
    }
  }
}

void FourierTransform::squash_to_log(size_t size) {
  float step = 1.06f;
  float lowf = 1.0f;
  size_t m = 0;
  size_t y = 0;

  data.max_ampl = 1.0f;
  data.max_phase = 1.0f;

  for (float f = lowf; (size_t)f < size; f = ceilf(f * step)) {
    float fs = ceilf(f * step);
    float a = 0.0f;
    float p = 0.0f;

    for (size_t q = (size_t)f; q < size && q < (size_t)fs; ++q) {
      float b = amp(bufs.extracted[q]);
      if (b > a) {
        a = b;
      }

      float ph = bufs.phases[q];
      if (ph > p) {
        p = ph;
      }
    }

    if (data.max_ampl < a) {
      data.max_ampl = a;
    }

    if (data.max_phase < p) {
      data.max_phase = p;
    }

    bufs.processed[m++] = a;
    bufs.processed_phases[y++] = p;
  }

  data.output_len = m;
  data.output_len_ptr = &data.output_len;
}

float FourierTransform::amp(float z) {
  if (z == 0.0f) {
    return 0.0f;
  }
  return logf(z);
} /*amp*/

void FourierTransform::visual_refine() {
  const int FPS = 60;
  for (size_t i = 0; i < data.output_len; ++i) {
    bufs.processed_phases[i] /= data.max_phase;
    bufs.processed[i] /= data.max_ampl;

    bufs.smoothed[i] += (bufs.processed[i] - bufs.smoothed[i]) *
                        settings.smoothing_amount * (1.0 / FPS);

    bufs.smear[i] += (bufs.smoothed[i] - bufs.smear[i]) *
                     settings.smearing_amount * (1.0 / FPS);
  }
}
