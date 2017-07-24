
/*
==================================================
	lib/cstd.cpp

	C++ runtime library routines
==================================================.
*/

// Very MSVC++ dependent. Will try to support different compiliers later.
#ifndef _MSC_VER
#error "MOS2 Kernel C++ Runtime requires Microsoft Visual C++ 2005 or later."
#endif

#include <stdint.h>

/*
===========================
	Dynamic initializer sections
===========================
*/

// Function pointer typedef for less typing //
typedef void (__cdecl *_PVFV)(void);

// Standard C++ Runtime (STD CRT) __xc_a points to beginning of initializer table
#pragma data_seg(".CRT$XCA")
_PVFV __xc_a[] = { 0 };

// Standard C++ Runtime (STD CRT) __xc_z points to end of initializer table
#pragma data_seg(".CRT$XCZ")
_PVFV __xc_z[] = { 0 };

// Select the default data segment again (.data) for the rest of the unit
#pragma data_seg()

// Now, move the CRT data into .data section so we can read/write to it
#pragma comment(linker, "/merge:.CRT=.data")

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
===========================
	Initialize global initializaters (Global constructs, et al)
===========================
*/
void __cdecl _initterm ( _PVFV * pfbegin,    _PVFV * pfend )
{
	// Go through each initializer
    while ( pfbegin < pfend )
    {
	  // Execute the global initializer
      if ( *pfbegin != 0 )
            (**pfbegin) ();

	    // Go to next initializer inside the initializer table
        ++pfbegin;
    }
}

/*
===================================
	Initialize the deinitializer function ptr table
===================================
*/
void __cdecl _atexit_init(void)
{
    max_atexitlist_entries = 32;

	// Warning: Normally, the STDC will dynamically allocate this. Because we have no memory manager, just choose
	// a base address that you will never use for now
 //   pf_atexitlist = (_PVFV *)0x500000;

	pf_atexitlist = (_PVFV *)0x5000;
}

/*
===================================
	Add entry into atexit deinitialzer table. Called by MSVC++ code.
===================================
*/
int __cdecl atexit(_PVFV fn)
{
	// Insure we have enough free space
	if (cur_atexitlist_entries>=max_atexitlist_entries)
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
void _cdecl Exit () {

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
void _cdecl InitializeConstructors()
{
   _atexit_init();
   _initterm(__xc_a, __xc_z); 
}

/*
===================================
	MSVC++ calls this routine if a pure virtual function is called
===================================
*/
int __cdecl _purecall_handler()
{
	// for now, halt the system 'til we have a way to display error
	for (;;);

	// print error message here
}

// Disable C4100 ('Unused Parameter') warning for now, 'til we could get these written
#pragma warning (disable:4100)

extern "C"
{

float __declspec(naked) _CIcos()
{
   _asm {
      fcos
      ret
   };
};

float __declspec(naked) _CIsin()
{
   _asm {
      fsin
      ret
   };
};

float __declspec(naked) _CIsqrt()
{
   _asm {
      fsqrt
      ret
   };
};

//! called by MSVC++ to convert a float to a long
long __declspec (naked) _ftol2_sse() {

	int a;
	_asm {
		fistp [a]
		mov	ebx, a
		ret
	}
}

//! required by MSVC++ runtime for floating point operations (Must be 1)
int _fltused = 1;

//! my implimentation of _aullshr
uint64_t _declspec (naked) _aullshr () {

   _asm {
        //! only handle 64bit shifts or more
        cmp     cl,64
        jae     invalid

		//! handle shifts between 0 and 31 bits
        cmp     cl, 32
        jae     more32
        shrd    eax,edx,cl
        shr     edx,cl
        ret

		//! handle shifts of 32-63 bits
   more32:
        mov     eax,edx
        xor     edx,edx
        and     cl,31
        shr     eax,cl
        ret

   //! invalid number (its less then 32bits), return 0
   invalid:
        xor     eax,eax
        xor     edx,edx
        ret
   }
}

};


/*
===================================
	Global allocators
===================================
*/
/*void *operator new(size_t size)
{
	// Nothing we can do 'til we have a memory manager. Defined here so C++ doesnt mess up
    return 0;
}

void* __cdecl operator new[] (unsigned int size)
{
	// see above
	return 0;
}*/

/*
===================================
	Global deallocators
===================================
*/
/*void __cdecl operator delete(void *p)
{
	// see above
}

void __cdecl operator delete[] (void * p)
{
	// see above
}*/

// enable warning
#pragma warning (default:4100)
