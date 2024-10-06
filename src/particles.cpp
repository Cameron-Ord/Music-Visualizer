#include "../include/particles.hpp"
#include "../include/globals.hpp"
#include "../include/rendering.hpp"
#include <cstdlib>

int particle_is_dead(int frame) { return frame > MAX_FRAME_TIME; }

Particle *render_create_particle(int bar_x, int bar_y, int bar_width) {
  Particle *particle = (Particle *)malloc(sizeof(Particle));
  if (!particle) {
    return nullptr;
  }

  particle->frame = rand() % 2;
  particle->h = 1;
  particle->w = 1;

  particle->x = bar_x + rand() % 12;
  particle->y = bar_y + rand() % 32;

  return particle;
}

void render_draw_particle(ParticleTrio *particle_buffer,
                          size_t *particle_buf_len,
                          const std::vector<Coordinates> *pos_start_buf,
                          const std::vector<Coordinates> *pos_end_buf,
                          const size_t *length, const SDL_Color *rgba,
                          const float *prim_hue,
                          const float *processed_phases) {

  if (*length > *particle_buf_len) {
    particle_buffer = (ParticleTrio *)realloc(particle_buffer,
                                              sizeof(ParticleTrio) * *length);
    *particle_buf_len = *length;
  }

  for (size_t i = 0; i < *length; i++) {
    float phased_hue = *prim_hue + (processed_phases[i] * 10.0) - 7.5;
    phased_hue = fmod(phased_hue, 360);

    HSL_TO_RGB conv = phase_hue_effect(&phased_hue);

    if (particle_buffer != NULL) {
      for (size_t p = 0; p < PARTICLE_COUNT; p++) {
        if (particle_buffer[i].buf[p] != NULL) {
          if (particle_is_dead(particle_buffer[i].buf[p]->frame)) {
            cull_dead_particle(particle_buffer[i].buf[p]);
            particle_buffer[i].buf[p] = NULL;
          }
        }

        if (particle_buffer[i].buf[p] == NULL) {
          if ((*pos_end_buf)[i].y > (*pos_start_buf)[i].y) {
            particle_buffer[i].buf[p] = render_create_particle(
                (*pos_start_buf)[i].x, (*pos_start_buf)[i].y,
                (*pos_start_buf)[i].copy_rect.w);
          }
        }

        if (particle_buffer[i].buf[p] != NULL) {
          SDL_Rect particle_rect = {
              particle_buffer[i].buf[p]->x, particle_buffer[i].buf[p]->y,
              particle_buffer[i].buf[p]->w, particle_buffer[i].buf[p]->h};

          SDL_SetRenderDrawColor(*rend.get_renderer(), conv.r, conv.g, conv.b,
                                 rgba->a - 75);
          SDL_RenderFillRect(*rend.get_renderer(), &particle_rect);

          particle_buffer[i].buf[p]->frame++;
        }
      }
    }
  }
}

void cull_dead_particle(Particle *dead_particle) {
  if (dead_particle) {
    free(dead_particle);
  }
}
