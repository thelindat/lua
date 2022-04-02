/*
** $Id: lgrit_lib.h $
** API definitions for grit-lua.
**
** See Copyright Notice in lua.h
*/

#ifndef lgritlib_h
#define lgritlib_h

#include "luaconf.h"
#include "lua.h"

/*
** {==================================================================
** grit-lua API: originally declared in lua.h
** ===================================================================
*/

LUA_API int lua_isvector2 (lua_State *L, int idx);
LUA_API int lua_isvector3 (lua_State *L, int idx);
LUA_API int lua_isvector4 (lua_State *L, int idx);
LUA_API int lua_isquat (lua_State *L, int idx);

LUA_API void lua_checkvector2 (lua_State *L, int idx, lua_VecF *x, lua_VecF *y);
LUA_API void lua_checkvector3 (lua_State *L, int idx, lua_VecF *x, lua_VecF *y, lua_VecF *z);
LUA_API void lua_checkvector4 (lua_State *L, int idx, lua_VecF *x, lua_VecF *y, lua_VecF *z, lua_VecF *w);
LUA_API void lua_checkquat (lua_State *L, int idx, lua_VecF *w, lua_VecF *x, lua_VecF *y, lua_VecF *z);

LUA_API void lua_pushvector2 (lua_State *L, lua_VecF x, lua_VecF y);
LUA_API void lua_pushvector3 (lua_State *L, lua_VecF x, lua_VecF y, lua_VecF z);
LUA_API void lua_pushvector4 (lua_State *L, lua_VecF x, lua_VecF y, lua_VecF z, lua_VecF w);
LUA_API void lua_pushquat (lua_State *L, lua_VecF w, lua_VecF x, lua_VecF y, lua_VecF z);

/* }================================================================== */

/*
** {==================================================================
** Extended grit-lua API: added in Lua5.4/LuaGLM port
** ===================================================================
*/

/*
** vector type variants exposed in the library to simplify the internal/external
** translation between vector types (grit-lua compatibility).
**
** @ImplicitVec: single component vectors are represented by LUA_TNUMBER
*/
#if !defined(LUA_VVECTOR3)
#define LUA_VVECTOR1 (LUA_TNUMBER | (1 << 4))
#define LUA_VVECTOR2 (LUA_TVECTOR | (0 << 4))
#define LUA_VVECTOR3 (LUA_TVECTOR | (1 << 4))
#define LUA_VVECTOR4 (LUA_TVECTOR | (2 << 4))
#define LUA_VQUAT (LUA_TVECTOR | (3 << 4))
#endif

/*
** Return the vector variant if the value at the given index is a vector, zero
** otherwise.
*/
LUA_API int lua_isvector (lua_State *L, int idx);

/*
** Populate 'vector' and return its variant if the value at the given index is a
** vector, zero otherwise.
*/
LUA_API int lua_tovector (lua_State *L, int idx, lua_Float4 *vector);

/* Push the vector of 'variant' represented by 'f4' onto the stack. */
LUA_API void lua_pushvector (lua_State *L, lua_Float4 f4, int variant);

/* Push a quaternion represented by 'f4' onto the stack. */
LUA_API void lua_pushquatf4 (lua_State *L, lua_Float4 f4);

/*
** Return true if the value at the given index is a matrix. Storing the matrix
** type in 'type' when not NULL; see LUAGLM_MATRIX_TYPE.
*/
LUA_API int lua_ismatrix (lua_State *L, int idx, int *type);
LUA_API int lua_tomatrix (lua_State *L, int idx, lua_Mat4 *matrix);
LUA_API void lua_pushmatrix (lua_State *L, const lua_Mat4 *matrix);

/* }================================================================== */

/*
** {==================================================================
** Miscellaneous
** ===================================================================
*/

/*
** Jenkins-hash the value at the given index: string values are hashed, boolean
** and numeric values are cast to lua_Integer, otherwise, zero is returned.
**
** ignore_case: A string value is hashed as-is. Otherwise, the lowercase of each
**   string character is computed then hashed.
*/
LUA_API lua_Integer luaglm_tohash (lua_State *L, int idx, int ignore_case);

/*
** Return the GLM type string (number, vector, or matrix) of the value at the
** given index.
*/
LUA_API const char *luaglm_typename (lua_State *L, int idx);

