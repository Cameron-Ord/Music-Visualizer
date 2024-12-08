#include "audio.h"
#include "main.h"
#include "utils.h"
#include <errno.h>
#include <sndfile.h>

void seek_forward(AudioDataContainer *adc) {
  int pos_cpy = adc->position + (1 << 15);
  if (pos_cpy > (int)adc->length) {
    pos_cpy = adc->length - M_BUF_SIZE;
  }

  adc->position = pos_cpy;
}

void seek_backward(AudioDataContainer *adc) {
  int pos_cpy = adc->position - (1 << 15);
  if (pos_cpy < 0) {
    pos_cpy = 0;
  }

  adc->position = pos_cpy;
}

void callback(void *userdata, uint8_t *stream, int length) {
  AudioDataContainer *adc = (AudioDataContainer *)userdata;
  if (!adc) {
    return;
  }

  if (!adc->buffer) {
    return;
  }

  const uint32_t file_length = adc->length;
  const uint32_t uint32_len = (uint32_t)length;
  const uint32_t samples = uint32_len / sizeof(float);
  const uint32_t remaining = (file_length - adc->position);
  const uint32_t copy = (samples < remaining) ? samples : remaining;

  if (adc->position >= file_length) {
    return;
  }

  if (stream && vis.dev) {
    float *f32_stream = (float *)stream;
    for (uint32_t i = 0; i < copy; i++) {
      if (i + adc->position < file_length) {
        f32_stream[i] = adc->buffer[i + adc->position] * 1.0f;
      }
    }
    adc->position += copy;
  }

  if (adc->position >= file_length) {
    return;
  }

  if ((adc->position + copy) < file_length) {
    fft_push(&adc->position, adc->next->fft_in, adc->buffer,
             copy * sizeof(float));
  }
}

void fft_push(const uint32_t *pos, float *in, float *buffer, size_t bytes) {
  if (buffer && in) {
    memcpy(in, buffer + *pos, bytes);
  }
}

void zero_values(AudioDataContainer *adc) {
  adc->bytes = 0;
  adc->channels = 0;
  adc->format = 0;
  adc->bytes = 0;
  adc->length = 0;
  adc->position = 0;
  adc->samples = 0;
  adc->SR = 0;
  adc->volume = 1.0;
}

int read_audio_file(const char *file_path, AudioDataContainer *adc) {
  if (!adc || !file_path) {
    return 0;
  }

  SNDFILE *sndfile = NULL;
  SF_INFO sfinfo;

  memset(&sfinfo, 0, sizeof(SF_INFO));

  fprintf(stdout, "Reading file -> %s\n", file_path);
  sndfile = sf_open(file_path, SFM_READ, &sfinfo);
  if (!sndfile) {
    fprintf(stderr, "Could not open file: %s -> %s\n", file_path,
            sf_strerror(NULL));
    return 0;
  }

  if (sfinfo.channels != 2) {
    fprintf(stderr, "Must be a two channel audio file!\n");
    return 0;
  }

  size_t samples = sfinfo.frames * sfinfo.channels;

  fprintf(stdout, "SAMPLE RATE %d\n", sfinfo.samplerate);
  fprintf(stdout, "BYTES %zu\n", samples * sizeof(float));
  fprintf(stdout, "SAMPLES %zu\n", samples);

  float *tmp = calloc(samples, sizeof(float));
  if (!tmp) {
    fprintf(stderr, "Could not allocate buffer! -> %s\n", strerror(errno));
    sf_close(sndfile);
    return 0;
  }

  sf_count_t read = sf_read_float(sndfile, tmp, samples);
  if (read < 0) {
    fprintf(stderr, "Error reading audio data! ->%s\n", sf_strerror(sndfile));
    free(tmp);
    sf_close(sndfile);
    return 0;
  }
  sf_close(sndfile);

  if (adc->buffer) {
    free(adc->buffer);
    adc->buffer = NULL;
  }

  zero_values(adc);
  if (adc->next) {
    zero_fft(adc->next, adc->next->next);
  }

  adc->channels = sfinfo.channels;
  adc->SR = sfinfo.samplerate;
  adc->format = sfinfo.format;
  adc->samples = sfinfo.frames * sfinfo.channels;
  adc->bytes = adc->samples * sizeof(float);
  adc->length = (uint32_t)(adc->samples);

  adc->buffer = tmp;

  return 1;
}

int load_song(AudioDataContainer *adc) {
  if (!adc) {
    return 0;
  }

  vis.spec.userdata = adc;
  vis.spec.callback = callback;
  vis.spec.channels = adc->channels;
  vis.spec.freq = adc->SR;
  vis.spec.format = AUDIO_F32SYS;
  vis.spec.samples = M_BUF_SIZE / adc->channels;

  vis.dev = SDL_OpenAudioDevice(NULL, 0, &vis.spec, NULL, 15);
  if (!vis.dev) {
    SDL_ERR_CALLBACK(SDL_GetError());
    return 0;
  }

  resume_device();
  return 1;
}

int get_status(const unsigned int *dev) {
  return SDL_GetAudioDeviceStatus(*dev);
}

void close_device(const unsigned int *dev) { SDL_CloseAudioDevice(*dev); }

void pause_device(void) {
  if (vis.dev) {
    if (SDL_GetAudioDeviceStatus(vis.dev) != SDL_AUDIO_PAUSED) {
      SDL_PauseAudioDevice(vis.dev, true);
    }
  }
}

void resume_device(void) {
  if (vis.dev) {
    if (SDL_GetAudioDeviceStatus(vis.dev) != SDL_AUDIO_PLAYING) {
      SDL_PauseAudioDevice(vis.dev, false);
    }
  }
}
