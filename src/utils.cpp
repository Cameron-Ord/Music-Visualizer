#include "../include/utils.hpp"
#include "../include/globals.hpp"
#include "SDL2/SDL_error.h"
#include <iostream>

void *find_key(std::string key,
               std::unordered_map<std::string, void *> *map_ptr) {
  auto it = map_ptr->find(key);
  if (it != map_ptr->end()) {
    return it->second;
  } else {
    return nullptr;
  }
}

void close_output_files() {
  if (check_ptrs(2, logs.stdout_file, logs.stderr_file)) {
    fclose(logs.stderr_file);
    fclose(logs.stdout_file);
  }
}

// Utility function for checking ptrs before dereferencing them. Really should
// use this in more places, as I am dereferencing pointers frequently..
bool check_ptrs(size_t length, ...) {
  va_list args;
  va_start(args, length);

  for (size_t i = 0; i < length; i++) {
    void *ptr = va_arg(args, void *);
    if (!ptr) {
      return false;
    }
  }

  return true;
}

void *scp(void *ptr) {
  if (!ptr) {
    std::cerr << "SDL failed to create pointer! -> " << SDL_GetError()
              << std::endl;
    close_output_files();
    exit(1);
  }

  return ptr;
}

int scc(int code) {
  if (code < 0) {
    std::cerr << "SDL failed to perform task! -> " << SDL_GetError()
              << std::endl;
    close_output_files();
    exit(1);
  }

  return code;
}