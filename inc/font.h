#ifndef FONT_H
#define FONT_H

struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;

#include <SDL2/SDL_render.h>
#include <stddef.h>

typedef struct {
  unsigned int id;
  unsigned char value;
  int w[2], h[2];
  SDL_Texture *texture[2];
} Character;

typedef struct {
  int size;
  TTF_Font *font;
} Font;

int _open_ttf_file(const char *path, const char *env);
int _fill_text_atlas(void);
const Character *atlas_lookup_char(unsigned int c);
#endif
