#include "utils.h"
#include <stdlib.h>
#include <string.h>

size_t get_length(size_t size, ...) {
  va_list args;
  va_start(args, size);
  size_t total = 0;

  for (size_t i = 0; i < size; i++) {
    size_t str_length = va_arg(args, size_t);
    total += str_length;
  }

  return total;
}

void free_ptrs(size_t size, ...) {
  va_list args;
  va_start(args, size);
  for (size_t i = 0; i < size; i++) {
    void *ptr = va_arg(args, void *);
    if (ptr) {
      free(ptr);
      ptr = NULL;
    }
  }
}

char *find_char_str(const char* search_key, Paths* buffer){
    Paths *start = buffer;

    while(buffer != NULL){
        if(strcmp(search_key, buffer->name) == 0){
            return buffer->path;
        }
        buffer++;
    }

    buffer = start;
    return NULL;
}
