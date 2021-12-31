/*
** $Id: lopcodes.c $
** Opcodes for Lua virtual machine
** See Copyright Notice in lua.h
*/

#define lopcodes_c
#define LUA_CORE

#include "lprefix.h"


#include "lopcodes.h"


/* ORDER OP */

LUAI_DDEF const lu_byte luaP_opmodes[NUM_OPCODES] = {
  #include "lopcodes_opmodes.h"
};

