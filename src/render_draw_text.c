#include "renderer.h"

void render_draw_text(SDL_Renderer *r, TextBuffer *buf, const int h,
                      const int w, const SDL_Color *sbg) {
  int vp_h = h * 0.5;
  SDL_Rect vp = {0, vp_h - (vp_h / 2), w, vp_h};
  SDL_RenderSetViewport(r, &vp);

  int y = TEXT_SPACING;
  const int x = TEXT_SPACING;
  int max_tw = 0;

  for (size_t i = buf->start; i < buf->size && y < vp_h; i++) {
    Text *t = buf[i].text;
    if (t->width > max_tw) {
      max_tw = t->width;
    }
    t->rect.x = (w - (w / 2)) - (t->width / 2), t->rect.y = y,
    t->rect.w = t->width, t->rect.h = t->height;
    y += t->rect.h + TEXT_SPACING;
  }

  SDL_SetRenderDrawColor(r, sbg->r, sbg->g, sbg->b, sbg->a);
  SDL_Rect bg = {(w - (w / 2)) - (max_tw - (max_tw / 2)), 0, max_tw + (x * 2),
                 y};
  bg.x -= TEXT_SPACING;
  SDL_RenderFillRect(r, &bg);
  // reset this variable to check for it again.
  y = TEXT_SPACING;
  for (size_t i = buf->start; i < buf->size && y < vp_h; i++) {
    Text *t = buf[i].text;

    if (i != buf->cursor) {
      SDL_RenderCopy(r, t->tex[0], NULL, &t->rect);
    } else {
      SDL_RenderCopy(r, t->tex[1], NULL, &t->rect);
    }

    y += t->rect.h + TEXT_SPACING;
  }
}
