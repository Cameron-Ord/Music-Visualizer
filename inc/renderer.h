#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL_render.h>
#include <stddef.h>

struct TextBuffer;
typedef struct TextBuffer TextBuffer;

typedef struct {
  const float *smear;
  const float *smooth;
  const size_t *length;
} RenderArgs;

typedef struct {
  int max_tw;
  int height;
  int first_y;
  int last_y;
  int max_y;
  int last_iter;
} MaxValues;

// Render functions
void render_seek_bar(const uint32_t *position, const uint32_t *length,
                     const int w, const SDL_Color *col, SDL_Renderer *r);
void render_bg(const SDL_Color *col, SDL_Renderer *r);
void render_draw_text(SDL_Renderer *r, TextBuffer *buf, const int h,
                      const int w);
void render_clear(SDL_Renderer *r);
void render_present(SDL_Renderer *r);
void render_draw_music(RenderArgs *args, const int w, const int h,
                       SDL_Renderer *r, const SDL_Color *p, SDL_Color *s);

#endif
