/*
BSD 3-Clause License

Copyright (c) 2021, Smart6502 (Xenon6502)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "vbe.h"
#include "font.h"
#include <libk/logging.h>
#include <main.h>
#include <stddef.h>

struct stivale2_struct_tag_framebuffer *fb_info;

size_t cursor_x = 5;
size_t cursor_y = 5;

uint32_t get_color(Color *color)
{
    return (uint32_t)((color->r << RED_SHIFT) | (color->g << GREEN_SHIFT) | (color->b << BLUE_SHIFT));
}

void VBE_draw_pixel(int x, int y, uint32_t color)
{
    size_t fb_i = x + (fb_info->framebuffer_pitch / sizeof(uint32_t)) * y;
    uint32_t *fb = (uint32_t *)fb_info->framebuffer_addr;

    fb[fb_i] = color;
}

struct stivale2_struct_tag_framebuffer *VBE_get_fb_info()
{
    return fb_info;
}
void VBE_clear_screen(Color color)
{

    cursor_y = 5;

    int i, j;
    for (i = 0; i < fb_info->framebuffer_width; i++)
    {
        for (j = 0; j < fb_info->framebuffer_height; j++)
        {
            VBE_draw_pixel(i, j, get_color(&color));
        }
    }
}

void VBE_init(struct stivale2_struct *info)
{
    module("VBE");
    struct stivale2_tag *tag = (struct stivale2_tag *)info->tags;
    struct stivale2_struct_tag_framebuffer *videoheader = videoheader;

    while (tag != NULL)
    {
        switch (tag->identifier)
        {

        case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID:
        {
            videoheader = (struct stivale2_struct_tag_framebuffer *)tag;
            fb_info = videoheader;

            break;
        }
        }

        tag = (struct stivale2_tag *)tag->next;
    }

    log(INFO, "Framebuffer info:");
    log(INFO, "\t Resolution: %dx%d", fb_info->framebuffer_width,
        fb_info->framebuffer_height);
    log(INFO, "\t Pitch: %d", fb_info->framebuffer_pitch);
    log(INFO, "\t BPP: %x", fb_info->framebuffer_bpp);
}

void VBE_putchar(char character, int position_x, int position_y, Color color)
{
    uint8_t iy, ix;
    for (iy = 0; iy < 8; iy++)
    {
        for (ix = 0; ix < 8; ix++)
        {
            if ((font[(uint8_t)character][iy] >> ix) & 1)
            {
                uint64_t offset = ((iy + position_y) * fb_info->framebuffer_pitch) +
                                  ((ix + position_x) * 4);
                *(uint32_t *)((uint64_t)fb_info->framebuffer_addr + offset) =
                    get_color(&color);
            }
        }
    }
}
void VBE_put(char c, Color color)
{
    if (c == '\n')
    {
        cursor_x = 5;
        cursor_y += 20;
    }
    if (c == '\0')
    {
        VBE_putchar(' ', cursor_x - 1, cursor_y, color);
    }
    else
    {
        VBE_putchar(c, cursor_x, cursor_y, color);
        cursor_x += 8;

        if (cursor_x >= (size_t)fb_info->framebuffer_width - 5)
        {
            cursor_x = 5;
            cursor_y += 10;
        }
    }
}

void VBE_puts(char *string, Color color)
{
    while (*string)
    {
        VBE_put(*string++, color);
    }
}

void VBE_putf(char *format, ...)
{

    unsigned int i;
    unsigned int ZERO = 0;
    char *s;

    va_list arg;
    va_start(arg, format);

    while (*format)
    {

        if (*format == '%')
        {
            format++;
            switch (*format)
            {
            case 'c':
                i = va_arg(arg, int);
                VBE_put(i, fg_color);
                break;

            case 'd':
                i = va_arg(arg, int);
                if (i < ZERO)
                {
                    i = -i;
                    VBE_put('-', fg_color);
                }
                VBE_puts(string_convert(i, 10), fg_color);
                break;

            case 'o':
                i = va_arg(arg, unsigned int);
                VBE_puts(string_convert(i, 8), fg_color);
                break;

            case 's':
                s = va_arg(arg, char *);
                VBE_puts(s, fg_color);
                break;

            case 'x':
                i = va_arg(arg, unsigned int);
                VBE_puts(string_convert(i, 16), fg_color);
                break;
            default:
                VBE_put('%', fg_color);
                break;
            }
        }
        else
        {
            VBE_put(*format, fg_color);
        }
        format++;
    }

    va_end(arg);

    VBE_put('\n', fg_color);
}

void VBE_cputf(Color color, char *format, ...)
{

    unsigned int i;
    unsigned int ZERO = 0;
    char *s;

    va_list arg;
    va_start(arg, format);

    while (*format)
    {
        if (*format == '%')
        {
            format++;
            switch (*format)
            {
            case 'c':
                i = va_arg(arg, int);
                VBE_put(i, color);
                break;

            case 'd':
                i = va_arg(arg, int);
                if (i < ZERO)
                {
                    i = -i;
                    VBE_put('-', color);
                }
                VBE_puts(string_convert(i, 10), color);
                break;

            case 'o':
                i = va_arg(arg, unsigned int);
                VBE_puts(string_convert(i, 8), color);
                break;

            case 's':
                s = va_arg(arg, char *);
                VBE_puts(s, color);
                break;

            case 'x':
                i = va_arg(arg, unsigned int);
                VBE_puts(string_convert(i, 16), color);
                break;
            default:
                VBE_put('%', color);
                break;
            }
        }
        else
        {
            VBE_put(*format, color);
        }
        format++;
    }

    va_end(arg);

    VBE_put('\n', color);
}
/* Bresenham's circle algorithm */

