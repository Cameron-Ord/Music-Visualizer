#ifndef MAIN_H
#define MAIN_H

#include "audiodefs.h"
#include "filesysdefs.h"
#include "fontdef.h"
#include "particledef.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdint.h>

#define WIN_W 600
#define WIN_H 400

typedef struct {
  // Audio devices
  SDL_AudioDeviceID dev;
  SDL_AudioSpec spec;

  // Flags
  int awaiting;
  int quit;

  // Application variables
  int target_frames;
  int smearing;
  int smoothing;
  char *home;

  // Colors
  SDL_Color primary;
  SDL_Color secondary;
  SDL_Color background;
  SDL_Color secondary_bg;
  SDL_Color text;
} Visualizer;

typedef struct {
  SDL_Window *w;
  int width, height;
} Window;

typedef struct {
  SDL_Renderer *r;
  size_t draw_amount;
} Renderer;

typedef struct {
  TTF_Font *font;
  int char_limit;
  int size;
} Font;

// Convienence generic pointer struct for passing pointer arguments
// Just make sure you type cast correctly if this is used.
typedef struct {
  void *arg1;
  void *arg2;
  void *arg3;
  void *arg4;
} VoidPtrArgs;

typedef struct {
  const float *smear;
  const float *smooth;
  const size_t *length;
} RenderArgs;

// Global structures
extern Font font;
extern Renderer rend;
extern Window win;
extern Visualizer vis;

// General/Utility functions
int open_ttf_file(const char *filename);
// Font related functions
TextBuffer *create_fonts(const Paths *paths_buf);
// Render functions
void render_seek_bar(const uint32_t *position, const uint32_t *length);
void render_bg(void);
void render_draw_text(TextBuffer *list_buf);
void render_clear(void);
void render_present(void);
void render_draw_music(RenderArgs *args);
// Events functions
int node_index(const char *direction, int node_index, int max);
void nav_down(TextBuffer *tbuf);
void nav_up(TextBuffer *tbuf);
void auto_play_nav(const size_t size, size_t *cursor);
void window_resized(void);
#endif // MAIN_H

