#ifndef PARTICLEDEF_H
#define PARTICLEDEF_H

#define MAX_FRAME_TIME 4
#define PARTICLE_COUNT 32
#include <SDL2/SDL_render.h>
#include <stdbool.h>

struct Particle {
  int frame;
  int is_dead;
  int x;
  int y;
  int w;
  int h;
};

typedef struct Particle Particle;

struct ParticleTrio {
  Particle *buf[PARTICLE_COUNT];
};

typedef struct ParticleTrio ParticleTrio;

#endif