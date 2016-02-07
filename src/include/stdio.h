#ifndef _STDIO_H_
#define _STDIO_H_


#ifndef NULL
#define NULL 0
#endif

int putchar(char c);

int puts(char *str);

void printf(char *format, ...);

// kernel mode
void printk(char *format, ... );
#endif
