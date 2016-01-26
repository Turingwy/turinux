#include "types.h"
#include "string.h"

int strlen(char *str) 
{
    int len = 0;
    while(*str++ != '\0')
        len++;
    return len;
}

void strcpy(char *dest, char *src) 
{
    while(*src != '\0') 
    {
        *dest++ = *src++;
    }
    *dest = '\0';
    return;
}

void strcat(char *dest, char *src)
{
    int len = strlen(dest);
    while(*src != 0) 
    {
        dest[len++] = *src++;
    }
    dest[len] = '\0';
}

int strcmp(char *str1, char *str2) 
{
    while(*str1 != '\0' && *str2 != '\0') 
    {
        if(*str1 < *str2)   return -1;
        else if(*str1++ > *str2++)  return 1;
    }
    if(*str1 == '\0')    return -1;
    else if(*str2 == '\0')  return 1;
    else return 0;
}

int itoa(uint32_t value, char *str, int radix) 
{
    if(value == 0) 
    {
        strcpy(str, "0");
        return 1;
    }
    char buffer[32];
    int i = 0;
    while(value != 0) 
    {
        if(value%radix < 10) 
        {
            buffer[i] = '0' + value%radix;
        } 
        else 
        {
            buffer[i] = 'A' + value%radix - 10;
        }
        i++;
        value /= radix;
    }
    str[i--] = '\0';
    int j = 0;
    while(i >= 0) 
    {
        str[j++] = buffer[i--];
    }
    return j;
}


