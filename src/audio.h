#ifndef AUDIO_H
#define AUDIO_H

#include "audiodefs.h"
#include <SDL2/SDL_audio.h>
#include <stdbool.h>
#include <stdint.h>

// Audio functions
void zero_values(AudioDataContainer *adc);
void callback(void *userdata, uint8_t *stream, int length);
bool read_audio_file(char *file_path, AudioDataContainer *adc);
bool load_song(AudioDataContainer *adc);
void fft_push(const uint32_t *pos, float *in, float *buffer, size_t bytes);
void calculate_window(float *hamming_values);
void hamming_window(float *in, const float *hamming_values, float *windowed);
bool pause_device(void);
bool resume_device(void);
void recursive_fft(float *in, size_t stride, Float_Complex *out, size_t n);
void freq_bin_algo(int sr, float *extracted);
void squash_to_log(FFTBuffers *bufs, FFTData *data);
float amp(float z);
void visual_refine(FFTBuffers *bufs, FFTData *data);
void extract_frequencies(FFTBuffers *bufs);
#endif
