#include "../include/particles.hpp"
#include "../include/rendering.hpp"
#include "../include/globals.hpp"
#include <cstdlib>




int particle_is_dead(int frame){
    return frame > MAX_FRAME_TIME;
}


Particle* render_create_particle(int bar_x, int bar_y){
    Particle *particle = (Particle*)malloc(sizeof(Particle));
    if(!particle){
        return nullptr;
    }

    particle->frame = rand() % 5;
    particle->h = 200;
    particle->w = 5;
    particle->is_dead = 0;

    particle->x = bar_x - 5 + rand() % 25;
    particle->y = bar_y - 5 + rand() % 25;

    return particle;
}

void render_draw_particle(Particle** particle_buffer, size_t *particle_buf_len, const size_t *length){
    if(*length > *particle_buf_len){
        particle_buffer = (Particle**)realloc(particle_buffer, sizeof(Particle*) * *length);
        *particle_buf_len = *length;
    }

    for(size_t i = 0; i < *length; i++){
        if(particle_buffer[i] != NULL){
            if(particle_is_dead(particle_buffer[i]->frame)){
                cull_dead_particle(particle_buffer[i]);
                particle_buffer[i] = NULL;
            }
        }

        if(particle_buffer[i] == NULL){
            particle_buffer[i] = render_create_particle(50,50);
        }

        if(particle_buffer[i] != NULL){
            SDL_Rect particle_rect = {particle_buffer[i]->x, particle_buffer[i]->y, 1, 1};
            
            std::cout << particle_buffer[i]->x << " " << particle_buffer[i]->y << std::endl;
            
            SDL_SetRenderDrawColor(*rend.get_renderer(), 255, 255, 255, 255);
            SDL_RenderFillRect(*rend.get_renderer(), &particle_rect);
        }
    }
}

void cull_dead_particle(Particle *dead_particle){
    if(dead_particle){
        free(dead_particle);
    }
}