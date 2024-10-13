#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  size_t dir_count;
  size_t file_count;
} FileSys;

typedef struct {
  char** names;
  char** paths;
}Paths;



Paths *win_find_directories(size_t *count);
Paths *unix_find_directories(size_t *count);

#endif
