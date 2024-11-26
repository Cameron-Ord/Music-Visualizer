#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include "filesysdefs.h"

#ifdef _WIN32
#include <windows.h>
Paths *win_find_directories(size_t *count);
Paths *win_find_files(size_t *count, const char *path);
#endif

#ifdef __linux__
#include <dirent.h>
Paths *unix_fs_search(const char *pathstr, const char *home);
Paths *unix_read_dir(const char *path);
int not_nav(const char *str);
int not_hidden(const char *str);
#endif

#endif
