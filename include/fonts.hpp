#ifndef SDL2_TTF_HPP
#define SDL2_TTF_HPP

#include "audiodefs.hpp"
#include "filedefs.hpp"
#include "fontdefs.hpp"
#include "table.hpp"
#include <SDL2/SDL_ttf.h>

#include <vector>

class SDL2Fonts {
public:
  SDL2Fonts();
  ~SDL2Fonts();
  SDL_Surface *create_text_surface(TTF_Font *font, const SDL_Color color,
                                   const std::string text);
  SDL_Texture *create_text_texture(SDL_Renderer *r, SDL_Surface *surf);
  SDL_Surface *destroy_text_surface(SDL_Surface *ptr);
  SDL_Texture *destroy_text_texture(SDL_Texture *ptr);
  bool open_font();
  Text create_text(const std::string text, const size_t text_id,
                   const SDL_Color color);
  TTF_Font *get_font_ptr();

  bool create_dir_text(const std::vector<Directory> *d, Node **df_table,
                       size_t *DFTableSize, const SDL_Color *color);

  bool create_file_text(const std::vector<Files> *f, Node **sf_table,
                        size_t *SFTableSize, const SDL_Color *color);

  void set_char_limit(int w);
  void destroy_allocated_fonts();

  std::string check_vector_index(size_t ttl_vec_size, const size_t *index,
                                 std::string direction);
  void create_settings_text(const SDL_Color color,
                            const FFTSettings *fft_settings);

  std::vector<SettingTextInt> *get_int_settings_vec();
  std::vector<SettingTextFloat> *get_float_settings_vec();

private:
  std::vector<std::string> setting_names;
  std::vector<SettingTextInt> int_settings_vec;
  std::vector<SettingTextFloat> float_settings_vec;
  std::string font_path;
  int font_size;
  size_t character_limit;
  TTF_Font *font;
  int aprx_h;
  int aprx_w;
};

#endif
