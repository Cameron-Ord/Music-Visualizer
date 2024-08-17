#ifndef PROGRAM_PATH_H
#define PROGRAM_PATH_H
#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class ProgramPath {
public:
  ProgramPath();
  ~ProgramPath();
  int create_music_source();
  int create_log_directories();
  std::string get_platform_home();
  int make_directory(const std::string path, const mode_t mode);
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
