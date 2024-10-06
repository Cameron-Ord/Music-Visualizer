#include "../include/particles.hpp"
#include "../include/globals.hpp"
#include "../include/rendering.hpp"
#include <cstdlib>

int particle_is_dead(int frame) { return frame > MAX_FRAME_TIME; }

Particle *render_create_particle(int bar_x, int bar_y) {
  Particle *particle = (Particle *)malloc(sizeof(Particle));
  if (!particle) {
    return nullptr;
  }

  particle->frame = rand() % 3;
  particle->h = 2;
  particle->w = 2;

  particle->x = bar_x - 5 + rand() % 25;
  particle->y = bar_y - 5 + rand() % 25;

  return particle;
}

void render_draw_particle(ParticleTrio *particle_buffer,
                          size_t *particle_buf_len,
                          const std::vector<Coordinates> *pos_buf,
                          const size_t *length, const SDL_Color *rgba) {

  if (*length > *particle_buf_len) {
    particle_buffer = (ParticleTrio *)realloc(particle_buffer,
                                              sizeof(ParticleTrio) * *length);
    *particle_buf_len = *length;
  }

  for (size_t i = 0; i < *length; i++) {
    if (particle_buffer != NULL) {
      for (size_t p = 0; p < PARTICLE_COUNT; p++) {
        if (particle_buffer[i].buf[p] != NULL) {
          if (particle_is_dead(particle_buffer[i].buf[p]->frame)) {
            cull_dead_particle(particle_buffer[i].buf[p]);
            particle_buffer[i].buf[p] = NULL;
          }
        }

        if (particle_buffer[i].buf[p] == NULL) {
          particle_buffer[i].buf[p] =
              render_create_particle((*pos_buf)[i].x, (*pos_buf)[i].y);
        }

        if (particle_buffer[i].buf[p] != NULL) {
          SDL_Rect particle_rect = {
              particle_buffer[i].buf[p]->x, particle_buffer[i].buf[p]->y,
              particle_buffer[i].buf[p]->w, particle_buffer[i].buf[p]->h};

          SDL_SetRenderDrawColor(*rend.get_renderer(), rgba->r, rgba->g,
                                 rgba->b, rgba->a);
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
