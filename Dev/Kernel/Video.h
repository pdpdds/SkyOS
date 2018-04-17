#pragma once
#include "stdint.h"
#include "Hal.h"

/* video mode info. */
#define WIDTH           800
#define HEIGHT          600
#define BPP             32
#define BYTES_PER_PIXEL 4

/* BGA LFB is at LFB_PHYSICAL. */
#define LFB_PHYSICAL 0x250000
#define LFB_VIRTUAL  0x250000


/* Definitions for BGA. Reference Graphics 1. */
#define VBE_DISPI_IOPORT_INDEX          0x01CE
#define VBE_DISPI_IOPORT_DATA           0x01CF
#define VBE_DISPI_INDEX_XRES            0x1
#define VBE_DISPI_INDEX_YRES            0x2
#define VBE_DISPI_INDEX_BPP             0x3
#define VBE_DISPI_INDEX_ENABLE          0x4
#define VBE_DISPI_DISABLED              0x00
#define VBE_DISPI_ENABLED               0x01
#define VBE_DISPI_LFB_ENABLED           0x40


void VbeBochsWrite(uint16_t index, uint16_t value);
void VbeBochsSetMode(uint16_t xres, uint16_t yres, uint16_t bpp);
void* VbeBochsMapLFB();
void fillScreen32();


