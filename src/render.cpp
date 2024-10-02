#include "../include/rendering.hpp"
#include "../include/macdefs.hpp"
#include "../include/enumdefs.hpp"


SDL2Renderer::SDL2Renderer() {
    r = NULL;
    bar_end_coords.clear();
    bar_start_coords.clear();
    font_draw_limit = 0;
    set_length = 0;
    setting_render_mode = FLOATS;
}

SDL2Renderer::~SDL2Renderer() {}

//   "Smoothing", "Smears", "Pre-Emphasis", "Highpass Filtering"
//

const int *SDL2Renderer::get_setting_render_mode() {
    return &setting_render_mode;
}

void SDL2Renderer::set_setting_render_mode(int MODE) {
    setting_render_mode = MODE;
}

void SDL2Renderer::render_draw_int_settings(
    std::vector<SettingTextInt> *int_sett, const WIN_SIZE *window_sizing,
    const SDL_Color *rgba, const SDL_Color *sec, const size_t *cursor) {
    const size_t size = int_sett->size();
    const int win_width = window_sizing->WIDTH;
    const int win_height = window_sizing->HEIGHT;

    int pixel_increment = win_height / (static_cast<int>(size) + 1);
    int pixel_accumulate = pixel_increment;

    for (size_t i = 0; i < size; i++) {
        SDL_Rect *setting_rect = &(*int_sett)[i].setting_text.rect;
        SDL_Rect *setting_val_rect = &(*int_sett)[i].setting_value_rect;
        SDL_Texture *texture = (*int_sett)[i].setting_text.tex;

        const int *setting_value = (*int_sett)[i].setting_value_ptr;
        setting_rect->x =
            static_cast<int>((win_width * 0.5)) - (setting_rect->w / 2);
        setting_rect->y = pixel_accumulate - (setting_rect->h / 2);

        const float normalized =
            static_cast<float>(*setting_value) / INT_SETTING_MAX;
        setting_val_rect->x =
            static_cast<int>(normalized * win_width) - (16 / 2);
        setting_val_rect->y =
            (pixel_accumulate + (setting_rect->h + 20)) - (16 / 2);
        setting_val_rect->w = 16;
        setting_val_rect->h = 16;

        SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a);
        SDL_RenderCopy(r, texture, NULL, setting_rect);
        SDL_RenderFillRect(r, setting_val_rect);

        if (i == *cursor) {
            SDL_Rect rect_cpy = *setting_val_rect;

            rect_cpy.h += 10;
            rect_cpy.w += 10;
            rect_cpy.x = static_cast<int>(normalized * win_width) - (26 / 2);
            rect_cpy.y = (pixel_accumulate + (setting_rect->h + 20)) - (26 / 2);

            SDL_SetRenderDrawColor(r, sec->r, sec->g, sec->b, sec->a);
            SDL_RenderFillRect(r, &rect_cpy);
        }

        pixel_accumulate += pixel_increment;
    }
}

void SDL2Renderer::render_draw_float_settings(
    std::vector<SettingTextFloat> *float_sett, const WIN_SIZE *window_sizing,
    const SDL_Color *rgba, const SDL_Color *sec, const size_t *cursor) {

    const size_t size = float_sett->size();
    const int win_width = window_sizing->WIDTH;
    const int win_height = window_sizing->HEIGHT;

    int pixel_increment = win_height / (static_cast<int>(size) + 1);
    int pixel_accumulate = pixel_increment;

    for (size_t i = 0; i < size; i++) {
        SDL_Rect *setting_rect = &(*float_sett)[i].setting_text.rect;
        SDL_Rect *setting_val_rect = &(*float_sett)[i].setting_value_rect;
        SDL_Texture *texture = (*float_sett)[i].setting_text.tex;

        const float *setting_value = (*float_sett)[i].setting_value_ptr;
        setting_rect->x =
            static_cast<int>((win_width * 0.5)) - (setting_rect->w / 2);
        setting_rect->y = pixel_accumulate - (setting_rect->h / 2);

        setting_val_rect->x =
            static_cast<int>(*setting_value * win_width) - (16 / 2);
        setting_val_rect->y =
            (pixel_accumulate + (setting_rect->h + 20)) - (16 / 2);
        setting_val_rect->w = 16;
        setting_val_rect->h = 16;

        SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a);
        SDL_RenderCopy(r, texture, NULL, setting_rect);
        SDL_RenderFillRect(r, setting_val_rect);

        if (i == *cursor) {
            SDL_Rect rect_cpy = *setting_val_rect;

            rect_cpy.h += 10;
            rect_cpy.w += 10;
            rect_cpy.x =
                static_cast<int>(*setting_value * win_width) - (26 / 2);
            rect_cpy.y = (pixel_accumulate + (setting_rect->h + 20)) - (26 / 2);

            SDL_SetRenderDrawColor(r, sec->r, sec->g, sec->b, sec->a);
            SDL_RenderFillRect(r, &rect_cpy);
        }

        pixel_accumulate += pixel_increment;
    }
}


void SDL2Renderer::reset_vector_positions() {}

void SDL2Renderer::set_font_draw_limit(int h) {
    font_draw_limit = std::min(24, std::max(1, (h - 100) / 28));
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
