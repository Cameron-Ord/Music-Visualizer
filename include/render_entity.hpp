#ifndef RENDER_ENTITY_HPP
#define RENDER_ENTITY_HPP
#include "defines.hpp"

struct Coordinates {
    int x;
    int y;
    int height;
    SDL_Rect copy_rect;
};

typedef Coordinates Coordinates;
class SDL2Renderer {
  public:
    SDL2Renderer();
    ~SDL2Renderer();

    SDL_Renderer **get_renderer();
    void *create_renderer(SDL_Window **w, SDL_Renderer **r);

    void create_sprite_surface();
    void create_sprite_texture();
    void render_clear();
    void render_bg(SDL_Color *rgba);
    void render_present();
    void render_set_text(const WIN_SIZE *sizes, std::vector<Text> *text_vec);
    void render_draw_text(std::vector<Text> *text_vec);
    void render_set_text_bg(const WIN_SIZE *sizes,
                            const std::vector<Text> *text_vec,
                            const size_t *cursor_index);
    void render_draw_text_bg(const SDL_Color *rgba);
    void reset_vector_positions();
    void render_draw_bars(size_t *len, SDL_Color *prim, SDL_Color *sec);
    void render_set_bars(const size_t *len, const int *win_height,
                         const int *win_width, float *smear, float *smooth);
    void set_font_draw_limit(int h);
    const size_t *get_font_draw_limit();
    void render_draw_settings_text(std::vector<Text> *text,
                                   const size_t *index);
    void render_set_settings_text(const WIN_SIZE *sizes,
                                  std::vector<Text> *text, const size_t *index);
    void render_set_option_values(const WIN_SIZE *sizes,
                                  std::vector<Text> *text, const size_t *index,
                                  const FFTSettings *fft_settings);
    void render_draw_option_value(const SDL_Color *rgba);

  private:
    std::string sprite_sheet_filename;
    SDL_Texture *sprite_texture;
    SDL_Surface *sprite_surface;
    SDL_Renderer *r;
    SDL_Rect text_bg;
    SDL_Rect settings_box;
    bool drawing_text_bg;
    size_t font_draw_limit;
    std::vector<Coordinates> bar_start_coords;
    std::vector<Coordinates> bar_end_coords;
};

#endif
