#ifndef PARTICLES_HPP
#define PARTICLES_HPP

#include "renderdefs.hpp"
#include <SDL2/SDL_rect.h>
#include <vector>

#define MAX_FRAME_TIME 6
#define PARTICLE_COUNT 48

struct Particle {
  int frame;
  int is_dead;
  int x;
  int y;
  int w;
  int h;
};

struct ParticleTrio {
  Particle *buf[PARTICLE_COUNT];
};

int particle_is_dead(int frame);
Particle *render_create_particle(int bar_x, int bar_y, int bar_width,
                                 int bar_height);
void cull_dead_particle(Particle *dead_particle);

void render_set_particles(ParticleTrio *particle_buffer,
                          size_t *particle_buf_len, const size_t *output_length,
                          const std::vector<Coordinates> *start_buf,
                          const std::vector<Coordinates> *end_buf);

void render_draw_particles(ParticleTrio *particle_buffer,
                           size_t *particle_buf_len, const float *prim_hue,
                           const float *processed_phases);
#endif
