#include "../include/files.hpp"

ProgramFiles::ProgramFiles() {
  std::vector<std::string> gfx_file_names{"music_icon.png",    "pause_icon.png",
                                          "play_icon.png",     "seek_icon.png",
                                          "settings_icon.png", "stop_icon.png"};

  for (std::string file_name_str : gfx_file_names) {
    Icon tmp = {.file_name = file_name_str,
                .texture = NULL,
                .surface = NULL,
                .is_valid = 0};
    icons_vec.push_back(tmp);
  }
}

ProgramFiles::~ProgramFiles() {}

bool ProgramFiles::fill_files(std::string src_path, std::string slash) {
  DIR *directory = NULL;
  struct dirent *entry;
  const char *path_char_ptr = src_path.c_str();
  int file_accumulator = 0;

  directory = opendir(path_char_ptr);
  if (directory == NULL) {
    fprintf(stderr, "Could not open directory! -> %s\n", strerror(errno));
    return false;
  }

  while ((entry = readdir(directory)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0) {
      continue;
    }

    if (strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    struct stat statbuffer;
    std::string full_path = src_path + slash + entry->d_name;
    if (stat(full_path.c_str(), &statbuffer) != 0) {
      fprintf(stderr, "Stat failed on %s -> %s\n", full_path.c_str(),
              strerror(errno));
      continue;
    }

    if (S_ISREG(statbuffer.st_mode)) {
      Files tmp = {.file_name = "", .file_id = file_accumulator};
      size_t entry_length = strlen(entry->d_name);
      for (size_t i = 0; i < entry_length; i++) {
        tmp.file_name += entry->d_name[i];
      }

      files.push_back(tmp);
      file_accumulator++;
    }
  }

  fprintf(stdout, "Files added -> %d\n", file_accumulator);
  file_count = file_accumulator;
  return true;
}

bool ProgramFiles::fill_directories(std::string src_path, std::string slash) {
  DIR *directory = NULL;
  struct dirent *entry;
  const char *path_char_ptr = src_path.c_str();
  int dir_accumulator = 0;

  directory = opendir(path_char_ptr);
  if (directory == NULL) {
    fprintf(stderr, "Could not open directory! -> %s\n", strerror(errno));
    return false;
  }

  while ((entry = readdir(directory)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0) {
      continue;
    }

    if (strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    struct stat statbuffer;
    std::string full_path = src_path + slash + entry->d_name;
    if (stat(full_path.c_str(), &statbuffer) != 0) {
      fprintf(stderr, "Stat failed on %s -> %s\n", full_path.c_str(),
              strerror(errno));
      continue;
    }

    if (S_ISDIR(statbuffer.st_mode)) {
      Directory tmp = {.directory_name = "", .directory_id = dir_accumulator};
      size_t entry_length = strlen(entry->d_name);
      for (size_t i = 0; i < entry_length; i++) {
        tmp.directory_name += entry->d_name[i];
      }

      directories.push_back(tmp);
      dir_accumulator++;
    }
  }

  fprintf(stdout, "Directories added -> %d\n", dir_accumulator);
  dir_count = dir_accumulator;
  return true;
}

void ProgramFiles::clear_files() {}
void ProgramFiles::clear_directories() {}

std::vector<Files> *ProgramFiles::retrieve_directory_files() { return &files; }
std::vector<Directory> *ProgramFiles::retrieve_directories() {
  return &directories;
}
