/*
** $Id: lopnames.h $
** Opcode names
** See Copyright Notice in lua.h
*/

#if !defined(lopnames_h)
#define lopnames_h

#include <stddef.h>


/* ORDER OP */

static const char *const opnames[] = {
  #include "lopnames_opnames.h"
  NULL
};

#endif

