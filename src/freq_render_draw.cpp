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
    float r_prime = prim->r / 255.0;
    float g_prime = prim->g / 255.0;
    float b_prime = prim->b / 255.0;

    float max = fmaxf(r_prime, fmaxf(g_prime, b_prime));
    float min = fminf(r_prime, fminf(g_prime, b_prime));
    
    float delta = max- min;
    float hue = 0.0;
    float saturation = 0.0;
    float lightness = (max + min) / 2.0;

    if(delta == 0.0){
        hue = 0.0;
        saturation = 0.0;
    } else {
        if(lightness > 0.5){
            saturation = delta / (2.0 - max - min);
        } else {
            saturation = delta / (max + min);
        }

        if(max == r_prime){
            hue = fmodf((g_prime - b_prime) / delta, 6.0);
        } else if(max == g_prime){
            hue = (b_prime- r_prime) / delta + 2.0;
        } else {
            hue = (r_prime - g_prime) / delta + 4.0;
        }

        hue *= 60.0;
        if(hue < 0){
            hue += 360.0;
        }
    }


    for (size_t i = 0; i < *len; ++i) {
        float phased_hue = hue + (processed_phases[i] * 20.0) - 7.5;
        phased_hue = fmod(phased_hue, 360.0);

        float C = 0.5;
        float X = C * (1 - fabs(fmod(phased_hue / 60.f, 2) - 1));
        float m = 0.5;
        
        r_prime = 0.0;
        g_prime = 0.0;
        b_prime = 0.0;

        if (phased_hue < 60) {
            r_prime = C;
            g_prime = X;
            b_prime = 0;
        } else if (phased_hue < 120) {
            r_prime = X;
            g_prime = C;
            b_prime = 0;
        } else if (phased_hue < 180) {
            r_prime = 0;
            g_prime = C;
            b_prime = X;
        } else if (phased_hue < 240) {
            r_prime = 0;
            g_prime = X;
            b_prime = C;
        } else if (phased_hue < 300) {
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

        SDL_SetRenderDrawColor(r, primary, secondary, tert, 255);
        SDL_RenderFillRect(r, &bar_end_coords[i].copy_rect);
        if (bar_end_coords[i].y > bar_start_coords[i].y) {
            SDL_SetRenderDrawColor(r, sec->r, sec->g, sec->b, sec->a);
            SDL_RenderFillRect(r, &bar_start_coords[i].copy_rect);
        }
    }
}
