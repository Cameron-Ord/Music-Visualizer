#ifndef AUDIODEFS_H
#define AUDIODEFS_H

#define M_BUF_SIZE (1 << 13)
#define HALF_BUFF_SIZE (M_BUF_SIZE / 2)

#include <stddef.h>
#include <stdint.h>

typedef struct {
  float real;
  float imag;
} Compf;

struct AudioData;
struct AudioDataContainer;
struct FFTBuffers;
struct FFTData;

typedef struct AudioDataContainer AudioDataContainer;
typedef struct AudioData AudioData;
typedef struct FFTBuffers FFTBuffers;
typedef struct FFTData FFTData;

struct AudioDataContainer {
  float *buffer;
  AudioData *ad;
  FFTBuffers *fftbuff;
  FFTData *fftdata;
  unsigned int *device;
};

struct AudioData {
  uint32_t length;
  uint32_t position;
  size_t samples;
  size_t bytes;
  int channels;
  int SR;
  int format;
  float volume;
};

// I squash stereo channels into a size of M_BUF_SIZE by summing and other means
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

#endif
