#ifndef TIMER_H_
#define TIMER_H_

#include "types.h"

#define TIMER_FREQUENCY 1193180
#define TIMER_PORT 0x43
#define TIMER_COUNT 50

extern uint32_t ticks;

void init_timer();

#endif
