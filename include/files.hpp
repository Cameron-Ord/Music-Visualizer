#ifndef FILES_HPP
#define FILES_HPP

#include <SDL2/SDL_render.h>
#include <dirent.h>
#include <errno.h>
#include <string>
#include <sys/stat.h>
#include <vector>

struct Icon {
  std::string  file_name;
  SDL_Texture* texture;
  SDL_Surface* surface;
  int          is_valid;
};

struct Directory {
  std::string directory_name;
  int         directory_id;
};

struct Files {
  std::string file_name;
  int         file_id;
};

typedef Icon      Icon;
typedef Directory Directory;
typedef Files     Files;

class ProgramFiles {
public:
  ProgramFiles();
  ~ProgramFiles();

  bool fill_directories(std::string src_path, std::string slash);
  bool fill_files(std::string src_path, std::string slash);
  void clear_files();
  void clear_directories();

  std::vector<Directory>* retrieve_directories();
  std::vector<Files>*     retrieve_directory_files();

private:
  int                    file_count;
  int                    dir_count;
  std::vector<Directory> directories;
  std::vector<Files>     files;
  std::vector<Icon>      icons_vec;
};

#endif
