#include "../include/render_entity.hpp"
#include <cmath>

void SDL2Renderer::render_set_bars(const size_t *len, const int *win_height,
                                   const int *win_width, float *smear,
                                   float *smooth, float *phases) {
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
        const int end_y_pos = h - static_cast<int>((end * h));
        const int end_bar_height = static_cast<int>(end * h);

        SDL_Rect box;

        box = { end_x_pos, end_y_pos, cell_width, end_bar_height };

        Coordinates end_positions = { end_x_pos, end_y_pos, end_bar_height,
                                      box };

        const int start_x_pos = (i * space);
        const int start_y_pos = h - static_cast<int>(start * h);
        const int start_bar_height = end_y_pos - start_y_pos;

        box = { start_x_pos, start_y_pos, cell_width, start_bar_height };

        Coordinates start_positions = { start_x_pos, start_y_pos,
                                        start_bar_height, box };

        bar_end_coords[i] = end_positions;
        bar_start_coords[i] = start_positions;
    }
}

void SDL2Renderer::render_draw_bars(size_t *len, SDL_Color *prim,
                                    SDL_Color *sec, float *processed_phases) {
    for (size_t i = 0; i < *len; ++i) {
        float hue = 225.0f + (processed_phases[i] * 15.0) - 5.0;
        hue = fmod(hue, 360.0);

        float C = 0.35f; 
        float X = C * (1 - fabs(fmod(hue / 60.f, 2) - 1));
        float m = 0.51f;
        float r_prime, g_prime, b_prime;

        if (hue < 60) {
            r_prime = C;
            g_prime = X;
            b_prime = 0;
        } else if (hue < 120) {
            r_prime = X;
            g_prime = C;
            b_prime = 0;
        } else if (hue < 180) {
            r_prime = 0;
            g_prime = C;
            b_prime = X;
        } else if (hue < 240) {
            r_prime = 0;
            g_prime = X;
            b_prime = C;
        } else if (hue < 300) {
            r_prime = X;
            g_prime = 0;
            b_prime = C;
        } else {
            r_prime = C;
            g_prime = 0;
            b_prime = X;
        }

        // Adjust RGB values
        uint8_t primary = static_cast<Uint8>((r_prime + m) * 255);
        uint8_t secondary = static_cast<Uint8>((g_prime + m) * 255);
        uint8_t tert = static_cast<Uint8>((b_prime + m) * 255);

        SDL_SetRenderDrawColor(r, primary, secondary, tert, prim->a);
        SDL_RenderFillRect(r, &bar_end_coords[i].copy_rect);
        if (bar_end_coords[i].y > bar_start_coords[i].y) {
            SDL_SetRenderDrawColor(r, prim->r, prim->g, prim->b, prim->a);
            SDL_RenderFillRect(r, &bar_start_coords[i].copy_rect);
        }
    }
}
