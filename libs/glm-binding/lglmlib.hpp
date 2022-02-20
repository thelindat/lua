/*
** $Id: lglmlib.hpp $
** GLM binding library
** See Copyright Notice in lua.h
*/

#ifndef lglmlib_hpp
#define lglmlib_hpp

#define LUAGLM_NAME "lua-glm"
#define LUAGLM_VERSION "lua-glm 0.6.39"
#define LUAGLM_COPYRIGHT "Copyright (C) 2022, Gottfried Leibniz"
#define LUAGLM_DESCRIPTION "glm bindings for Lua"

#include <lua.hpp>
#if defined(__cplusplus)
extern "C" {
#endif

#define LUA_GLMLIBNAME "glm"
LUAMOD_API int (luaopen_glm) (lua_State *L);

#if defined(__cplusplus)
}
#endif
#endif
