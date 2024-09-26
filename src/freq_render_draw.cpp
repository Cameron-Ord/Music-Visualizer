#include "../include/render_entity.hpp"

void SDL2Renderer::render_set_bars(const size_t *len, const int *win_height,
                                   const int *win_width, float *smear,
                                   float *smooth) {
    const int cell_width = *win_width / *len;
    const int h = *win_height;

    if (bar_end_coords.size() < (*len - 1)) {
        bar_end_coords.resize(*len);
    }

    if (bar_start_coords.size() < (*len - 1)) {
        bar_start_coords.resize(*len);
    }

    for (size_t i = 0; i < *len; ++i) {
        const float start = smear[i];
        const float end = smooth[i];

        const int space = cell_width + cell_width / 2;

        const int end_x_pos = (i * space);
        const int end_y_pos = h - (end * h);
        const int end_bar_height = end * h;

        SDL_Rect sample_end = { end_x_pos, end_y_pos, cell_width,
                                end_bar_height };

        Coordinates end_positions = { .x = end_x_pos,
                                      .y = end_y_pos,
                                      .height = end_bar_height,
                                      .copy_rect = sample_end };

        const int start_x_pos = (i * space);
        const int start_y_pos = h - (start * h);
        const int start_bar_height = end_y_pos - start_y_pos;

        SDL_Rect sample_start = { start_x_pos, start_y_pos, cell_width,
                                  start_bar_height };

        Coordinates start_positions = { .x = start_x_pos,
                                        .y = start_y_pos,
                                        .height = start_bar_height,
                                        .copy_rect = sample_start };

        bar_start_coords[i] = start_positions;
        bar_end_coords[i] = end_positions;
    }
}

void SDL2Renderer::render_draw_bars(size_t *len, SDL_Color *prim,
                                    SDL_Color *sec, SDL_Renderer *r) {
    for (size_t i = 0; i < *len; ++i) {
        SDL_SetRenderDrawColor(r, prim->r, prim->g, prim->b, prim->a);
        SDL_RenderFillRect(r, &bar_end_coords[i].copy_rect);

        if (bar_end_coords[i].y > bar_start_coords[i].y) {
            SDL_SetRenderDrawColor(r, sec->r, sec->g, sec->b, sec->a);
            SDL_RenderFillRect(r, &bar_start_coords[i].copy_rect);
        }
    }
}
