#ifndef _STRING_H_
#define _STRING_H_

int strlen(char *str);
void strcpy(char *dest, char *src);
void strcat(char *dest, char *src);
int strcmp(char *str1, char *str2);
int itoa(uint32_t value, char *str, int radix);
#endif
