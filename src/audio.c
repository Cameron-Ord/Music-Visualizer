#include "audio.h"
#include "SDL2/SDL_audio.h"
#include "audiodefs.h"
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

    if (adc->position >= file_length) {
      pause_device();
    }

    if (stream && vis.dev) {
      float *f32_stream = (float *)stream;
      if (f32_stream) {
        for (uint32_t i = 0; i < copy; i++) {
          f32_stream[i] = adc->buffer[i + adc->position] * 1.0f;
        }
      }

      adc->position += copy;
    }

    if (adc->position >= file_length) {
      pause_device();
    }

    if ((adc->position + copy) < file_length) {
      fft_push(&adc->position, adc->next->fft_in, adc->buffer,
               copy * sizeof(float));
    }
  }
}

void fft_push(const uint32_t *pos, float *in, float *buffer, size_t bytes) {
  if (buffer && in) {
    memcpy(in, buffer + *pos, bytes);
  }
}

// https://github.com/tsoding/musializer/blob/master/src/plug.c#L268
void recursive_fft(float *in, size_t stride, Float_Complex *out, size_t n) {
  if (n == 1) {
    out[0] = cfromreal(in[0]);
    return;
  }

  size_t half_n = n / 2;

  recursive_fft(in, stride * 2, out, half_n);
  recursive_fft(in + stride, stride * 2, out + half_n, half_n);
  // v = o*x
  // out = e - o*x e + o*x e e| e + o*x o - o*x o o
  for (size_t k = 0; k < n / 2; ++k) {
    float t = (float)k / n;
    Float_Complex v = mulcc(cexpf(cfromimag(-2 * M_PI * t)), out[k + n / 2]);
    Float_Complex e = out[k];
    out[k] = addcc(e, v);
    out[k + n / 2] = subcc(e, v);
  }

} /*fft_func*/

void extract_frequencies(FFTBuffers *bufs) {
  for (int i = 0; i < S_BUF_SIZE; ++i) {
    float real = crealf(bufs->out_raw[i]);
    float imag = cimagf(bufs->out_raw[i]);
    bufs->extracted[i] = sqrt(real * real + imag * imag);
    bufs->phases[i] = atan2(imag, real);
  }
}

void visual_refine(FFTBuffers *bufs, FFTData *data) {
  const int FPS = 60;
  for (size_t i = 0; i < data->output_len; ++i) {
    bufs->processed_phases[i] /= data->max_phase;
    bufs->processed[i] /= data->max_ampl;

    bufs->smoothed[i] +=
        (bufs->processed[i] - bufs->smoothed[i]) * 7 * (1.0 / FPS);

    bufs->smear[i] += (bufs->smoothed[i] - bufs->smear[i]) * 6 * (1.0 / FPS);
  }
}

float amp(float z) {
  if (z == 0.0f) {
    return 0.0f;
  }
  return logf(z);
}

void freq_bin_algo(int sr, float *extracted) {
  float freq_bin_size = (float)sr / S_BUF_SIZE;
  const size_t buf_size = 3;
  float bin_sums[buf_size];

  float low_cutoffs[] = {60.0f, 250.0f, 2000.0f};
  float high_cutoffs[] = {250.0f, 2000.0f, 5000.0f};

  int low_bin;
  int high_bin;

  for (size_t filter_index = 0; filter_index < buf_size; filter_index++) {
    low_bin = low_cutoffs[filter_index] / freq_bin_size;
    high_bin = high_cutoffs[filter_index] / freq_bin_size;

    bin_sums[filter_index] = 0.0f;

    for (int i = low_bin; i < high_bin; i++) {
      bin_sums[filter_index] += extracted[i];
    }

    // average it out.
    const int iter_count = high_bin - low_bin;
    bin_sums[filter_index] /= iter_count;
  }

  float max_bin_value = bin_sums[0];
  float min_bin_value = bin_sums[0];

  int max_bin_index = 0;
  int min_bin_index = 0;

  for (size_t filter_index = 0; filter_index < buf_size; filter_index++) {
    if (bin_sums[filter_index] > max_bin_value) {
      max_bin_value = bin_sums[filter_index];
      max_bin_index = filter_index;
      continue;
    }

    if (bin_sums[filter_index] < min_bin_value) {
      min_bin_value = bin_sums[filter_index];
      min_bin_index = filter_index;
    }
  }

  low_bin = low_cutoffs[max_bin_index] / freq_bin_size;
  high_bin = high_cutoffs[max_bin_index] / freq_bin_size;

  for (int i = low_bin; i < high_bin; i++) {
    extracted[i] *= 1.25;
  }

  low_bin = low_cutoffs[min_bin_index] / freq_bin_size;
  high_bin = high_cutoffs[min_bin_index] / freq_bin_size;

  for (int i = low_bin; i < high_bin; i++) {
    extracted[i] *= 0.75;
  }
}

