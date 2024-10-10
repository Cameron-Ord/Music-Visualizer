#include "../include/particles.hpp"
#include "../include/rendering.hpp"
#include "../include/utils.hpp"
#include <cstdlib>

void SDL2Renderer::render_set_bars(const size_t *len, const int *win_height,
                                   const int *win_width, float *smear,
                                   float *smooth) {

  if (*len == 0) {
    return;
  }

  const int h = *win_height;

  if (bar_end_coords.size() < *len) {
    bar_end_coords.resize(*len);
  }

  if (bar_start_coords.size() < *len) {
    bar_start_coords.resize(*len);
  }

  float cell_width_scale = static_cast<float>(*win_width) / *len;
  set_length = cell_width_scale < 2.0 ? (*len / 2) : *len;

  const int cell_width = *win_width / set_length;
  for (size_t i = 0; i < set_length; ++i) {
    const float start = smear[i];
    const float end = smooth[i];

    const int space = cell_width + cell_width / 2;

    const int end_x_pos = (i * space);
    const int end_y_pos = h - static_cast<int>((end * h));
    const int end_bar_height = static_cast<int>(end * h);

    SDL_Rect box;

    box = {end_x_pos, end_y_pos, cell_width, end_bar_height};

    Coordinates end_positions = {end_x_pos, end_y_pos, end_bar_height, box};

    const int start_x_pos = (i * space);
    const int start_y_pos = h - static_cast<int>(start * h);
    const int start_bar_height = end_y_pos - start_y_pos;

    box = {start_x_pos, start_y_pos, cell_width, start_bar_height};

    Coordinates start_positions = {start_x_pos, start_y_pos, start_bar_height,
                                   box};

    bar_end_coords[i] = end_positions;
    bar_start_coords[i] = start_positions;
  }
}

void SDL2Renderer::render_draw_bars(const float *prim_hue, const SDL_Color *sec,
                                    float *processed_phases) {
  if (set_length == 0) {
    return;
  }

  for (size_t i = 0; i < set_length; ++i) {
    float phased_hue = *prim_hue + (processed_phases[i] * 10.0) - 7.5;
    phased_hue = fmod(phased_hue, 360.0);

    HSL_TO_RGB conv = phase_hue_effect(&phased_hue);

    if (bar_end_coords[i].y > bar_start_coords[i].y) {
      scc(SDL_SetRenderDrawColor(r, sec->r, sec->g, sec->b, sec->a));
      scc(SDL_RenderFillRect(r, &bar_start_coords[i].copy_rect));
    }

    scc(SDL_SetRenderDrawColor(r, conv.r, conv.g, conv.b, 255));
    scc(SDL_RenderFillRect(r, &bar_end_coords[i].copy_rect));
  }
}
