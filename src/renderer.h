#ifndef RENDERER_H
#define RENDERER_H
#include "fontdef.h"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>

typedef struct {
  const float *smear;
  const float *smooth;
  const size_t *length;
} RenderArgs;

typedef struct {
  int y;
  int w;
} MaxValues;

// Render functions

MaxValues determine_max(const TextBuffer *t, const int h);
void render_draw_subbg(SDL_Renderer *r, const int w, const int h,
                       const SDL_Color *c, const MaxValues *m);
void render_draw_subg_outline(SDL_Renderer *r, const int w, const int h,
                              const SDL_Color *c, const MaxValues *m);
void render_seek_bar(const uint32_t *position, const uint32_t *length,
                     const int w, const SDL_Color *col, SDL_Renderer *r);
void render_bg(const SDL_Color *col, SDL_Renderer *r);
void render_draw_text(SDL_Renderer *r, TextBuffer *buf, const int h,
                      const int w, const MaxValues *m);
void render_clear(SDL_Renderer *r);
void render_present(SDL_Renderer *r);
void render_draw_music(RenderArgs *args, const int w, const int h,
                       SDL_Renderer *r, const SDL_Color *p, SDL_Color *s);

#endif
