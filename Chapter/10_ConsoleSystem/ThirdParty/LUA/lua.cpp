/*
** $Id: lua.c,v 1.21 1999/07/02 18:22:38 roberto Exp $
** Lua stand-alone interpreter
** See Copyright Notice in lua.h
*/


#include "windef.h"
#include "string.h"

#include "lua.h"
#include "luadebug.h"
#include "lualib.h"


int mainaa (int argc, char *argv[])
{
  int i = 0;
  lua_open();
  lua_pushstring("> "); lua_setglobal("_PROMPT");
  lua_userinit();
  
  int result = lua_dofile(argv[i]);
      
  lua_close();

  return 0;
}