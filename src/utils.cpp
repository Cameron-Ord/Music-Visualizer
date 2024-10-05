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