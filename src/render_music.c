#include "audiodefs.h"
#include "main.h"
#include "particledef.h"
#include "particles.h"

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

size_t set_length = 0;

float time_sm[M_BUF_SIZE];

// bufs->smoothed[i] += (bufs->processed[i] - bufs->smoothed[i]) * 7 * (1.0 /
// FPS);

void render_draw_music(const float *smear, const float *smoothed,
                       const float *windowed, const size_t *len,
                       ParticleTrio *p_buffer) {

  const int h = win.height;
  const int w = win.width;

  switch (vis.draw_state) {
  default:
    break;

    // This is pretty barebones for now
  case TIME_DOMAIN: {
    const int ds = 8;
    const int len = HALF_BUFF_SIZE / ds;
    const int cell_width = w / len;

    for (int i = 0; i < HALF_BUFF_SIZE / ds; i++) {
      int left = i * ds * 2;
      int right = i * ds * 2 + 1;

      time_sm[left] +=
          (windowed[left] - time_sm[left]) * 7 * (1.0 / vis.target_frames);
      time_sm[right] +=
          (windowed[right] - time_sm[right]) * 7 * (1.0 / vis.target_frames);

      float left_sample = time_sm[left] * h;
      float right_sample = time_sm[right] * h;

      const int space = cell_width + cell_width / 2;

      SDL_Rect left_box = {i * space, h - (h / 2), cell_width, left_sample};
      SDL_Rect right_box = {i * space, h - (h / 2), cell_width, right_sample};

      scc(SDL_SetRenderDrawColor(rend.r, vis.primary.r, vis.primary.g,
                                 vis.primary.b, vis.primary.a));
      SDL_RenderFillRect(rend.r, &left_box);
      SDL_RenderFillRect(rend.r, &right_box);
    }
  } break;
  case FREQ_DOMAIN: {
    float cell_width_scale = (float)w / *len;

    set_length = (size_t)(cell_width_scale < 2.0 ? (*len / 2) : *len);

    const int cell_width = w / (int)set_length;

    for (size_t i = 0; i < set_length; i++) {
      const float start = smear[i];
      const float end = smoothed[i];

      const int space = cell_width + cell_width / 2;

      const int end_x_pos = (i * space);
      const int end_y_pos = h - (int)(end * h);
      const int end_bar_height = (int)(end * h);

      SDL_Rect end_box = {end_x_pos, end_y_pos, cell_width, end_bar_height};

      const int start_x_pos = (i * space);
      const int start_y_pos = h - (int)(start * h);
      const int start_bar_height = end_y_pos - start_y_pos;

      SDL_Rect start_box = {start_x_pos, start_y_pos, cell_width,
                            start_bar_height};

      if (end_box.y > start_box.y) {
        scc(SDL_SetRenderDrawColor(rend.r, vis.secondary.r, vis.secondary.g,
                                   vis.secondary.b, vis.secondary.a));
        scc(SDL_RenderFillRect(rend.r, &start_box));
      }

      scc(SDL_SetRenderDrawColor(rend.r, vis.primary.r, vis.primary.g,
                                 vis.primary.b, vis.primary.a));
      scc(SDL_RenderFillRect(rend.r, &end_box));

      // If the window is too small, don't bother with particles.
      if (win.width < 400) {
        continue;
      }

      render_set_particles(p_buffer, &end_box, &start_box, i);

      if (p_buffer) {
        for (size_t j = 0; j < PARTICLE_COUNT; j++) {
          if (p_buffer[i].buf[j] != NULL) {
            int p_wid = p_buffer[i].buf[j]->w;
            int p_hei = p_buffer[i].buf[j]->h;
            int p_x = p_buffer[i].buf[j]->x;
            int p_y = p_buffer[i].buf[j]->y;

            SDL_Rect particle_rect = {p_x, p_y, p_wid, p_hei};
            scc(SDL_SetRenderDrawColor(rend.r, vis.primary.r, vis.primary.g,
                                       vis.primary.b, vis.primary.a));
            scc(SDL_RenderFillRect(rend.r, &particle_rect));

            p_buffer[i].buf[j]->frame++;
          }
        }
      }
    }

  } break;
  }
}
