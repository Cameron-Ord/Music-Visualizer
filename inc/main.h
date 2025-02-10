#ifndef MAIN_H
#define MAIN_H

#define WIN_W 600
#define WIN_H 400

#include <stddef.h>

struct SDL_AudioSpec;
typedef struct SDL_AudioSpec SDL_AudioSpec;

struct SDL_Color;
typedef struct SDL_Color SDL_Color;

typedef struct {
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

void sdl_err(const char *msg);
void errno_string(const char *function, const char *msg);

const SDL_Color *_sec(void);
const SDL_Color *_text(void);
#endif // MAIN_H
