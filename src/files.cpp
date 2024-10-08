#include "../include/files.hpp"
#include <filesystem>

ProgramFiles::ProgramFiles() {
  std::vector<std::string> gfx_file_names{"music_icon.png",    "pause_icon.png",
                                          "play_icon.png",     "seek_icon.png",
                                          "settings_icon.png", "stop_icon.png"};
}

ProgramFiles::~ProgramFiles() {}

bool ProgramFiles::fill_files(std::string src_path, std::string slash) {
  clear_files();
  size_t file_accumulator = 0;

  try {
    for (const auto &entry : std::filesystem::directory_iterator(src_path)) {
      if (entry.is_regular_file()) {
        const std::string const_entry_path = entry.path().string();
        std::string entry_path = entry.path().string();
        std::string entry_name = entry.path().filename().string();
        // Non ascii characters were causing issues so this is just how
        // I am dealing with it for now
        for (size_t i = 0; i < entry_path.size(); i++) {
          for (size_t j = 0; j < entry_name.size(); j++) {
            if (entry_path[i] == entry_name[j]) {
              if (static_cast<unsigned char>(entry_path[i]) > 127) {
                entry_path[i] = '_';
                entry_name[j] = '_';
              }
            }
          }
        }

        std::filesystem::rename(const_entry_path.c_str(), entry_path.c_str());
        Files tmp = {entry_name, file_accumulator};
        files.push_back(tmp);
        file_accumulator++;
      }
    }
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Could not open directory! -> " << e.what() << std::endl;
    return false;
  }

  fprintf(stdout, "Files added -> %zu\n", file_accumulator);
  file_count = file_accumulator;
  return true;
}

bool ProgramFiles::fill_directories(std::string src_path, std::string slash) {
  clear_directories();
  size_t dir_accumulator = 0;

  try {
    for (const auto &entry : std::filesystem::directory_iterator(src_path)) {
      if (entry.is_directory()) {
        const std::string const_entry_path = entry.path().string();
        std::string entry_path = entry.path().string();
        std::string entry_name = entry.path().filename().string();
        // Non ascii characters were causing issues so this is just how
        // I am dealing with it for now
        for (size_t i = 0; i < entry_path.size(); i++) {
          for (size_t j = 0; j < entry_name.size(); j++) {
            if (entry_path[i] == entry_name[j]) {
              if (static_cast<unsigned char>(entry_path[i]) > 127) {
                entry_path[i] = '_';
                entry_name[j] = '_';
              }
            }
          }
        }
        std::filesystem::rename(const_entry_path.c_str(), entry_path.c_str());
        Directory tmp = {entry_name, dir_accumulator};
        directories.push_back(tmp);
        dir_accumulator++;
      }
    }
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Could not open directory! -> " << e.what() << std::endl;
    return false;
  }

  dir_count = dir_accumulator;
  return true;
}

void ProgramFiles::clear_files() { files.clear(); }
void ProgramFiles::clear_directories() { directories.clear(); }

std::vector<Files> *ProgramFiles::retrieve_directory_files() { return &files; }
std::vector<Directory> *ProgramFiles::retrieve_directories() {
  return &directories;
}
