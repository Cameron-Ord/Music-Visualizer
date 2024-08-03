#include "../inc/utils.h"
#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/graphics.h"

f32
clamp(f32 vol, f32 amount, f32 min, f32 max) {
  f32 sum = vol += amount;
  if (sum < min) {
    return min;
  }
  if (sum > max) {
    return max;
  }
  return sum;
}

char*
check_sign(int num) {
  if (num > 0) {
    return "Positive";
  }

  if (num < 0) {
    return "Negative";
  }

  return "Zero";
}

int
check_pos(u32 audio_pos, u32 len) {
  if (audio_pos > 0 && audio_pos < len) {
    return 1;
  }
  return 0;
}

int
status_check(i8 status[]) {
  if (status[0] && status[1]) {
    return TRUE;
  }

  return FALSE;
}

int
point_in_rect(int x, int y, SDL_Rect rect) {
  return (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);
} /*point_in_rect*/

int
within_bounds_x(int x, int start, int end) {
  if (x >= start && x <= end) {
    return 1;
  }
  return 0;
}

int
find_clicked_song(FontData sf_arr[], int file_count, const int mouse_arr[]) {
  /*If the pointer is within the rectangle of any of these song titles, return the id(The id is determined by
   * the index inside the loop where the fonts are created, so it's essentially an index as well)*/
  for (int i = 0; i < file_count; i++) {
    SDL_Rect sf_rect = sf_arr[i].font_rect;
    if (point_in_rect(mouse_arr[0], mouse_arr[1], sf_rect)) {
      return sf_arr[i].id;
    }
  }
  return -1;
} /*find_clicked_song*/

FontData*
find_clicked_theme(FontData col[], const int mouse_arr[]) {
  for (int i = 0; i < COLOUR_LIST_SIZE; i++) {
    SDL_Rect rect = col[i].font_rect;
    if (point_in_rect(mouse_arr[0], mouse_arr[1], rect)) {
      return &col[i];
    }
  }
  return NULL;
}

char*
find_clicked_dir(FontData df_arr[], int dir_count, const int mouse_arr[]) {
  for (int i = 0; i < dir_count; i++) {
    SDL_Rect df_rect = df_arr[i].font_rect;
    if (point_in_rect(mouse_arr[0], mouse_arr[1], df_rect)) {
      return df_arr[i].text;
    }
  }
  return "NO_SELECTION";
} /*find_clicked_dir*/

void
get_window_container_size(SDL_Window* w, int* width, int* height) {
  SDL_GetWindowSize(w, width, height);
}
