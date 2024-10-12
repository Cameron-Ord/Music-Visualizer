#include "main.h"

void render_draw_text(Text **text_buf, const size_t *cursor,
                      const size_t *size) {

  int pixel_inc = win.height / *size + 1;
  int p_accumulate = pixel_inc;

  for (size_t i = 0; i < *size; i++) {
    text_buf[i]->rect.x = 25;
    text_buf[i]->rect.y = p_accumulate;
    p_accumulate += pixel_inc;

    SDL_RenderCopy(rend.r, text_buf[i]->tex, NULL, &text_buf[i]->rect);
  }
}
