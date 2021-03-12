#include "panic.h"
#include <devices/video/vbe.h>
#include <libk/logging.h>
#include <libk/random.h>
#include <main.h>
char *comments_lol[] = {
    "Something happened.",
    "F.",
    "The kernel crashed successfully.",
    "oops I did a woopsie doopsie OwO.",
    "Random error just to annoy you.",
    "Keyboard not found, press F to pay respects.",
    "Greenpeace free'd the mallocs",
    "Typo in the code.",
    "Excuse Me Sir, do you have a moment to talk about our Lord and Saviour?",
    "Never gonna give you up",
    "E.",
    "Error 404, kernel not found.",
    "To boot or not to boot.",
    ":(",
    "To fix or to not fix, that is the question.",
    "Kernel is not in the sudoers file. This incident will be reported.",
    "Error Code: INSERT_FUNNY_THING_HERE"

};

void __panic(char *file, const char function[20], int line, char *message)
{
    VBE_clear_screen(0, bg_color);

    __asm__("cli");

    VBE_puts("-------------------------------------------------------------------"
             "-----\n",
             red);
    VBE_puts("KERNEL PANIC\n", red);
    VBE_cputf(gray, " /* %s */", comments_lol[rand() % 17]);
    VBE_cputf(blue, "%s", message);
    VBE_putf("In %s at %s(), line %d", file, function, line);

    while (1)
        ;
}
