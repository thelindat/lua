/*
** $Id: lglm.h $
** Library & API definitions for LuaGLM.
**
** All functions defined in this header are expected to have C++ linkage
** and are to be isolated from the Lua core.
**
** See Copyright Notice in lua.h
*/
#ifndef lglm_h
#define lglm_h

#include "lua.hpp"
#include <glm/glm.hpp>

/*
** @COMPAT: Fix for missing "ext/quaternion_common.hpp" include in type_quat.hpp
**  was introduced in 0.9.9.9. Note, "detail/qualifier.hpp" forward declares the
**  quaternion type so this include must be placed before "type_quat.hpp".
*/
#if GLM_VERSION < 999
  #include <glm/gtc/quaternion.hpp>
#endif
#include <glm/detail/type_quat.hpp>

/*
@@ LUAGLM_LIBVERSION_MIN Minimum supported GLM_VERSION.
@@ LUAGLM_LIBVERSION_MAX Maximum supported GLM_VERSION.
*/
#define LUAGLM_LIBVERSION_MIN 991
#define LUAGLM_LIBVERSION_MAX 999
#if GLM_VERSION < LUAGLM_LIBVERSION_MIN || GLM_VERSION > LUAGLM_LIBVERSION_MAX
  #error "GLM error: unsupported version"
#endif

/* @COMPAT: GLM_DEFAULT_CTOR introduced in 0.9.9.9 */
#if !defined(GLM_DEFAULT_CTOR)
  #define GLM_DEFAULT_CTOR GLM_DEFAULT
#endif

/* @COMPAT: GLM_IF_CONSTEXPR introduced in 0.9.9.5 */
#if !defined(GLM_IF_CONSTEXPR)
  #define GLM_IF_CONSTEXPR if
#endif

/* @COMPAT: GLM_CONFIG_DEFAULTED_DEFAULT_CTOR introduced in 0.9.9.9 */
#if !defined(GLM_CONFIG_DEFAULTED_DEFAULT_CTOR)
  #define GLM_CONFIG_DEFAULTED_DEFAULT_CTOR GLM_CONFIG_DEFAULTED_FUNCTIONS
#endif

/* @COMPAT: fix GLM_NEVER_INLINE on COMPILER_VC */
#if defined(GLM_FORCE_INLINE) && (GLM_COMPILER & GLM_COMPILER_VC)
  #undef GLM_NEVER_INLINE
  #define GLM_NEVER_INLINE __declspec(noinline)
#endif

/*
** @COMPAT: defaulted constructors fixed in PR #1027
**  Compensating for that change will require expanding the glmVector, glmMatrix,
**  and GLM boundary structs to handle the implicitly deleted constructors and
**  assignment operators.
*/
#if GLM_HAS_DEFAULTED_FUNCTIONS && GLM_CONFIG_DEFAULTED_FUNCTIONS == GLM_DISABLE
  #error "GLM error: invalid GLM_FORCE_CTOR_INIT configuration (WIP)"
#endif

/*
** {==================================================================
** Configuration
** ===================================================================
*/

/*
@@ LUAGLM_API A mark for all core GLM API functions.
*/
#if !defined(LUAGLM_API)
  #define LUAGLM_API LUA_API
#endif

/* Floating point glm-operation type */
typedef LUAGLM_FLOAT_TYPE glm_Float;

/* Integer glm-operation type */
typedef LUAGLM_INT_TYPE glm_Integer;

/*
@@ LUAGLM_Q Specifies how vector, quat, and matrix types are qualified in terms
** of alignment and precision by the runtime/API. In practice, this is used to
** allow libraries to use different GLM alignment configurations.
*/
#if !defined(LUAGLM_Q)
#if defined(LUAGLM_FORCES_ALIGNED_GENTYPES)
  #if GLM_CONFIG_ALIGNED_GENTYPES == GLM_ENABLE
    #define LUAGLM_Q glm::qualifier::aligned_highp
  #else
    #define LUAGLM_Q glm::qualifier::highp
    #error "Invalid ALIGNED_GENTYPES configuration; compiler does not support aligned types"
  #endif
#else
  #define LUAGLM_Q glm::qualifier::highp
#endif
#endif

