#ifndef MAIN_H
#define MAIN_H

#include "audiodefs.h"
#include "filesysdefs.h"
#include "fontdef.h"
#include "particledef.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdint.h>

#define WIN_W 600
#define WIN_H 400

typedef struct {
  // Audio devices
  SDL_AudioDeviceID dev;
  SDL_AudioSpec spec;

  // Flags
  int stream_flag;
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
  SDL_Color foreground;
  SDL_Color tertiary;
} Visualizer;

typedef struct {
  SDL_Window *w;
  int width, height;
} Window;

typedef struct {
  SDL_Renderer *r;
  size_t title_limit;
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

// Global structures
extern Font font;
extern Renderer rend;
extern Window win;
extern Visualizer vis;

// General/Utility functions
int open_ttf_file(const char *filename);
void reset_input_buffer(size_t *pos, char *buf);
void set_state(int state);
void *scp(void *ptr);
int scc(int code);
int get_title_limit(int height);
int get_char_limit(int width);

char *create_input_buffer(size_t *size);
// Font related functions
TextBuffer *create_fonts(const Paths *paths_buf);
Text *create_search_text(const char *input_text_buffer,
                         const size_t *text_buf_len,
                         const size_t *input_buf_position);

// Render functions
void render_seek_bar(const uint32_t *position, const uint32_t *length);
void render_bg(void);
void render_draw_search_text(Text *text);
void render_draw_text(TextBuffer *list_buf);
void render_clear(void);
void render_present(void);
void render_draw_music(VoidPtrArgs *args, ParticleTrio *p_buffer);
void KILL_PARTICLES(ParticleTrio *p_buffer, size_t size);

// Events functions
int node_index(const char *direction, int node_index, int max);
size_t nav_down(TextBuffer *tbuf);
size_t nav_up(TextBuffer *tbuf);
void char_buf_insert(const char *text, char **input_buf, size_t *pos,
                     size_t *size, Text **search_text);
void window_resized(void);

#endif // MAIN_H
