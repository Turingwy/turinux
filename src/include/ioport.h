#ifndef _IOPORT_H_
#define _IOPORT_H_
#include "types.h"

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t value);

static inline void outsl(int port, const void *addr, int cnt)
{
    asm volatile("cld; rep outsl":
                    "=S"(addr), "=c"(cnt):
                    "d"(port), "0"(addr), "1"(cnt):
                    "cc");
}

static inline void insl(int port, const void *addr, int cnt)
{
    asm volatile("cld; rep insl" :
         "=D" (addr), "=c" (cnt) :
         "d" (port), "0" (addr), "1" (cnt) :
         "memory", "cc");
}
#endif
