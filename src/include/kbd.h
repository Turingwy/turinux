#ifndef _KBD_H_
#define _KBD_H_

/*
 * pc keyboard interface.
 */
#include "types.h"

#define KBD_STATPORT 0x64       // kbd controller status port
#define KBD_DIB      0x01       // kbd data in buffer
#define KBD_DATAPORT 0x60       // kbd data port

#define NO           0

#define SHIFT        (1<<0)
#define CTL         (1<<1)
#define ALT          (1<<2)

#define CAPSLOCK     (1<<3)
#define NUMLOCK      (1<<4)
#define SCROLLLOCK   (1<<5)

#define E0ESC        (1<<6)

#define KEY_HOME     0xE0
#define KEY_END      0xE1
#define KEY_UP       0xE2
#define KEY_DN       0xE3
#define KEY_LF       0xE4
#define KEY_RT       0xE5
#define KEY_PGUP    0xE6
#define KEY_PGDN     0xE7
#define KEY_INS      0xE8
#define KEY_DEL      0xE9

static uint8_t shiftcode[256] = 
{
        [0x1D] CTL,
        [0x2A] SHIFT,
        [0x36] SHIFT,
        [0x38] ALT,
        [0x9D] CTL,
        [0xB8] ALT
};

static uint8_t togglecode[256] = 
{
        [0x3A] CAPSLOCK,
        [0x45] NUMLOCK,
        [0x46] SCROLLLOCK
};

static uint8_t normalmap[256] =
{
    NO, 0x1B, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 
    '\t','q', 'w', 'e', 'r', 't', 'y', 'u', 'i','o', 'p', '[', ']', '\n', 
    NO, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    NO, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', NO, 
    '*', // 0x30

    NO,     // ALT
    ' ',    // Space
    NO,     // Caps Lock
    NO, NO, NO, NO, NO, NO, NO, NO, NO, NO,     // F1~F10
    NO, NO, '7', // 0x40
    '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.', NO, NO, NO, NO, // 0x50
    [0x9C] '\n', // KP_Enter
    [0xB5] '/', // KP_Div
    [0xC8] KEY_UP, [0xD0] KEY_DN,
    [0xC9] KEY_PGUP, [0xD1] KEY_PGDN,
    [0xCB] KEY_LF, [0xCD] KEY_RT,
    [0x97] KEY_HOME, [0xCF] KEY_END,
    [0xD2] KEY_INS, [0xD3] KEY_DEL
};

// keyboard map when shift key.
static uint8_t shiftmap[256] = 
{
    NO, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', 
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 
    NO, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    NO, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', NO, 
    
    '*',
    NO, ' ', NO, NO, NO, NO, NO, NO,
    NO, NO, NO, NO, NO, NO, NO, '7', // 0x40
    '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.', NO, NO, NO, NO, // 0x50
    [0x9C] '\n', // KP_Enter
    [0xB5] '/', // KP_Div
    [0xC8] KEY_UP, [0xD0] KEY_DN,
    [0xC9] KEY_PGUP, [0xD1] KEY_PGDN,
    [0xCB] KEY_LF, [0xCD] KEY_RT,
    [0x97] KEY_HOME, [0xCF] KEY_END,
    [0xD2] KEY_INS, [0xD3] KEY_DEL
};

int kbdgetc();

#endif
