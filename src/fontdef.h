#ifndef FONT_DEF
#define FONT_DEF
#define TEXT_SPACING 8
#define FONT_SIZE 16
#include <SDL2/SDL_render.h>

typedef struct {
  SDL_Texture *tex[2];
  SDL_Surface *surf[2];
  SDL_Rect rect;
  int width;
  int height;
  size_t id;
  int is_valid;
  char *name;
} Text;

typedef struct {
  Text *text;
  size_t size;
  size_t cursor;
  size_t start;
  size_t listed;
  int max;
  int is_valid;
} TextBuffer;

#endif // FONT_DEF_H
