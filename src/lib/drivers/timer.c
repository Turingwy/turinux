#include "ioport.h"
#include "console.h"
#include "idt.h"
#include "types.h"
#include "timer.h"

void timer_interrupt(regs_pt *regs) {
	static int tick_count = 0;
	printk("tick_count:%d\r", tick_count++);
}

void init_timer() {
	register_handler(32, timer_interrupt);
	
	outb(TIMER_PORT, 0x34);
	outb(TIMER_PORT-3, (0xFFFF) & 0xFF);
	outb(TIMER_PORT-3, ((0xFFFF) >> 8) & 0xFF);

}
