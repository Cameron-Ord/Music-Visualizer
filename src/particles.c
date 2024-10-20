#include "particles.h"
#include "particledef.h"

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

const size_t DEFAULT_PBUFFER_SIZE = 1024;

ParticleTrio *allocate_particle_buffer(size_t *particle_buffer_size) {
  ParticleTrio *particle_buffer =
      (ParticleTrio *)malloc(sizeof(ParticleTrio) * DEFAULT_PBUFFER_SIZE);
  if (!particle_buffer) {
    return NULL;
  }

  for (size_t i = 0; i < DEFAULT_PBUFFER_SIZE; i++) {
    for (size_t j = 0; j < PARTICLE_COUNT; j++) {
      particle_buffer[i].buf[j] = NULL;
    }
  }

  *particle_buffer_size = DEFAULT_PBUFFER_SIZE;
  return particle_buffer;
}

int particle_is_dead(int frame) { return frame > MAX_FRAME_TIME; }

Particle *render_create_particle(int bar_x, int bar_y, int bar_width,
                                 int bar_height) {

  // p_height uses the width in order to maintain the aspect ratio.
  int p_height = MAX(1, (int)(floorf(bar_width * 0.1)));
  int p_width = MAX(1, (int)(floorf(bar_width * 0.1)));

  // If it doesn't meet this criteria, just return NULL without ever calling
  // malloc
  if (bar_height < 50 || bar_width <= p_width) {
    return NULL;
  }

  Particle *particle = (Particle *)malloc(sizeof(Particle));
  if (!particle) {
    return NULL;
  }

  particle->frame = rand() % 2;
  particle->h = p_height;
  particle->w = p_width;

  particle->x = bar_x + rand() % (bar_width - particle->w);
  particle->y = bar_y + rand() % (bar_height - particle->h);

  return particle;
}

void KILL_PARTICLES(ParticleTrio *p_buffer, size_t size) {
  if (p_buffer) {
    for (size_t i = 0; i < size; i++) {
      for (size_t j = 0; j < PARTICLE_COUNT; j++) {
        if (p_buffer[i].buf[j]) {
          cull_dead_particle(p_buffer[i].buf[j]);
          p_buffer[i].buf[j] = NULL;
        }
      }
    }
  }
}

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
