#ifndef AUDIO_H
#define AUDIO_H
#include "audiodefs.h"
// Audio functions
void seek_forward(AudioDataContainer *adc);
void seek_backward(AudioDataContainer *adc);
void zero_values(AudioDataContainer *adc);
void callback(void *userdata, uint8_t *stream, int length);
int read_audio_file(const char *file_path, AudioDataContainer *adc);
int load_song(AudioDataContainer *adc);
void fft_push(const uint32_t *pos, float *in, float *buffer, size_t bytes);
void calculate_window(float *hamming_values);
void hamming_window(float *in, const float *hamming_values, float *windowed);
bool pause_device(void);
bool resume_device(void);
void iter_fft(float *in, Compf *out, size_t size);
void freq_bin_algo(int sr, float *extracted);
void filter(int sr, float *extracted);
void squash_to_log(FFTBuffers *bufs, FFTData *data);
float amp(float z);
void linear_mapping(FFTBuffers *bufs, FFTData *data);
void extract_frequencies(FFTBuffers *bufs);
void zero_fft(FFTBuffers *bufs, FFTData *data);
#endif
