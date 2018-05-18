
/*
==================================================
lib/cstd.cpp

C++ runtime library routines
==================================================.
*/

#include "stdint.h"

/*
===========================
Dynamic initializer sections
===========================
*/

// Function pointer typedef for less typing //
typedef void(__cdecl *_PVFV)(void);
typedef int(__cdecl *_PIFV)(void);
// Linker puts constructors between these sections, and we use them to locate constructor pointers.
#pragma section(".CRT$XIA",long,read)
#pragma section(".CRT$XIZ",long,read)
#pragma section(".CRT$XCA",long,read)
#pragma section(".CRT$XCZ",long,read)

#pragma section(".CRT$XIAA",long,read)
#pragma section(".CRT$XCAA",long,read)

// Pointers surrounding constructors
__declspec(allocate(".CRT$XIA")) _PIFV __xi_a[] = { 0 };
__declspec(allocate(".CRT$XIZ")) _PIFV __xi_z[] = { 0 };
__declspec(allocate(".CRT$XCA")) _PVFV __xc_a[] = { 0 };
__declspec(allocate(".CRT$XCZ")) _PVFV __xc_z[] = { 0 };


												 // Put .CRT data into .rdata section
#pragma comment(linker, "/merge:.CRT=.rdata")

/*
===========================
Globals
===========================
*/

// function pointer table to global deinitializer table //
static _PVFV * pf_atexitlist = 0;

// Maximum entries allowed in table //
static unsigned max_atexitlist_entries = 32;

// Current amount of entries in table //
static unsigned cur_atexitlist_entries = 0;


/*
===================================
Initialize the deinitializer function ptr table
===================================
*/
char runtimeTempBuffer[5000];
void __cdecl _atexit_init(void)
{
	max_atexitlist_entries = 32;

	// Warning: Normally, the STDC will dynamically allocate this. Because we have no memory manager, just choose
	// a base address that you will never use for now
	//   pf_atexitlist = (_PVFV *)0x500000;

	pf_atexitlist = (_PVFV *)runtimeTempBuffer;
}

/*
===================================
Add entry into atexit deinitialzer table. Called by MSVC++ code.
===================================
*/
int __cdecl atexit(_PVFV fn)
{
	// Insure we have enough free space
	if (cur_atexitlist_entries >= max_atexitlist_entries)
		return 1;
	else {

		// Add the exit routine
		*(pf_atexitlist++) = fn;
		cur_atexitlist_entries++;
	}

	return 0;
}

/*
===================================
Shutdown the CRT, and execute all global dtors.
===================================
*/
void _cdecl Exit() {

	// Go through the list, and execute all global exit routines
	while (cur_atexitlist_entries--) {

		// execute function
		(*(--pf_atexitlist)) ();
	}
}

/*
===================================
Executes all global dynamic initializers
===================================


*/

// Call C constructors
static int _initterm_e(_PIFV * pfbegin, _PIFV * pfend) {
	int ret = 0;

	// walk the table of function pointers from the bottom up, until
	// the end is encountered.  Do not skip the first entry.  The initial
	// value of pfbegin points to the first valid entry.  Do not try to
	// execute what pfend points to.  Only entries before pfend are valid.

	while (pfbegin < pfend  && ret == 0)
	{
		// if current table entry is non-NULL, call thru it.
		if (*pfbegin != 0)
			ret = (**pfbegin)();
		++pfbegin;
	}

	return ret;
}

// Call C++ constructors
static void _initterm(_PVFV * pfbegin, _PVFV * pfend)
{
	// walk the table of function pointers from the bottom up, until
	// the end is encountered.  Do not skip the first entry.  The initial
	// value of pfbegin points to the first valid entry.  Do not try to
	// execute what pfend points to.  Only entries before pfend are valid.
	while (pfbegin < pfend)
	{
		// if current table entry is non-NULL, call thru it.
		if (*pfbegin != 0)
		{
			(**pfbegin)();
		}
		++pfbegin;
	}
}


bool _cdecl InitializeConstructors()
{
	// Do C initialization
	int initret = _initterm_e(__xi_a, __xi_z);
	if (initret != 0) {
		return false;
	}

	// Do C++ initialization
	_initterm(__xc_a, __xc_z);
	return true;
}
/*
===================================
MSVC++ calls this routine if a pure virtual function is called
===================================
*/
int __cdecl _purecall_handler()
{
	// for now, halt the system 'til we have a way to display error
	return 0;

	// print error message here
}

// Disable C4100 ('Unused Parameter') warning for now, 'til we could get these written
#pragma warning (disable:4100)

extern "C"
{
	int _fltused = 1;
}
// enable warning
#pragma warning (default:4100)
