#include "audio.h"
#include "main.h"
#include <sndfile.h>

void callback(void *userdata, uint8_t *stream, int length) {
  if (!userdata) {
    pause_device();
  }

  AudioDataContainer *adc = (AudioDataContainer *)userdata;
  if (adc && adc->buffer) {
    const uint32_t file_length = adc->length;

    const uint32_t uint32_len = (uint32_t)length;
    const uint32_t samples = uint32_len / sizeof(float);
    const uint32_t remaining = (file_length - adc->position);

    const uint32_t copy = (samples < remaining) ? samples : remaining;

    if ((adc->position + copy) > file_length && !vis.scrolling) {
      vis.stream_flag = false;
      vis.next_song_flag = 1;
      pause_device();
    }

    if (stream && vis.dev && !vis.scrolling) {
      float *f32_stream = (float *)stream;
      if (f32_stream) {
        for (uint32_t i = 0; i < copy; i++) {
          f32_stream[i] = adc->buffer[i + adc->position] * 1.0f;
        }
      }
      adc->position += copy;

    } else if (stream && vis.dev && vis.scrolling) {
      float *f32_stream = (float *)stream;
      if (f32_stream) {
        for (uint32_t i = 0; i < copy; i++) {
          f32_stream[i] = 0.0f;
        }
      }
    }

    if ((adc->position + copy) > file_length && !vis.scrolling) {
      vis.stream_flag = false;
      vis.next_song_flag = 1;
      pause_device();
    }

    if ((adc->position + copy) < file_length) {
      fft_push(&adc->position, adc->next->fft_in, adc->buffer,
               copy * sizeof(float));
    }
  }
}

void fft_push(const uint32_t *pos, float *in, float *buffer, size_t bytes) {
  if (!bytes) {
    return;
  }

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

bool read_audio_file(const char *file_path, AudioDataContainer *adc) {

  if (vis.dev) {
    SDL_CloseAudioDevice(vis.dev);
  }

  SNDFILE *sndfile = NULL;
  SF_INFO sfinfo;

  if (!adc) {
    return false;
  }

  if (adc->buffer) {
    free(adc->buffer);
    adc->buffer = NULL;
  }

  if (adc->next) {
    memset(adc->next->extracted, 0, sizeof(float) * M_BUF_SIZE);
    memset(adc->next->fft_in, 0, sizeof(float) * M_BUF_SIZE);
    memset(adc->next->out_raw, 0, sizeof(Float_Complex) * M_BUF_SIZE);
    memset(adc->next->phases, 0, sizeof(float) * M_BUF_SIZE);
    memset(adc->next->processed_phases, 0, sizeof(float) * M_BUF_SIZE);
    memset(adc->next->processed, 0, sizeof(float) * M_BUF_SIZE);
    memset(adc->next->smear, 0, sizeof(float) * M_BUF_SIZE);
    memset(adc->next->smoothed, 0, sizeof(float) * M_BUF_SIZE);
    memset(adc->next->windowed, 0, sizeof(float) * M_BUF_SIZE);
  }

  memset(&sfinfo, 0, sizeof(SF_INFO));

  fprintf(stdout, "Reading file -> %s\n", file_path);
  zero_values(adc);
  sndfile = sf_open(file_path, SFM_READ, &sfinfo);
  if (!sndfile) {
    fprintf(stderr, "Could not open file: %s -> %s\n", file_path,
            sf_strerror(NULL));
    return false;
  }

  adc->channels = sfinfo.channels;
  adc->SR = sfinfo.samplerate;
  adc->format = sfinfo.format;
  adc->samples = sfinfo.frames * sfinfo.channels;
  adc->bytes = adc->samples * sizeof(float);

  if (adc->channels != 2) {
    fprintf(stderr, "Must be a two channel audio file!\n");
    return false;
  }

  fprintf(stdout, "SAMPLE RATE %d\n", adc->SR);
  fprintf(stdout, "BYTES %zu\n", adc->bytes);
  fprintf(stdout, "SAMPLES %zu\n", adc->samples);

  adc->buffer = (float *)calloc(adc->samples, sizeof(float));
  if (!adc->buffer) {
    fprintf(stderr, "Could not allocate buffer! -> %s\n", strerror(errno));
    sf_close(sndfile);
    return false;
  }

  sf_count_t read = sf_read_float(sndfile, adc->buffer, adc->samples);
  if (read < 0) {
    fprintf(stderr, "Error reading audio data! ->%s\n", sf_strerror(sndfile));
    free(adc->buffer);
    sf_close(sndfile);
    return false;
  }

  adc->length = (uint32_t)(adc->samples);
  sf_close(sndfile);
  return true;
}

bool load_song(AudioDataContainer *adc) {
  if (!adc) {
    return false;
  }

  vis.next_song_flag = 0;
  vis.spec.userdata = adc;
  vis.spec.callback = callback;
  vis.spec.channels = adc->channels;
  vis.spec.freq = adc->SR;
  vis.spec.format = AUDIO_F32SYS;
  vis.spec.samples = M_BUF_SIZE / adc->channels;

  vis.dev = SDL_OpenAudioDevice(NULL, 0, &vis.spec, NULL, 1);
  if (!vis.dev) {
    return false;
  }

  if (!resume_device()) {
    return false;
  }

  vis.stream_flag = true;
  return true;
}

bool pause_device(void) {
  if (vis.dev) {
    if (SDL_GetAudioDeviceStatus(vis.dev) != SDL_AUDIO_PAUSED) {
      SDL_PauseAudioDevice(vis.dev, true);
      return true;
    }
  }

  return false;
}

bool resume_device(void) {
  if (vis.dev) {
    if (SDL_GetAudioDeviceStatus(vis.dev) != SDL_AUDIO_PLAYING) {
      SDL_PauseAudioDevice(vis.dev, false);
      return true;
    }
  }

  return false;
}
