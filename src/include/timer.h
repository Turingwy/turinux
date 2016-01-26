#ifndef TIMER_H_
#define TIMER_H_

#include "types.h"

#define TIMER_FREQUENCY 1193180
#define TIMER_PORT 0x43
#define TIMER_COUNT 50

void init_timer();

#endif
