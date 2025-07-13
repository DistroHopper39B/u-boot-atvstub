/*
 * Copyright (C) 2015 Google, Inc.
 * Copyright (C) 2025 Sylas Hollander.
 * PURPOSE: Header for U-Boot Apple TV stub.
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include <multiboot.h>

#define ALIGN(x,a)		__ALIGN_MASK((x),(typeof(x))(a)-1)
#define ALIGN_DOWN(x, a)	ALIGN((x) - ((a) - 1), (a))
#define __ALIGN_MASK(x,mask)	(((x)+(mask))&~(mask))
#define PTR_ALIGN(p, a)		((typeof(p))ALIGN((unsigned long)(p), (a)))
#define IS_ALIGNED(x, a)		(((x) & ((typeof(x))(a) - 1)) == 0)

// Types for U-Boot files.

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef u32 ulong;

typedef u64 efi_physical_addr_t;
typedef u64 efi_virtual_addr_t;

#define efi_uintn_t size_t

#define EFI_GOT_RGBA8		0
#define EFI_GOT_BGRA8		1
#define EFI_GOT_BITMASK		2

struct efi_gop_mode_info {
    u32 version;
    u32 width;
    u32 height;
    u32 pixel_format;
    u32 pixel_bitmask[4];
    u32 pixels_per_scanline;
};

struct efi_gop_mode {
    u32 max_mode;
    u32 mode;
    struct efi_gop_mode_info *info;
    unsigned long info_size;
    efi_physical_addr_t fb_base;
    unsigned long fb_size;
};

struct efi_mem_desc {
    u32 type;
    u32 reserved;
    efi_physical_addr_t physical_start;
    efi_virtual_addr_t virtual_start;
    u64 num_pages;
    u64 attribute;
};

struct efi_entry_memmap {
    u32 version;
    u32 desc_size;
    u64 spare;
    struct efi_mem_desc desc[];
};

enum efi_entry_t {
    EFIET_END,	/* Signals this is the last (empty) entry */
    EFIET_MEMORY_MAP,
    EFIET_GOP_MODE,
    EFIET_SYS_TABLE,

    /* Number of entries */
    EFIET_MEMORY_COUNT,
};

#define EFI_TABLE_VERSION	1

/**
 * struct efi_info_hdr - Header for the EFI info table
 *
 * @version:	EFI_TABLE_VERSION
 * @hdr_size:	Size of this struct in bytes
 * @total_size:	Total size of this header plus following data
 * @spare:	Spare space for expansion
 */
struct efi_info_hdr {
    u32 version;
    u32 hdr_size;
    u32 total_size;
    u32 spare[5];
};

/**
 * struct efi_entry_hdr - Header for a table entry
 *
 * @type:	enum eft_entry_t
 * @size:	size of entry bytes excluding header and padding
 * @addr:	address of this entry (0 if it follows the header )
 * @link:	size of entry including header and padding
 * @spare1:	Spare space for expansion
 * @spare2:	Spare space for expansion
 */
struct efi_entry_hdr {
    u32 type;
    u32 size;
    u64 addr;
    u32 link;
    u32 spare1;
    u64 spare2;
};

struct efi_entry_systable {
    efi_physical_addr_t sys_table;
};

// Similar to efi_priv minus UEFI boot services specifics.
struct efi_atv_priv {
    struct efi_info_hdr *info;
    unsigned int info_size;
    void *next_hdr;
};