#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include "filesysdefs.h"

void *free_paths(Paths *buf, const size_t *count);
const char *find_pathstr(const char *search_key, Paths *buffer);
int find_type(const char *search_key, Paths *buffer);
const size_t *get_name_length(const char *search_key, Paths *buffer);

#ifdef _WIN32
#include <windows.h>
Paths *win_fs_search(const char *pathstr);
Paths *win_read_dir(const char *pathstr);
int get_file_type(const DWORD type);
#endif

#ifdef __linux__
#include <dirent.h>
Paths *unix_fs_search(const char *pathstr);
Paths *unix_read_dir(const char *path);
int get_file_type(const int type);
#endif

#endif
