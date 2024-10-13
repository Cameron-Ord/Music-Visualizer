#include "audio.h"
#include "SDL2/SDL_audio.h"
#include "audiodefs.h"
#include "main.h"
#include <sndfile.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void callback(void *userdata, uint8_t *stream, int length){
  AudioDataContainer *adc = (AudioDataContainer*)userdata;

  const uint32_t *file_length = &adc->length;
  uint32_t *pos = &adc->position;

  const uint32_t uint32_len = (uint32_t)length;
  const uint32_t samples = uint32_len / sizeof(float);
  const uint32_t remaining = (*file_length - *pos);

  const uint32_t copy = (samples < remaining) ? samples : remaining;

  float *f32_stream =(float *)stream;
  for (uint32_t i = 0; i < copy; i++) {
    f32_stream[i] = adc->buffer[i + *pos] * adc->volume;
  }

  *pos += copy;
}


bool read_audio_file(char* file_path, AudioDataContainer* adc) {

  SNDFILE *sndfile = NULL;
  SF_INFO sfinfo;

  memset(&sfinfo, 0, sizeof(SF_INFO));

  fprintf(stdout, "Reading file -> %s\n", file_path);

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

  fprintf(stdout, "SAMPLE RATE %d\n", adc->SR);
  fprintf(stdout, "BYTES %zu\n", adc->bytes);
  fprintf(stdout, "SAMPLES %zu\n", adc->samples);

  adc->buffer = (float *)realloc(adc->buffer, adc->bytes);
  if (!adc->buffer) {
    fprintf(stderr, "Could not allocate buffer! -> %s\n", strerror(errno));
    sf_close(sndfile);
    return false;
  }

  memset(adc->buffer, 0, adc->bytes);

  sf_count_t read = sf_read_float(sndfile, adc->buffer, adc->samples);
  if (read < 0) {
    fprintf(stderr, "Error reading audio data! ->%s\n", sf_strerror(sndfile));
    free(adc->buffer);
    sf_close(sndfile);
    return false;
  }

  adc->length = (uint32_t)(adc->samples);
  adc->volume = 1.0f;
  sf_close(sndfile);
  return true;
}


bool load_song(AudioDataContainer *adc){
    if(SDL_GetAudioDeviceStatus(vis.dev) != SDL_AUDIO_STOPPED){
        SDL_CloseAudioDevice(vis.dev);
    }

    vis.spec.userdata = adc;
    vis.spec.callback = callback;
    vis.spec.channels = adc->channels;
    vis.spec.freq = adc->SR;
    vis.spec.format = AUDIO_F32;
    vis.spec.samples = M_BUF_SIZE;

    vis.dev = SDL_OpenAudioDevice(NULL, 0, &vis.spec, NULL, 0);
    if(!vis.dev){
        return false;
    }
    
    if(!resume_device()){
        return false;
    }

    return true;
}

bool pause_device(){
    if(SDL_GetAudioDeviceStatus(vis.dev) != SDL_AUDIO_PAUSED){
        SDL_PauseAudioDevice(vis.dev, true);
        vis.stream_flag = false;
        return true;
    }

    return false;
}


bool resume_device(){
    if(SDL_GetAudioDeviceStatus(vis.dev) != SDL_AUDIO_PLAYING){
        SDL_PauseAudioDevice(vis.dev, false);
        vis.stream_flag = true;
        return true;
    }

    return false;
}
