#include "../include/render_entity.hpp"

void SDL2Renderer::render_draw_bars(size_t *len, float *smear, float *smooth,
                                    const int *win_height, const int *win_width,
                                    SDL_Color *prim, SDL_Color *sec,
                                    SDL_Renderer *r) {
  int cell_width = *win_width / *len;
  int h = *win_height;

  for (size_t i = 0; i < *len; ++i) {
    float start = smear[i];
    float end = smooth[i];

    int end_x_pos = (i * (int)(cell_width + cell_width / 2));
    int end_y_pos = h - (end * h);
    int end_bar_height = end * h;
    SDL_Rect sample_end = {end_x_pos, end_y_pos, cell_width, end_bar_height};

    SDL_SetRenderDrawColor(r, prim->r, prim->g, prim->b, prim->a);
    SDL_RenderFillRect(r, &sample_end);

    int start_x_pos = (i * (int)(cell_width + cell_width / 2));
    int start_y_pos = h - (start * h);

    if (end_y_pos > start_y_pos) {
      SDL_Rect sample_start = {start_x_pos, start_y_pos, cell_width,
                               end_y_pos - start_y_pos};
      SDL_SetRenderDrawColor(r, sec->r, sec->g, sec->b, sec->a);
      SDL_RenderFillRect(r, &sample_start);
    }
  }
}
