#include "../include/macdefs.hpp"
#include "../include/program_path.hpp"
#include <errno.h>
#include <string.h>

std::string ProgramPath::join_str(std::string source, std::string add) {
  source += return_slash();
  return source += add;
}

std::string ProgramPath::get_logging_path() { return LOG_PATH; }

std::string ProgramPath::return_slash() {
#ifdef __linux__
  return "/";
#endif

#ifdef _WIN32
  return "\";
#endif
      return "NOT_SUPPORTED";
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

int ProgramPath::make_directory(const std::string path, const mode_t mode) {
  const char *path_c_str = path.c_str();
#ifdef __linux__
  if (mkdir(path_c_str, mode) == 0) {
    return 1;
  }
  return 0;
#endif

#ifdef _WIN32
  if (mkdir(path_c_str) == 0) {
    return 1;
  }
  return 0;
#endif
  return 0;
}

int ProgramPath::create_music_source() {
  if (HOME_PATH == "NOT_SUPPORTED") {
    fprintf(stdout, "Platform not supported\n");
    return 0;
  }

  const std::string slash = return_slash();
  const std::string music_directory = "Music";
  const std::string music_path = HOME_PATH + slash + music_directory;

  mode_t mode = S_IRWXU;

  if (!make_directory(music_path, mode)) {
    if (errno != EEXIST) {
      fprintf(stderr, "Failed to create directory! -> %s\n", strerror(errno));
      return 0;
    }
  }

  const std::string music_source_path = music_path + slash + "MVSource";
  fprintf(stdout, "%s\n", music_source_path.c_str());
  if (!make_directory(music_source_path, mode)) {
    if (errno != EEXIST) {
      fprintf(stderr, "Failed to create directory! -> %s\n", strerror(errno));
      return 0;
    }
  }

  SOURCE_PATH = music_source_path;

  return 1;
}

int ProgramPath::create_log_directories() {
  if (HOME_PATH == "NOT_SUPPORTED") {
    fprintf(stdout, "Platform not supported\n");
    return 0;
  }

  const std::string slash = return_slash();
  const std::string music_directory = "Music";
  const std::string music_path = HOME_PATH + slash + music_directory;

  mode_t mode = S_IRWXU;

  if (!make_directory(music_path, mode)) {
    if (errno != EEXIST) {
      fprintf(stderr, "Failed to create directory! -> %s\n", strerror(errno));
      return 0;
    }
  }

  const std::string program_log_path = music_path + slash + "MVLogs";
  fprintf(stdout, "%s\n", program_log_path.c_str());
  if (!make_directory(program_log_path, mode)) {
    if (errno != EEXIST) {
      fprintf(stderr, "Failed to create directory! -> %s\n", strerror(errno));
      return 0;
    }
  }

  LOG_PATH = program_log_path;
  return 1;
}

ProgramPath::ProgramPath() {
  const std::string platform_home = get_platform_home();
  const char *home_to_char = platform_home.c_str();

  size_t i = 0;
  char *home = getenv(home_to_char);
  std::string home_env_str;

  while (home[i] != '\0') {
    home_env_str += home[i];
    i++;
  }

  HOME_PATH = home_env_str;
}

ProgramPath::~ProgramPath() {}
