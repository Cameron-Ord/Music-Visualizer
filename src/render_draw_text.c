#include "fontdef.h"
#include "renderer.h"

#define H_COEFF 0.5

MaxValues determine_max(const TextBuffer *t, const int h) {
  MaxValues m;
  int y = TEXT_SPACING;
  int max_tw = 0;

  const int _h = h * H_COEFF;

  for (size_t i = t->start; i < t->size && y < _h; i++) {
    Text *txt = t[i].text;
    if (txt->width > max_tw) {
      max_tw = txt->width;
    }

    if (y + (txt->height + TEXT_SPACING) < _h)
      y += txt->height + TEXT_SPACING;
  }

  m.y = y, m.w = max_tw;
  return m;
}

void render_draw_subbg(SDL_Renderer *r, const int w, const int h,
                       const SDL_Color *c, const MaxValues *m) {
  int vp_h = h * H_COEFF;
  SDL_Rect vp = {0, vp_h - (vp_h / 2), w, vp_h};
  SDL_RenderSetViewport(r, &vp);

  const int padding = 2;

  const int x = (w - (w / 2)) - (m->w / 2);
  SDL_Rect bg = {x - (padding / 2), TEXT_SPACING - (TEXT_SPACING / 2),
                 m->w + padding, m->y - TEXT_SPACING};

  SDL_SetRenderDrawColor(r, c->r, c->g, c->b, c->a);
  SDL_RenderFillRect(r, &bg);
}

void render_draw_subg_outline(SDL_Renderer *r, const int w, const int h,
                              const SDL_Color *c, const MaxValues *m) {
  int vp_h = h * H_COEFF;
  SDL_Rect vp = {0, vp_h - (vp_h / 2), w, vp_h};
  SDL_RenderSetViewport(r, &vp);

  const int padding = 8;

  const int x = (w - (w / 2)) - (m->w / 2);
  SDL_Rect bg = {x - (padding / 2), 0, m->w + padding, m->y};

  SDL_SetRenderDrawColor(r, c->r, c->g, c->b, c->a);
  SDL_RenderFillRect(r, &bg);
}

void render_draw_text(SDL_Renderer *r, TextBuffer *buf, const int h,
                      const int w, const MaxValues *m) {
  int vp_h = h * H_COEFF;
  SDL_Rect vp = {0, vp_h - (vp_h / 2), w, vp_h};
  SDL_RenderSetViewport(r, &vp);

  int y = TEXT_SPACING;
  for (size_t i = buf->start; i < buf->size && y < m->y; i++) {
    Text *t = buf[i].text;

    t->rect.x = (w - (w / 2)) - (t->width / 2), t->rect.y = y,
    t->rect.w = t->width, t->rect.h = t->height;

    y += t->height + TEXT_SPACING;

    if (i != buf->cursor) {
      SDL_RenderCopy(r, t->tex[0], NULL, &t->rect);
    } else {
      SDL_RenderCopy(r, t->tex[1], NULL, &t->rect);
    }
  }
}
