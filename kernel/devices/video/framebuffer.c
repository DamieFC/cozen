/*
BSD 3-Clause License

Copyright (c) 2021, Abbix, PQCraft
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
#include "framebuffer.h"
#include <libk/logging.h>

/* OOP C because I wanted to try it */

/* The font is terminus.

Copyright (c) 2011 Tilman Blumenbach,
with Reserved Font Name "Terminus (TTF)".

This Font Software is licensed under the SIL Open Font License, Version 1.1.
This license is  available with a FAQ at:
http://scripts.sil.org/OFL
*/
extern PSF fb_font;

struct stivale2_struct_tag_framebuffer *Framebuffer_get_info();

/* Initializing the framebuffer */
static Framebuffer current_framebuffer;

static void init(struct stivale2_struct *info, Framebuffer *self)
{
    /* Setting the logging module */
    module("Framebuffer");

    /* Getting info */
    struct stivale2_struct_tag_framebuffer *fb_info = Framebuffer_get_info(info);

    self->width = fb_info->framebuffer_width;
    self->height = fb_info->framebuffer_height;
    self->bpp = fb_info->framebuffer_bpp;
    self->pitch = fb_info->framebuffer_pitch;
    self->address = fb_info->framebuffer_addr;

    /* Initializing the VBE driver */
    VBE_init(info);
}

static void clear_screen(Framebuffer *self)
{
    VBE_clear_screen(self->bg_color);
}

struct stivale2_struct_tag_framebuffer *
Framebuffer_get_info(struct stivale2_struct *info)
{
    struct stivale2_tag *tag = (struct stivale2_tag *)info->tags;
    struct stivale2_struct_tag_framebuffer *fb_info = NULL;

    while (tag != NULL)
    {
        switch (tag->identifier)
        {

        case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID:
        {
            fb_info = (struct stivale2_struct_tag_framebuffer *)tag;

            break;
        }
        }

        tag = (struct stivale2_tag *)tag->next;
    }
    return fb_info;
}

static uint32_t get_color(Color *color)
{
    return (uint32_t)((color->r << RED_SHIFT) | (color->g << GREEN_SHIFT) | (color->b << BLUE_SHIFT));
}

static void putchar(Framebuffer *self, uint8_t c, size_t x, size_t y)
{
    /* FIXME: &self->fb_font.data is not working, we need to use the fb_font
   * directly */
    uint8_t *glyph = &fb_font.data[c * self->font.glyph_size];
    static const uint8_t masks[8] = {128, 64, 32, 16, 8, 4, 2, 1};

    size_t i, j;
    for (i = 0; i < self->font.height; i++)
    {
        for (j = 0; j < self->font.width; j++)
        {
            if (glyph[i] & masks[j])
                VBE_draw_pixel(x + j, y + i, get_color(&self->fg_color));

            else
                VBE_draw_pixel(x + j, y + i, get_color(&self->bg_color));
        }
    }
}

static void put(Framebuffer *self, uint8_t c)
{
    switch (c)
    {
    case '\0':
        break;
    case '\n':
        self->cursor_x = 0;
        self->cursor_y += self->font.height;
        break;
    case '\r':
        self->cursor_x = 0;
    default:
        putchar(self, c, self->cursor_x, self->cursor_y);
        self->cursor_x += self->font.width;
        break;
    }
    if (self->cursor_x >= ((size_t)self->width - 1))
    {
        self->cursor_x = 1;
        self->cursor_y += self->font.height;
    }
    if (self->cursor_y >= ((size_t)self->height - 1))
    {
        /* scroll */
        self->cursor_x = 0;
        self->cursor_y -= self->font.height;
    }
}

static void puts(Framebuffer *self, char *string)
{
    while (*string)
        put(self, *string++);
}

Framebuffer Framebuffer_get_current()
{
    return current_framebuffer;
}

Framebuffer _Framebuffer()
{
    Framebuffer new = {

        /* Functions */
        .init = init,
        .clear_screen = clear_screen,
        .puts = puts,
        .putchar = putchar,

        /* Font */
        .font = fb_font,

        /* Colors */
        .bg_color = bg_color,
        .fg_color = fg_color,

        /* Cursor */
        .cursor_x = 1,
        .cursor_y = 1,
    };

    current_framebuffer = new;

    return new;
}
