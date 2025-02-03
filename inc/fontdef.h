#ifndef FONT_DEF
#define FONT_DEF
#define TEXT_SPACING 8
#define FONT_SIZE 16
#define TXTMAX 256

struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;

#include <SDL2/SDL_render.h>
#include <stddef.h>

typedef struct {
  TTF_Font *font;
  int char_limit;
  int size;
} Font;

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
  size_t size;
  size_t cursor;
  size_t start;
  size_t clamped_size;
  int max_text_height;
  int is_valid;
} IdxInfo;

struct TextBuffer {
  Text *text;
  IdxInfo info;
};

typedef struct TextBuffer TextBuffer;

#endif // FONT_DEF_H
