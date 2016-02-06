#include "dev.h"
#include "types.h"
#include "fs.h"
#include "proc.h"
#include "console.h"
#include "kbd.h"

struct dev devs[DEVSCNT];
int conwrite(struct inode *, char *, int);
int conread(struct inode *, char *, int);

void consoleinit()
{
    devs[CONSOLE].write = conwrite;
    devs[CONSOLE].read = conread;
}

int conwrite(struct inode *in, char *addr, int len)
{
    iunlock(in);
    for(int i = 0; i < len && addr[i]; i++)
        putchar_normal(addr[i]);
    ilock(in);
    return len;
}

struct {
    char buf[256];
    uint8_t r;
    uint8_t w;
    uint8_t s;
} input;

int conread(struct inode *in , char *addr, int len)
{
    input.r = input.w;
    input.s = 1;
    while(input.r == input.w)
    {
        if(current_proc->killed)
            return -1;
            sleep(&input.r, 0);
    }
    input.s = 0;
    int i;
    for(i = 0; input.r != input.w; input.r=(input.r+1)%256, i++)
        *addr++ = input.buf[input.r];
    addr--;
    *addr = '\0';
    return i;
}

void flush_input(int old_len, int new_len, char *str)
{
    for(int i = 0; i < old_len; i++)
        putchar_normal('\b');
    
    for(int i = 0;i < new_len; i++)
        putchar_normal(str[i]);
}

void kbd_handler(regs_pt *regs)
{
    static int max_len;
    static int cur_len;
    if(input.s == 1)
    {
        max_len = 0;
        cur_len = 0;
        input.s = 2;
    } 

    if(input.s != 2)
        return;
    
    int ch = kbdgetc();
    if(!ch)
        return;
    switch(ch)
    {
        case KEY_LF:
            if(cur_len > 0) {
                cursor_left();
                cur_len--;
            }
            return;
        case KEY_RT:
            if(cur_len < max_len) {
                cursor_right();
                cur_len++;
            }
            return;
        case '\b':
            if(cur_len <= 0)
                return;
            cur_len--;
            max_len--;
            for(int i = (input.r+cur_len)%256; i != input.w; i=(i+1)%256)
            {
                input.buf[i] = input.buf[(i+1)%256];
            }
            input.w=(input.w-1)%256;
            for(int i = cur_len; i < max_len; i++)
                    cursor_right();
            flush_input(max_len+1, max_len, &input.buf[input.r]);
            for(int i = cur_len; i < max_len; i++)
                    cursor_left();
            return;
        case '\n':
            input.w=(input.w+1)%256;
            for(int i = cur_len; i < max_len; i++)
                    cursor_right();
            putchar_normal('\n');
            wakeup(&input.r);
            return;
    }

    // queue is full.
    if((input.w + 1)%256 == input.r)
    {
        wakeup(&input.r);
        return;
    }
        
    int i;
    for(i = input.w; i != (input.r+cur_len)%256; i=(i-1)%256)
        input.buf[i] = input.buf[(i-1)%256];
    input.buf[i] = ch;

    max_len++;
    cur_len++;
    for(i = cur_len-1; i < max_len-1; i++)
        cursor_right();
    flush_input(max_len-1, max_len, &input.buf[input.r]);
    for(i = cur_len; i < max_len; i++)
        cursor_left();
    input.w=(input.w+1)%256;
    input.w%=256;

}

void kbdinit()
{
    register_handler(33, kbd_handler);
}
