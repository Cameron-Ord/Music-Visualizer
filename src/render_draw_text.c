#include "fontdef.h"
#include "renderer.h"

const int text_spacing = 32;
const int space = text_spacing - (text_spacing / 2);

static int center(const int pos, const int w) { return pos - (w / 2); }
static int one_16th(const int val) { return center(val, val) / 16; }
static int s(const int h) { return space + center(h, h); }

MaxValues determine_max(const TextBuffer *buf, const int h) {
  MaxValues m = {0};
  if (!buf)
    return m;

  const int init = one_16th(h);
  int y = center(init, buf->text->height) + s(buf->text->height);
  m.first_y = y;

  int max_tw = 0;
  size_t i = 0;

  for (i = buf->start; i < buf->size && y < h - init; i++) {
    Text *t = buf[i].text;
    if (t->width > max_tw) {
      max_tw = t->width;
    }

    if (y + (t->height + space) > h - init) {
      m.last_y = y, m.height = y - init, m.max_tw = max_tw, m.last_iter = i;
      return m;
    }

    y += s(t->height);
    m.last_iter = i;
  }

  m.last_y = y, m.height = y - init, m.max_tw = max_tw;
  return m;
}

void render_draw_text(SDL_Renderer *r, TextBuffer *buf, const int h,
                      const int w) {
  if (!buf)
    return;

  const int init = one_16th(h);
  int y = center(init, buf->text->height) + s(buf->text->height);

  for (size_t i = buf->start; i < buf->size; i++) {
    Text *t = buf[i].text;

    if (y + (t->height + space) > h - init)
      return;

    t->rect.x = one_16th(w), t->rect.y = y, t->rect.w = t->width,
    t->rect.h = t->height;

    if (i != buf->cursor) {
      SDL_RenderCopy(r, t->tex[0], NULL, &t->rect);
    } else {
      SDL_RenderCopy(r, t->tex[1], NULL, &t->rect);
    }

    y += s(t->height);
  }
}
