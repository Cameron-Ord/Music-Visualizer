#include "particles.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

ParticleTrio *allocate_particle_buffer(size_t *particle_buffer_size) {
  ParticleTrio *particle_buffer =
      (ParticleTrio *)malloc(sizeof(ParticleTrio) * 256);
  if (!particle_buffer) {
    return NULL;
  }

  for (size_t i = 0; i < 256; i++) {
    for (size_t j = 0; j < PARTICLE_COUNT; j++) {
      particle_buffer[i].buf[j] = NULL;
    }
  }

  *particle_buffer_size = 256;
  return particle_buffer;
}

int particle_is_dead(int frame) { return frame > MAX_FRAME_TIME; }

Particle *render_create_particle(int bar_x, int bar_y, int bar_width,
                                 int bar_height) {
  Particle *particle = (Particle *)malloc(sizeof(Particle));
  if (!particle) {
    return NULL;
  }

  particle->frame = rand() % 3;
  particle->h = MAX(1, (int)(bar_width * 0.1));
  particle->w = MAX(1, (int)(bar_width * 0.1));

  if (bar_height < 25 || bar_width <= particle->w) {
    free(particle);
    return NULL;
  }

  particle->x = bar_x + rand() % (bar_width - particle->w);
  particle->y = bar_y + rand() % (bar_height - particle->h);

  return particle;
}

ParticleTrio *reallocate_particle_buffer(ParticleTrio *particle_buffer,
                                         size_t *particle_buffer_size,
                                         const size_t new_size) {}

void render_set_particles(ParticleTrio *particle_buffer, const SDL_Rect *end,
                          const SDL_Rect *start, size_t iter) {

  if (particle_buffer) {
    for (size_t p = 0; p < PARTICLE_COUNT; p++) {
      if (particle_buffer[iter].buf[p] != NULL) {
        if (particle_is_dead(particle_buffer[iter].buf[p]->frame)) {
          cull_dead_particle(particle_buffer[iter].buf[p]);
          particle_buffer[iter].buf[p] = NULL;
        }
      }

      if (particle_buffer[iter].buf[p] == NULL) {
        if (end->y > start->y) {
          particle_buffer[iter].buf[p] =
              render_create_particle(start->x, start->y, start->w, start->h);
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
