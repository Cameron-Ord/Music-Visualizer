#include "../include/sdl2_audio.hpp"

AudioData::AudioData() {
  buffer = NULL;
  length = 0;
  position = 0;
  samples = 0;
  bytes = 0;
  channels = 0;
  SR = 0;
  format = 0;
  volume = 1.0;
}

void AudioData::zero_values() {
  length = 0;
  position = 0;
  samples = 0;
  bytes = 0;
  channels = 0;
  SR = 0;
  format = 0;
}

bool AudioData::read_audio_file(std::string file_path) {
  zero_values();

  SNDFILE *sndfile = NULL;
  SF_INFO sfinfo;

  fprintf(stdout, "Reading file -> %s\n", file_path.c_str());

  sndfile = sf_open(file_path.c_str(), SFM_READ, &sfinfo);
  if (!sndfile) {
    fprintf(stderr, "Could not open file! -> %s\n", sf_strerror(NULL));
    return false;
  }

  if (sfinfo.channels != 2) {
    fprintf(stderr, "Must be a 2 channel audio file!\n");
    return false;
  }

  channels = sfinfo.channels;
  SR = sfinfo.samplerate;
  format = sfinfo.format;
  samples = sfinfo.frames * sfinfo.channels;
  bytes = samples * sizeof(float);

  fprintf(stdout, "SAMPLE RATE %d\n", SR);
  fprintf(stdout, "BYTES %ld\n", bytes);
  fprintf(stdout, "SAMPLES %ld\n", samples);

  buffer = (float *)realloc(buffer, bytes);
  if (!buffer) {
    fprintf(stderr, "Could not allocate buffer! -> %s\n", strerror(errno));
    sf_close(sndfile);
    return false;
  }
  memset(buffer, 0, bytes);

  sf_count_t read = sf_read_float(sndfile, buffer, samples);
  if (read < 0) {
    fprintf(stderr, "Error reading audio data! ->%s\n", sf_strerror(sndfile));
    free(buffer);
    sf_close(sndfile);
    return false;
  }

  length = (uint32_t)samples;
  sf_close(sndfile);
  return true;
}

AudioData::~AudioData() {}
