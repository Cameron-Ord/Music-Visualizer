#ifndef FFT_HPP
#define FFT_HPP
#include "audio_def.h"
#include "macdefs.hpp"
#include <SDL2/SDL_rect.h>
#include <cmath>
#include <complex>
#include <cstring>
#include <iostream>

struct FBuffers {
    float fft_in[DOUBLE_BUFF];
    float in_cpy[DOUBLE_BUFF];
    float pre_raw[BUFF_SIZE];
    std::complex<float> out_raw[BUFF_SIZE];
    std::complex<float> post_raw[BUFF_SIZE];
    float extracted[BUFF_SIZE];
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

struct AudioDataContainer;

class FourierTransform {
  public:
    FourierTransform();
    void fft_push(uint32_t pos, float *audio_data_buffer, int bytes);
    void generate_visual(AudioDataContainer *ad);
    void calculate_window();
    void hamming_window();
    float amp(float z);
    void squash_to_log(size_t size);
    void apply_smear();
    void apply_smoothing();
    FData *get_data();
    FBuffers *get_bufs();
    void fft_func(float *in, size_t stride, std::complex<float> *out, size_t n);
    void pre_emphasis();
    void extract_frequencies();
    void high_pass_filter(int SR, float cutoff_freq);

  private:
    FData data;
    FBuffers bufs;
};
#endif
