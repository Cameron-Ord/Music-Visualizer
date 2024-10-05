#ifndef UTILS_HPP
#define UTILS_HPP
#include <string>
#include <unordered_map>

void *find_key(std::string key,
               std::unordered_map<std::string, void *> *map_ptr);
#endif