/* lib:LuaGLM requirements */
#define GLM_STRING_INTEGER "integer"
#define GLM_STRING_NUMBER "number"
#define GLM_STRING_VECTOR "vector"
#define GLM_STRING_VECTOR1 "vector1"
#define GLM_STRING_VECTOR2 "vector2"
#define GLM_STRING_VECTOR3 "vector3"
#define GLM_STRING_VECTOR4 "vector4"
#define GLM_STRING_QUATERN "quat"
#define GLM_STRING_MATRIX "matrix"
#define GLM_STRING_SYMMATRIX "symmetric " GLM_STRING_MATRIX

/* }================================================================== */

/*
** {==================================================================
** GLM Interface
** ===================================================================
*/

/*
** Return true if the element at the given index is a vector, setting "size" to
** the dimensions of the vector.
*/
LUAGLM_API bool glm_isvector(lua_State *L, int idx, glm::length_t &size);

/* Return true if the element at the given index is a quaternion. */
LUAGLM_API bool glm_isquat(lua_State *L, int idx);

/*
** Return true if the element at the given index is a matrix, setting
** "dimensions" to the dimensions tag of the matrix; see LUAGLM_MATRIX_TYPE.
*/
LUAGLM_API bool glm_ismatrix(lua_State *L, int idx, glm::length_t &dimensions);

/* Push a vector/quaternion onto the stack. */
LUAGLM_API int glm_pushvec1(lua_State *L, const glm::vec<1, glm_Float, LUAGLM_Q> &v);
LUAGLM_API int glm_pushvec2(lua_State *L, const glm::vec<2, glm_Float, LUAGLM_Q> &v);
LUAGLM_API int glm_pushvec3(lua_State *L, const glm::vec<3, glm_Float, LUAGLM_Q> &v);
LUAGLM_API int glm_pushvec4(lua_State *L, const glm::vec<4, glm_Float, LUAGLM_Q> &v);
LUAGLM_API int glm_pushquat(lua_State *L, const glm::qua<glm_Float, LUAGLM_Q> &q);

/*
** Convert the element at the given index into a vector/quaternion if
** permissible, i.e., the dimensions of element is greater-than-or-equal to the
** dimensions of the conversion.
*/

LUAGLM_API glm::vec<1, glm_Float, LUAGLM_Q> glm_tovec1(lua_State *L, int idx);
LUAGLM_API glm::vec<2, glm_Float, LUAGLM_Q> glm_tovec2(lua_State *L, int idx);
LUAGLM_API glm::vec<3, glm_Float, LUAGLM_Q> glm_tovec3(lua_State *L, int idx);
LUAGLM_API glm::vec<4, glm_Float, LUAGLM_Q> glm_tovec4(lua_State *L, int idx);
LUAGLM_API glm::qua<glm_Float, LUAGLM_Q> glm_toquat(lua_State *L, int idx);

/* Push a matrix onto the stack. */
LUAGLM_API int glm_pushmat2x2(lua_State *L, const glm::mat<2, 2, glm_Float, LUAGLM_Q> &m);
LUAGLM_API int glm_pushmat2x3(lua_State *L, const glm::mat<2, 3, glm_Float, LUAGLM_Q> &m);
LUAGLM_API int glm_pushmat2x4(lua_State *L, const glm::mat<2, 4, glm_Float, LUAGLM_Q> &m);
LUAGLM_API int glm_pushmat3x2(lua_State *L, const glm::mat<3, 2, glm_Float, LUAGLM_Q> &m);
LUAGLM_API int glm_pushmat3x3(lua_State *L, const glm::mat<3, 3, glm_Float, LUAGLM_Q> &m);
LUAGLM_API int glm_pushmat3x4(lua_State *L, const glm::mat<3, 4, glm_Float, LUAGLM_Q> &m);
LUAGLM_API int glm_pushmat4x2(lua_State *L, const glm::mat<4, 2, glm_Float, LUAGLM_Q> &m);
LUAGLM_API int glm_pushmat4x3(lua_State *L, const glm::mat<4, 3, glm_Float, LUAGLM_Q> &m);
LUAGLM_API int glm_pushmat4x4(lua_State *L, const glm::mat<4, 4, glm_Float, LUAGLM_Q> &m);

/*
** Convert the element at the given index into a matrix if permissible, i.e.,
** the number of columns to the element is greater-than-or-equal to the
** dimensions of the conversion.
*/

