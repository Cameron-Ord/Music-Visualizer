#include "../include/particles.hpp"
#include "../include/globals.hpp"
#include "../include/rendering.hpp"
#include "../include/utils.hpp"
#include <cstdlib>

int particle_is_dead(int frame) { return frame > MAX_FRAME_TIME; }

Particle *render_create_particle(int bar_x, int bar_y, int bar_width,
                                 int bar_height) {
  Particle *particle = (Particle *)malloc(sizeof(Particle));
  if (!particle) {
    return NULL;
  }

  particle->frame = rand() % 3;
  particle->h = std::max(1, static_cast<int>(bar_width*0.1));
  particle->w = std::max(1, static_cast<int>(bar_width*0.1));

  if (bar_height < 25 || bar_width <= particle->w) {
    free(particle);
    return NULL;
  }

  particle->x = bar_x + rand() % (bar_width - particle->w);
  particle->y = bar_y + rand() % (bar_height - particle->h);

  return particle;
}

void render_set_particles(ParticleTrio *particle_buffer,
                          size_t *particle_buf_len, const size_t *output_length,
                          const std::vector<Coordinates> *start_buf,
                          const std::vector<Coordinates> *end_buf) {

  void *ptr_buf[] = {particle_buf_len, particle_buffer, (void *)output_length,
                     (void *)start_buf, (void *)end_buf};

  bool result = check_ptrs(5, ptr_buf);
  if (!result) {
    return;
  }

  if (*output_length > *particle_buf_len) {
    particle_buffer = (ParticleTrio *)realloc(
        particle_buffer, sizeof(ParticleTrio) * *output_length);
    *particle_buf_len = *output_length;
  }

  for (size_t i = 0; i < *output_length; i++) {
    const SDL_Rect start_rect = (*start_buf)[i].copy_rect;
    const SDL_Rect end_rect = (*end_buf)[i].copy_rect;

    for (size_t p = 0; p < PARTICLE_COUNT; p++) {
      if (particle_buffer[i].buf[p] != NULL) {
        if (particle_is_dead(particle_buffer[i].buf[p]->frame)) {
          cull_dead_particle(particle_buffer[i].buf[p]);
          particle_buffer[i].buf[p] = NULL;
        }
      }

      if (particle_buffer[i].buf[p] == NULL) {
        if (end_rect.y > start_rect.y) {
          particle_buffer[i].buf[p] = render_create_particle(
              start_rect.x, start_rect.y, start_rect.w, start_rect.h);
        }
      }
    }
  }
}

void render_draw_particles(ParticleTrio *particle_buffer,
                           size_t *particle_buf_len, const float *prim_hue,
                           const float *processed_phases) {

  void *ptr_buf[] = {particle_buffer, particle_buf_len, (void *)prim_hue,
                     (void *)processed_phases};
  bool result = check_ptrs(4, ptr_buf);
  if (!result) {
    return;
  }

  for (size_t i = 0; i < *particle_buf_len; i++) {
    float phased_hue = *prim_hue + (processed_phases[i] * 10.0) - 7.5;
    phased_hue = fmod(phased_hue, 360);
    HSL_TO_RGB conv = phase_hue_effect(&phased_hue);

    for (size_t g = 0; g < PARTICLE_COUNT; g++) {
      if (particle_buffer[i].buf[g]) {
        SDL_Rect particle_rect = {
            particle_buffer[i].buf[g]->x, particle_buffer[i].buf[g]->y,
            particle_buffer[i].buf[g]->w, particle_buffer[i].buf[g]->h};

        SDL_SetRenderDrawColor(*rend.get_renderer(), conv.r, conv.g, conv.b,
                               255 - 55);
        SDL_RenderFillRect(*rend.get_renderer(), &particle_rect);

        particle_buffer[i].buf[g]->frame++;
      }
    }
  }
}

void cull_dead_particle(Particle *dead_particle) {
  if (dead_particle) {
    free(dead_particle);
  }
}
