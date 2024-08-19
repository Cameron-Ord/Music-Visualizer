#include "../include/audio.hpp"
#include <SDL2/SDL_audio.h>

SDL2Audio::SDL2Audio() {
  song_paused = true;
  song_playing = false;
  song_ended = false;
}

void SDL2Audio::set_audio_spec(USERDATA *userdata) {
  spec.userdata = userdata;
  spec.callback = callback;
  spec.channels = userdata->ad->get_audio_data()->channels;
  spec.freq = userdata->ad->get_audio_data()->SR;
  spec.format = AUDIO_F32;
  spec.samples = (1 << 12);
}

bool SDL2Audio::open_audio_device() {
  dev = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
  if (!dev) {
    fprintf(stderr, "Could not open audio device! -> %s\n", SDL_GetError());
    return false;
  }
  return true;
}

void SDL2Audio::close_audio_device() { SDL_CloseAudioDevice(dev); }

void SDL2Audio::pause_audio() { SDL_PauseAudioDevice(dev, true); }

void SDL2Audio::resume_audio() { SDL_PauseAudioDevice(dev, false); }

SDL_AudioDeviceID *SDL2Audio::get_device() { return &dev; }
SDL_AudioSpec *SDL2Audio::get_spec() { return &spec; }

bool SDL2Audio::get_pause_state() { return song_paused; }
bool SDL2Audio::get_play_state() { return song_playing; }
bool SDL2Audio::get_end_state() { return song_ended; }

void callback(void *data, uint8_t *stream, int len) {
  USERDATA *userdata = (USERDATA *)data;

  uint32_t length = userdata->ad->get_audio_data()->length;
  uint32_t *pos = &userdata->ad->get_audio_data()->position;

  uint32_t remaining = (length - *pos);
  uint32_t copy = ((uint32_t)len / sizeof(float) < remaining)
                      ? (uint32_t)len / sizeof(float)
                      : remaining;

  float *f32_stream = (float *)stream;

  for (int i = 0; i < copy; i++) {
    f32_stream[i] = userdata->ad->get_audio_data()->buffer[i + *pos] * 1.0;
  }

  *pos += copy;
}

SDL2Audio::~SDL2Audio() {}