LUAGLM_API glm::mat<2, 2, glm_Float, LUAGLM_Q> glm_tomat2x2(lua_State *L, int idx);
LUAGLM_API glm::mat<2, 3, glm_Float, LUAGLM_Q> glm_tomat2x3(lua_State *L, int idx);
LUAGLM_API glm::mat<2, 4, glm_Float, LUAGLM_Q> glm_tomat2x4(lua_State *L, int idx);
LUAGLM_API glm::mat<3, 2, glm_Float, LUAGLM_Q> glm_tomat3x2(lua_State *L, int idx);
LUAGLM_API glm::mat<3, 3, glm_Float, LUAGLM_Q> glm_tomat3x3(lua_State *L, int idx);
LUAGLM_API glm::mat<3, 4, glm_Float, LUAGLM_Q> glm_tomat3x4(lua_State *L, int idx);
LUAGLM_API glm::mat<4, 2, glm_Float, LUAGLM_Q> glm_tomat4x2(lua_State *L, int idx);
LUAGLM_API glm::mat<4, 3, glm_Float, LUAGLM_Q> glm_tomat4x3(lua_State *L, int idx);
LUAGLM_API glm::mat<4, 4, glm_Float, LUAGLM_Q> glm_tomat4x4(lua_State *L, int idx);

/*
** Return the dimensions of the vector at the given index; zero on failure. This
** function is simply syntactic sugar for glm_isvector;
*/
static LUA_INLINE glm::length_t glm_vector_length(lua_State *L, int idx) {
  glm::length_t size = 0;
  return glm_isvector(L, idx, size) ? size : 0;
}

/* }================================================================== */

/*
** {==================================================================
**  GLM Object & Internal Definitions
** ===================================================================
*/

/// <summary>
/// Internal vector definition
/// </summary>
union glmVector {
  glm::vec<1, glm_Float, LUAGLM_Q> v1;
  glm::vec<2, glm_Float, LUAGLM_Q> v2;
  glm::vec<3, glm_Float, LUAGLM_Q> v3;
  glm::vec<4, glm_Float, LUAGLM_Q> v4;
  glm::qua<glm_Float, LUAGLM_Q> q;

#if GLM_CONFIG_DEFAULTED_FUNCTIONS == GLM_DISABLE
#if GLM_CONFIG_CTOR_INIT == GLM_CTOR_INITIALIZER_LIST
  glmVector() : v4(glm::vec<4, glm_Float, LUAGLM_Q>()) { }
#else
  glmVector() { v4 = glm::vec<4, glm_Float, LUAGLM_Q>(); }
#endif
#else
  glmVector() GLM_DEFAULT_CTOR;
#endif
  glmVector(const glm::vec<1, glm_Float, LUAGLM_Q> &_v) : v1(_v) { }
  glmVector(const glm::vec<2, glm_Float, LUAGLM_Q> &_v) : v2(_v) { }
  glmVector(const glm::vec<3, glm_Float, LUAGLM_Q> &_v) : v3(_v) { }
  glmVector(const glm::vec<4, glm_Float, LUAGLM_Q> &_v) : v4(_v) { }
  glmVector(const glm::qua<glm_Float, LUAGLM_Q> &_q) : q(_q) { }

  // Realignment Constructors

  template<glm::qualifier P> glmVector(const glm::vec<1, glm_Float, P> &_v) : v1(_v) { }
  template<glm::qualifier P> glmVector(const glm::vec<2, glm_Float, P> &_v) : v2(_v) { }
  template<glm::qualifier P> glmVector(const glm::vec<3, glm_Float, P> &_v) : v3(_v) { }
  template<glm::qualifier P> glmVector(const glm::vec<4, glm_Float, P> &_v) : v4(_v) { }
  template<glm::qualifier P> glmVector(const glm::qua<glm_Float, P> &_q) : q(_q) { }

  template<typename T, glm::qualifier P> glmVector(const glm::vec<1, T, P> &_v) : v1(_v) { }
  template<typename T, glm::qualifier P> glmVector(const glm::vec<2, T, P> &_v) : v2(_v) { }
  template<typename T, glm::qualifier P> glmVector(const glm::vec<3, T, P> &_v) : v3(_v) { }
  template<typename T, glm::qualifier P> glmVector(const glm::vec<4, T, P> &_v) : v4(_v) { }
  template<typename T, glm::qualifier P> glmVector(const glm::qua<T, P> &_q) : q(_q) { }

