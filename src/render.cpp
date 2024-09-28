#include "../include/render_entity.hpp"
#include <ostream>

SDL2Renderer::SDL2Renderer() {
    r = NULL;
    bar_end_coords.clear();
    bar_start_coords.clear();
    directories_index = 0;
    songs_index = 0;
    font_draw_limit = 8;
    START_TILE_PATH = "start_tile.png";
    MID_TILE_PATH = "mid_tile.png";
    END_TILE_PATH = "end_tile.png";

    mid_tile_surf = NULL;
    end_tile_surf = NULL;
    start_tile_surf = NULL;
    mid_tile_tex = NULL;
    end_tile_tex = NULL;
    start_tile_tex = NULL;
}

SDL2Renderer::~SDL2Renderer() {}

void SDL2Renderer::create_image_textures() {
    start_tile_tex = SDL_CreateTextureFromSurface(r, start_tile_surf);
    if (!start_tile_tex) {
        std::cerr << "Failed to create surface texture! ->" << SDL_GetError()
                  << std::endl;
        start_tile_tex = NULL;
    }

    mid_tile_tex = SDL_CreateTextureFromSurface(r, mid_tile_surf);
    if (!mid_tile_tex) {
        std::cerr << "Failed to create surface texture! ->" << SDL_GetError()
                  << std::endl;
        mid_tile_tex = NULL;
    }

    end_tile_tex = SDL_CreateTextureFromSurface(r, end_tile_surf);
    if (!end_tile_tex) {
        std::cerr << "Failed to create surface texture! ->" << SDL_GetError()
                  << std::endl;
        end_tile_tex = NULL;
    }
}

void SDL2Renderer::create_image_surfaces() {
    start_tile_surf = IMG_Load(START_TILE_PATH.c_str());
    if (!start_tile_surf) {
        std::cerr << "Failed to load image! ->" << IMG_GetError() << std::endl;
        start_tile_surf = NULL;
    }

    mid_tile_surf = IMG_Load(MID_TILE_PATH.c_str());
    if (!mid_tile_surf) {
        std::cerr << "Failed to load image! ->" << IMG_GetError() << std::endl;
        mid_tile_surf = NULL;
    }

    end_tile_surf = IMG_Load(END_TILE_PATH.c_str());
    if (!end_tile_surf) {
        std::cerr << "Failed to load image! ->" << IMG_GetError() << std::endl;
        end_tile_surf = NULL;
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
