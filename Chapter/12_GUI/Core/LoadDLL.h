#pragma once

#include "windef.h"
#include <stdio.h>
#include "fileio.h"

#define MEM_COMMIT 0x00001000
#define MEM_RESET_UNDO 0x1000000
#define MEM_RESET 0x00080000
#define MEM_RESERVE 0x00002000

#define MEM_DECOMMIT 0x4000
#define MEM_RELEASE 0x8000

#define PAGE_TARGETS_NO_UPDATE 0x40000000

#define PAGE_TARGETS_INVALID 0x40000000

#define PAGE_WRITECOPY 0x08

#define PAGE_READWRITE 0x04

#define PAGE_READONLY 0x02

#define PAGE_NOACCESS 0x01

#define PAGE_EXECUTE_WRITECOPY 0x80

#define  PAGE_EXECUTE_READWRITE 0x40

#define PAGE_EXECUTE_READ 0x20

#define PAGE_EXECUTE 0x10



typedef void *FARPROC;

#ifdef __cplusplus
extern "C" {
#endif

	/*
	// Conventions:
	// RVA: relative virtual address
	// VA: virtual address that is RVA+ImageBase
	//
	// LOAD_DLL_INFO
	// A structure that holds information about a "manually loaded" DLL.
	//
	// - size: size of the structure.
	// - flags: The flags parameter that was passed to the LoadDLL() function.
	// - image_base: VA that was used to do the base relocation.
	//   It is always the same as mem_block when you loaded the DLL including the headers.
	// - mem_block: VA of the big memory block that was allocated to hold the data of
	//   sections and additionally the headers if we want.
	// - dll_main: VA of DllMain(). Be careful cause it can be NULL.
	// - export_dir_rva: RVA of the export directory of the DLL.
	//   It can be NULL if there is no export info. It is needed when you want to load a
	//   DLL without header but the address of exported functions must be retrieved.
	*/

	typedef bool (WINAPI *LPDLLMAIN)(DWORD_PTR image_base, DWORD reason, LPVOID reserved);

	typedef struct LOAD_DLL_INFO
	{
		char		moduleName[256];
		int			refCount;
		size_t		size;
		int			flags;
		DWORD_PTR	image_base;
		void*		mem_block;
		LPDLLMAIN	dll_main;
		DWORD		export_dir_rva;
		HMODULE*	loaded_import_modules_array;
		unsigned	num_import_modules;
	} LOAD_DLL_INFO;


	/*
	// LOAD_DLL_READPROC:
	// The type of callback procedure we must support for LoadDLL(). It reads size number of bytes to
	// buff from the DLL data after seeking to position. It must return TRUE on success and FALSE
	// if the specified number of bytes could not be read. LoadDLL() will assume that the DLL file's
	// DOS header is at zero position. We can use this callback easily to load module from both file
	// or memory. The param receives the param value we pass to LoadDLL(). This is useful when you
	// are multithreading.
	*/

	typedef bool (*LOAD_DLL_READPROC)(void* buff, size_t position, size_t size, void* param);


	/* LoadDLL() error codes: */
	typedef enum ELoadDLLResult
	{
		ELoadDLLResult_OK = 0,
		/* The read procedure we provided returned FALSE for a read request. */
		ELoadDLLResult_ReadProcError = 1,
		/* Bad DLL file. Wrong header or a similar error. */
		ELoadDLLResult_InvalidImage = 2,
		/* Memory allocation error. */
		ELoadDLLResult_MemoryAllocationError = 3,
		/* The DLL could not be loaded to the preferred imagebase and there is no base relocation info in the module. */
		ELoadDLLResult_RelocationError = 4,
		/* The DLL relocation data seems to be bad. */
		ELoadDLLResult_BadRelocationTable = 5,
		/* An imported DLL could not be loaded. */
		ELoadDLLResult_ImportModuleError = 6,
		/* A function was not found in an imported DLL. */
		ELoadDLLResult_ImportFunctionError = 6,
		/* Bad import table contents. */
		ELoadDLLResult_ImportTableError = 7,
		/* We do not support import directories that contain only bound import info. */
		ELoadDLLResult_BoundImportDirectoriesNotSupported = 8,
		/* Error setting the memory page protection of loaded and relocated sections. */
		ELoadDLLResult_ErrorSettingMemoryProtection = 9,
		/* The DllMain() returned FALSE or caused an exception. */
		ELoadDLLResult_DllMainCallError = 10,
		ELoadDLLResult_DLLFileNotFound = 11,
		/* LoadDLL() was called with wrong parameters */
		ELoadDLLResult_WrongFunctionParameters = -2,
		ELoadDLLResult_UnknownError = -1,
	} ELoadDLLResult;

	/* LoadDLL() flags */
	typedef enum ELoadDLLFlag
	{
		/* Don't call the DllMain() of the loaded DLL. */
		ELoadDLLFlag_NoEntryCall = 0x01,
		/* Don't load the DOS/PE headers, only the data/code sections. This is useful only with LoadDLL(). */
		ELoadDLLFlag_NoHeaders   = 0x02,
	} ELoadDLLFlag;

	/*
	// Loads a DLL file that must be read by lpfRead() at zero position from its on stream.
	// @param read_proc_param: this custom parameter is passed to the read proc.
	// @param flags: a combination of ELoadDLLFlags.
	// @param info: Can be NULL. If not NULL then filled with info in case of success.
	*/
	ELoadDLLResult LoadDLL(LOAD_DLL_READPROC read_proc, void* read_proc_param, int flags, LOAD_DLL_INFO* info);


	/*
	// Some LoadDLL() functions if you don't want to mess with callback functions.
	// They provide you the ability to load a DLL from file or from memory.
	// * LoadDLLFromFile:
	//   Opens a file and loads a DLL whose data is placed in the file at
	//   offset dwDLLOffset. The size of the DLL is dwDLLSize bytes.
	// * LoadDLLFromMemory:
	//   We assume that you read the data of a DLL file to memory.
	//   lpvDLLData points to it and dwDLLSize it its size in bytes.
	//
	// You can use the DLL_SIZE_UNK constant as dwDLLSize parameter if you
	// don't know the size of the DLL and you let to read any number of
	// bytes starting from dwDLLOffset. Only the required amount of data
	// will be read from the file or memory buffer not more. (Headers, sections)
	*/

	#define DLL_SIZE_UNKNOWN ((DWORD)-1)

	ELoadDLLResult LoadDLLFromFileName(const char* filename, int flags, LOAD_DLL_INFO* info);
	ELoadDLLResult LoadDLLFromFileNameOffset(const char* filename, size_t dll_offset, size_t dll_size, int flags, LOAD_DLL_INFO* info);
	ELoadDLLResult LoadDLLFromCFile(FILE* f, size_t dll_offset, size_t dll_size, int flags, LOAD_DLL_INFO* info);
	ELoadDLLResult LoadDLLFromMemory(const void* dll_data, size_t dll_size, int flags, LOAD_DLL_INFO* info);

	bool UnloadDLL(LOAD_DLL_INFO* info);

	FARPROC myGetProcAddress_LoadDLLInfo(LOAD_DLL_INFO* info, const char* func_name);
	FARPROC MyGetProcAddress(HMODULE module, const char* func_name);
	FARPROC MyGetProcAddress_ExportDir(DWORD export_dir_rva, DWORD_PTR image_base, const char* func_name);


#ifdef __cplusplus
}
#endif
