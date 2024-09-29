#ifndef FFT_HPP
#define FFT_HPP
#include "defines.hpp"

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
