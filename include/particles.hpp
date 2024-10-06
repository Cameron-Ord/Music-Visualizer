#ifndef PARTICLES_HPP
#define PARTICLES_HPP

#include "renderdefs.hpp"
#include <SDL2/SDL_rect.h>
#include <vector>

#define MAX_FRAME_TIME 5
#define PARTICLE_COUNT 25

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
Particle *render_create_particle(int bar_x, int bar_y);
void cull_dead_particle(Particle *dead_particle);
void render_draw_particle(ParticleTrio *particle_buffer,
                          size_t *particle_buf_len,
                          const std::vector<Coordinates> *start_pos_buf,
                          const size_t *length, const SDL_Color *rgba);
#endif
