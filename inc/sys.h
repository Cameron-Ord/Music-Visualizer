#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stddef.h>

typedef enum { TYPE_DIRECTORY = 0, TYPE_FILE = 1, UNKNOWN = -1 } FT_ENUM;

typedef struct {
  char *path;
  size_t length;
} StrVals;

struct Paths {
  StrVals name;
  StrVals path;
  size_t size;
  int cursor;
  int type;
  int is_valid;
};

typedef struct Paths Paths;
int paths_check_sanity(const Paths *p);

#ifdef __linux__
#include <dirent.h>
Paths *unix_fs_search(const char *pathstr);
Paths *unix_read_dir(const char *path);
int get_file_type(const int type);
#endif

void free_entry(Paths *p);

#endif
