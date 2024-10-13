#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <stdint.h>


typedef struct {
  size_t dir_count;
  size_t file_count;
} FileSys;

typedef struct {
  char** names;
  char** paths;
}Paths;


#ifdef _WIN32
#include <windows.h>
Paths *win_find_directories(size_t *count);
Paths *win_find_files(size_t *count, char* path);
#endif

#ifdef __linux__
#include <dirent.h>
Paths *unix_find_directories(size_t *count);
Paths *unix_find_files(size_t *count, char* path);
#endif

#endif
