#ifndef AUDIO_H
#define AUDIO_H

#define M_BUF_SIZE (1 << 13)
#define HALF_BUFF_SIZE (M_BUF_SIZE / 2)

#include <stddef.h>
#include <stdint.h>

typedef struct {
  float real;
  float imag;
} Compf;

struct AudioData {
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

struct FFTBuffers {
  float fft_in[2][M_BUF_SIZE];
  Compf out_raw[M_BUF_SIZE];
  float extracted[M_BUF_SIZE];
  float processed_samples[M_BUF_SIZE];
  float smoothed[M_BUF_SIZE];
  float smear[M_BUF_SIZE];
};

struct FFTData {
  int buffer_access;
  size_t output_len;
  int cell_width;
  float max_ampl;
  float hamming_values[M_BUF_SIZE];
};

typedef struct FFTBuffers FFTBuffers;
typedef struct FFTData FFTData;
typedef struct AudioData AudioData;

struct SDL_AudioSpec;
typedef struct SDL_AudioSpec SDL_AudioSpec;

void _fft_push(const uint32_t *pos, float *buffer, const size_t bytes);
void _zero_fft(void);

void _resume_device(void);
void _pause_device(void);

void _close_device(void);
int _start_device(void);

int _file_read(const char *filepath);

// Audio functions
#endif
