#include "types.h"

void memcpy(void *dest, void *src, int len) 
{
    uint8_t *d = dest;
    uint8_t *s = src;
    for(int i = 0; i < len; i++) 
    {
        d[i] = s[i];
    }
}

void memset(void *addr, uint8_t v, int len) 
{
    uint8_t *ad = addr;
    for(int i = 0; i < len; i++)
    {
        ad[i] = v;        
    }
}

void bzero(void *addr, int len) {
    memset(addr, 0, len);
}


