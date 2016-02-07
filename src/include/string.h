#ifndef _STRING_H_
#define _STRING_H_
#include "types.h"

typedef unsigned int size_t;

size_t strlen(char *str);
size_t strcpy(char *dest, char *src);
void strcat(char *dest, char *src);
int strcmp(char *str1, char *str2);
int itoa(uint32_t value, char *str, int radix);
char *strtok(char *str, char *delim);
int tok_hasnext();

#endif
