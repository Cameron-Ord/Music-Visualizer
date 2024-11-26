#include "main.h"
#include "utils.h"

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

size_t nav_down(TextBuffer *tbuf) {
  int cursor = (int)tbuf->cursor;
  int start = (int)tbuf->start;
  const int listed = (int)tbuf->listed;
  const int size = (int)tbuf->size;

  cursor++;
  cursor = clamp(size, cursor);
  int offset = 6;

  if (cursor + offset >= listed) {
    if ((size - start) >= 12) {
      start += (cursor + offset) - listed;
    }
    start = clamp(size, start);
  }

  tbuf->start = (size_t)start;
  tbuf->cursor = (size_t)cursor;
  return (size_t)cursor;
}

size_t nav_up(TextBuffer *tbuf) {
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
  return (size_t)cursor;
}

void window_resized(void) {
  SDL_GetWindowSize(win.w, &win.width, &win.height);
  rend.title_limit = get_title_limit(win.height);
  font.char_limit = get_char_limit(win.width);
}
