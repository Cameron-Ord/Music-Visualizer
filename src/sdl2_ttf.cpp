#include "../include/font_entity.hpp"
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>

SDL2Fonts::SDL2Fonts() {
  font      = NULL;
  font_path = "dogicapixel.ttf";
  font_size = 16;
}

SDL2Fonts::~SDL2Fonts() {}

bool
SDL2Fonts::open_font() {
  font = TTF_OpenFont(font_path.c_str(), font_size);
  if (font == NULL) {
    fprintf(stderr, "Could not open TTF font -> %s\n", SDL_GetError());
    return false;
  }
  return true;
}

TTF_Font*
SDL2Fonts::get_font_ptr() {
  return font;
}

std::vector<Text>
SDL2Fonts::get_dir_vec() {
  return dir_text_vec;
}

std::vector<Text>
SDL2Fonts::get_song_vec() {
  return song_text_vec;
}

void
SDL2Fonts::create_dir_text(const std::vector<Directory> d, SDL_Renderer* r, const SDL_Color color,
                           TTF_Font* font) {
  for (Directory dir : d) {
    Text tmp = create_text(dir.directory_name, font, r, dir.directory_id, color);
    if (tmp.is_valid) {
      dir_text_vec.push_back(tmp);
    }
  }
}

void
SDL2Fonts::create_file_text(const std::vector<Files> f, SDL_Renderer* r, const SDL_Color color,
                            TTF_Font* font) {
  for (Files file : f) {
    Text tmp = create_text(file.file_name, font, r, file.file_id, color);
    if (tmp.is_valid) {
      song_text_vec.push_back(tmp);
    }
  }
}

SDL_Surface*
SDL2Fonts::create_text_surface(TTF_Font* font, const SDL_Color color, const std::string text) {
  SDL_Surface* surf_ptr = NULL;
  surf_ptr              = TTF_RenderText_Blended(font, text.c_str(), color);

  if (surf_ptr == NULL) {
    fprintf(stderr, "Could not create text surface! ->%s\n", SDL_GetError());
    return NULL;
  }

  return surf_ptr;
}

SDL_Texture*
SDL2Fonts::create_text_texture(SDL_Renderer* r, SDL_Surface* surf) {
  SDL_Texture* tex_ptr = NULL;
  tex_ptr              = SDL_CreateTextureFromSurface(r, surf);

  if (tex_ptr == NULL) {
    fprintf(stderr, "Could not create text texure! -> %s\n", SDL_GetError());
    SDL_FreeSurface(surf);
    return NULL;
  }

  return tex_ptr;
}

Text
SDL2Fonts::create_text(const std::string text, TTF_Font* font, SDL_Renderer* r, const int text_id,
                       const SDL_Color color) {
  Text text_entity = { .tex = NULL, .surf = NULL, .rect = { 0, 0, 0, 0 }, .id = text_id, .is_valid = false };
  SDL_Surface* surf_ptr = create_text_surface(font, color, text);
  if (surf_ptr == NULL) {
    return text_entity;
  }

  SDL_Texture* tex_ptr = create_text_texture(r, surf_ptr);
  if (tex_ptr == NULL) {
    return text_entity;
  }

  SDL_Rect tmp = { .x = 0, .y = 0, .w = surf_ptr->w, .h = surf_ptr->h };

  text_entity.rect     = tmp;
  text_entity.tex      = tex_ptr;
  text_entity.is_valid = true;

  surf_ptr = destroy_text_surface(surf_ptr);

  return text_entity;
}

SDL_Texture*
SDL2Fonts::destroy_text_texture(SDL_Texture* ptr) {
  SDL_DestroyTexture(ptr);
  return NULL;
}

SDL_Surface*
SDL2Fonts::destroy_text_surface(SDL_Surface* ptr) {
  SDL_FreeSurface(ptr);
  return NULL;
}
