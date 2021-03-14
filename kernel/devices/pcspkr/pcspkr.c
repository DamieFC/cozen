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
#include "pcspkr.h"
#include <libk/io.h>
#include <libk/module.h>
#include <system/interrupts/PIT.h>

void PCSpkr_init()
{
    IO_outb(0x61, IO_inb(0x61) | 0x1);
    module("PCSpkr");
    log(INFO, "Initialized PC speaker");
}

/* Change the c2 f */
void PCSpkr_set_c2(uint32_t hz)
{
    uint32_t div = BASE_FREQ / hz;
    IO_outb(PIT_CTL, 0xB6);
    IO_outb(TIMER2_CTL, div & 0xFF);
    IO_outb(TIMER2_CTL, div >> 8);
}

void PCSpkr_tone_on(uint32_t frequency)
{
    PCSpkr_set_c2(frequency);

    uint8_t tmp = IO_inb(0x61);
    if (tmp != (tmp | 3))
        IO_outb(0x61, tmp | 3);
}

void PCSpkr_tone_off()
{
    /* Shut it up */
    IO_outb(0x61, IO_inb(0x61) & 0xFC);
    PCSpkr_set_c2(1);
}

void PCSpkr_sleep(uint16_t delay)
{
    uint64_t cticks = PIT_get_ticks();
    while (1)
    {
        if (cticks + delay < PIT_get_ticks())
            break;
    }
}

void PCSpkr_tone(uint16_t frequency, uint16_t duration)
{
    /* I desire thee ears survive ;) */
    PCSpkr_tone_on(frequency);
    PCSpkr_sleep(duration);
    /* sleep for sometime */
    PCSpkr_tone_off();
}

void PCSpkr_beep(uint16_t duration)
{
    PCSpkr_tone(900, duration);
}
