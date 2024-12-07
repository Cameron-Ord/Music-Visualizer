#ifndef AUDIODEFS_H
#define AUDIODEFS_H

#define S_BUF_SIZE (1 << 14)
#define M_BUF_SIZE (1 << 13)
#define HALF_BUFF_SIZE (M_BUF_SIZE / 2)

#include <complex.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
 float real;
 float imag;
}Compf;

struct AudioDataContainer;
struct FFTBuffers;
struct FFTData;
struct BandStopFilter;

typedef struct AudioDataContainer AudioDataContainer;
typedef struct FFTBuffers FFTBuffers;
typedef struct FFTData FFTData;
typedef struct BandStopFilter BandStopFilter;

struct AudioDataContainer {
  float *buffer;
  uint32_t length;
  uint32_t position;
  size_t samples;
  size_t bytes;
  int channels;
  int SR;
  int format;
  float volume;
  FFTBuffers *next;
};

// I squash stereo channels into a size of M_BUF_SIZE by summing and other means
struct FFTBuffers {
  float fft_in[M_BUF_SIZE];
  float windowed[M_BUF_SIZE];
  Compf out_raw[M_BUF_SIZE];
  float extracted[M_BUF_SIZE];
  float processed_samples[M_BUF_SIZE];
  float smoothed[M_BUF_SIZE];
  float smear[M_BUF_SIZE];
  FFTData *next;
};

struct FFTData {
  size_t output_len;
  int cell_width;
  float max_ampl;
  float hamming_values[M_BUF_SIZE];
  AudioDataContainer *next;
};

#endif
