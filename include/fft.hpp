#ifndef FFT_HPP
#define FFT_HPP
#include "macdefs.hpp"
#include <SDL2/SDL_rect.h>
#include <algorithm>
#include <complex>

struct FBuffers {
  float fft_in[DOUBLE_BUFF];
  float in_cpy[DOUBLE_BUFF];
  float pre_raw[BUFF_SIZE];
  float _Complex out_raw[BUFF_SIZE];
  float _Complex post_raw[BUFF_SIZE];
  float processed[HALF_BUFF];
  float smoothed[HALF_BUFF];
  float smear[HALF_BUFF];
};

struct FData {
  int cell_width;
  size_t output_len;
  float max_ampl;
  SDL_Rect *rect_buff;
  float hamming_values[BUFF_SIZE];
};

class FourierTransform {
public:
  FourierTransform();
  void fft_push(uint32_t pos, float *audio_data_buffer, int bytes);
  void generate_visual();
  void calculate_window();
  void hamming_window();
  float amp(float _Complex z);
  void squash_to_log(size_t size);
  void apply_smear();
  void apply_smoothing();
  FData *get_data();
  FBuffers *get_bufs();
  void fft_func(float *in, size_t stride, float _Complex *out, size_t n);

private:
  FData data;
  FBuffers bufs;
};
#endif
