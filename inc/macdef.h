#ifndef MACDEF_H
#define MACDEF_H

#define TRUE 1
#define FALSE 0
#define FPS 60
#define TICKS_PER_FRAME (1000.0 / FPS)
#define BWIDTH 600
#define BHEIGHT 400
#define M_PI 3.14159265358979323846
#define RELATIVE_WIDTH 0.20

#define BUFF_SIZE (1 << 12)
#define DOUBLE_BUFF (BUFF_SIZE * 2)
#define HALF_BUFF (BUFF_SIZE / 2)
#define HALF_DOUBLE (DOUBLE_BUFF / 2)

#define COLOUR_LIST_SIZE 2
#define SIZE32 32
#define SIZE16 16

#define SCROLLBAR_WIDTH 24
#define SCROLLBAR_HEIGHT 10
#define SCROLLBAR_HEIGHT_OFFSET (SCROLLBAR_HEIGHT / 2) - 1
#define SCROLLBAR_OFFSET (SCROLLBAR_WIDTH / 2)

#define FONT_PATH "dogicapixel.ttf"
#define SETTINGS_ICON_PATH "settings_icon.png"
#define PLAY_ICON_PATH "play_icon.png"
#define STOP_ICON_PATH "stop_icon.png"
#define PAUSE_ICON_PATH "pause_icon.png"
#define SEEKER_ICON_PATH "seek_icon.png"
#define MUSIC_ICON_PATH "music_icon.png"

#endif
