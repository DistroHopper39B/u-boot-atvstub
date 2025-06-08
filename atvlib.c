/*
 * Copyright (C) 2025 Sylas Hollander.
 * PURPOSE: ATVLib main file.
 * SPDX-License-Identifier: MIT
 */

#include <atvlib.h>

mach_boot_args_t    *gBA;
boolean_t           verbose_enable;

// C entry point for Apple TV code
noreturn void atvlib_init(mach_boot_args_t *ba)
{
    gBA = ba;

    // Initialize console.
    if (!cons_init(&ba->video, COLOR_WHITE, COLOR_BLACK))
        halt();

    // Enable verbose output.
    verbose_enable = (ba->video.display_mode == DISPLAY_MODE_TEXT);
    if (verbose_enable)
        cons_clear_screen(COLOR_BLACK);

    printf("Hello, world!\n");

    halt();
}