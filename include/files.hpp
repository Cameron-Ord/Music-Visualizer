#ifndef FILES_HPP
#define FILES_HPP

#include "file_def.hpp"
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <system_error>

class ProgramFiles
{
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
    std::vector<Icon> icons_vec;
};

#endif
