#ifndef FILESYSDEFS_H
#define FILESYSDEFS_H

#include <stdint.h>
#include <stdlib.h>

typedef enum { TYPE_DIRECTORY = 0, TYPE_FILE = 1, UNKNOWN = -1 } FT_ENUM;

typedef struct {
  char *name;
  size_t name_length;
  char *path;
  size_t path_length;
  size_t size;
  int type;
  int is_valid;
} Paths;

#endif
