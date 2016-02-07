#ifndef _PAGE_FAULT_H_
#define _PAGE_FAULT_H_
#include "idt.h"

void page_fault(regs_pt *regs);

#endif
