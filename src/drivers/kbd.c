#include "kbd.h"
#include "idt.h"
#include "ioport.h"

int kbdgetc()
{
    static uint32_t shift;
    static uint8_t *charcode[2] = {
    normalmap, shiftmap
    };
    uint32_t st, data, c;
    
    st = inb(KBD_STATPORT);
    if((st & KBD_DIB) == 0)
         return -1;
    data = inb(KBD_DATAPORT);
    
    if(data == 0xE0){
        shift |= E0ESC;
        return 0;
    } else if(data & 0x80){
    // Key released
        data = (shift & E0ESC ? data : data & 0x7F);
        shift &= ~(shiftcode[data] | E0ESC);
        return 0;
    } else if(shift & E0ESC){
    // Last character was an E0 escape; or with 0x80
        data |= 0x80;
        shift &= ~E0ESC;
}

    shift |= shiftcode[data];
    shift ^= togglecode[data];
    c = charcode[shift & (CTL | SHIFT)][data];
    if(shift & CAPSLOCK){
        if('a' <= c && c <= 'z')
            c += 'A' - 'a';
        else if('A' <= c && c <= 'Z')
         c += 'a' - 'A';
    } 
    return c;
}

