/*
** $Id: lglm_core.h $
** Internal definitions for GLM objects
** See Copyright Notice in lua.h
*/

#ifndef lglm_core_h
#define lglm_core_h

#include "luaconf.h"
#include "lua.h"
#include "lobject.h"
#include "ltm.h"

/*
@@ LUAGLM_LIBVERSION Version number of the included GLM library. This value is
** manually redefined so it can be used by the strictly-C defined portions of
** the runtime.
**
** Temporary until 820a2c0e625f26000c688d841836bb10483be34d is remedied.
*/
#if !defined(LUAGLM_LIBVERSION)
  #define LUAGLM_LIBVERSION 999
#endif

/* Handle should-be-deprecated-instead-of-removed GLM_FORCE_QUAT_DATA_WXYZ */
#if defined(GLM_FORCE_QUAT_DATA_WXYZ)
  #undef GLM_FORCE_QUAT_DATA_XYZW
#endif

/*
@@ LUAGLM_QUAT_WXYZ quaternion layout: wxyz vs xyzw.
*/
#if LUAGLM_LIBVERSION < 999
#if defined(GLM_FORCE_QUAT_DATA_WXYZ)
  #define LUAGLM_QUAT_WXYZ 1
#else
  #define LUAGLM_QUAT_WXYZ 0
#endif
#elif defined(GLM_FORCE_QUAT_DATA_XYZW)
  #define LUAGLM_QUAT_WXYZ 0
#else
  #define LUAGLM_QUAT_WXYZ 1
#endif

/*
** MAXNUMBER2STR analogue. This value should be greater than (MAXNUMBER2STR * 16) + 64:
**  [d]mat4x4((%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f))
*/
#define LUAGLM_MAX2STR 1024

/*
** Return the vector variant (tag) associated with 'dimensions'. This function
** does not sanitize the input: assumes [2, 4].
*/
static LUA_INLINE lu_byte glm_variant (grit_length_t dimensions) {
  return cast_byte(makevariant(LUA_TVECTOR, (dimensions - 2) /* & 0x3 */));
}

/*
** Return the vector dimension associated with the variant (tag). This function
** does not sanitize the input and 'rtt' must be equal to makevariant(LUA_TVECTOR, X)
** for some 'X'
*/
static LUA_INLINE grit_length_t glm_dimensions (lu_byte rtt) {
  return cast(grit_length_t, (rtt == LUA_VQUAT) ? 4 : (2 + ((rtt & 0x30) >> 4)));
}

/* Half-precision floating-point cplusplus helpers */
#if defined(__cplusplus)
  #define f4_loadf4(F) F
  #define f4_cstoref4(F) F
  #define f4_cstore(x, y, z, w) luai_Float4(f4_init((luai_VecF)(x), (luai_VecF)(y), (luai_VecF)(z), (luai_VecF)(w)))
#endif

/*
** {==================================================================
** Internal vector functions
** ===================================================================
*/

/* 'fast track' equivalent macros for vectors; see luaV_fastget */
#define glmVec_fastgeti(T, I, S) (vecgeti((T), (I), (S)) != LUA_TNONE)
#define glmVec_fastgets(T, K, S) \
  ((tsslen((K)) == 1) && (vecgets((T), getstr((K)), (S)) != LUA_TNONE))

/* Helper function for vector integer access. */
static LUA_INLINE int vecgeti (const TValue *obj, lua_Integer n, StkId res) {
  const lua_Integer D = cast(lua_Integer, glm_dimensions(ttypetag(obj)));
  if (l_likely(n >= 1 && n <= D)) {  /* Accessing vectors is 0-based */
#if LUAGLM_QUAT_WXYZ  /* quaternion has WXYZ layout */
    if (ttypetag(obj) == LUA_VQUAT) n = ((n % 4) + 1);
#endif
    setfltvalue(s2v(res), cast_num(f4_loadf(vvalue_(obj).raw[n - 1])));
    return LUA_TNUMBER;
  }
  return LUA_TNONE;
}

/* Helper function for vector character access. */
static LUA_INLINE int vecgets (const TValue *obj, const char *k, StkId res) {
  grit_length_t _n = 0xF;
  switch (*k) {
    case 'x': case 'r': case '1': _n = 0; break;
    case 'y': case 'g': case '2': _n = 1; break;
    case 'z': case 'b': case '3': _n = 2; break;
    case 'w': case 'a': case '4': _n = 3; break;
    case 'n': {  /* Dimension field has precedence over TM_INDEX */
      setivalue(s2v(res), cast(lua_Integer, glm_dimensions(ttypetag(obj))));
      return LUA_TNUMBER;
    }
    default: {
      return LUA_TNONE;
    }
  }

  if (l_likely(_n < glm_dimensions(ttypetag(obj)))) {
#if LUAGLM_QUAT_WXYZ  /* quaternion has WXYZ layout */
    if (ttypetag(obj) == LUA_VQUAT) _n = ((_n + 1) % 4);
#endif
    setfltvalue(s2v(res), cast_num(f4_loadf(vvalue_(obj).raw[_n])));
    return LUA_TNUMBER;
  }
  return LUA_TNONE;
}

/* rawgeti variant for vector types. */
LUAI_FUNC int glmVec_rawgeti (const TValue *obj, lua_Integer n, StkId res);

