#ifndef AUDIO_H
#define AUDIO_H
#include <SDL2/SDL_audio.h>
#include <stdint.h>

//Audio functions
void callback(void* userdata, uint8_t *stream, int length);

#endif

