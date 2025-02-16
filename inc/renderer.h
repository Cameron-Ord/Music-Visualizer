#ifndef RENDERER_H
#define RENDERER_H

#include <stddef.h>

struct SDL_Renderer;
typedef struct SDL_Renderer SDL_Renderer;

struct SDL_Color;
typedef struct SDL_Color SDL_Color;

struct Paths;
typedef struct Paths Paths;

struct Renderer {
  SDL_Renderer *r;
  int blendmode;
  int accelerated;
};

typedef struct Renderer Renderer;

const void *create_renderer(void);
Renderer *get_renderer(void);
void destroy_renderer(void);

void render_draw_music(const int w, const int h, SDL_Renderer *r,
                       const SDL_Color *p, const SDL_Color *s);
void render_seek_bar(const int w, const SDL_Color *col, SDL_Renderer *r);
void render_bg(const SDL_Color *col);
void render_clear(void);
void render_present(void);
void render_node_text(const Paths *p);
#endif
