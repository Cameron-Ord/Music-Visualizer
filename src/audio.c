#include "../inc/audio.h"
#include "../inc/main.h"

#include <SDL2/SDL_audio.h>
#include <errno.h>
#include <sndfile.h>
#include <stdbool.h>

AudioData ad = {0};
SDL_AudioSpec spec = {0};
SDL_AudioDeviceID device = {0};
int device_needs_update = 1;

void callback(void *userdata, uint8_t *stream, int length);
const AudioData *get_ad(void);

const AudioData *get_ad(void) { return &ad; }

static unsigned int open_device(void) {
  device =
      SDL_OpenAudioDevice(NULL, 0, &spec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);
  if (!device) {
    sdl_err(SDL_GetError());
    return 0;
  }

  return 1;
}

static void close_device(void) { SDL_CloseAudioDevice(device); }

static void pause_device(void) {
  if (device) {
    if (SDL_GetAudioDeviceStatus(device) != SDL_AUDIO_PAUSED) {
      SDL_PauseAudioDevice(device, true);
    }
  }
}

static void resume_device(void) {
  if (device) {
    if (SDL_GetAudioDeviceStatus(device) != SDL_AUDIO_PLAYING) {
      SDL_PauseAudioDevice(device, false);
    }
  }
}

static void zero_values(void) {
  ad.bytes = 0;
  ad.channels = 0;
  ad.format = 0;
  ad.bytes = 0;
  ad.length = 0;
  ad.position = 0;
  ad.samples = 0;
  ad.SR = 0;
  ad.volume = 1.0;
}

static void set_spec(void) {
  spec.userdata = NULL;
  spec.callback = callback;
  spec.channels = ad.channels;
  spec.freq = ad.SR;
  spec.format = AUDIO_F32SYS;
  spec.samples = M_BUF_SIZE / ad.channels;
}

static int spec_cmp(void) {
  if (!spec.callback)
    return 1;

  if (spec.format != AUDIO_F32SYS)
    return 1;

  if (spec.channels != ad.channels)
    return 1;

  if (spec.freq != ad.SR)
    return 1;

  if (spec.samples != (M_BUF_SIZE) / ad.channels)
    return 1;

  return 0;
}

void callback(void *userdata, uint8_t *stream, int length) {

  if (ad.buffer) {
    const uint32_t uint32_len = (uint32_t)length;
    const uint32_t samples = uint32_len / sizeof(float);
    const uint32_t remaining = (ad.length - ad.position);
    const uint32_t copy = (samples < remaining) ? samples : remaining;

    if (ad.position >= ad.length) {
      pause_device();
    }

    if (stream) {
      float *f32_stream = (float *)stream;
      for (uint32_t i = 0; i < copy; i++) {
        if (i + ad.position < ad.length) {
          f32_stream[i] = ad.buffer[i + ad.position] * 1.0f;
        }
      }
      ad.position += copy;
    }

    if (ad.position >= ad.length) {
      pause_device();
    }

    const size_t bytes = copy * sizeof(float);
    if ((ad.position + copy) < ad.length) {
      _fft_push(&ad.position, ad.buffer, bytes);
    }
  }
}

static int read_audio_file(const char *file_path) {
  if (!file_path) {
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

  if (ad.buffer) {
    free(ad.buffer);
    ad.buffer = NULL;
  }

  zero_values();
  _zero_fft();

  ad.channels = sfinfo.channels;
  ad.SR = sfinfo.samplerate;
  ad.format = sfinfo.format;
  ad.samples = sfinfo.frames * sfinfo.channels;
  ad.bytes = ad.samples * sizeof(float);
  ad.length = (uint32_t)(ad.samples);

  if (device_needs_update == spec_cmp()) {
    set_spec();
  }

  ad.buffer = tmp;
  return 1;
}

int _file_read(const char *filepath) {
  if (!read_audio_file(filepath)) {
    return 0;
  };

  return 1;
}

int _start_device(void) {
  if (device_needs_update == 1) {
    if (device) {
      close_device();
    }

    if (!open_device()) {
      return 0;
    }
  }

  resume_device();
  return 1;
}

static int get_status(void) { return SDL_GetAudioDeviceStatus(device); }
int _get_status(void) { return get_status(); }

void _pause(void) { pause_device(); }
void _resume(void) { resume_device(); }
