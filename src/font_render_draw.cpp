#include "../include/rendering.hpp"
#include "../include/utils.hpp"

void SDL2Renderer::render_draw_text(std::vector<Text> *text_vec) {
  for (auto it = text_vec->begin(); it != text_vec->end(); it++) {
    if (it->is_valid) {
      scc(SDL_RenderCopy(r, it->tex, NULL, &it->rect));
    }
  }
}

void SDL2Renderer::render_draw_text_bg(const SDL_Color *rgba) {
  if (drawing_text_bg) {
    scc(SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a / 2));
    scc(SDL_RenderFillRect(r, &text_bg));
  }
}
