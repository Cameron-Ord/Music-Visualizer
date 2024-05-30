#ifndef MACRO_H
#define MACRO_H
#define PRINT_SDL_ERR(stream, msg) fprintf(stream, "%s\n", msg)
#define PRINT_STR_ERR(stream, context, msg) fprintf(stream, "%s : %s\n", context, msg)
#define FONT_PATH "/usr/share/fonts/fftplayer_fonts/OpenSans-Semibold.ttf"
#define SCROLLBAR_WIDTH 20
#define SCROLLBAR_HEIGHT 10
#define SCROLLBAR_HEIGHT_OFFSET (SCROLLBAR_HEIGHT / 2)
#define SCROLLBAR_OFFSET (SCROLLBAR_WIDTH / 2)
#define N (1 << 12)
#define DOUBLE_N (N * 2)
#define HALF_N (N / 2)
#define HALF_DOUB (DOUBLE_N / 2)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define ABS(x) (((x) < 0) ? -(x) : (x))
#define DIFF(a, b) ABS((a) - (b))
#define TRUE 1
#define FALSE 0
#define FPS 60
#define TICKS_PER_FRAME (1000.0 / FPS)
#define BWIDTH 1000
#define BHEIGHT 800
#define M_PI 3.14159265358979323846
#define CONST_YPOS 50

#endif
