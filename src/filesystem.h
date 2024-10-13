#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <stdint.h>
#include "filesysdefs.h"


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
