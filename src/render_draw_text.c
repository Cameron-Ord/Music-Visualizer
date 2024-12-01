#include "main.h"
#include <assert.h>

void render_draw_text(TextBuffer *buf) {
  int h = win.height;

  int y = TEXT_SPACING;
  const int x = TEXT_SPACING;

  buf->listed = buf->start;
  for (size_t i = buf->start; i < buf->size; i++) {
    Text *t = buf[i].text;

    int text_h = t->height;
    int text_w = t->width;

    SDL_Rect text_rect = {x, y, text_w, text_h};
    y += text_h + TEXT_SPACING;

    if (y >= h) {
      return;
    }

    if (i != buf->cursor) {
      SDL_RenderCopy(rend.r, t->tex[0], NULL, &text_rect);
    } else {
      SDL_RenderCopy(rend.r, t->tex[1], NULL, &text_rect);
    }

    buf->listed++;
  }
}
