#include "../inc/fontdef.h"
#include "../inc/renderer.h"

const int text_spacing = 32;

static int center(const int pos, const int w) { return pos - (w / 2); }
static int one_16th(const int val, const int item) {
  return center(val / 16, item);
}
static int one_half(const int val, const int item) {
  return center(val / 2, item);
}
static int s(const int h) {
  return center(text_spacing, text_spacing) + center(h, h);
}

static int clamp_max(const int max, const int idx) {
  if (idx > max - 1) {
    return max - 1;
  }

  if (idx < 0) {
    return 0;
  }

  return idx;
}

static int calculate_elements(const int h, const int curs, const int end,
                              const int text_height) {
  // Specify constraints
  const int offset = h / 2;
  const int bottom = one_16th(h, text_height);
  const int total_h = h - (offset + bottom);
  // Get total space occupied by a single font texture printed to screen
  const int used_space = s(text_height);
  // Get the remaining indexes
  const int remaining = (end - curs) + 1;
  // Divide and return the number of elements that will fit. clamping to the
  // remaining size
  return clamp_max(remaining, total_h / used_space);
}

void render_draw_text(SDL_Renderer *r, TextBuffer *buf, const int h,
                      const int w) {
  if (!buf)
    return;

  const size_t size = buf->info.size;
  const size_t curs = buf->info.cursor;
  const int th = buf->info.max_text_height;

  buf->info.clamped_size = calculate_elements(h, curs, size, th);

  const int used_space = s(buf->info.max_text_height);
  const int centered_start = one_half(h, used_space);
  int y = centered_start;

  const size_t end = buf->info.clamped_size;
  for (size_t i = 0; i < end; i++) {
    const size_t locn = (curs + i) % size;

    Text *t = buf[locn].text;
    t->rect.x = one_half(w, t->width), t->rect.y = y, t->rect.w = t->width,
    t->rect.h = t->height;

    if (locn != curs) {
      SDL_RenderCopy(r, t->tex[0], NULL, &t->rect);
    } else {
      SDL_RenderCopy(r, t->tex[1], NULL, &t->rect);
    }

    y += s(buf->info.max_text_height);
  }
}
