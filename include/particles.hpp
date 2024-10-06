#ifndef PARTICLES_HPP
#define PARTICLES_HPP

#include <SDL2/SDL_rect.h>

#define MAX_PARTICLES_PER_BAR 20
#define MAX_FRAME_TIME 10

struct Particle {
    int frame;
    int is_dead;
    int x;
    int y;
    int w;
    int h;
};

int particle_is_dead(int frame);
void render_draw_particle(Particle** particle_buffer, const size_t *particle_buf_len, const size_t *length);
Particle* render_create_particle(int bar_x, int bar_y);
void cull_dead_particle(Particle *dead_particle);
#endif