#ifndef AUDIO_DEF_H
#define AUDIO_DEF_H

#include <cstdint>

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
};

#endif
