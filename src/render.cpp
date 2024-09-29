#include "../include/render_entity.hpp"

SDL2Renderer::SDL2Renderer() {
    r = NULL;
    bar_end_coords.clear();
    bar_start_coords.clear();
    font_draw_limit = 8;
    sprite_sheet_filename = "sprites.png";
    sprite_surface = NULL;
    sprite_texture = NULL;
}

SDL2Renderer::~SDL2Renderer() {}

//   "Smoothing", "Smears", "Pre-Emphasis", "Highpass Filtering"
//
void SDL2Renderer::render_set_option_values(const WIN_SIZE *sizes,
                                            std::vector<Text> *text,
                                            const size_t *index,
                                            const FFTSettings *fft_settings) {
    std::string setting_name = (*text)[*index].name;
    int font_y = sizes->HEIGHT * 0.6;
    settings_box = { 0, 0, 0, 0 };

    if (setting_name == "Smoothing") {
        const float smoothing =
            static_cast<float>(fft_settings->smoothing_amount) / MAX_SMOOTHING;
        int font_x = static_cast<int>(smoothing * sizes->WIDTH);
        settings_box = { font_x- (16 / 2), font_y- (16 / 2), 16, 16 };
    }

    if (setting_name == "Smears") {
        const float smears =
            static_cast<float>(fft_settings->smearing_amount) / MAX_SMEARS;
        int font_x = static_cast<int>(smears * sizes->WIDTH);
        settings_box = { font_x- (16 / 2), font_y- (16 / 2), 16, 16 };
    }

    if (setting_name == "Pre-Emphasis") {
        const float pre_emph = fft_settings->filter_alpha;
        int font_x = static_cast<int>(pre_emph * sizes->WIDTH);
        settings_box = { font_x- (16 / 2), font_y- (16 / 2), 16, 16 };
    }

    if (setting_name == "Highpass Filtering") {
        const float highpass = fft_settings->filter_coeff;
        int font_x = static_cast<int>(highpass * sizes->WIDTH);
        settings_box = { font_x- (16 / 2), font_y - (16 / 2), 16, 16 };
    }
}

void SDL2Renderer::render_draw_option_value(const SDL_Color *rgba) {
    SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a);
    SDL_RenderFillRect(r, &settings_box);
}

// not used at the moment, but not removing
void SDL2Renderer::create_sprite_texture() {
    sprite_texture = SDL_CreateTextureFromSurface(r, sprite_surface);
    if (!sprite_texture) {
        std::cerr << "Failed to create surface texture! ->" << SDL_GetError()
                  << std::endl;
        sprite_texture = NULL;
    }
}

// not used at the moment, but not removing
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

void SDL2Renderer::render_clear() {
    SDL_RenderClear(r);
}
void SDL2Renderer::render_present() {
    SDL_RenderPresent(r);
}
void SDL2Renderer::render_bg(SDL_Color *rgba) {
    SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a);
}
SDL_Renderer **SDL2Renderer::get_renderer() {
    return &r;
}
