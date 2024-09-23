#include "../include/macdefs.hpp"
#include "../include/program_path.hpp"

ProgramPath::ProgramPath() {
  const std::string platform_home = get_platform_home();
  const char *home_to_char = platform_home.c_str();
  char *home;
  size_t size;
  errno_t err;
  _dupenv_s(&home,&size,home_to_char);
  std::string home_env_str = "";

  size_t i = 0;
  while (home[i] != '\0') {
    home_env_str += home[i];
    i++;
  }

  HOME_PATH = home_env_str;
}

ProgramPath::~ProgramPath() {}

void ProgramPath::set_opened_dir(std::string dir) { OPENED_DIR = dir; }

std::string ProgramPath::get_opened_dir() { return OPENED_DIR; }

std::string ProgramPath::get_src_path() { return SOURCE_PATH; }

std::string ProgramPath::get_home_path() { return HOME_PATH; }

std::string ProgramPath::join_str(std::string source, std::string add) {
  source += return_slash();
  return source += add;
}

std::string ProgramPath::get_logging_path() { return LOG_PATH; }

std::string ProgramPath::return_slash() {
  std::string slash;
#ifdef __linux__
  slash = "/";
  return slash;
#elif _WIN32
  slash = "\\";
  return slash;
#endif
  slash = "NOT SUPPORTED";
  return slash;
}

std::string ProgramPath::get_platform_home() {
#ifdef __linux__
  return "HOME";
#endif

#ifdef _WIN32
  return "USERPROFILE";
#endif

  return "NOT_SUPPORTED";
}

bool ProgramPath::make_directory(const std::string path) {
  const char *path_c_str = path.c_str();
}

bool ProgramPath::create_music_source() {
  if (HOME_PATH == "NOT_SUPPORTED") {
    fprintf(stdout, "Platform not supported\n");
    return false;
  }

  const std::string slash = return_slash();
  const std::string music_directory = "Music";
  const std::string music_path = HOME_PATH + slash + music_directory;

//make a parent music path here for folders

  const std::string music_source_path = music_path + slash + "MVSource";

//create the source dir for folders containing audio files

  SOURCE_PATH = music_source_path;
  return true;
}

bool ProgramPath::create_log_directories() {
  if (HOME_PATH == "NOT_SUPPORTED") {
    fprintf(stdout, "Platform not supported\n");
    return false;
  }

  const std::string slash = return_slash();
  const std::string music_directory = "Music";
  const std::string music_path = HOME_PATH + slash + music_directory;

//make a parent music path here for folders

  const std::string program_log_path = music_path + slash + "MVLogs";

//create the source dir for folders containing log files

  LOG_PATH = program_log_path;
  return true;
}
