#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "types.h"
#include "idt.h"

#define SYSCALL_LEN 64
#define SYS_PUTS 1

typedef int (*syscall_t)();

void syscall(regs_pt *regs);

#endif
