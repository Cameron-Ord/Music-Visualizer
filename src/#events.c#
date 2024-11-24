#include "main.h"
#include "utils.h"

size_t nav_down(size_t *cursor, const size_t *count) {
  int signed_cursor = (int)*cursor;
  int signed_count = (int)*count;

  signed_cursor++;
  if (signed_cursor > signed_count - 1) {
    signed_cursor = 0;
  }

  *cursor = (size_t)signed_cursor;

  return (size_t)signed_cursor;
}

size_t nav_up(size_t *cursor, const size_t *count) {
  int signed_cursor = (int)*cursor;
  int signed_count = (int)*count;

  signed_cursor--;
  if (signed_cursor < 0) {
    signed_cursor = signed_count - 1;
  }

  *cursor = (size_t)signed_cursor;

  return (size_t)signed_cursor;
}

void window_resized(void) {
  SDL_GetWindowSize(win.w, &win.width, &win.height);
  rend.title_limit = get_title_limit(win.height);
  font.char_limit = get_char_limit(win.width);
}

// Arguments are clearly defined in main.c at the window resize event
TextBuffer *font_swap_pointer(TextBuffer *buf, const size_t *count,
                              const Paths *content, TextBuffer *search_buffer,
                              const size_t *s_count) {
  TextBuffer *replace = create_fonts(content, count);
  if (search_buffer && buf) {
    for (size_t i = 0; i < *count; i++) {
      // Prevent invalid access
      if (i >= *s_count) {
        break;
      }

      if (replace[i].text && search_buffer[i].text) {
        if (!search_buffer[i].text->name) {
          continue;
        }

        if (strcmp(replace[i].text->name, search_buffer[i].text->name) == 0) {
          search_buffer[i].text = NULL;
          search_buffer[i].text = replace[i].text;
        }
      }
    }
  }

  free_text_buffer(buf, count);
  return replace;
}

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
