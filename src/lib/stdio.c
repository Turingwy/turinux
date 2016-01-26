#include "types.h"
#include "console.h"
#include "stdarg.h"
#include "string.h"

static void putstr(char *str, video_color back, video_color fore) 
{
    while(*str != '\0') 
    {
        putchar_color(*str++, back, fore);   
    }
}

void printk(char *format, ... ) 
{
    va_list args;
    va_start(args, format);
    char buffer[1024];
    int n = 0;
    while(*format != 0) 
    {
        switch (*format) 
        {
        case '%':
                switch (*++format) 
                {
                case 'c':
                    buffer[n++] = va_arg(args, char);
                    break;
                case 'd': 
                {
                    int value = va_arg(args, int);
                    if(value < 0) 
                    {
                        value = -value;
                        buffer[n++] = '-';
                    }
                    n += itoa(value, buffer+n, 10);
                    break;
                }
                case 'p':
                case 'x':
                    buffer[n++] = '0';
                    buffer[n++] = 'x';
                    n += itoa(va_arg(args, uint32_t), buffer+n, 16);
                    break;
                case '%':
                    buffer[n++] = '%';
                }
                format++;
                break;
        default:
                buffer[n++] = *format++;
        }
    }
    buffer[n] = '\0';
    
    putstr(buffer, black, white);
}
