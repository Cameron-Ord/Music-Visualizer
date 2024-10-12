#include "../include/fonts.hpp"
#include "../include/globals.hpp"
#include "../include/utils.hpp"

SDL2Fonts::SDL2Fonts() {
  font = NULL;
  font_path = "dogicapixel.ttf";
  font_size = 16;
  character_limit = 50;
  setting_names = {"Smoothing", "Smears", "Low Range Filtering",
                   "Mid Range Filtering", "High Range Filtering"};
}

SDL2Fonts::~SDL2Fonts() {}

bool SDL2Fonts::open_font() {
  font = TTF_OpenFont(font_path.c_str(), font_size);
  if (font == NULL) {
    fprintf(stderr, "Could not open TTF font -> %s\n", SDL_GetError());
    return false;
  }
  return true;
}

std::string SDL2Fonts::check_vector_index(size_t ttl_vec_size,
                                          const size_t *index,
                                          std::string direction) {
  int signed_size = static_cast<int>(ttl_vec_size);
  int signed_index = static_cast<int>(*index);

  if (signed_size - 1 < 0) {
    return "EMPTY";
  }

  if (direction == "UP") {
    signed_index -= 1;
    if (signed_index < 0) {
      return "MIN";
    }
    return "SAFE";
  } else if (direction == "DOWN") {
    signed_index += 1;
    if (signed_index > signed_size - 1) {
      return "MAX";
    }
    return "SAFE";
  }

  return "INVALID";
}

TTF_Font *SDL2Fonts::get_font_ptr() { return font; }

void SDL2Fonts::set_char_limit(int w) {
  character_limit = std::min(175, std::max(8, (w - 200) / 12));
}

void SDL2Fonts::destroy_allocated_fonts() {
  for (auto it = int_settings_vec.begin(); it != int_settings_vec.end(); it++) {
    it->setting_text.tex = destroy_text_texture(it->setting_text.tex);
  }

  int_settings_vec.clear();

  for (auto it = float_settings_vec.begin(); it != float_settings_vec.end();
       it++) {
    it->setting_text.tex = destroy_text_texture(it->setting_text.tex);
  }

  float_settings_vec.clear();
}

void SDL2Fonts::create_settings_text(const SDL_Color color,
                                     const FFTSettings *fft_settings) {
  for (auto it = int_settings_vec.begin(); it != int_settings_vec.end(); it++) {
    it->setting_text.tex = destroy_text_texture(it->setting_text.tex);
  }

  for (auto it = float_settings_vec.begin(); it != float_settings_vec.end();
       it++) {
    it->setting_text.tex = destroy_text_texture(it->setting_text.tex);
  }

  int_settings_vec.clear();
  float_settings_vec.clear();

  const std::vector<std::string> int_setting_names = {"Smoothing", "Smears"};
  const std::vector<const int *> int_setting_ptrs = {
      &fft_settings->smoothing_amount, &fft_settings->smearing_amount};
  const std::vector<std::string> float_setting_names = {
      "Low Range Filtering", "Mid Range Filtering", "High Range Filtering"};
  const std::vector<const float *> float_setting_ptrs = {
      &fft_settings->filter_coeffs[0], &fft_settings->filter_coeffs[1],
      &fft_settings->filter_coeffs[2]};

  int_settings_vec.resize(int_setting_names.size());

  for (size_t i = 0; i < int_setting_names.size(); i++) {
    size_t id = i;
    Text tmp = create_text(int_setting_names[i], id, color);
    SettingTextInt tmp_int;
    if (tmp.is_valid) {
      tmp_int.setting_text = tmp;
      tmp_int.setting_value_ptr = int_setting_ptrs[i];
      tmp_int.setting_value_rect = {0, 0, 0, 0};
      int_settings_vec[i] = tmp_int;
    }
  }

  float_settings_vec.resize(float_setting_names.size());

  for (size_t i = 0; i < float_setting_names.size(); i++) {
    size_t id = i;
    Text tmp = create_text(float_setting_names[i], id, color);
    SettingTextFloat tmp_float;
    if (tmp.is_valid) {
      tmp_float.setting_text = tmp;
      tmp_float.setting_value_ptr = float_setting_ptrs[i];
      tmp_float.setting_value_rect = {0, 0, 0, 0};
      float_settings_vec[i] = tmp_float;
    }
  }
}

std::vector<SettingTextInt> *SDL2Fonts::get_int_settings_vec() {
  return &int_settings_vec;
}

