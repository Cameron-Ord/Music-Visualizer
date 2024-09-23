#include "../include/audio.hpp"

AudioData::AudioData() {
  a_data.buffer = NULL;
  a_data.length = 0;
  a_data.position = 0;
  a_data.samples = 0;
  a_data.bytes = 0;
  a_data.channels = 0;
  a_data.SR = 0;
  a_data.format = 0;
  a_data.volume = 1.0;
}

void AudioData::zero_values() {
  a_data.length = 0;
  a_data.position = 0;
  a_data.samples = 0;
  a_data.bytes = 0;
  a_data.channels = 0;
  a_data.SR = 0;
  a_data.format = 0;
}

AudioDataContainer *AudioData::get_audio_data() { return &a_data; }

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

  a_data.channels = sfinfo.channels;
  a_data.SR = sfinfo.samplerate;
  a_data.format = sfinfo.format;
  a_data.samples = sfinfo.frames * sfinfo.channels;
  a_data.bytes = a_data.samples * sizeof(float);

  fprintf(stdout, "SAMPLE RATE %d\n", a_data.SR);
  fprintf(stdout, "BYTES %zu\n", a_data.bytes);
  fprintf(stdout, "SAMPLES %zu\n", a_data.samples);

  a_data.buffer = (float *)realloc(a_data.buffer, a_data.bytes);
  if (!a_data.buffer) {
    char msg[256];
    strerror_s(msg, sizeof(msg), errno);
    fprintf(stderr, "Could not allocate buffer! -> %s\n", msg);
    sf_close(sndfile);
    return false;
  }

  memset(a_data.buffer, 0, a_data.bytes);

  sf_count_t read = sf_read_float(sndfile, a_data.buffer, a_data.samples);
  if (read < 0) {
    fprintf(stderr, "Error reading audio data! ->%s\n", sf_strerror(sndfile));
    free(a_data.buffer);
    sf_close(sndfile);
    return false;
  }

  a_data.length = (uint32_t)a_data.samples;
  sf_close(sndfile);
  return true;
}

AudioData::~AudioData() {}
