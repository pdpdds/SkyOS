#pragma once

#include "./v8086/rme.h"
#include "vesa.h"
#include "Math.h"

#define PROMPT_FOR_MODE 0

/* Friggin' frick, this should be a config option
* because it's 4096 on some instances of Qemu,
* ie the one on my laptop, but it's 2048 on
* the EWS machines. */
#define BOCHS_BUFFER_SIZE 2048
#define PREFERRED_VY 4096
#define PREFERRED_B 32

/* vm86 Helpers */
typedef uint32_t  FARPTR;
typedef uintptr_t addr_t;
#define MK_FP(seg, off)        ((FARPTR) (((uint32_t) (seg) << 16) | (uint16_t) (off)))
#define FP_SEG(fp)             (((FARPTR) fp) >> 16)
#define FP_OFF(fp)             (((FARPTR) fp) & 0xffff)
#define FP_TO_LINEAR(seg, off) ((void*) ((((uint16_t) (seg)) << 4) + ((uint16_t) (off))))
#define LINEAR_TO_FP(ptr)      (MK_FP(((addr_t) (ptr) - ((addr_t) (ptr) & 0xf)) / 16, ((addr_t)(ptr) & 0xf)))

void graphics_install_vesa(uint16_t resX, uint16_t resY);