/* getfield variant for vector types. */
LUAI_FUNC int glmVec_rawgets (const TValue *obj, const char *k, StkId res);

/* rawget variant for vector types. */
LUAI_FUNC int glmVec_rawget (const TValue *obj, TValue *key, StkId res);

/* OP_GETTABLE variant for vector types. */
LUAI_FUNC void glmVec_get (lua_State *L, const TValue *obj, TValue *key, StkId res);

/* OP_GETI variant for vector types. */
LUAI_FUNC void glmVec_geti (lua_State *L, const TValue *obj, lua_Integer key, StkId res);

/*
** luaV_objlen variant for vector types: place the magnitude of the vector (obj)
** at the specified stack index (res).
*/
LUAI_FUNC void glmVec_objlen (const TValue *obj, StkId res);

/* luaV_equalobj variant for vector types. */
LUAI_FUNC int glmVec_equalObj (lua_State *L, const TValue *o1, const TValue *o2);

/* luaT_tryconcatTM variant for vector types: append a value to a vector type */
LUAI_FUNC int glmVec_concat (const TValue *obj, const TValue *value, StkId res);

/*
** luaO_tostring/tostringbuff variant for vector types: convert a vector object
** to a string, adding it to a buffer.
*/
LUAI_FUNC int glmVec_tostr (const TValue *obj, char *buff, size_t len);

/*
** equalkey variant for vector types: return true if the vector key 'k1' is
** equal to the vector key in node 'n2', false otherwise.
*/
LUAI_FUNC int glmVec_equalKey (const TValue *k1, const Node *n2, int rtt);

/* Return a hash of the given vector object (some function of its components) */
LUAI_FUNC size_t glmVec_hash (const TValue *obj);

/* Return true if each component of the given vector object is finite. */
LUAI_FUNC int glmVec_isfinite (const TValue *obj);

/*
** Pops a key from the stack and pushes a <key, value> pair from the vector at
** the given stack index, the 'next' pair after the given key. If there are no
** more elements in the vector, then returns 0 and pushes nothing.
*/
LUAI_FUNC int glmVec_next (const TValue *obj, StkId key);

/* }================================================================== */

/*
** {==================================================================
** Internal matrix functions
** ===================================================================
*/

/* Fast path equivalent macros. */
#define glmMat_fastgeti(T, I, S) (glmMat_vmgeti((T), (I), (S)) != LUA_TNONE)

/* Create a new collectible matrix object, linking it to the allgc list */
LUAI_FUNC GCMatrix *glmMat_new (lua_State *L);

/* rawgeti variant for matrix types */
LUAI_FUNC int glmMat_rawgeti (const TValue *obj, lua_Integer n, StkId res);

/*
** glmMat_rawgeti that does not set 'res' to nil on invalid access.
**
** @NOTE: matgeti is an inlined function that could be directly used by lvm.c.
** However, certain GLM configurations will implicitly align glm::vec3. This
** logic does not exist within the C boundary of the runtime at the moment (see
** @ImplicitAlign).
*/
LUAI_FUNC int glmMat_vmgeti (const TValue *obj, lua_Integer n, StkId res);

/* rawget variant for matrix types. */
LUAI_FUNC int glmMat_rawget (const TValue *obj, TValue *key, StkId res);

/* lua_rawset variant for matrix types. */
LUAI_FUNC void glmMat_rawset (lua_State *L, const TValue *obj, TValue *key, TValue *val);

/* OP_GETTABLE variant for matrix types. */
LUAI_FUNC void glmMat_get (lua_State *L, const TValue *obj, TValue *key, StkId res);

/* OP_GETI variant for matrix types. */
LUAI_FUNC void glmMat_geti (lua_State *L, const TValue *obj, lua_Integer key, StkId res);

/* lua_settable variant for matrix types */
LUAI_FUNC void glmMat_set (lua_State *L, const TValue *obj, TValue *key, TValue *val);

/* OP_SETI variant for matrix types. */
LUAI_FUNC void glmMat_seti (lua_State *L, const TValue *obj, lua_Integer key, TValue *val);

/*
** luaV_objlen variant for matrix types: place the component count of the matrix
** (obj) at the specified stack index (res).
*/
LUAI_FUNC void glmMat_objlen (const TValue *obj, StkId res);

/*
** luaO_tostring/tostringbuff variant for matrix types: convert a matrix object
** to a string, adding it to a buffer
*/
LUAI_FUNC int glmMat_tostr (const TValue *obj, char *buff, size_t len);

/*
** Pops a key from the stack and pushes a <key, value> pair from the matrix at
** the given stack index, the 'next' pair after the given key. If there are no
** more elements in the matrix, then returns 0 and pushes nothing.
*/
LUAI_FUNC int glmMat_next (const TValue *obj, StkId key);

/* luaV_equalobj variant for matrix types */
LUAI_FUNC int glmMat_equalObj (lua_State *L, const TValue *o1, const TValue *o2);

/* }================================================================== */

/*
** {==================================================================
** Miscellaneous
** ===================================================================
*/

/* trybinTM handler for GLM objects */
LUAI_FUNC int luaglm_trybinTM (lua_State *L, const TValue *p1, const TValue *p2, StkId res, TMS event);

/* Jenkins one-at-a-time hash. */
LUAI_FUNC lua_Integer luaO_HashString (const char *string, size_t length, int ignore_case);

/* }================================================================== */

#endif
