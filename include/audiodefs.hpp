#ifndef AUDIODEFS_HPP
#define AUDIODEFS_HPP

#include "macdefs.hpp"
#include <SDL2/SDL_rect.h>
#include <cmath>
#include <complex>
#include <iostream>

void callback(void *data, uint8_t *stream, int len);

struct FBuffers {
  float in_cpy[DOUBLE_BUFF];
  float pre_raw[BUFF_SIZE];
  std::complex<float> out_raw[BUFF_SIZE];
  std::complex<float> post_raw[BUFF_SIZE];
  float extracted[BUFF_SIZE];
  float phases[BUFF_SIZE];
  float processed[HALF_BUFF];
  float processed_phases[HALF_BUFF];
  float smoothed[HALF_BUFF];
  float smear[HALF_BUFF];
};

struct FData {
  int cell_width;
  size_t output_len;
  float max_ampl;
  float max_phase;
  SDL_Rect *rect_buff;
  float hamming_values[BUFF_SIZE];
  int sample_rate;
};

struct AudioDataContainer {
  void (*fft_push_fn)(uint32_t, float *, float *, int);
  float fft_in[DOUBLE_BUFF];
  float *buffer;
  uint32_t length;
  uint32_t position;
  size_t samples;
  size_t bytes;
  int channels;
  int SR;
  int format;
  float volume;
};

struct FFTSettings {
  int smoothing_amount;
  int smearing_amount;
  float filter_coeffs[3];
};
#endif