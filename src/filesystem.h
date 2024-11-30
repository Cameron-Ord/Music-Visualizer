#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include "filesysdefs.h"

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
