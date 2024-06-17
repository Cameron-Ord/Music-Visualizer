#include "../inc/init.h"
#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/music_visualizer.h"

int
initialize_SDL() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return -1;
  }
  return 0;
}

int
create_window(SDL_Window** w) {
  *w = SDL_CreateWindow("Music Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, BWIDTH, BHEIGHT,
                        0);
  if (!*w) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return -1;
  }
  SDL_SetWindowResizable(*w, SDL_TRUE);
  SDL_SetWindowMinimumSize(*w, BWIDTH, BHEIGHT);
  return 0;
}

int
create_renderer(SDL_Window** w, SDL_Renderer** r) {
  if (*w) {
    *r = SDL_CreateRenderer(*w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!*r) {
      PRINT_SDL_ERR(stderr, SDL_GetError());
      SDL_DestroyWindow(*w);
      return -1;
    }
    return 0;
  }
  return -1;
}

SDL_AudioDeviceID
create_audio_device(SDL_AudioSpec* spec) {
  SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, spec, NULL, 0);
  if (!dev) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return -1;
  }
  return dev;
}

int
initialize_TTF() {
  if (TTF_Init() < 0) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return -1;
  }
  return 0;
}

int
open_font(TTFData* cntxtdata) {
  cntxtdata->font = TTF_OpenFont(FONT_PATH, cntxtdata->font_size);
  if (cntxtdata->font == NULL) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return -1;
  }
  return 0;
}

int
initialize_sdl_image() {
  if (IMG_Init(IMG_INIT_PNG) != 2) {
    return 1;
  }

  return 0;
}

void
get_window_container_size(SDL_Window* w, SDLContainer* SDLCnt) {
  SDL_GetWindowSize(w, &SDLCnt->win_width, &SDLCnt->win_height);
}

void
instantiate_buffers(FTransformBuffers* bufs) {
  memset(bufs->out_raw_prim, 0, DOUBLE_BUFF * sizeof(f32c));
  memset(bufs->processed_prim, 0, (DOUBLE_BUFF) * sizeof(f32));
  memset(bufs->smoothed_prim, 0, (DOUBLE_BUFF) * sizeof(f32));
  memset(bufs->fft_in_prim, 0, (DOUBLE_BUFF) * sizeof(f32));
  memset(bufs->fft_in_sec, 0, (DOUBLE_BUFF) * sizeof(f32));
}
