/*
 * Copyright (C) 2015 Google, Inc.
 * Copyright (C) 2025 Sylas Hollander.
 * PURPOSE: U-Boot Apple TV stub.
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <atvlib.h>

#include <uboot.h>

#if 1
#include "uboot_bin.h"
#else
unsigned char uboot_bin[] = {};
unsigned char uboot_bin_len = 0;
#endif

extern linear_framebuffer_t fb;

#define INFO_BUF_LEN 4096

uint8_t info_table_buf[INFO_BUF_LEN] = {0}; // should be plenty of space

static void setup_info_table(struct efi_atv_priv *priv)
{
    struct efi_info_hdr *info = (struct efi_info_hdr *) info_table_buf;
    priv->info_size = INFO_BUF_LEN;

    memset(info, 0, INFO_BUF_LEN);
    info->version   = EFI_TABLE_VERSION;
    info->hdr_size  = sizeof(*info);
    priv->info      = info;
    priv->next_hdr  = (char *) info + info->hdr_size;
}

static void add_entry_addr(struct efi_atv_priv *priv, enum efi_entry_t type,
                           void *ptr1, int size1, void *ptr2, int size2)
{
    struct efi_entry_hdr *hdr = priv->next_hdr;

    hdr->type = type;
    hdr->size = size1 + size2;
    hdr->addr = 0;
    hdr->link = ALIGN(sizeof(*hdr) + hdr->size, 16);
    priv->next_hdr += hdr->link;
    memcpy(hdr + 1, ptr1, size1);
    memcpy((void *)(hdr + 1) + size1, ptr2, size2);
    priv->info->total_size = (ulong)priv->next_hdr - (ulong)priv->info;
}

struct multiboot_header *get_multiboot_header(uint8_t *file)
{
    for (int i = 0; i < MULTIBOOT_SEARCH; i += 4)
    {
        uint32_t *current_pos = (uint32_t *) (file + i);
        if (*current_pos == MULTIBOOT_HEADER_MAGIC)
        {
            return (struct multiboot_header *) current_pos;
        }
    }

    return NULL;
}

noreturn void load_uboot(void)
{
    struct efi_atv_priv local_priv, *priv = &local_priv;
    struct efi_gop_mode mode;
    struct efi_gop_mode_info info;
    struct efi_entry_systable table;
    struct efi_entry_memmap map;
    struct multiboot_header *mbh = NULL;

    trace("Loading U-Boot...\n");

    setup_info_table(priv);

    // Graphics
    info.version                = 0;
    info.width                  = fb.width;
    info.height                 = fb.height;
    info.pixel_format           = EFI_GOT_BGRA8;
    info.pixels_per_scanline    = fb.pixels_per_row;

    mode.fb_base                = fb.base;
    mode.fb_size                = fb.size;
    mode.info_size              = sizeof(struct efi_gop_mode_info);
    mode.info                   = &info;
    add_entry_addr(priv, EFIET_GOP_MODE, &mode, sizeof(mode), &info, sizeof(info));

    // EFI System Table
    table.sys_table = (ulong) gBA->efi_sys_tbl;
    add_entry_addr(priv, EFIET_SYS_TABLE, &table, sizeof(table), NULL, 0);

    // Memory Map
    map.version     = gBA->efi_mem_desc_ver;
    map.desc_size   = gBA->efi_mem_desc_size;
    add_entry_addr(priv, EFIET_MEMORY_MAP, &map, sizeof(map), (void *) gBA->efi_mem_map_ptr, (int) gBA->efi_mem_map_size);

    // End
    add_entry_addr(priv, EFIET_END, NULL, 0, 0, 0);

    mbh = get_multiboot_header(uboot_bin);
    if (!mbh)
    {
        fail(__FILE__, __LINE__, "Cannot find multiboot header in U-Boot executable!");
    }

    // Copy U-Boot to lowmem
    memcpy((void *) mbh->load_addr, (const void *) uboot_bin, uboot_bin_len);

    typedef void (*func_t)(int bist, int unused, ulong info)
            __attribute__((regparm(3)));

    ((func_t) mbh->entry_addr)(0, 0, (ulong) priv->info);

    fail(__FILE__, __LINE__, "UNREACHABLE");
}