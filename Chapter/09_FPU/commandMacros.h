#pragma once
#include "windef.h"

#define	gFrameWorkVersion	"2.0.0"    // major, minor, bugfix


#define START_COMMAND_TABLE		\
typedef struct \
{ \
    char   szCommand[20];\
    bool    bHasArguments;\
    long    (*ProcessingFunc)(char *);\
    char   comments[100];\
}\
CMD_ENTRY, *PCMD_ENTRY; \
	\
CMD_ENTRY g_Commands[] =\
{

#define CMD_ENTRY(x,y,z,zz)    {x,y,z,zz},

#define END_COMMAND_TABLE	\
};\
ULONG	g_NumberOfCommands	=  sizeof(g_Commands)/sizeof(CMD_ENTRY);