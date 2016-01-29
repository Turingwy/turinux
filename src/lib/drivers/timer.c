#include "ioport.h"
#include "console.h"
#include "idt.h"
#include "types.h"
#include "timer.h"
#include "proc.h"

uint32_t ticks;

void timer_interrupt(regs_pt *regs) {
    ticks++;
    wakeup(&ticks);
    if(regs->cs & 0x03 == 0x03)
        sched();
}

void init_timer() {
	register_handler(32, timer_interrupt);
	
	outb(TIMER_PORT, 0x34);
	outb(TIMER_PORT-3, (0xFFFF) & 0xFF);
	outb(TIMER_PORT-3, ((0xFFFF) >> 8) & 0xFF);

}
