#ifndef FFT_HPP
#define FFT_HPP
#include "audiodefs.hpp"
#include "macdefs.hpp"
#include <cmath>
#include <complex>
#include <iostream>
#include <vector>
void recursive_fft(float *in, size_t stride, std::complex<float> *out,
                   size_t n);
void iterative_fft(float *in, std::complex<float> *out, size_t size);
void fft_push(uint32_t pos, float *fft_in, float *audio_data_buffer, int bytes);

class FourierTransform {
public:
  FourierTransform();
  void generate_visual();
  void calculate_window();
  void hamming_window();
  float amp(float z);
  void squash_to_log(size_t size);
  void visual_refine();
  FData *get_data();
  FBuffers *get_bufs();
  void extract_frequencies();
  void multi_band_stop(int SR);
  void set_filter_coeff(size_t i, float amount);
  void set_smoothing(int amount);
  void set_smear(int amount);
  void freq_bin_algo(int SR);
  const FFTSettings *get_settings();
  const AudioDataContainer *get_audio_data();
  void set_audio_data_ptr(AudioDataContainer *audio_data);

private:
  AudioDataContainer *a_data;
  FData data;
  FBuffers bufs;
  FFTSettings settings;
  std::vector<float> low_cutoffs;
  std::vector<float> high_cutoffs;
};
#endif
