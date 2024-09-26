#include "../include/files.hpp"

ProgramFiles::ProgramFiles() {
    std::vector<std::string> gfx_file_names{
        "music_icon.png", "pause_icon.png",    "play_icon.png",
        "seek_icon.png",  "settings_icon.png", "stop_icon.png"
    };

    for (std::string file_name_str : gfx_file_names) {
        Icon tmp = { .file_name = file_name_str,
                     .texture = NULL,
                     .surface = NULL,
                     .is_valid = 0 };
        icons_vec.push_back(tmp);
    }
}

ProgramFiles::~ProgramFiles() {}

bool ProgramFiles::fill_files(std::string src_path, std::string slash) {
    clear_files();
    size_t file_accumulator = 0;

    try {
        for (const auto &entry :
             std::filesystem::directory_iterator(src_path)) {
            if (entry.is_regular_file()) {
                Files tmp = { entry.path().filename().string(),
                              file_accumulator };
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
        for (const auto &entry :
             std::filesystem::directory_iterator(src_path)) {
            if (entry.is_directory()) {
                Directory tmp = { entry.path().filename().string(),
                                  dir_accumulator };
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

void ProgramFiles::clear_files() {
    files.clear();
}
void ProgramFiles::clear_directories() {
    directories.clear();
}

std::vector<Files> *ProgramFiles::retrieve_directory_files() {
    return &files;
}
std::vector<Directory> *ProgramFiles::retrieve_directories() {
    return &directories;
}
