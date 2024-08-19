#ifndef PROGRAM_PATH_HPP
#define PROGRAM_PATH_HPP
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

class ProgramPath {
public:
  ProgramPath();
  ~ProgramPath();

  bool create_music_source();
  bool create_log_directories();
  bool make_directory(const std::string path, const mode_t mode);

  std::string get_home_path();
  std::string get_src_path();
  std::string get_platform_home();
  std::string return_slash();
  std::string get_logging_path();
  std::string join_str(std::string source, std::string add);

private:
  std::string HOME_PATH;
  std::string LOG_PATH;
  std::string ERR_LOG_FILE;
  std::string LOG_FILE;
  std::string SOURCE_PATH;
};
#endif
