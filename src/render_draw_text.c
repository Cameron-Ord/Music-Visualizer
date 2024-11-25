#include "main.h"
#include <assert.h>

void render_draw_search_text(Text *text) {
  if (text) {
    text->rect.x = (win.width * 0.5) - (text->rect.w * 0.5);
    text->rect.y = (win.height * 0.5) - (text->rect.h * 0.5);
    scc(SDL_RenderCopy(rend.r, text->tex[0], NULL, &text->rect));
  }
}

void render_draw_text(TextBuffer *buf) {
  int h = win.height;
  int spacing = 4;

  int y = spacing;
  const int x = spacing;

  buf->listed = 0;
  for (size_t i = buf->start; i < buf->size; i++) {
    Text *t = buf[i].text;

    int text_h = t->height;
    int text_w = t->width;

    SDL_Rect text_rect = {x, y, text_w, text_h};
    y += text_h + spacing;

    if (y >= h) {
      return;
    }

    if (i != buf->cursor) {
      SDL_RenderCopy(rend.r, t->tex[1], NULL, &text_rect);
    } else {
      SDL_RenderCopy(rend.r, t->tex[0], NULL, &text_rect);
    }

    buf->listed++;
  }
}
