#include "../include/font_entity.hpp"

SDL2Fonts::SDL2Fonts() {
    font = NULL;
    font_path = "dogicapixel.ttf";
    font_size = 16;
    character_limit = 50;
    setting_names = { "Smoothing", "Smears", "Low Range Filtering",
                      "Mid Range Filtering", "High Range Filtering" };
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

Text *SDL2Fonts::find_integer_font(int setting_num) {
    for (size_t i = 0; i < int_nums.size(); i++) {
        const int from_str = std::stoi(int_nums[i].name);
        if (from_str == setting_num) {
            return &int_nums[i];
        }
    }
    return NULL;
}

Text *SDL2Fonts::find_float_font(float setting_num) {
    for (size_t i = 0; i < float_nums.size(); i++) {
        const float from_str = std::stof(float_nums[i].name);
        if (from_str == setting_num) {
            return &float_nums[i];
        }
    }
    return NULL;
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

TTF_Font *SDL2Fonts::get_font_ptr() {
    return font;
}

std::vector<Text> *SDL2Fonts::retrieve_indexed_dir_textvector(size_t index) {
    return &dir_text_vec[index];
}

std::vector<Text> *SDL2Fonts::retrieve_indexed_song_textvector(size_t index) {
    return &song_text_vec[index];
}

size_t SDL2Fonts::get_song_vec_size() {
    return song_text_vec.size();
}

size_t SDL2Fonts::get_dir_vec_size() {
    return dir_text_vec.size();
}

void SDL2Fonts::set_char_limit(int w) {
    if (w < 200) {
        character_limit = 8;
        return;
    }
    if (w < 600) {
        character_limit = 20;
        return;
    }

    if (w < 1000) {
        character_limit = 40;
        return;
    }

    if (w < 1400) {
        character_limit = 60;
        return;
    }

    if (w < 1800) {
        character_limit = 80;
        return;
    }

    if (w < 2200) {
        character_limit = 90;
        return;
    }

    if (w > 2200) {
        character_limit = 100;
        return;
    }
}

void SDL2Fonts::destroy_allocated_fonts() {
    for (auto it = dir_text_vec.begin(); it != dir_text_vec.end(); it++) {
        destroy_dir_text(it);
    }

    dir_text_vec.clear();

    for (auto it = song_text_vec.begin(); it != song_text_vec.end(); it++) {
        destroy_file_text(it);
    }

    song_text_vec.clear();

    for (auto it = int_settings_vec.begin(); it != int_settings_vec.end();
         it++) {
        it->setting_text.tex = destroy_text_texture(it->setting_text.tex);
    }

    int_settings_vec.clear();

    for (auto it = float_settings_vec.begin(); it != float_settings_vec.end();
         it++) {
        it->setting_text.tex = destroy_text_texture(it->setting_text.tex);
    }

    float_settings_vec.clear();

    for (auto it = float_nums.begin(); it != float_nums.end(); it++) {
        it->tex = destroy_text_texture(it->tex);
    }
    float_nums.clear();

    for (auto it = int_nums.begin(); it != int_nums.end(); it++) {
        it->tex = destroy_text_texture(it->tex);
    }
    int_nums.clear();
}

void SDL2Fonts::create_float_number_text(const SDL_Color color, TTF_Font *font,
                                         SDL_Renderer *r) {
    for (auto it = float_nums.begin(); it != float_nums.end(); it++) {
        it->tex = destroy_text_texture(it->tex);
    }
    float_nums.clear();

    int id = 0;
    for (float i = 0.0f; i < 1.0f; i += 0.01) {
        Text tmp = create_text(std::to_string(i), font, r, id, color);
        if (tmp.is_valid) {
            float_nums.push_back(tmp);
        }
        id++;
    }
}

void SDL2Fonts::create_integer_number_text(const SDL_Color color,
                                           TTF_Font *font, SDL_Renderer *r) {
    for (auto it = int_nums.begin(); it != int_nums.end(); it++) {
        it->tex = destroy_text_texture(it->tex);
    }
    int_nums.clear();

    for (int i = 0; i < 20; i++) {
        Text tmp = create_text(std::to_string(i), font, r, i, color);
        if (tmp.is_valid) {
            int_nums.push_back(tmp);
        }
    }
}

void SDL2Fonts::create_settings_text(const SDL_Color color, TTF_Font *font,
                                     SDL_Renderer *r,
                                     const FFTSettings *fft_settings) {
    for (auto it = int_settings_vec.begin(); it != int_settings_vec.end();
         it++) {
        it->setting_text.tex = destroy_text_texture(it->setting_text.tex);
    }

    for (auto it = float_settings_vec.begin(); it != float_settings_vec.end();
         it++) {
        it->setting_text.tex = destroy_text_texture(it->setting_text.tex);
    }

    int_settings_vec.clear();
    float_settings_vec.clear();

    const std::vector<std::string> int_setting_names = { "Smoothing",
                                                         "Smears" };
    const std::vector<const int *> int_setting_ptrs = {
        &fft_settings->smoothing_amount, &fft_settings->smearing_amount
    };
    const std::vector<std::string> float_setting_names = {
        "Low Range Filtering", "Mid Range Filtering", "High Range Filtering"
    };
    const std::vector<const float *> float_setting_ptrs = {
        &fft_settings->filter_coeffs[0],
        &fft_settings->filter_coeffs[1], 
        &fft_settings->filter_coeffs[2]
    };

    for (size_t i = 0; i < int_setting_names.size(); i++) {
        size_t id = i;
        Text tmp = create_text(int_setting_names[i], font, r, id, color);
        SettingTextInt tmp_int;
        if (tmp.is_valid) {
            tmp_int.setting_text = tmp;
            tmp_int.setting_value_ptr = int_setting_ptrs[i];
            tmp_int.setting_value_rect = { 0, 0, 0, 0 };
            int_settings_vec.push_back(tmp_int);
        }
    }

    for (size_t i = 0; i < float_setting_names.size(); i++) {
        size_t id = i;
        Text tmp = create_text(float_setting_names[i], font, r, id, color);
        SettingTextFloat tmp_float;
        if (tmp.is_valid) {
            tmp_float.setting_text = tmp;
            tmp_float.setting_value_ptr = float_setting_ptrs[i];
            tmp_float.setting_value_rect = { 0, 0, 0, 0 };
            float_settings_vec.push_back(tmp_float);
        }
    }
}

std::vector<SettingTextInt> *SDL2Fonts::get_int_settings_vec() {
    return &int_settings_vec;
}

std::vector<SettingTextFloat> *SDL2Fonts::get_float_settings_vec() {
    return &float_settings_vec;
}

void SDL2Fonts::create_dir_text(const std::vector<Directory> d, SDL_Renderer *r,
                                const SDL_Color color, TTF_Font *font,
                                const size_t *text_limit) {
    for (auto it = dir_text_vec.begin(); it != dir_text_vec.end(); it++) {
        destroy_dir_text(it);
    }

    dir_text_vec.clear();

    std::vector<Text> tmp_vector;
    for (size_t i = 0; i < d.size(); i++) {
        int id = i % *text_limit;
        Text tmp = create_text(d[i].directory_name, font, r, id, color);
        if (tmp.is_valid) {
            if (tmp_vector.size() < *text_limit) {
                tmp_vector.push_back(tmp);
            } else {
                dir_text_vec.push_back(tmp_vector);
                tmp_vector.clear();
                tmp_vector.push_back(tmp);
            }
        }
    }

    if (tmp_vector.size() != 0) {
        dir_text_vec.push_back(tmp_vector);
    }
}

void SDL2Fonts::create_file_text(const std::vector<Files> f, SDL_Renderer *r,
                                 const SDL_Color color, TTF_Font *font,
                                 const size_t *text_limit) {
    for (auto it = song_text_vec.begin(); it != song_text_vec.end(); it++) {
        destroy_file_text(it);
    }

    song_text_vec.clear();

    std::vector<Text> tmp_vector;
    for (size_t i = 0; i < f.size(); i++) {
        int id = i % *text_limit;
        Text tmp = create_text(f[i].file_name, font, r, id, color);
        if (tmp.is_valid) {
            if (tmp_vector.size() < *text_limit) {
                tmp_vector.push_back(tmp);
            } else {
                song_text_vec.push_back(tmp_vector);
                tmp_vector.clear();
                tmp_vector.push_back(tmp);
            }
        }
    }

    if (tmp_vector.size() != 0) {
        song_text_vec.push_back(tmp_vector);
    }
}

void SDL2Fonts::destroy_file_text(
    std::vector<std::vector<Text>>::iterator &file_vec) {
    std::vector<Text> *f = &(*file_vec);
    for (auto it = f->begin(); it != f->end(); it++) {
        it->tex = destroy_text_texture(it->tex);
    }

    f->clear();
}

void SDL2Fonts::destroy_dir_text(
    std::vector<std::vector<Text>>::iterator &dir_vec) {
    std::vector<Text> *d = &(*dir_vec);
    for (auto it = d->begin(); it != d->end(); it++) {
        it->tex = destroy_text_texture(it->tex);
    }

    d->clear();
}

SDL_Surface *SDL2Fonts::create_text_surface(TTF_Font *font,
                                            const SDL_Color color,
                                            const std::string text) {
    SDL_Surface *surf_ptr = NULL;
    surf_ptr = TTF_RenderText_Blended(font, text.c_str(), color);

    if (surf_ptr == NULL) {
        fprintf(stderr, "Could not create text surface! ->%s\n",
                SDL_GetError());
        return NULL;
    }

    return surf_ptr;
}

SDL_Texture *SDL2Fonts::create_text_texture(SDL_Renderer *r,
                                            SDL_Surface *surf) {
    SDL_Texture *tex_ptr = NULL;
    tex_ptr = SDL_CreateTextureFromSurface(r, surf);

    if (tex_ptr == NULL) {
        fprintf(stderr, "Could not create text texure! -> %s\n",
                SDL_GetError());
        SDL_FreeSurface(surf);
        return NULL;
    }

    return tex_ptr;
}

Text SDL2Fonts::create_text(const std::string text, TTF_Font *font,
                            SDL_Renderer *r, const size_t text_id,
                            const SDL_Color color) {
    Text text_entity = {
        NULL, NULL, { 0, 0, 0, 0 }, 0, 0, text_id, false, text,
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

    SDL_Texture *tex_ptr = create_text_texture(r, surf_ptr);
    if (tex_ptr == NULL) {
        return text_entity;
    }

    SDL_Rect tmp = { 0, 0, surf_ptr->w, surf_ptr->h };

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
