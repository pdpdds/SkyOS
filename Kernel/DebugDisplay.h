#ifndef _DEBUGDISPLAY_H
#define _DEBUGDISPLAY_H
//****************************************************************************
//**
//**    DebugDisplay.h
//**    - Provides display capabilities for debugging. Because it is
//**	  specifically for debugging and not final release, we don't
//** 	  care for portability here
//**
//****************************************************************************

//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================

#include <stdint.h>

//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================
//============================================================================
//    INTERFACE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
//============================================================================
//    INTERFACE DATA DECLARATIONS
//============================================================================
//============================================================================
//    INTERFACE FUNCTION PROTOTYPES
//============================================================================

extern void DebugPutc (unsigned char c);
extern void DebugClrScr (const uint8_t c);
extern void DebugPuts (char* str);
extern "C" int DebugPrintf (const char* str, ...);
extern unsigned DebugSetColor (const unsigned c);
extern void DebugGotoXY (unsigned x, unsigned y);
extern void DebugGetXY (unsigned* x, unsigned* y);
extern int DebugGetHorz ();
extern int DebugGetVert ();

//============================================================================
//    INTERFACE OBJECT CLASS DEFINITIONS
//============================================================================
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================
//****************************************************************************
//**
//**    END [FILE NAME]
//**
//****************************************************************************
#endif
