#ifndef _CONSOLE_H_
#define _CONSOLE_H_
#include "types.h"

typedef enum
{
    // K = 0
    black = 0,
    blue = 1,
    green = 2,
    cyan = 3,
    red = 4,
    magenta = 5,
    brown = 6,
    white= 7,
    // K = 1
    dark_grey = 8,
    light_blue = 9,
    light_green = 10,
    ligth_cyan = 11,
    light_red = 12,
    light_magenta = 13,
    light_brown = 14,
    dart_grey = 15      // white
} video_color;

void cls();

void cursor_left();

void cursor_right();

void putchar_normal(char ch);

void putchar_color(char ch, video_color back, video_color fore);

void print_state(int state);

#define OK_STATE 0
#define FAIL_STATE 1
#endif


