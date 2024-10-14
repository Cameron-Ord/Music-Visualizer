#ifndef PARTICLES_H
#define PARTICLES_H

#include "particledef.h"

int particle_is_dead(int frame);
Particle *render_create_particle(int bar_x, int bar_y, int bar_width,
                                 int bar_height);
void cull_dead_particle(Particle *dead_particle);
ParticleTrio *reallocate_particle_buffer(ParticleTrio *particle_buffer,
                                         size_t *particle_buffer_size,
                                         const size_t new_size);
void render_set_particles(ParticleTrio *particle_buffer, const SDL_Rect *end,
                          const SDL_Rect *start, size_t iter);
ParticleTrio *allocate_particle_buffer(size_t *particle_buffer_size);
#endif
