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
        const int end_y_pos = h - (end * h);
        int end_bar_height = end * h;

        const float end_rect_amount =
            static_cast<float>(end_bar_height) / cell_width;

        Coordinates end_positions = { .x = end_x_pos,
                                      .y = end_y_pos,
                                      .height = end_bar_height };

        float integer;
        float fractional = std::modf(end_rect_amount, &integer);

        int accumulator = 0;

        for (float g = 0.0f; g < end_rect_amount; g += 1.0f) {
            SDL_Rect tmp = { .x = end_x_pos,
                             .y = end_y_pos + accumulator,
                             .w = cell_width,
                             .h = cell_width };

            end_positions.copy_rect.push_back(tmp);
            accumulator += cell_width;
        }

        if (fractional > 0.0) {
            int new_height = cell_width * fractional;
            SDL_Rect tmp = { .x = end_x_pos,
                             .y = end_y_pos + accumulator,
                             .w = cell_width,
                             .h = new_height };
            end_positions.copy_rect.push_back(tmp);
        }

        bar_end_coords[i] = end_positions;

        const int start_x_pos = (i * space);
        const int start_y_pos = h - (start * h);
        const int start_bar_height = end_y_pos - start_y_pos;

        const float start_rect_amount =
            static_cast<float>(start_bar_height) / cell_width;

        Coordinates start_positions = { .x = start_x_pos,
                                        .y = start_y_pos,
                                        .height = start_bar_height };

        float integer_start;
        float fractional_start = std::modf(start_bar_height, &integer_start);

        int accumulator_start = 0;

        for (float g = 0.0f; g < start_rect_amount; g += 1.0) {
            SDL_Rect tmp = { .x = start_x_pos,
                             .y = start_y_pos + accumulator_start,
                             .w = cell_width,
                             .h = cell_width };

            start_positions.copy_rect.push_back(tmp);
            accumulator_start += cell_width;
        }

        if (fractional_start > 0.0) {
            int new_height = cell_width * fractional_start;
            SDL_Rect tmp = { .x = end_x_pos,
                             .y = end_y_pos + accumulator_start,
                             .w = cell_width,
                             .h = new_height };
            start_positions.copy_rect.push_back(tmp);
        }

        bar_start_coords[i] = start_positions;
    }
}

void SDL2Renderer::render_draw_bars(size_t *len, SDL_Color *prim,
                                    SDL_Color *sec, SDL_Renderer *r) {

    for (size_t i = 0; i < *len; ++i) {
        size_t end_size = bar_end_coords[i].copy_rect.size();
        for (size_t g = 0; g < end_size; g++) {
            if (g == 0) {
                SDL_Rect src = {0,16,16,16};
                SDL_RenderCopy(r, sprite_texture,&src,
                               &bar_end_coords[i].copy_rect[g]);
                continue;
            }
            SDL_Rect src = {16,16,16,16};
            SDL_RenderCopy(r, sprite_texture, &src,
                           &bar_end_coords[i].copy_rect[g]);
        }
        if (bar_end_coords[i].y > bar_start_coords[i].y) {
            size_t start_size = bar_start_coords[i].copy_rect.size();
            for (size_t d = 0; d < start_size; d++) {
                if (d == 0) {
                    SDL_Rect src = {0,0,16,16};
                    SDL_RenderCopy(r, sprite_texture, &src,
                                   &bar_start_coords[i].copy_rect[d]);
                    continue;
                }
                SDL_Rect src = {16,0,16,16};
                SDL_RenderCopy(r, sprite_texture, &src,
                               &bar_start_coords[i].copy_rect[d]);
            }
        }
    }
}
