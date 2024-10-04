#include "../include/fft.hpp"
#include "../include/globals.hpp"
#include "../include/macdefs.hpp"
#include "../include/switch.hpp"

SDL2Audio::SDL2Audio() {
  next_song_flag = 0;
  audio_streaming = 0;
}

void callback(void *data, uint8_t *stream, int len) {
  AudioDataContainer *a = static_cast<AudioDataContainer *>(data);

  const uint32_t *length = &a->length;
  uint32_t *pos = &a->position;

  const uint32_t uint32_len = static_cast<uint32_t>(len);
  const uint32_t samples = uint32_len / sizeof(float);
  const uint32_t remaining = (*length - *pos);

  const uint32_t copy = (samples < remaining) ? samples : remaining;

  float *f32_stream = reinterpret_cast<float *>(stream);
  for (uint32_t i = 0; i < copy; i++) {
    f32_stream[i] = a->buffer[i + *pos] * 1.0;
  }

  a->fft_push_fn(*pos, a->fft_in, a->buffer, copy * sizeof(float));

  *pos += copy;
}

void SDL2Audio::audio_state_handler(AudioData *ad, ProgramFiles *files,
                                    ProgramPath *pathing,
                                    ThreadData *FTransformThread) {
  switch (*sdl2_ad.get_stream_flag()) {
  default: {
    break;
  }
  case PLAYING: {
    switch (*sdl2_ad.get_next_song_flag()) {
    case WAITING: {
      uint32_t position = ad->get_audio_data()->position;
      uint32_t length = ad->get_audio_data()->length;

      if (position >= length) {
        sdl2_ad.set_flag(NEXT, sdl2_ad.get_next_song_flag());
      } else {
      }
      break;
    }

    case NEXT: {
      goto_next_song(files, pathing, ad);
      break;
    }

    default: {
      break;
    }
    }
    break;
  }
  }
}

void SDL2Audio::set_audio_spec(AudioDataContainer *a) {
  spec.userdata = a;
  spec.callback = callback;
  spec.channels = a->channels;
  spec.freq = a->SR;
  spec.format = AUDIO_F32;
  spec.samples = BUFF_SIZE;
}

bool SDL2Audio::open_audio_device() {
  dev = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
  if (!dev) {
    fprintf(stderr, "Could not open audio device! -> %s\n", SDL_GetError());
    return false;
  }
  return true;
}

void SDL2Audio::close_audio_device() {
  if (SDL_GetAudioDeviceStatus(dev)) {
    SDL_CloseAudioDevice(dev);
  }
}

void SDL2Audio::pause_audio() {
  if (SDL_GetAudioDeviceStatus(dev)) {
    SDL_PauseAudioDevice(dev, true);
    audio_streaming = 0;
  }
}

void SDL2Audio::resume_audio() {
  if (SDL_GetAudioDeviceStatus(dev)) {
    SDL_PauseAudioDevice(dev, false);
    audio_streaming = 1;
  }
}

void SDL2Audio::set_flag(int flag, int *mutable_value) {
  *mutable_value = flag;
}

int *SDL2Audio::get_next_song_flag() { return &next_song_flag; }

int *SDL2Audio::get_stream_flag() { return &audio_streaming; }

SDL_AudioDeviceID *SDL2Audio::get_device() { return &dev; }
SDL_AudioSpec *SDL2Audio::get_spec() { return &spec; }

SDL2Audio::~SDL2Audio() {}
