#include "main.h"
#include <assert.h>

void render_draw_search_text(Text* text){
  if(text){
    text->rect.x = 50;
    text->rect.y = 50;
    scc(SDL_RenderCopy(rend.r, text->tex[0], NULL, &text->rect));
  }
}


void render_draw_text(TextBuffer *list_buf, const size_t *item_count,
                      const size_t *cursor) {

  if (!list_buf || !item_count || !cursor || !rend.r) {
    return;
  }

  size_t iter_count = rend.title_limit;
  if (iter_count > *item_count) {
    iter_count = *item_count;
  }

  const int iter_divisor = iter_count + 1;

  int pixel_inc = win.height / iter_divisor;
  int p_accumulate = pixel_inc * (iter_divisor / 2);
  int p_max = pixel_inc * iter_divisor;

  int last_locn = 0;
  int line_flag = 0;

  for (size_t i = 0; i < iter_count; i++) {
    // wrap around
    size_t locn = (*cursor + i) % *item_count;
    // The final pixel accumulation will always go over the conditional check
    // number at the last iteration, so this check is at the start of the loop.
    // That way, when the increment goes over and there are still remaining
    // elements, then and only then does this conditional check and assignments
    // execute
    if (list_buf[locn].text) {
      if (p_accumulate >= p_max) {
        p_accumulate = pixel_inc;
        line_flag = 1;
      }

      SDL_Texture *font_tex_ptr = NULL;
      if (i == 0) {
        font_tex_ptr = list_buf[locn].text->tex[1];
      } else {
        font_tex_ptr = list_buf[locn].text->tex[0];
      }

      list_buf[locn].text->rect.x =
          (win.width * 0.5) - (list_buf[locn].text->rect.w * 0.5);
      list_buf[locn].text->rect.y =
          p_accumulate - (list_buf[locn].text->rect.h * 0.5);

      scc(SDL_RenderCopy(rend.r, font_tex_ptr, NULL,
                         &list_buf[locn].text->rect));

      p_accumulate += pixel_inc;

      last_locn = locn;
    }
  }

  size_t bg_locn = (*cursor) % *item_count;
  if (list_buf[bg_locn].text) {
    SDL_Rect text_rect = list_buf[bg_locn].text->rect;
    SDL_Rect bg_rect = {text_rect.x - 5, text_rect.y - 5, text_rect.w + 10,
                        text_rect.h + 10};

    SDL_SetRenderDrawColor(rend.r, vis.secondary_bg.r, vis.secondary_bg.g,
                           vis.secondary_bg.b, vis.secondary_bg.a * 0.33);
    SDL_RenderFillRect(rend.r, &bg_rect);

    if (line_flag) {
      SDL_Rect last_rect = list_buf[last_locn].text->rect;
      const int distance = text_rect.y - last_rect.y;

      int line_x = (win.width * 0.5) - ((last_rect.w - 10) * 0.5);
      int line_y = text_rect.y - (distance * 0.5);
      int line_w = last_rect.w - 10;
      int line_h = 4;

      SDL_Rect line = {line_x, line_y + (line_h * 0.5), line_w, line_h};

      int line_x_shadow = (win.width * 0.5) - ((last_rect.w - 4) * 0.5);
      int line_y_shadow = text_rect.y - (distance * 0.5);
      int line_w_shadow = last_rect.w - 4;
      int line_h_shadow = 8;

      SDL_Rect line_shadow = {line_x_shadow, line_y_shadow, line_w_shadow,
                              line_h_shadow};

      SDL_SetRenderDrawColor(rend.r, vis.secondary.r, vis.secondary.g,
                             vis.secondary.b, vis.secondary.a * 0.125);
      SDL_RenderFillRect(rend.r, &line_shadow);

      SDL_SetRenderDrawColor(rend.r, vis.secondary.r, vis.secondary.g,
                             vis.secondary.b, vis.secondary.a * 0.75);
      SDL_RenderFillRect(rend.r, &line);
    }
  }
}
