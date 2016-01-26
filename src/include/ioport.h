#ifndef _IOPORT_H_
#define _IOPORT_H_
#include "types.h"

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t value);

#endif