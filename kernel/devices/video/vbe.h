/*
BSD 3-Clause License

Copyright (c) 2021, Abbix
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
#ifndef VBE_H
#define VBE_H
#define RED_SHIFT 16
#define GREEN_SHIFT 8
#define BLUE_SHIFT 0

#include <boot/stivale2.h>
#include <stddef.h>

typedef struct
{
    uint8_t r, g, b;
} Color;

typedef struct
{
    size_t x, y;
} Position;

enum shapes
{
    RECTANGLE,
    RHOMBUS,
    TRIANGLE
};

static Color bg_color = {20, 32, 52};
static Color fg_color = {129, 161, 193};

void VBE_init(struct stivale2_struct *info);
void VBE_clear_screen(Color color);
void VBE_putchar(char character, int position_x, int position_y, Color color);
void VBE_puts(char *string, Color color);
void VBE_put(char c, Color color);
void VBE_draw_pixel(int x, int y, uint32_t color);
void VBE_putf(char *format, ...);
void VBE_cputf(Color color, char *format, ...);
void VBE_display_circle(int xc, int yc, int radius);
void VBE_draw_line(int x0, int y0, int x1, int y1);
void VBE_draw_shape(int shape, int width, int height, int x, int y);
struct stivale2_struct_tag_framebuffer *VBE_get_fb_info();
#endif
