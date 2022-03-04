/*
** $Id: lgrit_lib.h $
** API definitions for grit-lua.
**
** All functions defined in this header are expected to have default linkage and
** interface with other Lua libraries. For simplicity, this header also
** maintains all LuaGLM API functions under the same constraints.
**
** See Copyright Notice in lua.h
*/

#ifndef lgritlib_h
#define lgritlib_h

#include "luaconf.h"
#include "lua.h"
#include "lauxlib.h"

/*
** {==================================================================
** LuaGLM C-API
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

/* Return the name of the GLM type (number, vector, matrix) at 'idx'. */
LUA_API const char *luaglm_typename (lua_State *L, int idx);

/*
** Pushes onto the stack a formatted string of the vector/matrix at 'idx' and
** returns a pointer to this string.
*/
LUA_API const char *luaglm_pushstring (lua_State *L, int idx);

/*
** Place the contents of the vector or matrix at 'idx' onto the stack, returning
** the number of elements (i.e., dimensions of vector or columns of matrix).
*/
LUA_API int luaglm_unpack (lua_State *L, int idx);

/* }================================================================== */

/*
** {==================================================================
** @DEPRECATED: grit-lua API
** ===================================================================
*/

/* These functions are defined in lua.h of grit-lua */

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
** @DEPRECATED: Extended grit-lua API
** ===================================================================
*/

/*
** vector variants exposed in the library to simplify the internal/external
** translation between vector-types. (grit-lua compatibility)
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

/* Return the length of the vector if it is indeed a vector, zero otherwise. */
LUA_API int lua_isvector (lua_State *L, int idx);
LUA_API int lua_tovector (lua_State *L, int idx, lua_Float4 *vector);
LUA_API void lua_pushvector (lua_State *L, lua_Float4 f4, int variant);
LUA_API void lua_pushquatf4 (lua_State *L, lua_Float4 f4);

/*
** Return true if the object at the given index is a matrix. Storing its
** dimension tag in 'dimensions' when not NULL; see LUAGLM_MATRIX_TYPE.
*/
LUA_API int lua_ismatrix (lua_State *L, int idx, int *dimensions);
LUA_API int lua_tomatrix (lua_State *L, int idx, lua_Mat4 *matrix);
LUA_API void lua_pushmatrix (lua_State *L, const lua_Mat4 *matrix);

/* }================================================================== */

/*
** {==================================================================
** Miscellaneous
** ===================================================================
*/

/*
** Jenkins-hash the object at 'idx'. String values are hashed, boolean and
** numeric values are casted to lua_Integer; otherwise, zero is returned.
**
** ignore_case: A string value is hashed as-is. Otherwise, the lowercase of each
**  string character is computed then hashed.
*/
LUA_API lua_Integer lua_tohash (lua_State *L, int idx, int ignore_case);

/* }================================================================== */

/*
** {==================================================================
** @DEPRECATED: grit-lua base library compatibility
** ===================================================================
*/

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
** Spherical linear interpolation of two vectors/quaternions.
**   qua<T, Q> glm::slerp(qua<T, Q> const &x, qua<T, Q> const &y, T a)
**   vec<3, T, Q> glm::slerp(vec<3, T, Q> const &x, vec<3, T, Q> const &y, T const &a)
*/
LUA_API int luaglm_slerp (lua_State *L);

/* grit-lua math library extension */
LUA_API int luaglm_clamp (lua_State *L);

/* }================================================================== */

#endif
