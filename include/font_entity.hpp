#ifndef SDL2_TTF_HPP
#define SDL2_TTF_HPP

#include "defines.hpp"


class SDL2Fonts {
  public:
    SDL2Fonts();
    ~SDL2Fonts();
    SDL_Surface *create_text_surface(TTF_Font *font, const SDL_Color color,
                                     const std::string text);
    SDL_Texture *create_text_texture(SDL_Renderer *r, SDL_Surface *surf);
    void create_dir_text(const std::vector<Directory> d, SDL_Renderer *r,
                         const SDL_Color color, TTF_Font *font,
                         const size_t *text_limit);
    void create_file_text(const std::vector<Files> f, SDL_Renderer *r,
                          const SDL_Color color, TTF_Font *font,
                          const size_t *text_limit);
    void destroy_file_text(std::vector<std::vector<Text>>::iterator &file_vec);
    void destroy_dir_text(std::vector<std::vector<Text>>::iterator &dir_vec);
    SDL_Surface *destroy_text_surface(SDL_Surface *ptr);
    SDL_Texture *destroy_text_texture(SDL_Texture *ptr);
    bool open_font();
    Text create_text(const std::string text, TTF_Font *font, SDL_Renderer *r,
                     const size_t text_id, const SDL_Color color);
    TTF_Font *get_font_ptr();

    void set_char_limit(int w);
    void destroy_allocated_fonts();

    size_t get_dir_vec_size();
    size_t get_song_vec_size();
    std::vector<Text> *retrieve_indexed_dir_textvector(size_t index);
    std::vector<Text> *retrieve_indexed_song_textvector(size_t index);
    std::string check_vector_index(size_t ttl_vec_size, const size_t *index, std::string direction);

  private:
    std::vector<std::vector<Text>> dir_text_vec;
    std::vector<std::vector<Text>> song_text_vec;
    std::string font_path;
    int font_size;
    size_t character_limit;
    TTF_Font *font;
    int aprx_h;
    int aprx_w;
};

#endif
