#ifndef AUDIO_H
#define AUDIO_H

#include "audiodefs.h"
#include <SDL2/SDL_audio.h>
#include <stdint.h>
#include <stdbool.h>

//Audio functions
void callback(void* userdata, uint8_t *stream, int length);
bool read_audio_file(char* file_path, AudioDataContainer* adc);
bool load_song(AudioDataContainer *adc);
bool pause_device(void);
bool resume_device(void);

#endif