/*
** Push onto the stack a formatted string of the vector/matrix value at the
** given index. Returning a pointer to the string.
*/
LUA_API const char *luaglm_pushstring (lua_State *L, int idx);

/*
** Unpack the contents of the vector or matrix value at the given index onto the
** stack and return the number of elements (i.e., dimensions of vector or
** columns of matrix).
*/
LUA_API int luaglm_unpack (lua_State *L, int idx);

/* }================================================================== */

/*
** {==================================================================
** grit-lua/LuaGLM library functions
** ===================================================================
*/

/* Constructors */

LUA_API int luaglm_vec (lua_State *L);
LUA_API int luaglm_vec1 (lua_State *L);
LUA_API int luaglm_vec2 (lua_State *L);
LUA_API int luaglm_vec3 (lua_State *L);
LUA_API int luaglm_vec4 (lua_State *L);

LUA_API int luaglm_ivec (lua_State *L);
LUA_API int luaglm_ivec1 (lua_State *L);
LUA_API int luaglm_ivec2 (lua_State *L);
LUA_API int luaglm_ivec3 (lua_State *L);
LUA_API int luaglm_ivec4 (lua_State *L);

LUA_API int luaglm_bvec (lua_State *L);
LUA_API int luaglm_bvec1 (lua_State *L);
LUA_API int luaglm_bvec2 (lua_State *L);
LUA_API int luaglm_bvec3 (lua_State *L);
LUA_API int luaglm_bvec4 (lua_State *L);

LUA_API int luaglm_mat (lua_State *L);
LUA_API int luaglm_mat2x2 (lua_State *L);
LUA_API int luaglm_mat2x3 (lua_State *L);
LUA_API int luaglm_mat2x4 (lua_State *L);
LUA_API int luaglm_mat3x2 (lua_State *L);
LUA_API int luaglm_mat3x3 (lua_State *L);
LUA_API int luaglm_mat3x4 (lua_State *L);
LUA_API int luaglm_mat4x2 (lua_State *L);
LUA_API int luaglm_mat4x3 (lua_State *L);
LUA_API int luaglm_mat4x4 (lua_State *L);

LUA_API int luaglm_qua (lua_State *L);

/*
** Returns the dot product of x and y, i.e., result = x * y.
**   T glm::dot(qua<T, Q> const &x, qua<T, Q> const &y)
**   T glm::dot(vec<L, T, Q> const &x, vec<L, T, Q> const &y)
*/
LUA_API int luaglm_dot (lua_State *L);

/*
** Returns the cross product of x and y.
**   qua<T, Q> glm::cross(qua<T, Q> const &x, qua<T, Q> const &y)
**   vec<3, T, Q> glm::cross(qua<T, Q> const &x, vec<3, T, Q> const &y)
**   vec<3, T, Q> glm::cross(vec<3, T, Q> const &x, vec<3, T, Q> const &y)
**   vec<3, T, Q> glm::cross(vec<3, T, Q> const &x, qua<T, Q> const &y)
**   T glm::cross(vec<2, T, Q> const &x, vec<2, T, Q> const &y)
*/
LUA_API int luaglm_cross (lua_State *L);

/*
** Returns the m/q inverse.
**   qua<T, Q> glm::inverse(qua<T, Q> const &q)
**   mat<C, R, T, Q> glm::inverse(mat<C, R, T, Q> const &m)
*/
LUA_API int luaglm_inverse (lua_State *L);

/*
** Returns a vector in the same direction as x but with length of 1.
**   qua<T, Q> glm::normalize(qua<T, Q> const &x)
**   vec<L, T, Q> glm::normalize(vec<L, T, Q> const &x)
*/
LUA_API int luaglm_normalize (lua_State *L);

/*
** Returns the spherical linear interpolation of two vectors or quaternions.
**   qua<T, Q> glm::slerp(qua<T, Q> const &x, qua<T, Q> const &y, T a)
**   vec<3, T, Q> glm::slerp(vec<3, T, Q> const &x, vec<3, T, Q> const &y, T const &a)
*/
LUA_API int luaglm_slerp (lua_State *L);

/* grit-lua math library extension */
LUA_API int luaglm_clamp (lua_State *L);

/* }================================================================== */

#endif
