#include "../include/render_entity.hpp"
#include <ostream>

SDL2Renderer::SDL2Renderer() {
    r = NULL;
    bar_end_coords.clear();
    bar_start_coords.clear();
    directories_index = 0;
    songs_index = 0;
    font_draw_limit = 8;
    sprite_sheet_filename = "sprites.png";
}

SDL2Renderer::~SDL2Renderer() {}

void SDL2Renderer::create_sprite_texture() {
    sprite_texture = SDL_CreateTextureFromSurface(r, sprite_surface);
    if (!sprite_texture) {
        std::cerr << "Failed to create surface texture! ->" << SDL_GetError()
                  << std::endl;
        sprite_texture = NULL;
    }
}

void SDL2Renderer::create_sprite_surface() {
    sprite_surface = IMG_Load(sprite_sheet_filename.c_str());
    if (!sprite_surface) {
        std::cerr << "Failed to load image! ->" << IMG_GetError() << std::endl;
        sprite_surface = NULL;
    }
}

void SDL2Renderer::reset_vector_positions() {}

void SDL2Renderer::set_font_draw_limit(int h) {
    if (h < 200) {
        font_draw_limit = 1;
        return;
    }

    if (h < 300) {
        font_draw_limit = 2;
    } else if (h > 300 && h < 760) {
        font_draw_limit = 4;
    } else if (h > 760 && h < 1280) {
        font_draw_limit = 6;
    } else {
        font_draw_limit = 8;
    }
}

const size_t *SDL2Renderer::get_font_draw_limit() {
    return &font_draw_limit;
}

void SDL2Renderer::set_song_index(size_t i) {
    songs_index = i;
}

void SDL2Renderer::set_dir_index(size_t i) {
    directories_index = i;
}

size_t SDL2Renderer::get_dir_index() {
    return directories_index;
}

size_t SDL2Renderer::get_song_index() {
    return songs_index;
}

void *SDL2Renderer::create_renderer(SDL_Window **w, SDL_Renderer **r) {
    *r = SDL_CreateRenderer(
        *w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!*r) {
        fprintf(stderr, "Could not create renderer! -> %s", SDL_GetError());
        SDL_DestroyWindow(*w);
        return NULL;
    }

    return *r;
}

void SDL2Renderer::render_clear(SDL_Renderer *r) {
    SDL_RenderClear(r);
}
void SDL2Renderer::render_present(SDL_Renderer *r) {
    SDL_RenderPresent(r);
}
void SDL2Renderer::render_bg(SDL_Renderer *r, SDL_Color *rgba) {
    SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a);
}
SDL_Renderer **SDL2Renderer::get_renderer() {
    return &r;
}