  // Assignment Operators

  inline void operator=(const glm::vec<1, glm_Float, LUAGLM_Q> &_v) { v1 = _v; }
  inline void operator=(const glm::vec<2, glm_Float, LUAGLM_Q> &_v) { v2 = _v; }
  inline void operator=(const glm::vec<3, glm_Float, LUAGLM_Q> &_v) { v3 = _v; }
  inline void operator=(const glm::vec<4, glm_Float, LUAGLM_Q> &_v) { v4 = _v; }
  inline void operator=(const glm::qua<glm_Float, LUAGLM_Q> &_q) { q = _q; }

  template<typename T, glm::qualifier P> inline void operator=(const glm::vec<1, T, P> &_v) { v1 = glm::vec<1, glm_Float, LUAGLM_Q>(_v); }
  template<typename T, glm::qualifier P> inline void operator=(const glm::vec<2, T, P> &_v) { v2 = glm::vec<2, glm_Float, LUAGLM_Q>(_v); }
  template<typename T, glm::qualifier P> inline void operator=(const glm::vec<3, T, P> &_v) { v3 = glm::vec<3, glm_Float, LUAGLM_Q>(_v); }
  template<typename T, glm::qualifier P> inline void operator=(const glm::vec<4, T, P> &_v) { v4 = glm::vec<4, glm_Float, LUAGLM_Q>(_v); }
  template<typename T, glm::qualifier P> inline void operator=(const glm::qua<T, P> &_q) { q = glm::qua<glm_Float, LUAGLM_Q>(_q); }

  // Reassignment; glm::vec = glmVector.

  inline int Get(glm::vec<1, glm_Float, LUAGLM_Q> &_v) const { _v = v1; return 1; }
  inline int Get(glm::vec<2, glm_Float, LUAGLM_Q> &_v) const { _v = v2; return 1; }
  inline int Get(glm::vec<3, glm_Float, LUAGLM_Q> &_v) const { _v = v3; return 1; }
  inline int Get(glm::vec<4, glm_Float, LUAGLM_Q> &_v) const { _v = v4; return 1; }
  inline int Get(glm::qua<glm_Float, LUAGLM_Q> &_q) const { _q = q; return 1; }

  template<typename T, glm::qualifier P> inline int Get(glm::vec<1, T, P> &_v) const { _v = glm::vec<1, T, P>(v1); return 1; }
  template<typename T, glm::qualifier P> inline int Get(glm::vec<2, T, P> &_v) const { _v = glm::vec<2, T, P>(v2); return 1; }
  template<typename T, glm::qualifier P> inline int Get(glm::vec<3, T, P> &_v) const { _v = glm::vec<3, T, P>(v3); return 1; }
  template<typename T, glm::qualifier P> inline int Get(glm::vec<4, T, P> &_v) const { _v = glm::vec<4, T, P>(v4); return 1; }
  template<typename T, glm::qualifier P> inline int Get(glm::qua<T, P> &_q) const { _q = glm::qua<T, P>(q); return 1; }
};