std::vector<SettingTextFloat> *SDL2Fonts::get_float_settings_vec() {
  return &float_settings_vec;
}

bool SDL2Fonts::create_dir_text(const std::vector<Directory> *d,
                                Node **df_table, size_t *DFTableSize,
                                const SDL_Color *color) {
  if (check_ptrs(3, d, df_table, color)) {
    const size_t size = d->size();
    // Reallocate the hash table size to the actual amount of directories
    // instead of the default. This prevents overflows and more often saves
    // memory.
    df_table = (Node **)realloc(df_table, sizeof(Node *) * size);
    if (!df_table) {
      *DFTableSize = 0;
      std::cerr << "Reallocation failed! -> " << strerror(errno) << std::endl;
      return false;
    }

    for (size_t i = 0; i < size; i++) {
      Text text_sample = create_text((*d)[i].directory_name, i, *color);
      if (!insert_node(df_table, i, &text_sample, size)) {
        *DFTableSize = 0;
        std::cerr << "Failed to create node! -> " << strerror(errno)
                  << std::endl;
        return false;
      }
    }
    *DFTableSize = size;
  }

  return true;
}

bool SDL2Fonts::create_file_text(const std::vector<Files> *f, Node **sf_table,
                                 size_t *SFTableSize, const SDL_Color *color) {
  if (check_ptrs(3, f, sf_table, color)) {
    const size_t size = f->size();
    // Reallocate the hash table size to the actual amount of directories
    // instead of the default. This prevents overflows and more often saves
    // memory.
    sf_table = (Node **)realloc(sf_table, sizeof(Node *) * size);
    if (!sf_table) {
      *SFTableSize = 0;
      std::cerr << "Reallocation failed! -> " << strerror(errno) << std::endl;
      return false;
    }

    for (size_t i = 0; i < size; i++) {
      Text text_sample = create_text((*f)[i].file_name, i, *color);
      if (!insert_node(sf_table, i, &text_sample, size)) {
        *SFTableSize = 0;
        std::cerr << "Failed to create node! -> " << strerror(errno)
                  << std::endl;
        return false;
      }
    }

    *SFTableSize = size;
  }

  return true;
}

SDL_Surface *SDL2Fonts::create_text_surface(TTF_Font *font,
                                            const SDL_Color color,
                                            const std::string text) {
  SDL_Surface *surf_ptr = NULL;
  surf_ptr = TTF_RenderText_Blended(font, text.c_str(), color);

  if (surf_ptr == NULL) {
    fprintf(stderr, "Could not create text surface! ->%s\n", SDL_GetError());
    return NULL;
  }

  return surf_ptr;
}

SDL_Texture *SDL2Fonts::create_text_texture(SDL_Renderer *r,
                                            SDL_Surface *surf) {
  SDL_Texture *tex_ptr = NULL;
  tex_ptr = SDL_CreateTextureFromSurface(r, surf);

  if (tex_ptr == NULL) {
    fprintf(stderr, "Could not create text texure! -> %s\n", SDL_GetError());
    SDL_FreeSurface(surf);
    return NULL;
  }

  return tex_ptr;
}

Text SDL2Fonts::create_text(const std::string text, const size_t text_id,
                            const SDL_Color color) {
  Text text_entity = {
      NULL, NULL, {0, 0, 0, 0}, 0, 0, text_id, false, text,
  };

  std::string text_cpy = text;
  if (text_cpy.size() > character_limit) {
    text_cpy.erase(character_limit);
    text_cpy += "...";
  }

  SDL_Surface *surf_ptr = create_text_surface(font, color, text_cpy);
  if (surf_ptr == NULL) {
    return text_entity;
  }

  SDL_Texture *tex_ptr = create_text_texture(rend.get_renderer(), surf_ptr);
  if (tex_ptr == NULL) {
    return text_entity;
  }

  SDL_Rect tmp = {0, 0, surf_ptr->w, surf_ptr->h};

  text_entity.rect = tmp;
  text_entity.tex = tex_ptr;
  text_entity.is_valid = true;
  text_entity.width = surf_ptr->w;
  text_entity.height = surf_ptr->h;

  surf_ptr = destroy_text_surface(surf_ptr);

  return text_entity;
}

SDL_Texture *SDL2Fonts::destroy_text_texture(SDL_Texture *ptr) {
  SDL_DestroyTexture(ptr);
  return NULL;
}

SDL_Surface *SDL2Fonts::destroy_text_surface(SDL_Surface *ptr) {
  SDL_FreeSurface(ptr);
  return NULL;
}
