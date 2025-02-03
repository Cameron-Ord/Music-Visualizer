#ifndef FILESYSDEFS_H
#define FILESYSDEFS_H

#include <stddef.h>

typedef enum { TYPE_DIRECTORY = 0, TYPE_FILE = 1, UNKNOWN = -1 } FT_ENUM;

#define PBUFMAX 256

typedef struct {
  char *path;
  size_t length;
} StrVals;

struct Paths {
  StrVals name;
  StrVals path;
  size_t size;
  int id;
  int type;
  int is_valid;
};

typedef struct Paths Paths;

#endif
