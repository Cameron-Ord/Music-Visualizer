#ifndef MACRO_H
#define MACRO_H
#define BUFF_SIZE (1 << 12)
#define DOUBLE_BUFF (BUFF_SIZE * 2)
#define HALF_BUFF (BUFF_SIZE / 2)
#define HALF_DOUBLE (DOUBLE_BUFF / 2)
#define WIN_THREAD_COUNT 3
#define PRINT_SDL_ERR(stream, msg) fprintf(stream, "%s\n", msg)
#define PRINT_STR_ERR(stream, context, msg) fprintf(stream, "%s : %s\n", context, msg)
#define SCROLLBAR_WIDTH 24
#define SCROLLBAR_HEIGHT 10
#define SCROLLBAR_HEIGHT_OFFSET (SCROLLBAR_HEIGHT / 2) - 1
#define SCROLLBAR_OFFSET (SCROLLBAR_WIDTH / 2)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define ABS(x) (((x) < 0) ? -(x) : (x))
#define DIFF(a, b) ABS((a) - (b))
#define TRUE 1
#define FALSE 0
#define FPS 60
#define TICKS_PER_FRAME (1000.0 / FPS)
#define BWIDTH 600
#define BHEIGHT 400
#define M_PI 3.14159265358979323846
#define Y_OFFSET(x) (x + 50)
#define free_ptr(ptr)                                                                                        \
  if ((ptr) != NULL) {                                                                                       \
    free(ptr);                                                                                               \
    (ptr) = NULL;                                                                                            \
  }

#define SIZE32 32
#define SIZE16 16
#define FONT_PATH "Roboto-Regular.ttf"
#define SETTINGS_ICON_PATH "settings_icon.png"
#define PLAY_ICON_PATH "play_icon.png"
#define STOP_ICON_PATH "stop_icon.png"
#define PAUSE_ICON_PATH "pause_icon.png"
#define SEEKER_ICON_PATH "seek_icon.png"
#endif