void squash_to_log(FFTBuffers *bufs, FFTData *data) {
  float step = 1.06f;
  float lowf = 1.0f;
  size_t m = 0;
  size_t y = 0;

  data->max_ampl = 1.0f;
  data->max_phase = 1.0f;

  for (float f = lowf; (size_t)f < M_BUF_SIZE; f = ceilf(f * step)) {
    float fs = ceilf(f * step);
    float a = 0.0f;
    float p = 0.0f;

    for (size_t q = (size_t)f; q < M_BUF_SIZE && q < (size_t)fs; ++q) {
      float b = amp(bufs->extracted[q]);
      if (b > a) {
        a = b;
      }

      float ph = bufs->phases[q];
      if (ph > p) {
        p = ph;
      }
    }

    if (data->max_ampl < a) {
      data->max_ampl = a;
    }

    if (data->max_phase < p) {
      data->max_phase = p;
    }

    bufs->processed[m++] = a;
    bufs->processed_phases[y++] = p;
  }

  data->output_len = m;
}

void hamming_window(float *in, const float *hamming_values, float *windowed) {
  /*Iterate for the size of a single channel*/
  for (int i = 0; i < M_BUF_SIZE; ++i) {
    int left = i * 2;
    int right = i * 2 + 1;

    windowed[left] = in[left] * hamming_values[left];
    windowed[right] *= in[right] * hamming_values[right];
  }
}

void calculate_window(float *hamming_values) {
  for (int i = 0; i < S_BUF_SIZE; ++i) {
    float Nf = (float)S_BUF_SIZE;
    float t = (float)i / (Nf - 1);
    hamming_values[i] = 0.54 - 0.46 * cosf(2 * M_PI * t);
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
    memset(adc->next->extracted, 0, sizeof(float) * S_BUF_SIZE);
    memset(adc->next->fft_in, 0, sizeof(float) * S_BUF_SIZE);
    memset(adc->next->out_raw, 0, sizeof(Float_Complex) * S_BUF_SIZE);
    memset(adc->next->phases, 0, sizeof(float) * S_BUF_SIZE);
    memset(adc->next->processed_phases, 0, sizeof(float) * M_BUF_SIZE);
    memset(adc->next->processed, 0, sizeof(float) * M_BUF_SIZE);
    memset(adc->next->smear, 0, sizeof(float) * M_BUF_SIZE);
    memset(adc->next->smoothed, 0, sizeof(float) * M_BUF_SIZE);
    memset(adc->next->windowed, 0, sizeof(float) * S_BUF_SIZE);
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

  vis.spec.userdata = adc;
  vis.spec.callback = callback;
  vis.spec.channels = adc->channels;
  vis.spec.freq = adc->SR;
  vis.spec.format = AUDIO_F32SYS;
  vis.spec.samples = M_BUF_SIZE;

  vis.dev = SDL_OpenAudioDevice(NULL, 0, &vis.spec, NULL, 1);
  if (!vis.dev) {
    return false;
  }

  if (!resume_device()) {
    return false;
  }

  return true;
}

bool pause_device(void) {
  if (vis.dev) {
    if (SDL_GetAudioDeviceStatus(vis.dev) != SDL_AUDIO_PAUSED) {
      SDL_PauseAudioDevice(vis.dev, true);
      vis.stream_flag = false;
      return true;
    }
  }

  return false;
}

bool resume_device(void) {
  if (vis.dev) {

    if (SDL_GetAudioDeviceStatus(vis.dev) != SDL_AUDIO_PLAYING) {
      SDL_PauseAudioDevice(vis.dev, false);
      vis.stream_flag = true;
      return true;
    }
  }

  return false;
}
