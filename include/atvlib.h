/*
 * Copyright (C) 2025 Sylas Hollander.
 * Copyright (C) 2020-2025 Jiaxun Yang.
 * PURPOSE: ATVLib main header file.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "types.h"
#include "cons.h"
#include "baselibc_string.h"
#include "boot_args.h"
#include "tinyprintf.h"

extern mach_boot_args_t     *gBA;

extern noreturn void halt(void);