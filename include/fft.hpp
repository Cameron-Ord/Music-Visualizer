#ifndef FFT_HPP
#define FFT_HPP
#include "defines.hpp"

int FFT_THREAD(void* data);

class FourierTransform {
  public:
    FourierTransform();
    void fft_push(uint32_t pos, float *audio_data_buffer, int bytes);
    void generate_visual(AudioDataContainer *ad);
    void calculate_window();
    void hamming_window();
    float amp(float z);
    void squash_to_log(size_t size);
    void visual_refine();
    FData *get_data();
    FBuffers *get_bufs();
    void fft_func(float *in, size_t stride, std::complex<float> *out, size_t n);
    void extract_frequencies();
    void multi_band_stop(int SR);
    void set_filter_coeff(size_t i, float amount);
    void set_smoothing(int amount);
    void set_smear(int amount);
    void freq_bin_algo(int SR);
    int FFT_THREAD(void *data);

    const FFTSettings *get_settings();

  private:
    FData data;
    FBuffers bufs;
    FFTSettings settings;
    std::vector<float> low_cutoffs;
    std::vector<float> high_cutoffs;
};
#endif
