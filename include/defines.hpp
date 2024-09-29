#ifndef DEFINES_HPP
#define DEFINES_HPP
#include "macdefs.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include <vector>
#include <filesystem>
#include <complex>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <stdbool.h>
#include <iostream>

class SDL2INTERNAL;
class SDL2Audio;
class SDL2Renderer;
class SDL2Window;
class SDL2KeyInputs;
class SDL2Fonts;
class AudioData;
class ProgramPath;
class ProgramFiles;
class FourierTransform;
class ProgramThemes;

struct USERDATA {
    class SDL2Audio *sdl2_ad;
    class AudioData *ad;
    class FourierTransform *fft;
};

struct StdClassWrapper {
    AudioData *ad;
    ProgramPath *pathing;
    ProgramFiles *files;
    FourierTransform *fft;
    ProgramThemes *themes;
};

struct SDL2Wrapper {
    SDL2INTERNAL *sdl2;
    SDL2Audio *sdl2_ad;
    SDL2Renderer *rend;
    SDL2Window *win;
    SDL2KeyInputs *key;
    SDL2Fonts *fonts;
};

struct WIN_SIZE {
    int WIDTH;
    int HEIGHT;
};

typedef enum {
    AT_DIRECTORIES = 0,
    AT_SONGS = 1,
    LISTENING = 2,
} USER_STATE;

typedef enum {
    Q = SDLK_q,
    UP = SDLK_UP,
    DOWN = SDLK_DOWN,
    LEFT = SDLK_LEFT,
    RIGHT = SDLK_RIGHT,
    ENTER = SDLK_RETURN,
    BACKSPACE = SDLK_BACKSPACE,
    SPACE = SDLK_SPACE,
    ESCAPE = SDLK_ESCAPE,
    P_KEY = SDLK_p,
} SDL_KEYCODES_ENUM;

typedef enum { STOPPED = 0, PLAYING = 1, PAUSED = 2 } STREAM_FLAGS;
typedef enum { WAITING = 0, NEXT = 1 } PB_FLAGS;

void callback(void *data, uint8_t *stream, int len);

struct FBuffers {
    float fft_in[DOUBLE_BUFF];
    float in_cpy[DOUBLE_BUFF];
    float pre_raw[BUFF_SIZE];
    std::complex<float> out_raw[BUFF_SIZE];
    std::complex<float> post_raw[BUFF_SIZE];
    float extracted[BUFF_SIZE];
    float processed[HALF_BUFF];
    float smoothed[HALF_BUFF];
    float smear[HALF_BUFF];
};

struct FData {
    int cell_width;
    size_t output_len;
    float max_ampl;
    SDL_Rect *rect_buff;
    float hamming_values[BUFF_SIZE];
};

struct AudioDataContainer {
    float *buffer;
    uint32_t length;
    uint32_t position;
    size_t samples;
    size_t bytes;
    int channels;
    int SR;
    int format;
    float volume;
};

struct Icon {
    std::string file_name;
    SDL_Texture *texture;
    SDL_Surface *surface;
    int is_valid;
};

struct Directory {
    std::string directory_name;
    size_t directory_id;
};

struct Files {
    std::string file_name;
    size_t file_id;
};

struct Text {
    SDL_Texture *tex;
    SDL_Surface *surf;
    SDL_Rect rect;
    int width;
    int height;
    size_t id;
    bool is_valid;
    std::string name;
};

typedef Icon Icon;
typedef Directory Directory;
typedef Files Files;
typedef Text Text;
typedef USERDATA USERDATA;

#endif