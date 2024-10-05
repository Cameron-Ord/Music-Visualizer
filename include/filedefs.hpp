#ifndef FILEDEFS_HPP
#define FILEDEFS_HPP
#include <iostream>
struct Directory {
  std::string directory_name;
  size_t directory_id;
};

struct Files {
  std::string file_name;
  size_t file_id;
};
#endif