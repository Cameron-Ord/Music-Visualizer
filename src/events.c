#include "../inc/events.h"
#include <string.h>

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
