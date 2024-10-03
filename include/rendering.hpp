#ifndef RENDER_ENTITY_HPP
#define RENDER_ENTITY_HPP

#include "audiodefs.hpp"
#include "renderdefs.hpp"
#include "sdl2defs.hpp"
#include <iostream>
#include <vector>
#include "fontdefs.hpp"
#include "theme.hpp"

class SDL2Renderer {
  public:
    SDL2Renderer();
    ~SDL2Renderer();

    SDL_Renderer **get_renderer();
    void *create_renderer(SDL_Window **w, SDL_Renderer **r);
    void render_clear();
    void render_bg(const SDL_Color *rgba);
    void render_present();
    void render_set_text(const WIN_SIZE *sizes, std::vector<Text> *text_vec);
    void render_draw_text(std::vector<Text> *text_vec);
    void render_set_text_bg(const WIN_SIZE *sizes,
                            const std::vector<Text> *text_vec,
                            const size_t *cursor_index);
    void render_draw_text_bg(const SDL_Color *rgba);
    void reset_vector_positions();
    void render_draw_bars(const SDL_Color *prim,  const SDL_Color *sec,
                          float *processed_phases);
    void render_set_bars(const size_t *len, const int *win_height,
                         const int *win_width, float *smear, float *smooth,
                         float *processed_phases);
    void set_font_draw_limit(int h);
    const size_t *get_font_draw_limit();

    void render_draw_int_settings(std::vector<SettingTextInt> *int_sett,
                                  const WIN_SIZE *window_sizing,
                                  const SDL_Color *rgba, const SDL_Color *sec,
                                  const size_t *cursor);
    void render_draw_float_settings(std::vector<SettingTextFloat> *float_sett,
                                    const WIN_SIZE *window_sizing,
                                    const SDL_Color *rgba, const SDL_Color *sec,
                                    const size_t *cursor);
    void render_state_handler(ProgramThemes *themes, FData *ftdata, FBuffers *ftbufs);
    const int *get_setting_render_mode();
    void set_setting_render_mode(int MODE);

  private:
    int setting_render_mode;
    size_t set_length;
    SDL_Renderer *r;
    SDL_Rect text_bg;
    SDL_Rect settings_box;
    SDL_Rect settings_num_rect;
    bool drawing_text_bg;
    size_t font_draw_limit;
    std::vector<Coordinates> bar_start_coords;
    std::vector<Coordinates> bar_end_coords;
};

#endif
