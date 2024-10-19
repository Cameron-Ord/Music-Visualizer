#ifndef FILESYSDEFS_H
#define FILESYSDEFS_H

#include <stdint.h>
#include <stdlib.h>

typedef struct {
  size_t dir_count;
  size_t file_count;
} FileSys;

typedef struct {
  char *name;
  size_t name_length;
  char *path;
  size_t path_length;
} Paths;

#endif
