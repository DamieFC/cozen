/*
BSD 3-Clause License

Copyright (c) 2021, Abbix and Smart6502
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

#include "main.h"
#include <acpi/acpi.h>
#include <ascii.h>
#include <boot/boot.h>
#include <boot/stivale2.h>
#include <devices/keyboard/keyboard.h>
#include <devices/pcspkr/pcspkr.h>
#include <devices/serial/serial.h>
#include <libk/logging.h>
#include <libk/module.h>
#include <libk/random.h>
#include <memory/pmm.h>
#include <pci/PCI.h>
#include <stddef.h>
#include <stdint.h>
#include <system/GDT.h>
#include <system/common.h>
#include <system/interrupts/IDT.h>
#include <system/interrupts/PIT.h>
#include <system/smp.h>

void kmain(struct stivale2_struct *info)
{
    module("Kernel");
    log(INFO, "Starting cozen...");

    GDT_init();
    IDT_init();

    PIT_init(1000);

    Serial_init();

    VBE_init(info);
    VBE_clear_screen(bg_color);

    info = (void *)info + MEM_OFFSET;

    BootInfo boot_info = Boot_get_info(info);

    DateTime date = RTC_get_date_time();

    VBE_putf("\n\nTime Information:");
    VBE_putf("\tDate: %x/%x/20%x", date.month, date.day, date.year);
    VBE_putf("\tTime: %d:%d:%d\n", date.time.hour, date.time.minute, date.time.second);

    srand(RTC_get_seconds());

    CPU_init((void *)boot_info.smp_info);
    PMM_init((void *)boot_info.memory_map);

    /* VMM_init();*/

    PCI_init();

    ACPI_init(boot_info.rsdp_location);
    Keyboard_init();
    PCSpkr_init();

    VBE_putf("Kernel initialized in %dms", PIT_get_ticks());
    VBE_puts("\nWelcome to ", white);
    VBE_puts("CozenOS!\n", blue);
    VBE_puts("Hope you're feeling cozy here!\n", white);

    Framebuffer fb = _Framebuffer();
    fb.init(info, &fb);
    fb.puts(&fb, "Using built-in PSF font!\n\\nworks");

    int beeps;
    for (beeps = 0; beeps < 3; beeps++)
    {
        PCSpkr_beep(50);
        PIT_sleep(950);
    }

    set_ascii();

    while (1)
        ;
}
