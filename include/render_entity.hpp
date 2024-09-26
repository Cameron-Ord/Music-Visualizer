#ifndef RENDER_ENTITY_HPP
#define RENDER_ENTITY_HPP

#include "font_def.hpp"
#include "sdl2_entity.hpp"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <vector>

struct Coordinates{
int x;
int y;
int height;
SDL_Rect copy_rect;
};

typedef Coordinates Coordinates;
class SDL2Renderer
{
  public:
    SDL2Renderer();
    ~SDL2Renderer();

    SDL_Renderer **get_renderer();
    void *create_renderer(SDL_Window **w, SDL_Renderer **r);
    void render_clear(SDL_Renderer *r);
    void render_bg(SDL_Renderer *r, SDL_Color *rgba);
    void render_present(SDL_Renderer *r);
    void render_set_text(const WIN_SIZE *sizes, std::vector<Text> *text_vec);
    void render_draw_text(SDL_Renderer *r, std::vector<Text> *text_vec);
    void render_set_text_bg(const WIN_SIZE *sizes,
                            const std::vector<Text> *text_vec,
                            const size_t *cursor_index);
    void render_draw_text_bg(SDL_Renderer *r, const SDL_Color *rgba);
    void reset_vector_positions();
    void render_draw_bars(size_t *len,
                          SDL_Color *prim, SDL_Color *sec, SDL_Renderer *r);
    void render_set_bars(const size_t *len, const int *win_height, const int *win_width, float *smear, float *smooth);
    void set_font_draw_limit(int h);
    const size_t *get_font_draw_limit();

    size_t get_dir_index();
    size_t get_song_index();

    SDL_Surface *render_create_surface();
    SDL_Texture *render_create_texture();

    void set_dir_index(size_t i);
    void set_song_index(size_t i);

  private:
    std::string START_TILE_PATH;
    std::string END_TILE_PATH;
    std::string MID_TILE_PATH;
    SDL_Texture *mid_tile_tex;
    SDL_Texture *end_tile_tex;
    SDL_Texture *start_tile_tex;
    SDL_Renderer *r;
    SDL_Rect text_bg;
    bool drawing_text_bg;
    size_t font_draw_limit;
    size_t directories_index;
    size_t songs_index;
    std::vector<Coordinates> bar_start_coords;
    std::vector<Coordinates> bar_end_coords;
};

#endif
