#
# Copyright (C) 2025 Sylas Hollander.
# PURPOSE: Makefile for Apple TV bare-metal programs
# SPDX-License-Identifier: MIT
#

# If the generation of headers is parallelized, they will likely not exist or be corrupted by the time Clang gets to them
.NOTPARALLEL: uboot_bin.h

# Check what OS we're running. Should work on Linux and macOS.
OSTYPE = $(shell uname)

# Target defs for Linux cross compiler.
TARGET = i386-apple-darwin8

# Definitions for compiler
CC := clang

# Definitions for linker
ifeq ($(OSTYPE),Linux)
	LD := /opt/cross/bin/$(TARGET)-ld
else
	LD := ld
endif

# Flags for mach-o linker
LDFLAGS := 	-static \
           	-segalign 0x1000 \
           	-segaddr __TEXT 0x00800000 \
           	-sectalign __TEXT __text 0x1000 \
           	-sectalign __DATA __common 0x1000 \
           	-sectalign __DATA __bss 0x1000 \

CFLAGS := -Wall -Werror -nostdlib -fno-stack-protector -fno-builtin -O0 -std=gnu11 --target=$(TARGET) -Iinclude

OBJS := start.o atvlib.o baselibc_string.o cons.o tinyprintf.o debug.o u_boot_atvstub.o

all: uboot_bin.h mach_kernel

uboot_bin.h: $(UBOOT)
ifdef UBOOT
	xxd -n uboot_bin -i $< > $@
else
	$(error "No `U-Boot.bin` file specified. Specify one by appending UBOOT=/path/to/u-boot.bin")
endif

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
mach_kernel: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@
all: mach_kernel

clean:
	rm -f uboot_bin.h *.o mach_kernel