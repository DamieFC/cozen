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
#ifndef ASCII_H
#define ASCII_H
#include <devices/rtc/rtc.h>
#include <devices/serial/serial.h>
#include <devices/video/vbe.h>
char ascii_art[] = "\e[0;34m\n"
                   "  _____\n"
                   " |  ___|\n"
                   " | |      _____   _____    ______   _ ____\n"
                   " | |     |  _  | |__  _|  |  __ |  | `__  |\n"
                   " | |___  | |_| |   / /__  | |_//_  | |  | |\n"
                   " \\_____| |_____|  |_____| |______| |_|  |_|\n"
                   " ────────────────────────────────────────────\e[0m\n"
                   "Copyright (c) 2020-2021 CozenOS authors\n"
                   "                                               \n";

/* Unused for now
static char *strcpy(char *destination, const char *source)
{
    if (!destination)
        return NULL;

    char *ptr = destination;

    while (*source != '\0')
    {
        *destination = *source;
        destination++;
        source++;
    }

    *destination = '\0';

    return ptr;
}
*/

void set_ascii()
{
    Serial_write_string(ascii_art);
    if (RTC_get_hours() < 12)
    {
        Serial_write_string("Good Morning!\n");
    }
    if (RTC_get_hours() >= 12 && RTC_get_hours() < 18)
    {
        Serial_write_string("Good Afternoon!\n");
    }
    if (RTC_get_hours() >= 18)
    {
        Serial_write_string("Good Evening!\n");
    }
}
#endif
