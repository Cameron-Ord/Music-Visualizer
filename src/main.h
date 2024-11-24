#ifndef MAIN_H
#define MAIN_H

#include "audiodefs.h"
#include "filesysdefs.h"
#include "fontdef.h"
#include "particledef.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
  DIRECTORIES = 0,
  SONGS = 1,
  PLAYBACK = 2,
  SEARCHING_SONGS = 3,
  SEARCHING_DIRS = 4,
  DISPLAY_SEARCH = 5,
} USER_STATE;
typedef enum { FREQ_DOMAIN = 0, TIME_DOMAIN = 1 } DRAW_STATE;

#define WIN_W 600
#define WIN_H 400

typedef struct {
  // Audio devices
  SDL_AudioDeviceID dev;
  SDL_AudioSpec spec;

  // Boolean flags
  bool stream_flag;
  bool quit;

  // Integer flags
  int next_song_flag;
  int scrolling;
  int current_state;
  int last_state;
  int draw_state;

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
  size_t dir_list_index;
  size_t file_list_index;
  size_t file_cursor;
  size_t dir_cursor;
  size_t search_cursor;
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

int is_key(int input, int required);
void reset_input_buffer(size_t *pos, char *buf);
void set_state(int state);
void *scp(void *ptr);
int scc(int code);
int get_title_limit(int height);
int get_char_limit(int width);
const char *seek_path(TextBuffer *buf, const size_t *cursor, Paths *contents);
void fill_file_contents(Paths **file_contents, TextBuffer **file_text_buffer,
                        size_t *file_count, const char *path_str,
                        Paths *(*find_files)(size_t *, const char *));
void select_file(AudioDataContainer *adc, const char *path_str);

char *create_input_buffer(size_t *size);
// Font related functions
TextBuffer *create_fonts(const Paths *paths_buf, const size_t *count);
Text *create_search_text(const char *input_text_buffer,
                         const size_t *text_buf_len,
                         const size_t *input_buf_position);

void seek_forward(AudioDataContainer *adc);
void seek_backward(AudioDataContainer *adc);

// Render functions
void render_bg(void);
void render_draw_search_text(Text *text);
void render_draw_text(TextBuffer *list_buf, const size_t *item_count,
                      const size_t *cursor);
void render_clear(void);
void render_present(void);
void render_draw_music(const float *smear, const float *smoothed,
                       const float *windowed, const size_t *len,
                       ParticleTrio *p_buffer);
void KILL_PARTICLES(ParticleTrio *p_buffer, size_t size);

// Events functions
size_t nav_down(size_t *cursor, const size_t *count);
size_t nav_up(size_t *cursor, const size_t *count);
TextBuffer *font_swap_pointer(TextBuffer *buf, const size_t *count,
                              const Paths *content, TextBuffer *search_buffer,
                              const size_t *s_count);
void char_buf_insert(const char *text, char **input_buf, size_t *pos,
                     size_t *size, Text **search_text);
void window_resized(void);

#endif // MAIN_H
