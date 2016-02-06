#include "types.h"
#include "console.h"
#include "ioport.h"
#define ABS_CURSOR() (cursor_y * 80 + cursor_x)
#define VIDEO_BUFF 0xC00B8000
static uint16_t *video_array = (uint16_t*)VIDEO_BUFF;
static uint32_t cursor_x, cursor_y;

static void move_cursor() 
{
    uint16_t abs_cursor = ABS_CURSOR();
    outb(0x3D4, 14);
    outb(0x3D5, abs_cursor >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, abs_cursor & 0xFF);
}

void get_cursor(int *x, int *y)
{
    *x = cursor_x;
    *y = cursor_y;
}

void cursor_left()
{
    if(cursor_x) {
        cursor_x--;
        move_cursor();
    }
}

void cursor_right()
{
    if(cursor_x < 79) {
        cursor_x++;
        move_cursor();
    }
}
void cls() 
{
    uint16_t color = (( black << 4 ) | (white & 0x0F));
    uint16_t nul_value = 0x20 | (color << 8);
    for(int i = 0; i < 80*25; i++) {
        video_array[i] = nul_value;
    }
    cursor_x = 0;
    cursor_y = 0;
    move_cursor();
}

void putchar_color(char ch, video_color back, video_color fore) {
    if(ch == '\n')
    {
        cursor_y++;
        cursor_x = 0;
        scroll();
        move_cursor();
        return;
    } else if(ch == '\r') 
    {
        cursor_x = 0;
        move_cursor();
        return;
    } else if(ch == '\b')
    {
        if(cursor_x)  cursor_x--;
        video_array[ABS_CURSOR()] = ' ' | (((black<< 4) | (white & 0x0F))) << 8;
        move_cursor();
        return;
    } 
    uint16_t color = ((back << 4) | (fore & 0x0F));
    uint16_t value = ch | (color << 8);
    uint16_t abs_cursor = ABS_CURSOR();
    video_array[abs_cursor] = value;
    cursor_x++;
    if(cursor_x >= 80) 
    {
        cursor_x = 0;
        cursor_y++;
        scroll();
    }
    move_cursor();

}

void putchar_normal(char ch) 
{
    putchar_color(ch, black, white);
}

void scroll() 
{
    if(cursor_y < 25)   return;

    for(int i = 80; i < 80*25; i++) 
    {
        video_array[i - 80] = video_array[i];
    }
    uint16_t color = (( black << 4 ) | (white & 0x0F));
    uint16_t nul_value = 0x20 | (color << 8);
    for(int i = 80*24; i < 80*25; i++) {
        video_array[i] = nul_value;
    }
    cursor_y--;
    move_cursor();
}


