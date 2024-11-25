#include "main.h"
#include "utils.h"

size_t nav_down(TextBuffer *tbuf) {
  int offset = 4;
  int cursor = (int)tbuf->cursor;
  int start = (int)tbuf->start;
  int listed = (int)tbuf->listed;
  const int size = (int)tbuf->size;

  cursor++;
  if (cursor > size - 1) {
    cursor = size - 1;
  }

  if ((listed - offset) > 0 && cursor >= listed - offset) {
    start = cursor - offset;
  }

  if (start < 0) {
    start = 0;
  }

  if (start > size - 1) {
    start = size - 1;
  }

  tbuf->start = (size_t)start;
  tbuf->cursor = (size_t)cursor;
  return (size_t)cursor;
}

size_t nav_up(TextBuffer *tbuf) {
  int offset = 4;
  int cursor = (int)tbuf->cursor;
  int start = (int)tbuf->start;

  cursor--;
  if (cursor < 0) {
    cursor = 0;
  }

  if (cursor - offset < start) {
    start = start - offset;
  }

  if (start < 0) {
    start = 0;
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

// Arguments are clearly defined in main.c at the window resize event

void char_buf_insert(const char *text, char **input_buf, size_t *pos,
                     size_t *size, Text **search_text) {
  size_t len = strlen(text);
  for (size_t i = 0; i < len; i++) {
    append_char(&text[i], input_buf, pos, size);
  }

  // Send the position along with the size so we know where to assign
  // the null terminator.
  *search_text = destroy_search_text(*search_text);
  *search_text = create_search_text(*input_buf, size, pos);
}
