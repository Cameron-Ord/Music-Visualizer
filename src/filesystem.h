#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{
    char **dir_titles;
    size_t dir_count;
    char **file_titles;
    size_t file_count;
} FileSys;

char** win_find_directories(size_t *count);
char* unix_find_directories(void);

#endif
