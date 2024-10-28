#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include "filesysdefs.h"

#ifdef _WIN32
#include <windows.h>
Paths *win_find_directories(size_t *count);
Paths *win_find_files(size_t *count, const char *path);
int win_mkdir(const char *path);
#endif

#ifdef __linux__
#include <dirent.h>
Paths *unix_find_directories(size_t *count);
Paths *unix_find_files(size_t *count, const char *path);
int unix_mkdir(const char *path);
#endif

#endif