/// <summary>
/// Internal matrix definition.
/// </summary>
LUAGLM_ALIGNED_TYPE(struct, glmMatrix) {
  union {
    glm::mat<2, 2, glm_Float, LUAGLM_Q> m22;
    glm::mat<2, 3, glm_Float, LUAGLM_Q> m23;
    glm::mat<2, 4, glm_Float, LUAGLM_Q> m24;
    glm::mat<3, 2, glm_Float, LUAGLM_Q> m32;
    glm::mat<3, 3, glm_Float, LUAGLM_Q> m33;
    glm::mat<3, 4, glm_Float, LUAGLM_Q> m34;
    glm::mat<4, 2, glm_Float, LUAGLM_Q> m42;
    glm::mat<4, 3, glm_Float, LUAGLM_Q> m43;
    glm::mat<4, 4, glm_Float, LUAGLM_Q> m44;
  };
  glm::length_t dimensions;

#if GLM_CONFIG_DEFAULTED_DEFAULT_CTOR == GLM_DISABLE
#if GLM_CONFIG_CTOR_INIT == GLM_CTOR_INITIALIZER_LIST
  glmMatrix() : m44(glm::mat<4, 4, glm_Float, LUAGLM_Q>()), dimensions(LUAGLM_MATRIX_4x4) { }
#else
  glmMatrix() { dimensions = LUAGLM_MATRIX_4x4; m44 = glm::mat<4, 4, glm_Float, LUAGLM_Q>(); }
#endif
#else
  glmMatrix() GLM_DEFAULT_CTOR;
#endif
  glmMatrix(const glm::mat<2, 2, glm_Float, LUAGLM_Q> &_m) : m22(_m), dimensions(LUAGLM_MATRIX_2x2) { }
  glmMatrix(const glm::mat<2, 3, glm_Float, LUAGLM_Q> &_m) : m23(_m), dimensions(LUAGLM_MATRIX_2x3) { }
  glmMatrix(const glm::mat<2, 4, glm_Float, LUAGLM_Q> &_m) : m24(_m), dimensions(LUAGLM_MATRIX_2x4) { }
  glmMatrix(const glm::mat<3, 2, glm_Float, LUAGLM_Q> &_m) : m32(_m), dimensions(LUAGLM_MATRIX_3x2) { }
  glmMatrix(const glm::mat<3, 3, glm_Float, LUAGLM_Q> &_m) : m33(_m), dimensions(LUAGLM_MATRIX_3x3) { }
  glmMatrix(const glm::mat<3, 4, glm_Float, LUAGLM_Q> &_m) : m34(_m), dimensions(LUAGLM_MATRIX_3x4) { }
  glmMatrix(const glm::mat<4, 2, glm_Float, LUAGLM_Q> &_m) : m42(_m), dimensions(LUAGLM_MATRIX_4x2) { }
  glmMatrix(const glm::mat<4, 3, glm_Float, LUAGLM_Q> &_m) : m43(_m), dimensions(LUAGLM_MATRIX_4x3) { }
  glmMatrix(const glm::mat<4, 4, glm_Float, LUAGLM_Q> &_m) : m44(_m), dimensions(LUAGLM_MATRIX_4x4) { }

  // Realignment Constructors

  template<glm::qualifier P> glmMatrix(const glm::mat<2, 2, glm_Float, P> &_m) : m22(_m), dimensions(LUAGLM_MATRIX_2x2) { }
  template<glm::qualifier P> glmMatrix(const glm::mat<2, 3, glm_Float, P> &_m) : m23(_m[0], _m[1]), dimensions(LUAGLM_MATRIX_2x3) { }
  template<glm::qualifier P> glmMatrix(const glm::mat<2, 4, glm_Float, P> &_m) : m24(_m), dimensions(LUAGLM_MATRIX_2x4) { }
  template<glm::qualifier P> glmMatrix(const glm::mat<3, 2, glm_Float, P> &_m) : m32(_m), dimensions(LUAGLM_MATRIX_3x2) { }
  template<glm::qualifier P> glmMatrix(const glm::mat<3, 3, glm_Float, P> &_m) : m33(_m), dimensions(LUAGLM_MATRIX_3x3) { }
  template<glm::qualifier P> glmMatrix(const glm::mat<3, 4, glm_Float, P> &_m) : m34(_m), dimensions(LUAGLM_MATRIX_3x4) { }
  template<glm::qualifier P> glmMatrix(const glm::mat<4, 2, glm_Float, P> &_m) : m42(_m), dimensions(LUAGLM_MATRIX_4x2) { }
  template<glm::qualifier P> glmMatrix(const glm::mat<4, 3, glm_Float, P> &_m) : m43(_m), dimensions(LUAGLM_MATRIX_4x3) { }
  template<glm::qualifier P> glmMatrix(const glm::mat<4, 4, glm_Float, P> &_m) : m44(_m), dimensions(LUAGLM_MATRIX_4x4) { }

  // Assignment Operators

  inline void operator=(const glm::mat<2, 2, glm_Float, LUAGLM_Q> &_m) { dimensions = LUAGLM_MATRIX_2x2; m22 = _m; }
  inline void operator=(const glm::mat<2, 3, glm_Float, LUAGLM_Q> &_m) { dimensions = LUAGLM_MATRIX_2x3; m23 = _m; }
  inline void operator=(const glm::mat<2, 4, glm_Float, LUAGLM_Q> &_m) { dimensions = LUAGLM_MATRIX_2x4; m24 = _m; }
  inline void operator=(const glm::mat<3, 2, glm_Float, LUAGLM_Q> &_m) { dimensions = LUAGLM_MATRIX_3x2; m32 = _m; }
  inline void operator=(const glm::mat<3, 3, glm_Float, LUAGLM_Q> &_m) { dimensions = LUAGLM_MATRIX_3x3; m33 = _m; }
  inline void operator=(const glm::mat<3, 4, glm_Float, LUAGLM_Q> &_m) { dimensions = LUAGLM_MATRIX_3x4; m34 = _m; }
  inline void operator=(const glm::mat<4, 2, glm_Float, LUAGLM_Q> &_m) { dimensions = LUAGLM_MATRIX_4x2; m42 = _m; }
  inline void operator=(const glm::mat<4, 3, glm_Float, LUAGLM_Q> &_m) { dimensions = LUAGLM_MATRIX_4x3; m43 = _m; }
  inline void operator=(const glm::mat<4, 4, glm_Float, LUAGLM_Q> &_m) { dimensions = LUAGLM_MATRIX_4x4; m44 = _m; }

  // Reassignment; glm::mat = glmMatrix.

  inline int Get(glm::mat<2, 2, glm_Float, LUAGLM_Q> &_m) const { _m = m22; return 1; }
  inline int Get(glm::mat<2, 3, glm_Float, LUAGLM_Q> &_m) const { _m = m23; return 1; }
  inline int Get(glm::mat<2, 4, glm_Float, LUAGLM_Q> &_m) const { _m = m24; return 1; }
  inline int Get(glm::mat<3, 2, glm_Float, LUAGLM_Q> &_m) const { _m = m32; return 1; }
  inline int Get(glm::mat<3, 3, glm_Float, LUAGLM_Q> &_m) const { _m = m33; return 1; }
  inline int Get(glm::mat<3, 4, glm_Float, LUAGLM_Q> &_m) const { _m = m34; return 1; }
  inline int Get(glm::mat<4, 2, glm_Float, LUAGLM_Q> &_m) const { _m = m42; return 1; }
  inline int Get(glm::mat<4, 3, glm_Float, LUAGLM_Q> &_m) const { _m = m43; return 1; }
  inline int Get(glm::mat<4, 4, glm_Float, LUAGLM_Q> &_m) const { _m = m44; return 1; }
};

