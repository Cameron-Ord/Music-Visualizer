#ifndef MAIN_H
#define MAIN_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#define WIN_W 600
#define WIN_H 400

typedef struct {
  // Audio devices
  SDL_AudioDeviceID dev;
  SDL_AudioSpec *spec;

  // Flags
  int awaiting;
  int quit;

  // Application variables
  int target_frames;
  int smearing;
  int smoothing;
  char *home;

  // Colors
} Visualizer;

typedef struct {
  SDL_Color primary;
  SDL_Color secondary;
  SDL_Color background;
  SDL_Color secondary_bg;
  SDL_Color text;
} Colors;

typedef struct {
  SDL_Window *w;
  int width, height;
} Window;

typedef struct {
  SDL_Renderer *r;
} Renderer;

typedef struct {
  TTF_Font *font;
  int char_limit;
  int size;
} Font;

typedef struct {
  size_t p_node;
  size_t cur_node;
  size_t node_curs;
} NodeIdx;

#endif // MAIN_H
