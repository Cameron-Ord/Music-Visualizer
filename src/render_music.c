#include "audiodefs.h"
#include "main.h"
#include "particledef.h"
#include "particles.h"
#include "utils.h"
#include <stdint.h>

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

void render_seek_bar(const uint32_t *position, const uint32_t *length) {
  float normalized = (float)*position / *length;
  int screen_location = normalized * win.width;

  int box_width = 14;
  int box_height = 8;

  int y = 0 - (box_height * 0.5);
  int x = screen_location - (box_width * 0.5);

  SDL_Rect box = {x, y, box_width, box_height};
  SDL_SetRenderDrawColor(rend.r, vis.primary.r, vis.primary.g, vis.primary.b,
                         vis.primary.a);
  SDL_RenderFillRect(rend.r, &box);
}

static void determine_ds(int *ds, const size_t *i, const int *min_cell_width, const int *win_w){
    const int iter = *i / *ds;
    const int cell_width = *win_w / iter;
    if(cell_width < *min_cell_width){
      *ds *= 2;
      determine_ds(ds, i, min_cell_width, win_w);
    }
}

void render_draw_music(RenderArgs *args, ParticleTrio *p_buffer) {
  size_t *len = (size_t *)args->length;
  const float *smear = args->smear;
  const float *smoothed = args->smooth;
  const float *phases = args->phases;

  const int h = win.height;
  const int w = win.width;

  int cell_width = w / (int)*len;
  size_t buf_iter = *len;

  const int min_cell_width = 4;

  float smooth_cpy[*len];
  float smear_cpy[*len];
  float phase_cpy[*len];

  if (cell_width < min_cell_width) {
    cell_width = min_cell_width;
  }

  int req_width = cell_width * *len;
  if (req_width > w) {
    memset(smooth_cpy, 0, sizeof(float) * *len);
    memset(phase_cpy, 0, sizeof(float) * *len);
    memset(smear_cpy, 0, sizeof(float) * *len);

    int down_sampling = 2;
    determine_ds(&down_sampling, &buf_iter, &min_cell_width, &w);

    for (size_t i = 0; i < buf_iter / down_sampling; i++) {
      smooth_cpy[i] = smoothed[i * down_sampling];
      smear_cpy[i] = smear[i * down_sampling];
      phase_cpy[i] = phases[i * down_sampling];
    }

    buf_iter /= down_sampling;
    cell_width = w / buf_iter;
  } else {
    memcpy(smooth_cpy, smoothed, sizeof(float) * *len);
    memcpy(smear_cpy, smear, sizeof(float) * *len);
    memcpy(phase_cpy, phases, sizeof(float) * *len);
  }

  for (size_t i = 0; i < buf_iter; ++i) {
    const float start = smear_cpy[i];
    const float end = smooth_cpy[i];
    const float phase = phase_cpy[i];

    const int space = cell_width + cell_width / 2;

    const int end_x_pos = (i * space);
    const int end_y_pos = h - (int)(end * (h * 0.9));
    const int end_bar_height = (int)(end * (h * 0.9));

    SDL_Rect end_box = {end_x_pos, end_y_pos, cell_width, end_bar_height};

    const int start_x_pos = (i * space);
    const int start_y_pos = h - (int)(start * (h * 0.9));
    const int start_bar_height = end_y_pos - start_y_pos;

    SDL_Rect start_box = {start_x_pos, start_y_pos, cell_width,
                          start_bar_height};

    uint8_t alpha;
    SDL_Color rcolor;

    alpha = determine_alpha(end);
    rcolor = determine_rgba(phase, &vis.secondary_bg, alpha);

    if (end_box.y > start_box.y) {
      scc(SDL_SetRenderDrawColor(rend.r, rcolor.r, rcolor.g, rcolor.b,
                                 rcolor.a));
      scc(SDL_RenderFillRect(rend.r, &start_box));
    }

    alpha = determine_alpha(end);
    rcolor = determine_rgba(phase, &vis.primary, alpha);

    scc(SDL_SetRenderDrawColor(rend.r, rcolor.r, rcolor.g, rcolor.b, rcolor.a));
    scc(SDL_RenderFillRect(rend.r, &end_box));

    // If the window is too small, don't bother with particles.
    if (win.width < 400) {
      continue;
    }

    // Probably want to put this in the above condition later
    render_set_particles(p_buffer, &end_box, &start_box, i);

    if (p_buffer) {
      for (size_t j = 0; j < PARTICLE_COUNT; j++) {
        if (p_buffer[i].buf[j] != NULL) {
          int p_wid = p_buffer[i].buf[j]->w;
          int p_hei = p_buffer[i].buf[j]->h;
          int p_x = p_buffer[i].buf[j]->x;
          int p_y = p_buffer[i].buf[j]->y;

          SDL_Color particle_colour = vis.primary;
          int rand_alpha_factor = (rand() % 255) + 1;
          float alpha_mul = (float)rand_alpha_factor / 255;

          SDL_Rect particle_rect = {p_x, p_y, p_wid, p_hei};
          scc(SDL_SetRenderDrawColor(rend.r, particle_colour.r,
                                     particle_colour.g, particle_colour.b,
                                     particle_colour.a * alpha_mul));
          scc(SDL_RenderFillRect(rend.r, &particle_rect));

          p_buffer[i].buf[j]->frame++;
        }
      }
    }
  }
}