/*
** Pushes a vector with dimensions 'd' represented by 'v' onto the stack.
** Returning one on success (i.e., valid dimension argument), zero otherwise.
**
** @NOTE: If Lua is compiled with LUA_USE_APICHECK, a runtime error will be
**  thrown instead of returning zero.
*/
LUAGLM_API int glm_pushvec(lua_State *L, const glmVector &v, glm::length_t d);

/*
** Pushes a quaternion represented by 'q' onto the stack. Returning one on
** success, zero otherwise.
**
** @NOTE: This function is identical to glm_pushquat, but without the (implicit)
**  conversion.
*/
LUAGLM_API int glm_pushvec_quat(lua_State *L, const glmVector &q);

/*
** Creates a new Matrix object, represented by 'm', and places it onto the stack.
** Returning one on success, zero otherwise (invalid glmMatrix dimensions).
**
** @NOTE: If Lua is compiled with LUA_USE_APICHECK, a runtime error with be
**  thrown instead of returning zero.
*/
LUAGLM_API int glm_pushmat(lua_State *L, const glmMatrix &m);

/* }================================================================== */

/*
** {==================================================================
**  C/CPP boundary-interfacing structures
** ===================================================================
*/
#if defined(LUA_GRIT_API)

/// <summary>
/// A union for aliasing the Lua vector definition (lua_Float4) with the GLM
/// vector definition. As these structures are byte-wise identical, no alignment
/// or strict-aliasing issues should exist.
/// </summary>
union glmVectorBoundary {
  glmVector glm;
  lua_Float4 lua;

  glmVectorBoundary(const glmVector &_v) : glm(_v) { }
  glmVectorBoundary(const lua_Float4 &_v) : lua(_v) { }
};

