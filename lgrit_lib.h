/*
** $Id: lgrit_lib.h $
** API & Auxiliary definitions for gritLua.
** See Copyright Notice in lua.h
*/


#ifndef lgritlib_h
#define lgritlib_h

#include "luaconf.h"
#include "lua.h"

#define cast_vec(i) ((lua_VecF)(i))

#define V_ZERO (cast_vec(0.0))
#define V_HALF (cast_vec(0.5))
#define V_ONE (cast_vec(1.0))
#define V_TWO (cast_vec(2.0))
#define V_PI cast_vec(3.141592653589793238462643383279502884)

/* Default initializer for the zero vector */
#define V_ZEROVEC { .x = V_ZERO, .y = V_ZERO, .z = V_ZERO, .w = V_ZERO }

/*
** {==================================================================
** API
** ===================================================================
*/

LUA_API int (lua_isvector1) (lua_State *L, int idx, int flags);  /* lua_isnumber casted to lua_VecF */
LUA_API int (lua_isvector2) (lua_State* L, int idx, int flags);
LUA_API int (lua_isvector3) (lua_State* L, int idx, int flags);
LUA_API int (lua_isvector4) (lua_State* L, int idx, int flags);
LUA_API int (lua_isquat) (lua_State* L, int idx, int flags);

LUA_API void (lua_checkv1) (lua_State *L, int idx, int flags, lua_Float4 *v);
LUA_API void (lua_checkv2) (lua_State *L, int idx, int flags, lua_Float4 *v);
LUA_API void (lua_checkv3) (lua_State *L, int idx, int flags, lua_Float4 *v);
LUA_API void (lua_checkv4) (lua_State *L, int idx, int flags, lua_Float4 *v);
LUA_API void (lua_checkquat) (lua_State *L, int idx, int flags, lua_Float4 *v);

/* Compatibility API, replaced by: a single lua_pushvector in lua.h */

LUA_API void (lua_pushvector2) (lua_State *L, lua_VecF x, lua_VecF y);
LUA_API void (lua_pushvector3) (lua_State *L, lua_VecF x, lua_VecF y, lua_VecF z);
LUA_API void (lua_pushvector4) (lua_State *L, lua_VecF x, lua_VecF y, lua_VecF z, lua_VecF w);
LUA_API void (lua_pushquat) (lua_State *L, lua_VecF x, lua_VecF y, lua_VecF z, lua_VecF w);

/* }================================================================== */

/*
** {==================================================================
** Base
** ===================================================================
*/

/* Return the label associated with a given vector dimension. */
LUA_API const char *(lua_dimension_label) (lua_State *L, int idx);

/* Number of dimensions associated with the vector type */
LUA_API int (lua_dimensions_count) (lua_State *L, int tp);

/*
** Returns the name of the type encoded by the value tp, which must be a vector
** variant
*/
LUA_API const char *(lua_vectypename) (lua_State *L, int tp);

/*
** Unpack an individual vector and place its contents to onto the Lua stack,
** returning the number of elements (i.e., dimensions of vector).
*/
LUA_API int (lua_unpackvec) (lua_State *L);

/* Push a string representing the vector object on top of the stack. */
LUA_API const char *(lua_pushvecstring) (lua_State *L, int idx);

LUA_API int (lua_vectorN) (lua_State *L);
LUA_API int (lua_vector2) (lua_State *L);
LUA_API int (lua_vector3) (lua_State *L);
LUA_API int (lua_vector4) (lua_State *L);
LUA_API int (lua_quat) (lua_State *L);

/* }================================================================== */

/*
** {==================================================================
** Vector Math
** ===================================================================
*/
/*
**
** vector math library for grit.
**
** This file is in its infancy and will closely monitor either:
** (1) https://github.com/Unity-Technologies/Unity.Mathematics/
** or (2) http://code.google.com/p/slimmath/
**
*/

LUAI_FUNC int (luaVec_dot) (lua_State *L);
LUAI_FUNC int (luaVec_cross) (lua_State *L);
LUAI_FUNC int (luaVec_inv) (lua_State *L);
LUAI_FUNC int (luaVec_norm) (lua_State *L);
LUAI_FUNC int (luaVec_slerp) (lua_State *L);

LUAI_FUNC int (luaVec_abs) (lua_State *L);
LUAI_FUNC int (luaVec_sin) (lua_State *L);
LUAI_FUNC int (luaVec_cos) (lua_State *L);
LUAI_FUNC int (luaVec_tan) (lua_State *L);
LUAI_FUNC int (luaVec_asin) (lua_State *L);
LUAI_FUNC int (luaVec_acos) (lua_State *L);
LUAI_FUNC int (luaVec_atan) (lua_State *L);
LUAI_FUNC int (luaVec_floor) (lua_State *L);
LUAI_FUNC int (luaVec_ceil) (lua_State *L);
LUAI_FUNC int (luaVec_fmod) (lua_State *L);
LUAI_FUNC int (luaVec_sqrt) (lua_State *L);
LUAI_FUNC int (luaVec_log) (lua_State *L);
LUAI_FUNC int (luaVec_exp) (lua_State *L);
LUAI_FUNC int (luaVec_deg) (lua_State *L);
LUAI_FUNC int (luaVec_rad) (lua_State *L);
LUAI_FUNC int (luaVec_min) (lua_State *L);
LUAI_FUNC int (luaVec_max) (lua_State *L);
LUAI_FUNC int (luaVec_clamp) (lua_State *L);

#if defined(LUA_COMPAT_MATHLIB)
LUAI_FUNC int (luaV_sinh) (lua_State *L);
LUAI_FUNC int (luaV_cosh) (lua_State *L);
LUAI_FUNC int (luaV_tanh) (lua_State *L);
LUAI_FUNC int (luaV_pow) (lua_State *L);
LUAI_FUNC int (luaV_log10) (lua_State *L);
#endif

/* }================================================================== */

#endif
