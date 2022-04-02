/*
** $Id: lglmlib.hpp $
** GLM binding library
** See Copyright Notice in lua.h
*/

#ifndef lglmlib_hpp
#define lglmlib_hpp

#define LUAGLM_VERSION_MAJOR "0"
#define LUAGLM_VERSION_MINOR "7"
#define LUAGLM_VERSION_PATCH "4"

#define LUAGLM_NAME "LuaGLM"
#define LUAGLM_COPYRIGHT "Copyright (C) 2022, Gottfried Leibniz"
#define LUAGLM_DESCRIPTION "glm bindings for Lua"
#define LUAGLM_VERSION LUAGLM_VERSION_MAJOR "." LUAGLM_VERSION_MINOR "." LUAGLM_VERSION_PATCH

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
