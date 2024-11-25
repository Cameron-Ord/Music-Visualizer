#ifndef AUDIODEFS_H
#define AUDIODEFS_H

#define S_BUF_SIZE (1 << 14)
#define M_BUF_SIZE (1 << 13)
#define HALF_BUFF_SIZE (M_BUF_SIZE / 2)

#include <complex.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// I stole this from tsoding cause I had originally written my complex numbers
// stuff in C++ and im lazy. So thanks Tsoding, I love you.

// https://github.com/tsoding/musializer/blob/master/src/plug.c#L268

#ifdef _MSC_VER
#define Float_Complex _Fcomplex
#define cfromreal(re) _FCbuild(re, 0)
#define cfromimag(im) _FCbuild(0, im)
#define mulcc _FCmulcc
#define addcc(a, b) _FCbuild(crealf(a) + crealf(b), cimagf(a) + cimagf(b))
#define subcc(a, b) _FCbuild(crealf(a) - crealf(b), cimagf(a) - cimagf(b))
#else
#define Float_Complex float _Complex
#define cfromreal(re) (re)
#define cfromimag(im) ((im) * I)
#define mulcc(a, b) ((a) * (b))
#define addcc(a, b) ((a) + (b))
#define subcc(a, b) ((a) - (b))
#endif

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
  Float_Complex out_raw[M_BUF_SIZE];
  float extracted[M_BUF_SIZE];
  float phases[M_BUF_SIZE];
  float processed[M_BUF_SIZE];
  float processed_phases[M_BUF_SIZE];
  float smoothed[M_BUF_SIZE];
  float smear[M_BUF_SIZE];
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
  AudioDataContainer *next;
};

#endif
