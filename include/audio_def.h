#ifndef AUDIO_DEF_H
#define AUDIO_DEF_H

typedef enum {STOPPED = 0, PLAYING = 1, PAUSED = 2} STREAM_FLAGS;
typedef enum {WAITING= 0, NEXT = 1} PB_FLAGS;

#include <cstdint>
#include <cstring>

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
