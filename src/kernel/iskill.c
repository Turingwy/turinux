#include "types.h"
#include "proc.h"
#include "idt.h"

void iskill(regs_pt *regs)
{
    if(current_proc->killed == 1) {
        exit();
    }
}
