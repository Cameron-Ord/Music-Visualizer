#include "../include/render_entity.hpp"

void SDL2Renderer::render_draw_text(std::vector<Text> *text_vec) {
    for (auto it = text_vec->begin(); it != text_vec->end(); it++) {
        if (it->is_valid) {
            SDL_RenderCopy(r, it->tex, NULL, &it->rect);
        }
    }
}

void SDL2Renderer::render_draw_text_bg(const SDL_Color *rgba) {
    if (drawing_text_bg) {
        SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a / 2);
        SDL_RenderFillRect(r, &text_bg);
    }
}