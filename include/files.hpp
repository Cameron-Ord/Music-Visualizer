#ifndef FILES_HPP
#define FILES_HPP
#include <SDL2/SDL_render.h>
#include <string>
#include <vector>

struct Icon {
  std::string path;
  SDL_Texture *texture;
  SDL_Surface *surface;
  int is_valid;
};

struct Directory {
  std::string partial_path;
  std::string full_path;
  int directory_count;
  int is_valid;
};

struct Files {
  std::string partial_path;
  std::string full_path;
  int file_count;
  int is_valid;
};

typedef Icon Icon;
typedef Directory Directory;
typedef Files Files;

class ProgramFiles {
public:
  ProgramFiles();
  ~ProgramFiles();
  std::vector<Directory> *retrieve_directories();
  std::vector<Files> *retrieve_directory_files();

private:
  std::vector<Directory> directories;
  std::vector<Files> files;
  std::vector<Icon> icons_vec;
};

#endif
