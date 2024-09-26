#include "../include/render_entity.hpp"

void SDL2Renderer::render_set_text_bg(const WIN_SIZE *sizes,
                                      const std::vector<Text> *text_vec,
                                      const size_t *cursor_index) {
    int pixel_increment =
        sizes->HEIGHT / (static_cast<int>(text_vec->size()) + 1);
    int pixel_accumulate = pixel_increment;

    drawing_text_bg = false;

    for (auto it = text_vec->begin(); it != text_vec->end(); it++) {
        if (it->id == *cursor_index) {
            text_bg.w = it->width + 10;
            text_bg.h = it->height + 10;
            text_bg.x = 25 - 5;
            text_bg.y = pixel_accumulate - 5;
            drawing_text_bg = true;
        }
        pixel_accumulate += pixel_increment;
    }
}

void SDL2Renderer::render_set_text(const WIN_SIZE *sizes,
                                   std::vector<Text> *text_vec) {
    int pixel_increment =
        sizes->HEIGHT / (static_cast<int>(text_vec->size()) + 1);
    int pixel_accumulate = pixel_increment;

    for (auto it = text_vec->begin(); it != text_vec->end(); it++) {
        if (it->is_valid) {
            it->rect.x = 25;
            it->rect.y = pixel_accumulate;
        }
        pixel_accumulate += pixel_increment;
    }
}
