#include "../include/render_entity.hpp"

void SDL2Renderer::render_set_bars(const size_t *len, const int *win_height,
                                   const int *win_width, float *smear,
                                   float *smooth) {
    const int cell_width = *win_width / *len;
    const int h = *win_height;

    SDL_Rect base_tile_rect = { 0, 0, cell_width, cell_width };

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
        const int end_x_pos = i * space;
        const int end_y_pos = h - static_cast<int>((end * h));
        int end_bar_height = static_cast<int>(end * h);

        float rect_amountf;
        int rect_amount_rounded;
        int accumulator;

        rect_amountf = static_cast<float>(end_bar_height) / cell_width;
        rect_amount_rounded = static_cast<int>(std::ceil(rect_amountf));

        Coordinates end_positions = { .x = end_x_pos,
                                      .y = end_y_pos,
                                      .height = end_bar_height };

        accumulator = 0;
        for (int j = 0; j < rect_amount_rounded; j++) {
            SDL_Rect tmp = { end_x_pos, end_y_pos + accumulator, cell_width,
                             cell_width };

            end_positions.copy_rect.push_back(tmp);
            accumulator += cell_width;
        }

        const int start_x_pos = (i * space);
        const int start_y_pos = h - static_cast<int>(start * h);
        const int start_bar_height = end_y_pos - start_y_pos;

        rect_amountf = static_cast<float>(start_bar_height) / cell_width;
        rect_amount_rounded = static_cast<int>(std::ceil(rect_amountf));

        Coordinates start_positions = { .x = start_x_pos,
                                        .y = start_y_pos,
                                        .height = start_bar_height };

        accumulator = 0;
        for (int j = 0; j < rect_amount_rounded; j++) {
            SDL_Rect tmp = { start_x_pos, start_y_pos + accumulator, cell_width,
                             cell_width };

            start_positions.copy_rect.push_back(tmp);
            accumulator += cell_width;
        }

        bar_end_coords[i] = end_positions;
        bar_start_coords[i] = start_positions;
    }
}

void SDL2Renderer::render_draw_bars(size_t *len, SDL_Color *prim,
                                    SDL_Color *sec, SDL_Renderer *r) {

    for (size_t i = 0; i < *len; ++i) {
        size_t end_size = bar_end_coords[i].copy_rect.size();
        for (size_t g = 0; g < end_size; g++) {
            SDL_Rect src;
            src = { 16, 16, 16, 16 };
            SDL_RenderCopy(r, sprite_texture, &src,
                           &bar_end_coords[i].copy_rect[g]);
        }
        if (bar_end_coords[i].y > bar_start_coords[i].y) {
            size_t start_size = bar_start_coords[i].copy_rect.size();
            for (size_t d = 0; d < start_size; d++) {
                SDL_Rect src;
                src = { 16, 0, 16, 16 };
                SDL_RenderCopy(r, sprite_texture, &src,
                               &bar_start_coords[i].copy_rect[d]);
            }
        }
    }
}
