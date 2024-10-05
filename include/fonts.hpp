#ifndef SDL2_TTF_HPP
#define SDL2_TTF_HPP

#include "audiodefs.hpp"
#include "filedefs.hpp"
#include "fontdefs.hpp"
#include <SDL2/SDL_ttf.h>

#include <vector>

class SDL2Fonts {
public:
  SDL2Fonts();
  ~SDL2Fonts();
  SDL_Surface *create_text_surface(TTF_Font *font, const SDL_Color color,
                                   const std::string text);
  SDL_Texture *create_text_texture(SDL_Renderer *r, SDL_Surface *surf);
  void create_dir_text(const std::vector<Directory> d, const SDL_Color color);
  void create_file_text(const std::vector<Files> f, const SDL_Color color);
  void destroy_file_text(std::vector<std::vector<Text>>::iterator &file_vec);
  void destroy_dir_text(std::vector<std::vector<Text>>::iterator &dir_vec);
  SDL_Surface *destroy_text_surface(SDL_Surface *ptr);
  SDL_Texture *destroy_text_texture(SDL_Texture *ptr);
  bool open_font();
  Text create_text(const std::string text, const size_t text_id,
                   const SDL_Color color);
  TTF_Font *get_font_ptr();

  void set_char_limit(int w);
  void destroy_allocated_fonts();

  size_t get_dir_vec_size();
  size_t get_song_vec_size();
  std::vector<std::vector<Text>> *get_full_dir_textvector();
  std::vector<std::vector<Text>> *get_full_song_textvector();
  std::vector<Text> *retrieve_indexed_dir_textvector(size_t index);
  std::vector<Text> *retrieve_indexed_song_textvector(size_t index);
  std::string check_vector_index(size_t ttl_vec_size, const size_t *index,
                                 std::string direction);

  void create_float_number_text(const SDL_Color color);
  void create_integer_number_text(const SDL_Color color);
  void create_settings_text(const SDL_Color color,
                            const FFTSettings *fft_settings);

  std::vector<SettingTextInt> *get_int_settings_vec();
  std::vector<SettingTextFloat> *get_float_settings_vec();
  Text *find_integer_font(int setting_num);
  Text *find_float_font(float setting_num);

private:
  std::vector<Text> float_nums;
  std::vector<Text> int_nums;
  std::vector<std::string> setting_names;
  std::vector<SettingTextInt> int_settings_vec;
  std::vector<SettingTextFloat> float_settings_vec;
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
