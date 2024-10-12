#include "main.h"

size_t nav_down(size_t *cursor, size_t *locn, size_t max_length) {
  int signed_cursor = (int)*cursor;
  int signed_length = (int)max_length;

  signed_cursor++;
  if (signed_cursor > signed_length - 1) {
    signed_cursor = 0;
  }

  *locn += 1;
  fprintf(stdout, "%d - %zu\n", signed_cursor, max_length);
  return (size_t)signed_cursor;
}

size_t nav_up(size_t *cursor, size_t *locn, size_t max_length) {
  int signed_cursor = (int)*cursor;
  int signed_length = (int)max_length;

  signed_cursor--;
  if (signed_cursor < 0) {
    signed_cursor = signed_length - 1;
  }
  *locn -= 1;
  return (size_t)signed_cursor;
}