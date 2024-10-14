#include "main.h"

size_t nav_down(size_t *cursor, const size_t *count) {
  int signed_cursor = (int)*cursor;
  int signed_count = (int)*count;

  signed_cursor++;
  if(signed_cursor > signed_count - 1){
    signed_cursor = 0;
  }

  printf("%d\n", signed_cursor);

  *cursor = (size_t)signed_cursor;

  return (size_t)signed_cursor;
}

size_t nav_up(size_t *cursor, const size_t *count) {
  int signed_cursor = (int)*cursor;
  int signed_count = (int)*count;
  
  signed_cursor--;
  if(signed_cursor < 0){
    signed_cursor = signed_count - 1;
  }

  printf("%d\n", signed_cursor);
  *cursor = (size_t)signed_cursor;

  return (size_t)signed_cursor;
}