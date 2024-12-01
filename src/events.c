#include "main.h"
#include "utils.h"

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

void nav_down(TextBuffer *tbuf) {
  if (!tbuf) {
    return;
  }

  int cursor = (int)tbuf->cursor;
  int start = (int)tbuf->start;
  const int listed = (int)tbuf->listed;
  const int size = (int)tbuf->size;

  cursor++;
  cursor = clamp(size, cursor);
  int offset = 6;

  if (cursor + offset >= listed) {
    // call min_titles, set the tbuf->max variable and return a 0 or 1 depending
    // on whether there are excess titles depending on the current window height
    if (min_titles(tbuf) && (size - start) > tbuf->max) {
      start += (cursor + offset) - listed;
    }
    start = clamp(size, start);
  }

  tbuf->start = (size_t)start;
  tbuf->cursor = (size_t)cursor;
}

void nav_up(TextBuffer *tbuf) {
  if (!tbuf) {
    return;
  }

  int cursor = (int)tbuf->cursor;
  int start = (int)tbuf->start;
  const int size = (int)tbuf->size;

  cursor--;
  cursor = clamp(size, cursor);
  int offset = 6;

  if (cursor <= start + offset) {
    start -= (start - cursor) + offset;
    start = clamp(size, start);
  }

  tbuf->start = (size_t)start;
  tbuf->cursor = (size_t)cursor;
}

void window_resized(void) {
  SDL_GetWindowSize(win.w, &win.width, &win.height);
  font.char_limit = get_char_limit(win.width);
}
