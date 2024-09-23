#ifndef PROGRAM_PATH_HPP
#define PROGRAM_PATH_HPP
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <filesystem>


class ProgramPath {
public:
  ProgramPath();
  ~ProgramPath();

  bool create_music_source();
  bool create_log_directories();
  bool make_directory(const std::string path);

  std::string get_home_path();
  std::string get_src_path();
  std::string get_platform_home();
  std::string return_slash();
  std::string get_logging_path();
  std::string join_str(std::string source, std::string add);
  void set_opened_dir(std::string dir);
  std::string get_opened_dir();

private:
  std::string OPENED_DIR;
  std::string HOME_PATH;
  std::string LOG_PATH;
  std::string ERR_LOG_FILE;
  std::string LOG_FILE;
  std::string SOURCE_PATH;
};
#endif
