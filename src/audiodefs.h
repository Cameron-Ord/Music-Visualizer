#ifndef AUDIODEFS_H
#define AUDIODEFS_H

#define M_BUF_SIZE (1<<12)
#define S_BUF_SIZE (M_BUF_SIZE * 2)
#define HALF_BUFF_SIZE (M_BUF_SIZE / 2)

#include <stdint.h>
#include <complex.h>

struct AudioDataContainer;
struct FFTBuffers;
struct FFTData;
struct BandStopFilter;

typedef struct AudioDataContainer AudioDataContainer;
typedef struct FFTBuffers FFTBuffers;
typedef struct FFTData FFTData;
typedef struct BandStopFilter BandStopFilter;

struct AudioDataContainer {
    float* buffer;
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

struct FFTBuffers {
  float in_cpy[S_BUF_SIZE];
  float pre_raw[M_BUF_SIZE];
  _Complex float out_raw[M_BUF_SIZE];
  float extracted[M_BUF_SIZE];
  float phases[M_BUF_SIZE];
  float processed[HALF_BUFF_SIZE];
  float processed_phases[HALF_BUFF_SIZE];
  float smoothed[HALF_BUFF_SIZE];
  float smear[HALF_BUFF_SIZE];
  FFTData *next;
};

struct BandStopFilter {
  int smoothing_amount;
  int smearing_amount;
  float filter_coeffs[3];
};

struct FFTData {
    size_t output_len;
  int cell_width;
  float max_ampl;
  float max_phase;
  float hamming_values[M_BUF_SIZE];
  AudioDataContainer* next;
};


#endif