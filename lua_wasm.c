/*
** $Id: lua_wasm.c $
** Lua WASM exported functions
** See Copyright Notice in lua.h
*/

#define lua_wasm_c

#include "lprefix.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#define LUA_EMSCRIPTEN_NAME "=(lua_wasm)"
#if defined(__cplusplus)
  #define LUA_EMSCRIPTEN_API extern "C"
#else
  #define LUA_EMSCRIPTEN_API
#endif

/* Cache of execution result; address crosses language boundaries */
static char *g_ReportResult = NULL;

/* Message handler used to run all chunks */
static int lwasm_msghandler (lua_State *L) {
  const char *msg = lua_tostring(L, 1);
  if (msg == NULL) {  /* is error object not a string? */
    /* does it have a metamethod that produces a string? */
    if (luaL_callmeta(L, 1, "__tostring") && lua_type(L, -1) == LUA_TSTRING) {
      return 1;  /* that is the message */
    }
    msg = lua_pushfstring(L, "(error object is a %s value)", luaL_typename(L, 1));
  }

  luaL_traceback(L, L, msg, 1);  /* append a standard traceback */
  return 1;  /* return the traceback */
}

/* Clear previous results */
static void lwasm_clear (void) {
  if (g_ReportResult != NULL) {
    free(g_ReportResult);
    g_ReportResult = NULL;
  }
}

/* Copy the string on top of the Lua stack to g_ReportResult */
static const char *lwasm_report (lua_State *L) {
  size_t len = 0;
  const char *s = lua_tolstring(L, -1, &len);
  lwasm_clear();
  if (s != NULL) {
    if ((g_ReportResult = (char *)malloc(len + 1)) != NULL) {
      memcpy(g_ReportResult, s, len);
      g_ReportResult[len] = '\0';
      return g_ReportResult;
    }
    return "cannot allocate result: not enough memory";
  }
  return "cannot return result: not a string";
}

/*
** Interface to 'lua_pcall', which initializes the libraries and sets the
** appropriate message function.
*/
static int lwasm_docall (lua_State *L, const char *script, int narg, int nres) {
  int status;
  lua_pushboolean(L, 1);  /* signal for libraries to ignore env. vars. */
  lua_setfield(L, LUA_REGISTRYINDEX, "LUA_NOENV");
  luaL_openlibs(L);  /* open standard libraries */
  lua_gc(L, LUA_GCGEN, 0, 0);  /* GC in generational mode */
  if ((status = luaL_loadbuffer(L, script, strlen(script), LUA_EMSCRIPTEN_NAME)) == LUA_OK) {
    int base = lua_gettop(L) - narg;  /* function index */
    lua_pushcfunction(L, lwasm_msghandler);  /* push message handler */
    lua_insert(L, base);  /* put it under function and args */
    status = lua_pcall(L, narg, nres, base);
    lua_remove(L, base);  /* remove message handler from the stack */
  }
  return status;
}

LUA_EMSCRIPTEN_API const char *lua_wasm_execute (const char *script) {
  lua_State *L = NULL;
  const char *result = NULL;
  lwasm_clear();  /* clear previous result */
  if ((L = luaL_newstate()) == NULL) {  /* create state */
    return "cannot create state: not enough memory";
  }

  if (lwasm_docall(L, script, 0, 0) != LUA_OK) {  /* do the call */
    result = lwasm_report(L);
    lua_pop(L, 1);  /* remove message */
  }

  lua_close(L);
  return result;
}