/// <summary>
/// A union for aliasing the Lua matrix definition (lua_Mat4) with the GLM
/// matrix definition.
/// </summary>
union glmMatrixBoundary {
  glmMatrix glm;
  lua_Mat4 lua;

  glmMatrixBoundary(const glmMatrix &_m) : glm(_m) { }
  glmMatrixBoundary(const lua_Mat4 &_m) : lua(_m) { }
};

/* lua_Float4/lua_Mat4 -> glmVector/glmMatrix */
#define glm_vec_boundary(o) reinterpret_cast<glmVectorBoundary *>(o)->glm
#define glm_mat_boundary(o) reinterpret_cast<glmMatrixBoundary *>(o)->glm
#define glm_constvec_boundary(o) reinterpret_cast<const glmVectorBoundary *>(o)->glm
#define glm_constmat_boundary(o) reinterpret_cast<const glmMatrixBoundary *>(o)->glm

/* glmVector/glmMatrix -> lua_Float4/lua_Mat4 */
#define lua_vec_boundary(o) reinterpret_cast<glmVectorBoundary *>(o)->lua
#define lua_mat_boundary(o) reinterpret_cast<glmMatrixBoundary *>(o)->lua
#define lua_constvec_boundary(o) reinterpret_cast<const glmVectorBoundary *>(o)->lua
#define lua_constmat_boundary(o) reinterpret_cast<const glmMatrixBoundary *>(o)->lua

/* Additional defensive checks around the aliasing and alignment of lua_Mat4 and glmMatrix. */
#if GLM_HAS_STATIC_ASSERT
  GLM_STATIC_ASSERT(true
    && sizeof(lua_CFloat4) == sizeof(lua_Float4)
    && sizeof(lua_Float4) == sizeof(glmVector)
    && offsetof(lua_Float4, raw[0]) == offsetof(glmVector, v4.x)
    && offsetof(lua_Float4, raw[1]) == offsetof(glmVector, v4.y)
    && offsetof(lua_Float4, raw[2]) == offsetof(glmVector, v4.z)
    && offsetof(lua_Float4, raw[3]) == offsetof(glmVector, v4.w)
    && offsetof(glmVector, v2.x) == offsetof(glmVector, v4.x)
    && offsetof(glmVector, v3.x) == offsetof(glmVector, v4.x), "Inconsistent Structures: lua_Float4 / glm::vec<4, glm_Float>"
  );

  #if LUAGLM_USE_ANONYMOUS_STRUCT
  GLM_STATIC_ASSERT(true
    && offsetof(lua_Float4, x) == offsetof(glmVector, v4.x)
    && offsetof(lua_Float4, y) == offsetof(glmVector, v4.y)
    && offsetof(lua_Float4, z) == offsetof(glmVector, v4.z)
    && offsetof(lua_Float4, w) == offsetof(glmVector, v4.w), "Inconsistent Offsets");
  #endif

  GLM_STATIC_ASSERT(true
    && sizeof(grit_length_t) == sizeof(glm::length_t)
    && sizeof(lua_Mat4) == sizeof(glmMatrix)
    && sizeof(lua_Mat4::Columns::m2) == sizeof(glmMatrix::m24)
    && sizeof(lua_Mat4::Columns::m3) == sizeof(glmMatrix::m34)
    && sizeof(lua_Mat4::Columns::m4) == sizeof(glmMatrix::m44)
    && offsetof(lua_Mat4, dimensions) == offsetof(glmMatrix, dimensions)
    && offsetof(lua_Mat4, m.m2) == offsetof(glmMatrix, m24)
    && offsetof(lua_Mat4, m.m3) == offsetof(glmMatrix, m34)
    && offsetof(lua_Mat4, m.m4) == offsetof(glmMatrix, m44), "Inconsistent Structures: lua_Mat4 / glmMatrix"
  );

  GLM_STATIC_ASSERT(true
    && sizeof(glmVectorBoundary) == sizeof(lua_Float4)
    && sizeof(glmVectorBoundary) == sizeof(glmVector)
    && sizeof(glmMatrixBoundary) == sizeof(lua_Mat4)
    && sizeof(glmMatrixBoundary) == sizeof(glmMatrix), "Inconsistent Boundary Types!"
  );
#endif
#endif
/* }================================================================== */

#endif
