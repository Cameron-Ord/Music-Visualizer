#ifndef MAIN_H
#define MAIN_H
#include "filesystem.h"
#include "fontdef.h"
#include "particledef.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum { DIRECTORIES = 0, SONGS = 1, PLAYBACK = 2 } USER_STATE;

#define FONT_PATH "dogicapixel.ttf"
#define DEFAULT_TABLE_SIZE 1024

#define WIN_W 600
#define WIN_H 400

typedef struct {
  SDL_AudioDeviceID dev;
  SDL_AudioSpec spec;
  bool stream_flag;
  int next_song_flag;
  int scrolling;
  bool quit;
  int current_state;
  SDL_Color primary;
  SDL_Color secondary;
  SDL_Color background;
  SDL_Color text;
  SDL_Color text_bg;
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
  size_t dir_list_index;
  size_t file_list_index;
  size_t file_cursor;
  size_t dir_cursor;
} Events;

typedef struct {
  TTF_Font *font;
  int char_limit;
  int size;
} Font;

typedef struct {
  TextBuffer *list;
  size_t max_len;
  size_t list_size;
  size_t *cursor;
  size_t *list_index;
} NavListArgs;

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
extern Events key;

// General/Utility functions
void *scp(void *ptr);
int scc(int code);
int get_title_limit(int height);
int get_char_limit(int width);

// Font related functions
TextBuffer *create_fonts(Paths *paths_buf, const size_t *count);

// Render functions
void render_bg(void);
void render_draw_text(TextBuffer *list_buf, const size_t *item_count,
                      const size_t *cursor);
void render_clear(void);
void render_present(void);
void render_draw_music(const float *smear, const float *smoothed,
                       const size_t *len, ParticleTrio *p_buffer);

// Events functions
size_t nav_down(size_t *cursor, const size_t *count);
size_t nav_up(size_t *cursor, const size_t *count);

#endif // MAIN_H
