#ifndef UTILS_HPP
#define UTILS_HPP
#include <cstdbool>
#include <string>
#include <unordered_map>

void *find_key(std::string key,
               std::unordered_map<std::string, void *> *map_ptr);

bool check_ptrs(size_t length, void *ptr_buf[]);

#endif
