#include "../include/sdl2_entity.hpp"

SDL2Fonts::SDL2Fonts() {
  font = NULL;
  font_path = "dogicapixel.ttf";
  font_size = 16;
}

SDL2Fonts::~SDL2Fonts() {}

int SDL2Fonts::open_font() {
  font = TTF_OpenFont(font_path.c_str(), font_size);
  if (font == NULL) {
    fprintf(stderr, "Could not open TTF font -> %s\n", SDL_GetError());
    return 0;
  }
  return 1;
}

void SDL2Fonts::destroy_dir_text() {}

void SDL2Fonts::destroy_file_text() {}

int SDL2Fonts::create_dir_text(std::vector<Directory> d, SDL_Renderer *r,
                               SDL_Color color) {}
int SDL2Fonts::create_file_text(std::vector<Files> d, SDL_Renderer *r,
                                SDL_Color color) {}

SDL_Surface *SDL2Fonts::create_text_surface(TTF_Font *font, SDL_Color color,
                                            std::string text) {}
SDL_Texture *SDL2Fonts::create_text_texture(SDL_Renderer *r,
                                            SDL_Surface *surf) {}
void SDL2Fonts::destroy_text_surface() {}
void SDL2Fonts::destroy_text_texture() {}

Text SDL2Fonts::create_text(std::string text, SDL_Renderer *r,
                            SDL_Color color) {}
