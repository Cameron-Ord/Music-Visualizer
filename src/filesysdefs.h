#ifndef FILESYSDEFS_H
#define FILESYSDEFS_H

#include <stdint.h>
#include <stdlib.h>

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
