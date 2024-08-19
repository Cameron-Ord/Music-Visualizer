#ifndef SDL2_AUDIO_HPP
#define SDL2_AUDIO_HPP

#include <SDL2/SDL_audio.h>
#include <sndfile.h>
#include <string>

void callback();

class AudioData {
public:
  int read_audio_file(std::string file_path);

private:
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

class SDL2Audio {
public:
  void set_audio_spec();

private:
};

#endif
