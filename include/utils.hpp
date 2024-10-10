#ifndef UTILS_HPP
#define UTILS_HPP
#include <cstdbool>
#include <stdarg.h>
#include <string>
#include <unordered_map>

int scc(int code);
void* scp(void* ptr);
void close_output_files();

void *find_key(std::string key,
               std::unordered_map<std::string, void *> *map_ptr);

bool check_ptrs(size_t length, ...);
#endif
