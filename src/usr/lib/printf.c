#include "usysc.h"
#include "string.h"
#include "stdarg.h"

int putchar(char c)
{
    write(1, &c, 1);
    return c;
}

int puts(char *str)
{
    size_t len = strlen(str);
    str[len] = '\n';
    size_t r = write(1, str, len+1);
    str[len] = '\0';
    return r;
}

void printf(char *format, ... ) 
{
    va_list args;
    va_start(args, format);
    char buffer[512];
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
                case 's':
                    n += strcpy(buffer+n, va_arg(args, char *));
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
                    n += itoa(va_arg(args, unsigned int), buffer+n, 16);
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
    write(1, buffer, n);
}

