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
    light_grey = 7,
    // K = 1
    dark_grey = 8,
    light_blue = 9,
    light_green = 10,
    ligth_cyan = 11,
    light_red = 12,
    light_magenta = 13,
    light_brown = 14,
    white = 15
} video_color;

void cls();

#endif


