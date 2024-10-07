#include "../include/utils.hpp"

void *find_key(std::string key,
               std::unordered_map<std::string, void *> *map_ptr) {
  auto it = map_ptr->find(key);
  if (it != map_ptr->end()) {
    return it->second;
  } else {
    return nullptr;
  }
}

// Utility function for checking ptrs before dereferencing them. Really should
// use this in more places, as I am dereferencing pointers frequently..
bool check_ptrs(size_t length, void *ptr_buf[]) {
  for (size_t i = 0; i < length; i++) {
    if (!ptr_buf[i]) {
      return false;
    }
  }

  return true;
}
