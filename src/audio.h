#ifndef AUDIO_H
#define AUDIO_H
#include "audiodefs.h"
// Audio functions
void close_device(const unsigned int *dev);
void seek_forward(AudioDataContainer *adc);
void seek_backward(AudioDataContainer *adc);
void zero_values(AudioDataContainer *adc);
void callback(void *userdata, uint8_t *stream, int length);
int read_audio_file(const char *file_path, AudioDataContainer *adc);
int load_song(AudioDataContainer *adc);
void fft_push(const uint32_t *pos, float *in, float *buffer, size_t bytes);
void calculate_window(float *hamming_values);
void hamming_window(float *in, const float *hamming_values, float *windowed);
int get_status(const unsigned int *dev);
void pause_device(void);
void resume_device(void);
void iter_fft(float *in, Compf *out, size_t size);
void squash_to_log(FFTBuffers *bufs, FFTData *data);
float amp(float z);
void linear_mapping(FFTBuffers *bufs, FFTData *data);
float get_freq(const Compf *c);
void zero_fft(FFTBuffers *bufs, FFTData *data);
#endif
