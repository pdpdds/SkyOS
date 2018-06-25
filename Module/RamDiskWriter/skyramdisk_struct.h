#pragma once

#include <Windows.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ramdisk spawning status codes
typedef uint8_t g_ramdisk_spawn_status;
#define G_RAMDISK_SPAWN_STATUS_SUCCESSFUL			1
#define G_RAMDISK_SPAWN_STATUS_FAILED_NOT_FOUND		2
#define G_RAMDISK_SPAWN_STATUS_FAILED_NOT_VALID		3
#define G_RAMDISK_SPAWN_STATUS_FAILED_NOT_PERMITTED	4

/**
 * Maximum length of paths within the ramdisk
 */
#define G_RAMDISK_MAXIMUM_PATH_LENGTH				512

// types of ramdisk entries
typedef int g_ramdisk_entry_type;
#define G_RAMDISK_ENTRY_TYPE_UNKNOWN	-1
#define G_RAMDISK_ENTRY_TYPE_FOLDER		0
#define G_RAMDISK_ENTRY_TYPE_FILE		1

/**
 * Ramdisk entry information struct used within system calls
 */
typedef struct {
	g_ramdisk_entry_type type;
	char name[512];
	unsigned int length;
} g_ramdisk_entry_info;

#ifdef __cplusplus
}
#endif
