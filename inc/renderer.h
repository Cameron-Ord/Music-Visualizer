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

void render_bg(const SDL_Color *col);
void render_clear(void);
void render_present(void);
void render_node_text(const Paths *p);
#endif
