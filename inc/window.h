#ifndef WINDOW_H
#define WINDOW_H
#include <stddef.h>

struct SDL_Window;
typedef struct SDL_Window SDL_Window;

struct Window {
  SDL_Window *w;
  int width, height;
  int x, y;
};

typedef struct Window Window;

const void *create_window(void);
Window *get_window(void);
void close_window(void);
void win_push_update(void);

#endif // WINDOW_H
