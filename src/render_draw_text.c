#include "main.h"

void render_draw_text(TextBuffer *list_buf, const size_t *cursor,
                      const size_t *size) {

  int pixel_inc = win.height / (list_buf->size + 1);
  int p_accumulate = pixel_inc;

  for (size_t i = 0; i < list_buf->size; i++) {
    list_buf->buf[i]->rect.x = 25;
    list_buf->buf[i]->rect.y = p_accumulate;
    p_accumulate += pixel_inc;

    if(i == *cursor){
      SDL_Rect bg = {list_buf->buf[i]->rect.x - 5, list_buf->buf[i]->rect.y - 5, list_buf->buf[i]->rect.w + 10, list_buf->buf[i]->rect.h + 10};
      SDL_SetRenderDrawColor(rend.r, vis.text_bg.r, vis.text_bg.g, vis.text_bg.b, vis.text_bg.a * 0.33);
      SDL_RenderFillRect(rend.r, &bg);
    }

    SDL_RenderCopy(rend.r, list_buf->buf[i]->tex, NULL, &list_buf->buf[i]->rect);
  }
}
