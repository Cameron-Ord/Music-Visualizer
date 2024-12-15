#include "events.h"

void auto_play_nav(const size_t size, size_t *curs) {
  int tmp_i = (int)*curs;
  int size_signed = (int)size;
  tmp_i++;

  if (tmp_i > size_signed - 1) {
    tmp_i = 0;
  }

  if (tmp_i < 0) {
    tmp_i = size_signed - 1;
  }

  *curs = tmp_i;
}

static int get_i(const char *direction);

static int get_i(const char *direction) {
  if (strcmp("--", direction) == 0) {
    return -1;
  }

  if (strcmp("++", direction) == 0) {
    return 1;
  }

  return 0;
}

int node_index(const char *direction, int node_index, int max) {
  int tmp_index = node_index;
  tmp_index += get_i(direction);
  if (tmp_index > max - 1) {
    tmp_index = max - 1;
  }

  if (tmp_index < 0) {
    tmp_index = 0;
  }

  return tmp_index;
}

static int clamp(int size, int curs) {
  if (curs < 0)
    curs = 0;

  if (curs > size - 1)
    curs = size - 1;

  return curs;
}

static int clamp_wrap(int size, int curs) {
  if (curs < 0)
    curs = size - 1;

  if (curs > size - 1)
    curs = 0;

  return curs;
}

size_t auto_nav_down(const size_t cursor, const size_t max) {
  int curs = (int)cursor;

  curs++;
  curs = clamp_wrap((int)max, curs);

  return curs;
}

size_t mv_start_pos(const int n, const size_t start, const size_t max) {
  int incremented = (int)start + n;
  printf("before %d\n", incremented);
  incremented = clamp((int)max, incremented);
  printf("after %d\n", incremented);
  return incremented;
}

size_t nav_down(const size_t cursor, const size_t max) {
  int curs = (int)cursor;

  curs++;
  curs = clamp((int)max, curs);

  return curs;
}

size_t nav_up(const size_t cursor, const size_t max) {
  int curs = (int)cursor;

  curs--;
  curs = clamp((int)max, curs);

  return curs;
}
