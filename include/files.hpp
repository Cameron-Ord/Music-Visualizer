#ifndef FILES_HPP
#define FILES_HPP

#include "filedefs.hpp"
#include <vector>

class ProgramFiles {
public:
  ProgramFiles();
  ~ProgramFiles();

  bool fill_directories(std::string src_path, std::string slash);
  bool fill_files(std::string src_path, std::string slash);
  void clear_files();
  void clear_directories();

  std::vector<Directory> *retrieve_directories();
  std::vector<Files> *retrieve_directory_files();

private:
  int file_count;
  int dir_count;
  std::vector<Directory> directories;
  std::vector<Files> files;
};

#endif