void draw_circle(int xc, int yc, Position position)
{
    int y = position.y;
    int x = position.x;

    VBE_draw_pixel(xc + x, yc + y, get_color(&white));
    VBE_draw_pixel(xc - x, yc + y, get_color(&white));
    VBE_draw_pixel(xc + x, yc - y, get_color(&white));
    VBE_draw_pixel(xc - x, yc - y, get_color(&white));
    VBE_draw_pixel(xc + y, yc + x, get_color(&white));
    VBE_draw_pixel(xc - y, yc + x, get_color(&white));
    VBE_draw_pixel(xc + y, yc - x, get_color(&white));
    VBE_draw_pixel(xc - y, yc - x, get_color(&white));
}

void VBE_display_circle(int xc, int yc, int radius)
{

    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    Position position = {x, y};

    draw_circle(xc, yc, position);

    while (y >= x)
    {
        x++;

        if (d > 0)
        {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else
        {
            d = d + 4 * x + 6;
        }

        Position n_position = {x, y};
        draw_circle(xc, yc, n_position);
    }
}

static int abs(int i)
{
    return i < 0 ? -i : i;
}
/* Bresenham's line algorithm */

void VBE_draw_line(int x0, int y0, int x1, int y1)
{

    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;

    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        VBE_draw_pixel(x0, y0, get_color(&white));

        if (x0 == x1 && y0 == y1)
            break;

        e2 = err;

        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void VBE_draw_shape(int shape, int width, int height, int x, int y)
{
    module("VBE");
    switch (shape)
    {
    case RECTANGLE:
    {
        VBE_draw_line(x, y, width, y);

        VBE_draw_line(x, y, x, y + height);

        VBE_draw_line(width, y, width, y + height);

        VBE_draw_line(x, y + height, width, y + height);
        break;
    }

    case TRIANGLE:
    {
        VBE_draw_line(width, y, width * 3, y);

        VBE_draw_line(width, y, width * 2, y * 2);

        VBE_draw_line(width * 3, y, width * 2, y * 2);

        break;
    }
    default:
        log(ERROR, "Invalid shape!");
        break;
    }
}
