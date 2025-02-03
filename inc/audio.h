#ifndef AUDIO_H
#define AUDIO_H
#include "audiodefs.h"

struct SDL_AudioSpec;
typedef struct SDL_AudioSpec SDL_AudioSpec;

// Audio functions
int access_clamp(const int access);
void clean_buffers(FFTBuffers *b);
int spec_compare(const SDL_AudioSpec *s, const AudioDataContainer *a);
void close_device(unsigned int dev);
void callback(void *userdata, uint8_t *stream, int length);
int read_audio_file(const char *file_path, AudioDataContainer *adc);
void set_spec(AudioDataContainer *adc, SDL_AudioSpec *s);
void calculate_window(float *hamming_values);
float window(const float in, const float coeff);
int get_status(const unsigned int *dev);
void pause_device(unsigned int dev);
void resume_device(unsigned int dev);
void iter_fft(float *in, float *coeffs, Compf *out, size_t size);
void squash_to_log(FFTBuffers *bufs, FFTData *data);
float amp(float z);
void linear_mapping(FFTBuffers *bufs, FFTData *data, const int smear_c,
                    const int smooth_c, const int tframes);
unsigned int open_device(SDL_AudioSpec *spec);
float get_freq(const Compf *c);
void zero_fft(FFTBuffers *bufs, FFTData *data);
#endif
