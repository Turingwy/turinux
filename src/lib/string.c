#include "types.h"
#include "string.h"

int strlen(char *str) 
{
    int len = 0;
    while(*str++ != '\0')
        len++;
    return len;
}

int strcpy(char *dest, char *src) 
{
    int len = 0;
    while(*src != '\0') 
    {
        *dest++ = *src++;
        len++;
    }
    *dest = '\0';
    return len;
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
    if(*str1 == *str2)   return 0;   
    else if(*str1 == '\0')    return -2;
    else return 2;
}

static char *laststr;

char *strtok(char *str, char *delim)
{
    int len = strlen(delim);
    if(str)
        laststr = str;
    if(*laststr)
        str = laststr;
    else
        return NULL;
    while(*laststr)
    {
        if(strcmp(laststr, delim) == 2) {
            *laststr = '\0';
            laststr += len;
            while (strcmp(laststr, delim) == 2) {
                laststr += len;
            }
            return str;
        }
        laststr++;
    }
    return str;
}

int tok_hasnext()
{
    return *laststr ? 1:0;
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


