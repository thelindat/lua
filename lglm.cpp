/*
** $Id: lglm.cpp $
** Vector and Matrix object definitions.
** See Copyright Notice in lua.h
*/

#define lglm_cpp
#define LUA_CORE

/*
** For default builds this file will be compiled as C++ and then linked against
** the rest of the Lua compiled in C. Some care is required when crossing
** language boundaries and linking. lglm_string.hpp is a supplemental header
** used for emulating GLM specific features with that in mind.
**
@@ LUA_C_LINKAGE is a flag for defining the linkage specification of the Lua
** core, libraries, and interpreter.
@@ LUA_API_LINKAGE is a mark for the above linkage specification.
*/
#if defined(LUA_C_LINKAGE)
  #define LUA_API_LINKAGE "C"
#else
  #define LUA_API_LINKAGE "C++"
#endif

/* Ensure the experimental flag is enabled for GLM headers. */
#if !defined(GLM_ENABLE_EXPERIMENTAL)
  #define GLM_ENABLE_EXPERIMENTAL
#endif

#include <cstring>

/*
** @COMPAT: Fix for missing "ext/quaternion_common.hpp" include in type_quat.hpp
**  was introduced in 0.9.9.9. Note, "detail/qualifier.hpp" forward declares the
**  quaternion type so this include must be placed before "type_quat.hpp".
*/
#include <glm/glm.hpp>
#if GLM_VERSION < 999
#include <glm/gtc/quaternion.hpp>
#endif
#include <glm/detail/type_quat.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/common.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/exterior_product.hpp>
#include <glm/gtx/vector_query.hpp>
#include <glm/ext/scalar_relational.hpp>
#include <glm/ext/matrix_relational.hpp>
#include <glm/ext/matrix_transform.hpp>

extern LUA_API_LINKAGE {
#include "luaconf.h"
#include "lua.h"
#include "ldebug.h"
#include "lfunc.h"
#include "lgc.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "lvm.h"

#include "lapi.h"
#include "lauxlib.h"
#include "lgrit_lib.h"
#include "lglm_core.h"
}

#include "lglm.hpp"
#include "lglm_string.hpp"

/* Ensure C boundary references correct GLM library. */
#if LUAGLM_LIBVERSION != GLM_VERSION
  #error "GLM error: A different version of LUAGLM is already defined."
#endif

/* Handle should-be-deprecated-instead-of-removed GLM_FORCE_QUAT_DATA_WXYZ */
#if defined(GLM_FORCE_QUAT_DATA_WXYZ)
  #undef GLM_FORCE_QUAT_DATA_XYZW
/*
** @LUAGLM_QUAT_WXYZ: Constructor changed in 0.9.9.9. Broken and disabled until
** 820a2c0e625f26000c688d841836bb10483be34d is fixed.
*/
#elif defined(GLM_FORCE_QUAT_DATA_XYZW)
  #error "please compile without GLM_FORCE_QUAT_DATA_XYZW"
#endif

/*
@@ LUAGLM_FORCES_ALIGNED_GENTYPES: Force LuaGLM to use aligned types by default;
** requires GLM_FORCE_DEFAULT_ALIGNED_GENTYPES to also be enabled.
*/
#if defined(GLM_FORCE_DEFAULT_ALIGNED_GENTYPES) && !defined(LUAGLM_FORCES_ALIGNED_GENTYPES)
  #error "GLM_FORCE_DEFAULT_ALIGNED_GENTYPES: Requires LUAGLM_FORCES_ALIGNED_GENTYPES"
#elif !defined(GLM_FORCE_DEFAULT_ALIGNED_GENTYPES) && defined(LUAGLM_FORCES_ALIGNED_GENTYPES)
  #error "LUAGLM_FORCES_ALIGNED_GENTYPES: Requires GLM_FORCE_DEFAULT_ALIGNED_GENTYPES"
#endif

/*
** @GCCHack: Functions with C linkage should avoid SIMD functions that directly
** reference __builtin_*, e.g., _mm_shuffle_ps and ia32_shufps (avoid gxx_personality).
** The intent is to avoid fno-exceptions when compiling this unit, however, that
** *should* not be an issue anymore.
**
** @QuatHack: workarounds for incorrect SIMD implementations:
**    type_quat_simd.inl:180:31: error: ‘const struct glm::vec<4, float, glm::aligned_highp>’
**    has no member named ‘Data’
**
**    type_quat_simd.inl:94:11: error: could not convert ‘Result’ from
**    ‘glm::vec<4, float, glm::aligned_highp>’ to ‘glm::qua<float, glm::aligned_highp>’
*/
#undef LUAGLM_ALIGNED
#if GLM_CONFIG_ALIGNED_GENTYPES == GLM_ENABLE && defined(GLM_FORCE_DEFAULT_ALIGNED_GENTYPES)
  #define LUAGLM_ALIGNED
  #if defined(LUA_C_LINKAGE) && defined(__GNUG__) && !defined(__OPTIMIZE__) && !defined(LUAGLM_FORCE_HIGHP)
    #define LUAGLM_FORCE_HIGHP
  #endif
#endif

/* return helpers */
#define glm_runerror(L, M) (luaG_runerror((L), (M)), 0)
#define glm_typeError(L, O, M) (luaG_typeerror((L), (O), (M)), 0)
#define glm_finishset(L, T, K, V) (luaV_finishset((L), (T), (K), (V), GLM_NULLPTR), 1)

/* lua_gettop() macro */
#if !defined(_gettop)
  #define _gettop(L) (cast_int((L)->top - ((L)->ci->func + 1)))
  #define _isvalid(L, o) (!ttisnil(o) || o != &G(L)->nilvalue)
#endif

#if !defined(ispseudo)
  #define ispseudo(i) ((i) <= LUA_REGISTRYINDEX)
#endif

/// <summary>
/// index2value copied from lapi.c
/// </summary>
static inline TValue *glm_index2value(lua_State *L, int idx) {
  CallInfo *ci = L->ci;
  if (idx > 0) {
    StkId o = ci->func + idx;
    api_check(L, idx <= L->ci->top - (ci->func + 1), "unacceptable index");
    return (o >= L->top) ? &G(L)->nilvalue : s2v(o);
  }
  else if (!ispseudo(idx)) {  /* negative index */
    api_check(L, idx != 0 && -idx <= L->top - (ci->func + 1), "invalid index");
    return s2v(L->top + idx);
  }
  else if (idx == LUA_REGISTRYINDEX)
    return &G(L)->l_registry;
  else { /* upvalues */
    idx = LUA_REGISTRYINDEX - idx;
    api_check(L, idx <= MAXUPVAL + 1, "upvalue index too large");
    if (ttisCclosure(s2v(ci->func))) { /* C closure? */
      CClosure *func = clCvalue(s2v(ci->func));
      return (idx <= func->nupvalues) ? &func->upvalue[idx - 1] : &G(L)->nilvalue;
    }
    else { /* light C function or Lua function (through a hook)?) */
      api_check(L, ttislcf(s2v(ci->func)), "caller not a C function");
      return &G(L)->nilvalue; /* no upvalues */
    }
  }
}

/// <summary>
/// Statically cast a tagged value to the specified type parameter. Returning
/// true on success, false otherwise.
/// </summary>
template<typename T>
static bool glm_castvalue(const TValue *value, T &out) {
  switch (ttypetag(value)) {
    case LUA_VTRUE: out = static_cast<T>(1); break;
    case LUA_VFALSE: out = static_cast<T>(0); break;
    case LUA_VNUMINT: out = static_cast<T>(ivalue(value)); break;
    case LUA_VNUMFLT: out = static_cast<T>(fltvalue(value)); break;
    default: {
      out = static_cast<T>(0);
      return false;
    }
  }
  return true;
}

/// <summary>
/// Parse the given number object as a vector/matrix accessible index.
/// </summary>
static inline lua_Integer glm_flttointeger(const TValue *obj) {
  const lua_Number n = l_floor(fltvalue(obj));
  if (n >= cast_num(LUA_MININTEGER) && n < -cast_num(LUA_MININTEGER))
    return static_cast<lua_Integer>(n);

  return 0;
}

/*
** {==================================================================
** Object Conversion
** ===================================================================
*/

/* Additional type cases (see: llimits.h) */
#define glm_castfloat(i) static_cast<glm_Float>((i))

/* nvalue */
#define glm_toflt(obj) glm_castfloat(nvalue(obj))

/* convert an object to an integer (without string coercion) */
#define glm_tointeger(o) (ttisinteger(o) ? ivalue(o) : glm_flttointeger(o))

/* raw object fields */
#define glm_vvalue_raw(o) glm_constvec_boundary(&vvalue_raw(o))
#define glm_v2value_raw(o) glm_vvalue_raw(o).v2
#define glm_v3value_raw(o) glm_vvalue_raw(o).v3
#define glm_v4value_raw(o) glm_vvalue_raw(o).v4
#define glm_qvalue_raw(o) glm_vvalue_raw(o).q

/* glm::type references */
#define glm_vvalue(o) glm_constvec_boundary(vvalue_ref(o))
#define glm_v2value(o) glm_vvalue(o).v2
#define glm_v3value(o) glm_vvalue(o).v3
#define glm_v4value(o) glm_vvalue(o).v4
#define glm_qvalue(o) glm_vvalue(o).q
#define glm_setvvalue2s(s, x, o)        \
  LUA_MLM_BEGIN                         \
  TValue *io = s2v(s);                  \
  glm_vec_boundary(&vvalue_(io)) = (x); \
  settt_(io, (o));                      \
  LUA_MLM_END

#define glm_mvalue(o) glm_constmat_boundary(mvalue_ref(o))
#define glm_setmvalue2s(L, o, x) glm_setmvalue(L, s2v(o), x)
#define glm_setmvalue(L, obj, x) \
  LUA_MLM_BEGIN                  \
  TValue *io = (obj);            \
  GCMatrix *x_ = (x);            \
  val_(io).gc = obj2gco(x_);     \
  settt_(io, ctb(LUA_VMATRIX));  \
  checkliveness(L, io);          \
  LUA_MLM_END

/* }================================================================== */

/*
** {==================================================================
** Vector Object API
** ===================================================================
*/

/* @NOTE: equal objects must have equal hashes; use with caution. */
#if defined(LUAGLM_EPS_EQUAL)
  #define _glmeq(a, b) (glm::all(glm::equal((a), (b), glm::epsilon<glm_Float>())))
#else
  #define _glmeq(a, b) ((a) == (b))
#endif

/* trybinTM where the first element is predefined */
static int num_trybinTM(lua_State *L, const TValue *p1, const TValue *p2, StkId res, TMS event);
static int vec_trybinTM(lua_State *L, const TValue *p1, const TValue *p2, StkId res, TMS event);
static int quat_trybinTM(lua_State *L, const TValue *p1, const TValue *p2, StkId res, TMS event);
static int mat_trybinTM(lua_State *L, const TValue *p1, const TValue *p2, StkId res, TMS event);

/// <summary>
/// The vector-type equivalent to luaV_finishget. The 'angle' and 'axis' fields
/// are grit-lua compatibility fields for quaternion types.
///
/// @NOTE: If the quaternion type has a metatable then the 'angle' and 'axis'
///   fields are no longer parsed. Ideally all compatibility bloat will be
///   removed from this codebase. It is already ugly enough as-is.
/// </summary>
static void vec_finishget(lua_State *L, const TValue *obj, TValue *key, StkId res) {
  const TValue *tm = luaT_gettmbyobj(L, obj, TM_INDEX);
  if (notm(tm)) {
    if (ttisstring(key) && ttisquat(obj)) {
      const char *str = svalue(key);
      if (strcmp(str, "angle") == 0) {
        setfltvalue(s2v(res), glm::degrees(cast_num(glm::angle(glm_qvalue(obj)))));
        return;
      }
      else if (strcmp(str, "axis") == 0) {
        const glmVector out(glm::axis(glm_qvalue(obj)));
        glm_setvvalue2s(res, out, LUA_VVECTOR3);
        return;
      }
    }
    setnilvalue(s2v(res));
  }
  // Finish the vector access and try the metamethod
  else if (ttisfunction(tm))  /* is metamethod a function? */
    luaT_callTMres(L, tm, obj, key, res); /* call it */
  else {
    // This logic would be considered the first 'loop' of luaV_finishget
    const TValue *slot = GLM_NULLPTR;
    const TValue *t = tm;  /* else try to access 'tm[key]' */
    if (luaV_fastget(L, t, key, slot, luaH_get)) {  /* fast track? */
      setobj2s(L, res, slot);  /* done */
      return;
    }
    luaV_finishget(L, t, key, res, slot);
  }
}

/// <summary>
/// Runtime swizzle operation: returning the number of copied vector fields on
/// success, zero on failure.
/// </summary>
template<glm::length_t L>
static glm::length_t swizzle(const lua_Float4 &v, const char *key, lua_Float4 &out) {
  glm::length_t i = 0;
  for (; i < 4 && key[i] != '\0'; ++i) {
    switch (key[i]) {
      case 'x': GLM_IF_CONSTEXPR (L < 1) return 0; out.raw[i] = static_cast<lua_VecF>(v.raw[0]); break;
      case 'y': GLM_IF_CONSTEXPR (L < 2) return 0; out.raw[i] = static_cast<lua_VecF>(v.raw[1]); break;
      case 'z': GLM_IF_CONSTEXPR (L < 3) return 0; out.raw[i] = static_cast<lua_VecF>(v.raw[2]); break;
      case 'w': GLM_IF_CONSTEXPR (L < 4) return 0; out.raw[i] = static_cast<lua_VecF>(v.raw[3]); break;
      default: {
        return 0;
      }
    }
  }
  return i;
}

int glmVec_rawgeti(const TValue *obj, lua_Integer n, StkId res) {
  const int result = vecgeti(obj, n, res);
  if (result == LUA_TNONE) {
    setnilvalue(s2v(res));
    return LUA_TNIL;
  }
  return result;
}

#define SINGLE_CHAR(K) ((K) != GLM_NULLPTR && (K)[0] != '\0' && (K)[1] == '\0')
int glmVec_rawgets(const TValue *obj, const char *k, StkId res) {
  const int result = SINGLE_CHAR(k) ? vecgets(obj, k, res) : LUA_TNONE;
  if (result == LUA_TNONE) {
    setnilvalue(s2v(res));
    return LUA_TNIL;
  }
  return result;
}

int glmVec_rawget(const TValue *obj, TValue *key, StkId res) {
  int result = LUA_TNONE;
  switch (ttype(key)) {
    case LUA_TNUMBER: {
      result = vecgeti(obj, glm_tointeger(key), res);
      break;
    }
    case LUA_TSTRING: {
      // The 'dim', 'axis', and 'angle' fields are viewed as 'meta' fields. To
      // simplify some logic, 'n' (shorthand dimensions) will be exposed by
      // rawget.
      if (vslen(key) == 1)
        result = vecgets(obj, svalue(key), res);
      break;
    }
    default: {
      break;
    }
  }

  if (result == LUA_TNONE) {
    setnilvalue(s2v(res));
    return LUA_TNIL;
  }
  return result;
}

void glmVec_geti(lua_State *L, const TValue *obj, lua_Integer c, StkId res) {
  if (vecgeti(obj, c, res) == LUA_TNONE) {  // Attempt metatable access
    TValue key;
    setivalue(&key, c);
    vec_finishget(L, obj, &key, res);
  }
}

void glmVec_get(lua_State *L, const TValue *obj, TValue *key, StkId res) {
  if (ttisnumber(key)) {
    if (vecgeti(obj, glm_tointeger(key), res) != LUA_TNONE) {
      return;
    }
  }
  else if (ttisstring(key)) {
    const char *str = svalue(key);
    const size_t str_len = vslen(key);
    if (str_len == 1) {  // hot-path single character access
      if (vecgets(obj, str, res) != LUA_TNONE) {
        return;
      }
    }
    // Allow runtime swizzle operations prior to metamethod access.
    else if (str_len <= 4) {
      lua_Float4 out;
      glm::length_t count = 0;
      switch (ttypetag(obj)) {
        case LUA_VVECTOR2: count = swizzle<2>(vvalue_(obj), str, out); break;
        case LUA_VVECTOR3: count = swizzle<3>(vvalue_(obj), str, out); break;
        case LUA_VVECTOR4: count = swizzle<4>(vvalue_(obj), str, out); break;
        case LUA_VQUAT: {
#if LUAGLM_QUAT_WXYZ  // quaternion has WXYZ layout
          const lua_Float4 &v = vvalue_(obj);
          const lua_Float4 swap = { { v.raw[1], v.raw[2], v.raw[3], v.raw[0] } };
          count = swizzle<4>(swap, str, out);
#else
          count = swizzle<4>(vvalue_(obj), str, out);
#endif
          break;
        }
        default: {
          break;
        }
      }

      switch (count) {
        case 1: setfltvalue(s2v(res), cast_num(out.raw[0])); return;
        case 2: setvvalue(s2v(res), out, LUA_VVECTOR2); return;
        case 3: setvvalue(s2v(res), out, LUA_VVECTOR3); return;
        case 4: {
          // Quaternion was swizzled and resultant vector is still normalized.
          // Keep quaternion semantics.
          if (ttisquat(obj) && glm::isNormalized(glm_vec_boundary(&out).v4, glm::epsilon<glm_Float>())) {
#if LUAGLM_QUAT_WXYZ  // quaternion has WXYZ layout
            const lua_Float4 &swap = out;
            out = { { swap.raw[3], swap.raw[0], swap.raw[1], swap.raw[2] } };
#endif
            setvvalue(s2v(res), out, LUA_VQUAT);
          }
          else {
            setvvalue(s2v(res), out, LUA_VVECTOR4);
          }
          return;
        }
        default: {
          // grit-lua compatibility: dimension field takes priority over tag methods
          if (strcmp(str, "dim") == 0) {
            setivalue(s2v(res), static_cast<lua_Integer>(glm_dimensions(ttypetag(obj))));
            return;
          }
          break;
        }
      }
    }
  }

  vec_finishget(L, obj, key, res);  // Metatable Access
}

void glmVec_objlen(const TValue *obj, StkId res) {
  const glmVector &v = glm_vvalue(obj);
  switch (ttypetag(obj)) {
    case LUA_VVECTOR2: setfltvalue(s2v(res), cast_num(glm::length(v.v2))); break;
    case LUA_VVECTOR3: setfltvalue(s2v(res), cast_num(glm::length(v.v3))); break;
    case LUA_VVECTOR4: setfltvalue(s2v(res), cast_num(glm::length(v.v4))); break;
    case LUA_VQUAT: setfltvalue(s2v(res), cast_num(glm::length(v.q))); break;
    default: {
      setfltvalue(s2v(res), cast_num(0));
      break;
    }
  }
}

int glmVec_equalObj(lua_State *L, const TValue *o1, const TValue *o2) {
  bool result = false;
  const glmVector &v = glm_vvalue(o1);
  const glmVector &other_v = glm_vvalue(o2);
  switch (ttypetag(o1)) {
    case LUA_VVECTOR2: result = _glmeq(v.v2, other_v.v2); break;
    case LUA_VVECTOR3: result = _glmeq(v.v3, other_v.v3); break;
    case LUA_VVECTOR4: result = _glmeq(v.v4, other_v.v4); break;
    case LUA_VQUAT: result = _glmeq(v.q, other_v.q); break;
    default: {
      break;
    }
  }

  if (result == false && L != GLM_NULLPTR) {
    const TValue *tm = luaT_gettmbyobj(L, o1, TM_EQ);
    if (!notm(tm)) {
      luaT_callTMres(L, tm, o1, o2, L->top);  /* call TM */
      result = !l_isfalse(s2v(L->top));
    }
  }
  return result;
}

int glmVec_concat(const TValue *obj, const TValue *value, StkId res) {
  const glmVector &v = glm_vvalue(obj);
  glmVector result = v;  // Create a copy of the vector
  glm::length_t dims = glm_dimensions(ttypetag(obj));  // Current dimensions
  if (ttisinteger(value) && dims < 4)
    result.v4[dims++] = glm_castfloat(ivalue(value));
  else if (ttisfloat(value) && dims < 4)
    result.v4[dims++] = glm_castfloat(fltvalue(value));
  else if (ttisboolean(value) && dims < 4)
    result.v4[dims++] = glm_castfloat(!l_isfalse(value));
  else if (ttisvector(value)) {
    const glm::length_t v_dims = glm_dimensions(ttypetag(value));
    if ((dims + v_dims) > 4) {  // Outside valid dimensions
      return 0;
    }

    for (glm::length_t i = 0; i < v_dims; ++i) {
      result.v4[dims++] = glm_v4value(value)[i];
    }
  }
  else {
    return 0;
  }

  glm_setvvalue2s(res, result, glm_variant(dims));
  return 1;
}

int glmVec_tostr(const TValue *obj, char *buff, size_t len) {
  const glmVector &v = glm_vvalue(obj);
  int copy = 0;
  switch (ttypetag(obj)) {
    case LUA_VVECTOR1: copy = glm::detail::format_type(buff, len, v.v1); break;
    case LUA_VVECTOR2: copy = glm::detail::format_type(buff, len, v.v2); break;
    case LUA_VVECTOR3: copy = glm::detail::format_type(buff, len, v.v3); break;
    case LUA_VVECTOR4: copy = glm::detail::format_type(buff, len, v.v4); break;
    case LUA_VQUAT: copy = glm::detail::format_type(buff, len, v.q); break;
    default: {
      break;
    }
  }
  lua_assert(copy >= 0);
  return copy;
}

int glmVec_equalKey(const TValue *k1, const Node *n2, int rtt) {
  // @NOTE: Ideally _glmeq would be used. However, that would put the table in
  // an invalid state: mainposition != equalkey.
  switch (withvariant(rtt)) {
    case LUA_VVECTOR2: return glm_v2value(k1) == glm_v2value_raw(keyval(n2));
    case LUA_VVECTOR3: return glm_v3value(k1) == glm_v3value_raw(keyval(n2));
    case LUA_VVECTOR4: return glm_v4value(k1) == glm_v4value_raw(keyval(n2));
    case LUA_VQUAT: return glm_qvalue(k1) == glm_qvalue_raw(keyval(n2));
    default: {
      return 0;
    }
  }
}

size_t glmVec_hash(const TValue *obj) {
  // Uses a custom glm::hash implementation without the dependency on std::hash
  switch (ttypetag(obj)) {
    case LUA_VVECTOR2: return glm::hash::hash(glm_v2value(obj));
    case LUA_VVECTOR3: return glm::hash::hash(glm_v3value(obj));
    case LUA_VVECTOR4: return glm::hash::hash(glm_v4value(obj));
    case LUA_VQUAT: return glm::hash::hash(glm_qvalue(obj));
    default: {
      return 0xDEAD;  // C0D3
    }
  }
}

namespace glm {
  /// <summary>
  /// Return true if all components of the vector are finite.
  ///
  /// @NOTE: -ffast-math will break this condition.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER bool __isfinite(vec<L, T, Q> const &v) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'isnan' only accept floating-point inputs");

    bool result = true;
    for (length_t l = 0; l < L; ++l)
      result &= glm::isfinite(v[l]);
    return result;
  }
}

int glmVec_isfinite(const TValue *obj) {
  switch (ttypetag(obj)) {
    case LUA_VVECTOR2: return glm::__isfinite(glm_v2value(obj));
    case LUA_VVECTOR3: return glm::__isfinite(glm_v3value(obj));
    case LUA_VVECTOR4: return glm::__isfinite(glm_v4value(obj));
    case LUA_VQUAT: return glm::__isfinite(glm_v4value(obj)); // @HACK
    default: {
      break;
    }
  }
  return 0;
}

int glmVec_next(const TValue *obj, StkId key) {
  TValue *key_obj = s2v(key);
  if (ttisnil(key_obj)) {
    setivalue(key_obj, 1);
    if (vecgeti(obj, 1, key + 1) == LUA_TNONE) {
      setnilvalue(s2v(key + 1));
    }
    return 1;
  }
  else if (ttisnumber(key_obj)) {
    lua_Integer l_nextIdx = glm_tointeger(key_obj);
    l_nextIdx = luaL_intop(+, l_nextIdx, 1);  // first empty element

    const lua_Integer D = cast(lua_Integer, glm_dimensions(ttypetag(obj)));
    if (l_nextIdx >= 1 && l_nextIdx <= D) {
      setivalue(key_obj, l_nextIdx);  // Iterator values are 1-based
      if (vecgeti(obj, l_nextIdx, key + 1) == LUA_TNONE) {
        setnilvalue(s2v(key + 1));
      }
      return 1;
    }
  }
  return 0;
}

int glm_trybinTM(lua_State *L, const TValue *p1, const TValue *p2, StkId res, TMS event) {
  switch (ttype(p1)) {
    case LUA_TNUMBER: return num_trybinTM(L, p1, p2, res, event);
    case LUA_TMATRIX: return mat_trybinTM(L, p1, p2, res, event);
    case LUA_TVECTOR: {
      if (ttisquat(p1))  // quaternion-specific implementation
        return quat_trybinTM(L, p1, p2, res, event);
      return vec_trybinTM(L, p1, p2, res, event);
    }
    default: {
      break;
    }
  }
  return 0;
}

/* }================================================================== */

/*
** {==================================================================
** Matrix Object API
** ===================================================================
*/

#define INVALID_MATRIX_DIMENSIONS "invalid " GLM_STRING_MATRIX " dimension"

/// <summary>
/// If "raw" is true (denoting 'rawset'), the function will throw a Lua runtime
/// error when attempting to operate on invalid keys/fields. Otherwise, this
/// function (like non 'raw' functions) will attempt to reference a metatable.
/// </summary>
static int glmMat_auxset(lua_State *L, const TValue *obj, TValue *key, TValue *val, bool raw) {
  if (!ttisnumber(key)) {  // Invalid index for matrix
    return raw ? glm_typeError(L, key, "index") : glm_finishset(L, obj, key, val);
  }

  glmMatrix &m = glm_mat_boundary(mvalue_ref(obj));
  const glm::length_t m_size = LUAGLM_MATRIX_COLS(m.dimensions);
  const glm::length_t m_secondary = LUAGLM_MATRIX_ROWS(m.dimensions);
  const glm::length_t dim = static_cast<glm::length_t>(glm_tointeger(key));
  if (ttisvector(val)) {
    const bool expanding = dim <= 4 && (dim == (m_size + 1));
    if (glm_dimensions(ttypetag(val)) != m_secondary)  // Invalid vector being appended
      return raw ? glm_runerror(L, INVALID_MATRIX_DIMENSIONS) : glm_finishset(L, obj, key, val);
    else if (dim <= 0 || (dim > m_size && !expanding))  // Index out of bounds.
      return raw ? glm_runerror(L, INVALID_MATRIX_DIMENSIONS) : glm_finishset(L, obj, key, val);

    switch (m_secondary) {
      case 2: m.m42[dim - 1] = glm_v2value(val); break;
      case 3: m.m43[dim - 1] = glm_v3value(val); break;
      case 4: {
#if LUAGLM_QUAT_WXYZ  // quaternion has WXYZ layout
        if (ttisquat(val)) {
          const glm::qua<glm_Float> &q = glm_qvalue(val);
          m.m44[dim - 1] = glm::vec<4, glm_Float>(q.x, q.y, q.z, q.w);
        }
        else
#endif
        {
          m.m44[dim - 1] = glm_v4value(val);
        }
        break;
      }
      default: {
        return raw ? glm_runerror(L, INVALID_MATRIX_DIMENSIONS) : glm_finishset(L, obj, key, val);
      }
    }

    m.dimensions = LUAGLM_MATRIX_TYPE(m_size + (expanding ? 1 : 0), m_secondary);
    return 1;
  }
  else if (ttisnil(val)) {  // Attempt to shrink the dimension of the matrix
    if (dim == m_size && dim > 2) {  // Matrices must have at least two columns; >= 2x2
      m.dimensions = LUAGLM_MATRIX_TYPE(m_size - 1, m_secondary);
      return 1;
    }
    return raw ? glm_runerror(L, GLM_STRING_MATRIX " must have at least two columns")
               : glm_finishset(L, obj, key, val);
  }
  return raw ? glm_runerror(L, "attempt to set a " GLM_STRING_MATRIX " value with an incorrect index")
             : glm_finishset(L, obj, key, val);
}

/// <summary>
/// Helper function for generalized matrix int-access
/// </summary>
static int matgeti (const TValue *obj, lua_Integer n, StkId res) {
  const grit_length_t gidx = cast(grit_length_t, n);
  const glmMatrix &m = glm_mvalue(obj);
  if (l_likely(gidx >= 1 && gidx <= LUAGLM_MATRIX_COLS(m.dimensions))) {
    switch (LUAGLM_MATRIX_ROWS(m.dimensions)) {
      case 2: glm_setvvalue2s(res, m.m42[gidx - 1], LUA_VVECTOR2); return LUA_VVECTOR2;
      case 3: glm_setvvalue2s(res, m.m43[gidx - 1], LUA_VVECTOR3); return LUA_VVECTOR3; // @ImplicitAlign
      case 4: glm_setvvalue2s(res, m.m44[gidx - 1], LUA_VVECTOR4); return LUA_VVECTOR4;
      default: {
        break;
      }
    }
  }
  return LUA_TNONE;
}

GCMatrix *glmMat_new(lua_State *L) {
  GCObject *o = luaC_newobj(L, LUA_VMATRIX, sizeof(GCMatrix));
  GCMatrix *mat = gco2mat(o);
  glm_mat_boundary(&mat->mat4) = glm::identity<glm::mat<4, 4, glm_Float>>();
  return mat;
}

int glmMat_rawgeti(const TValue *obj, lua_Integer n, StkId res) {
  const int result = matgeti(obj, n, res);
  if (result == LUA_TNONE) {
    setnilvalue(s2v(res));
    return LUA_TNIL;
  }
  return result;
}

int glmMat_vmgeti(const TValue *obj, lua_Integer n, StkId res) {
  return matgeti(obj, n, res);
}

int glmMat_rawget(const TValue *obj, TValue *key, StkId res) {
  if (!ttisnumber(key)) {  // Allow float-to-int coercion
    setnilvalue(s2v(res));
    return LUA_TNIL;
  }
  return glmMat_rawgeti(obj, glm_tointeger(key), res);
}

void glmMat_rawset(lua_State *L, const TValue *obj, TValue *key, TValue *val) {
  glmMat_auxset(L, obj, key, val, true);
}

void glmMat_get(lua_State *L, const TValue *obj, TValue *key, StkId res) {
  if (!ttisnumber(key) || matgeti(obj, glm_tointeger(key), res) == LUA_TNONE) {
    vec_finishget(L, obj, key, res);
  }
}

void glmMat_geti(lua_State *L, const TValue *obj, lua_Integer c, StkId res) {
  if (matgeti(obj, c, res) == LUA_TNONE) {
    TValue key;
    setivalue(&key, c);
    vec_finishget(L, obj, &key, res);
  }
}

void glmMat_set(lua_State *L, const TValue *obj, TValue *key, TValue *val) {
  glmMat_auxset(L, obj, key, val, false);
}

void glmMat_seti(lua_State *L, const TValue *obj, lua_Integer c, TValue *val) {
  TValue key;
  setivalue(&key, c);
  glmMat_auxset(L, obj, &key, val, false);
}

void glmMat_objlen(const TValue *obj, StkId res) {
  setivalue(s2v(res), static_cast<lua_Integer>(LUAGLM_MATRIX_COLS(mvalue_dims(obj))));
}

int glmMat_tostr(const TValue *obj, char *buff, size_t len) {
  int copy = 0;

  // Use a custom glm::to_string implementation that avoids using std::string
  // and CRT allocator use. In addition, gtx/string_cast is broken for GCC when
  // GLM_FORCE_INLINE is enabled.
  const glmMatrix &m = glm_mvalue(obj);
  switch (m.dimensions) {
    case LUAGLM_MATRIX_2x2: copy = glm::detail::format_type(buff, len, m.m22); break;
    case LUAGLM_MATRIX_2x3: copy = glm::detail::format_type(buff, len, m.m23); break;
    case LUAGLM_MATRIX_2x4: copy = glm::detail::format_type(buff, len, m.m24); break;
    case LUAGLM_MATRIX_3x2: copy = glm::detail::format_type(buff, len, m.m32); break;
    case LUAGLM_MATRIX_3x3: copy = glm::detail::format_type(buff, len, m.m33); break;
    case LUAGLM_MATRIX_3x4: copy = glm::detail::format_type(buff, len, m.m34); break;
    case LUAGLM_MATRIX_4x2: copy = glm::detail::format_type(buff, len, m.m42); break;
    case LUAGLM_MATRIX_4x3: copy = glm::detail::format_type(buff, len, m.m43); break;
    case LUAGLM_MATRIX_4x4: copy = glm::detail::format_type(buff, len, m.m44); break;
    default: {
      break;
    }
  }

  lua_assert(copy >= 0);
  return copy;
}

int glmMat_next(const TValue *obj, StkId key) {
  TValue *key_value = s2v(key);
  if (ttisnil(key_value)) {
    setivalue(key_value, 1);
    glmMat_rawgeti(obj, 1, key + 1);
    return 1;
  }
  else if (ttisnumber(key_value)) {
    lua_Integer l_nextIdx = glm_tointeger(key_value);
    l_nextIdx = luaL_intop(+, l_nextIdx, 1);  // first empty element

    const lua_Integer D = cast(lua_Integer, LUAGLM_MATRIX_COLS(mvalue_dims(obj)));
    if (l_nextIdx >= 1 && l_nextIdx <= D) {
      setivalue(key_value, l_nextIdx);  // Iterator values are 1-based
      glmMat_rawgeti(obj, l_nextIdx, key + 1);
      return 1;
    }
  }
  return 0;
}

int glmMat_equalObj(lua_State *L, const TValue *o1, const TValue *o2) {
  bool result = false;
  const glmMatrix &m = glm_mvalue(o1);
  const glmMatrix &other_m = glm_mvalue(o2);
  if (m.dimensions == other_m.dimensions) {
    switch (m.dimensions) {
      case LUAGLM_MATRIX_2x2: result = _glmeq(m.m22, other_m.m22); break;
      case LUAGLM_MATRIX_2x3: result = _glmeq(m.m23, other_m.m23); break;
      case LUAGLM_MATRIX_2x4: result = _glmeq(m.m24, other_m.m24); break;
      case LUAGLM_MATRIX_3x2: result = _glmeq(m.m32, other_m.m32); break;
      case LUAGLM_MATRIX_3x3: result = _glmeq(m.m33, other_m.m33); break;
      case LUAGLM_MATRIX_3x4: result = _glmeq(m.m34, other_m.m34); break;
      case LUAGLM_MATRIX_4x2: result = _glmeq(m.m42, other_m.m42); break;
      case LUAGLM_MATRIX_4x3: result = _glmeq(m.m43, other_m.m43); break;
      case LUAGLM_MATRIX_4x4: result = _glmeq(m.m44, other_m.m44); break;
      default: {
        break;
      }
    }
  }

  if (!result && L != GLM_NULLPTR) {
    const TValue *tm = luaT_gettmbyobj(L, o1, TM_EQ);
    if (!notm(tm)) {
      luaT_callTMres(L, tm, o1, o2, L->top);  /* call TM */
      result = !l_isfalse(s2v(L->top));
    }
  }

  return result;
}

/* }================================================================== */

/*
** {==================================================================
** GLM Interface
** ===================================================================
*/

#define INVALID_VECTOR_TYPE "invalid " GLM_STRING_VECTOR " type"

/// <summary>
/// Generalized TValue to glm::vec conversion; using glmVector.Get to implicitly
/// handle type conversions.
/// </summary>
template <glm::length_t D, typename T>
static glm::vec<D, T> glm_tovec(lua_State *L, int idx) {
  glm::vec<D, glm_Float> result(0);

  const TValue *o = glm_index2value(L, idx);
  if (ttisvector(o) && glm_dimensions(ttypetag(o)) >= D) {
    glm_vvalue(o).Get(result);
  }
  return result;
}

/// <summary>
/// Generalized TValue to glm::mat conversion; using glmMatrix.Get to implicitly
/// handle type conversions.
/// </summary>
template <glm::length_t C, glm::length_t R, typename T>
static glm::mat<C, R, T> glm_tomat(lua_State *L, int idx) {
  glm::mat<C, R, T> result = glm::identity<glm::mat<C, R, glm_Float>>();
  lua_lock(L);
  const TValue *o = glm_index2value(L, idx);
  if (l_likely(ttismatrix(o))) {
    const glmMatrix &m = glm_mvalue(o);
    if (LUAGLM_MATRIX_COLS(m.dimensions) >= C && LUAGLM_MATRIX_ROWS(m.dimensions) == R) {
      m.Get(result);
    }
  }
  lua_unlock(L);
  return result;
}

LUAGLM_API int glm_pushvec(lua_State *L, const glmVector &v, glm::length_t dimensions) {
  GLM_STATIC_ASSERT(LUAGLM_Q == glm::defaultp, "LUAGLM_QUALIFIER");  // Sanitize LUAGLM_FORCES_ALIGNED_GENTYPES
  if (l_likely(dimensions >= 2 && dimensions <= 4)) {
    lua_lock(L);
    glm_setvvalue2s(L->top, v, glm_variant(dimensions));
    api_incr_top(L);
    lua_unlock(L);
  }
  else if (dimensions == 1)  // @ImplicitVec
    lua_pushnumber(L, cast_num(v.v1.x));
  else {
#if defined(LUA_USE_APICHECK)
    luaG_runerror(L, INVALID_VECTOR_TYPE);
#endif
    return 0;
  }
  return 1;
}

LUAGLM_API int glm_pushvec_quat(lua_State *L, const glmVector &q) {
  GLM_STATIC_ASSERT(LUAGLM_Q == glm::defaultp, "LUAGLM_QUALIFIER");
  lua_lock(L);
  glm_setvvalue2s(L->top, q, LUA_VQUAT);
  api_incr_top(L);
  lua_unlock(L);
  return 1;
}

LUAGLM_API int glm_pushmat(lua_State *L, const glmMatrix &m) {
  GLM_STATIC_ASSERT(LUAGLM_Q == glm::defaultp, "LUAGLM_QUALIFIER");
  GCMatrix *mat = GLM_NULLPTR;
#if defined(LUA_USE_APICHECK)
  const glm::length_t m_size = LUAGLM_MATRIX_COLS(m.dimensions);
  const glm::length_t m_secondary = LUAGLM_MATRIX_ROWS(m.dimensions);
  if (m_size < 2 || m_size > 4 || m_secondary < 2 || m_secondary > 4) {
    luaG_runerror(L, INVALID_MATRIX_DIMENSIONS);
    return 0;
  }
#endif

  lua_lock(L);
  mat = glmMat_new(L);
  glm_mat_boundary(&mat->mat4) = m;
  glm_setmvalue2s(L, L->top, mat);
  api_incr_top(L);
  luaC_checkGC(L);
  lua_unlock(L);
  return 1;
}

LUAGLM_API bool glm_isvector(lua_State *L, int idx, glm::length_t &size) {
  bool result = true;
  const TValue *o = glm_index2value(L, idx);
  if (ttisvector(o) && !ttisquat(o))
    size = glm_dimensions(ttypetag(o));
  else if (ttisnumber(o))
    size = 1;
  else {
    result = false;
  }
  return result;
}

LUAGLM_API bool glm_isquat(lua_State *L, int idx) {
  const TValue *o = glm_index2value(L, idx);
  return ttisquat(o);
}

LUAGLM_API bool glm_ismatrix(lua_State *L, int idx, glm::length_t &dimensions) {
  bool result = false;
  lua_lock(L);
  const TValue *o = glm_index2value(L, idx);
  if (ttismatrix(o)) {
    dimensions = mvalue_dims(o);
    result = true;
  }
  lua_unlock(L);
  return result;
}

LUAGLM_API int glm_pushvec1(lua_State *L, const glm::vec<1, glm_Float> &v) { lua_pushnumber(L, cast_num(v.x)); return 1; }
LUAGLM_API int glm_pushvec2(lua_State *L, const glm::vec<2, glm_Float> &v) { return glm_pushvec(L, glmVector(v), 2); }
LUAGLM_API int glm_pushvec3(lua_State *L, const glm::vec<3, glm_Float> &v) { return glm_pushvec(L, glmVector(v), 3); }
LUAGLM_API int glm_pushvec4(lua_State *L, const glm::vec<4, glm_Float> &v) { return glm_pushvec(L, glmVector(v), 4); }
LUAGLM_API int glm_pushquat(lua_State *L, const glm::qua<glm_Float> &q) { return glm_pushvec_quat(L, glmVector(q)); }

LUAGLM_API glm::vec<1, glm_Float> glm_tovec1(lua_State *L, int idx) { return glm::vec<1, glm_Float>(glm_castfloat(lua_tonumber(L, idx))); }
LUAGLM_API glm::vec<2, glm_Float> glm_tovec2(lua_State *L, int idx) { return glm_tovec<2, glm_Float>(L, idx); }
LUAGLM_API glm::vec<3, glm_Float> glm_tovec3(lua_State *L, int idx) { return glm_tovec<3, glm_Float>(L, idx); }
LUAGLM_API glm::vec<4, glm_Float> glm_tovec4(lua_State *L, int idx) { return glm_tovec<4, glm_Float>(L, idx); }
LUAGLM_API glm::qua<glm_Float> glm_toquat(lua_State *L, int idx) {
  const TValue *o = glm_index2value(L, idx);
  return ttisquat(o) ? glm_qvalue(o) : glm::quat_identity<glm_Float, glm::defaultp>();
}

LUAGLM_API int glm_pushmat2x2(lua_State *L, const glm::mat<2, 2, glm_Float> &m) { return glm_pushmat(L, glmMatrix(m)); }
LUAGLM_API int glm_pushmat2x3(lua_State *L, const glm::mat<2, 3, glm_Float> &m) { return glm_pushmat(L, glmMatrix(m)); }
LUAGLM_API int glm_pushmat2x4(lua_State *L, const glm::mat<2, 4, glm_Float> &m) { return glm_pushmat(L, glmMatrix(m)); }
LUAGLM_API int glm_pushmat3x2(lua_State *L, const glm::mat<3, 2, glm_Float> &m) { return glm_pushmat(L, glmMatrix(m)); }
LUAGLM_API int glm_pushmat3x3(lua_State *L, const glm::mat<3, 3, glm_Float> &m) { return glm_pushmat(L, glmMatrix(m)); }
LUAGLM_API int glm_pushmat3x4(lua_State *L, const glm::mat<3, 4, glm_Float> &m) { return glm_pushmat(L, glmMatrix(m)); }
LUAGLM_API int glm_pushmat4x2(lua_State *L, const glm::mat<4, 2, glm_Float> &m) { return glm_pushmat(L, glmMatrix(m)); }
LUAGLM_API int glm_pushmat4x3(lua_State *L, const glm::mat<4, 3, glm_Float> &m) { return glm_pushmat(L, glmMatrix(m)); }
LUAGLM_API int glm_pushmat4x4(lua_State *L, const glm::mat<4, 4, glm_Float> &m) { return glm_pushmat(L, glmMatrix(m)); }

LUAGLM_API glm::mat<2, 2, glm_Float> glm_tomat2x2(lua_State *L, int idx) { return glm_tomat<2, 2, glm_Float>(L, idx); }
LUAGLM_API glm::mat<2, 3, glm_Float> glm_tomat2x3(lua_State *L, int idx) { return glm_tomat<2, 3, glm_Float>(L, idx); }
LUAGLM_API glm::mat<2, 4, glm_Float> glm_tomat2x4(lua_State *L, int idx) { return glm_tomat<2, 4, glm_Float>(L, idx); }
LUAGLM_API glm::mat<3, 2, glm_Float> glm_tomat3x2(lua_State *L, int idx) { return glm_tomat<3, 2, glm_Float>(L, idx); }
LUAGLM_API glm::mat<3, 3, glm_Float> glm_tomat3x3(lua_State *L, int idx) { return glm_tomat<3, 3, glm_Float>(L, idx); }
LUAGLM_API glm::mat<3, 4, glm_Float> glm_tomat3x4(lua_State *L, int idx) { return glm_tomat<3, 4, glm_Float>(L, idx); }
LUAGLM_API glm::mat<4, 2, glm_Float> glm_tomat4x2(lua_State *L, int idx) { return glm_tomat<4, 2, glm_Float>(L, idx); }
LUAGLM_API glm::mat<4, 3, glm_Float> glm_tomat4x3(lua_State *L, int idx) { return glm_tomat<4, 3, glm_Float>(L, idx); }
LUAGLM_API glm::mat<4, 4, glm_Float> glm_tomat4x4(lua_State *L, int idx) { return glm_tomat<4, 4, glm_Float>(L, idx); }

/* }================================================================== */

/*
** {==================================================================
** @DEPRECATED: grit-lua lbaselib
** ===================================================================
*/

namespace glm {
  /// <summary>
  /// @GLMFix: Generalized slerp implementation.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> __objslerp(vec<L, T, Q> const &x, vec<L, T, Q> const &y, T const &a) {
    const T CosAlpha = dot(x, y);

    // Perform a linear interpolation when CosAlpha is close to 1 to avoid side
    // effect of sin(angle) becoming a zero denominator
    if (CosAlpha > static_cast<T>(1) - epsilon<T>())
      return mix(x, y, a);
    else {
      const T Alpha = acos(CosAlpha);  // get angle (0 -> pi)
      const T SinAlpha = sin(Alpha);  // get sine of angle between vectors (0 -> 1)
      const T t1 = sin((static_cast<T>(1) - a) * Alpha) / SinAlpha;
      const T t2 = sin(a * Alpha) / SinAlpha;
      return x * t1 + y * t2;
    }
  }
}

lua_Integer luaO_HashString(const char *string, size_t length, int ignore_case) {
  unsigned int hash = 0;
  for (size_t i = 0; i < length; ++i) {
    hash += cast_uint(ignore_case ? string[i] : tolower(string[i]));
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }

  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);

  // Initial implementation used sign-extended hashes
#if defined(LUA_GRIT_COMPAT)
  return (lua_Integer)(int)hash;
#else
  return l_castU2S(hash);
#endif
}

/* grit-lua functions stored in lbaselib; considered deprecated */

LUA_API int glmVec_dot(lua_State *L) {
  const TValue *x = glm_index2value(L, 1);
  const TValue *y = glm_index2value(L, 2);
  if (ttypetag(x) == ttypetag(y)) {
    switch (ttypetag(x)) {
      case LUA_VNUMINT: lua_pushinteger(L, ivalue(x) * ivalue(y)); break;
      case LUA_VNUMFLT: lua_pushnumber(L, nvalue(x) * nvalue(y)); break;
      case LUA_VVECTOR2: lua_pushnumber(L, cast_num(glm::dot(glm_v2value(x), glm_v2value(y)))); break;
      case LUA_VVECTOR3: lua_pushnumber(L, cast_num(glm::dot(glm_v3value(x), glm_v3value(y)))); break;
      case LUA_VVECTOR4: lua_pushnumber(L, cast_num(glm::dot(glm_v4value(x), glm_v4value(y)))); break;
      case LUA_VQUAT: lua_pushnumber(L, cast_num(glm::dot(glm_qvalue(x), glm_qvalue(y)))); break;
      default: {
        return luaL_typeerror(L, 1, GLM_STRING_NUMBER " or " GLM_STRING_VECTOR " type");
      }
    }
  }
  else if (ttisnumber(x) && ttisnumber(y))  // integer coercion
    lua_pushnumber(L, nvalue(x) * nvalue(y));
  else {
    return luaL_typeerror(L, 1, GLM_STRING_NUMBER " or " GLM_STRING_VECTOR " type");
  }
  return 1;
}

LUA_API int glmVec_cross(lua_State *L) {
  const TValue *x = glm_index2value(L, 1);
  const TValue *y = glm_index2value(L, 2);
  switch (ttypetag(x)) {
    case LUA_VVECTOR2: {
      if (ttypetag(y) == LUA_VVECTOR2) {
        lua_pushnumber(L, cast_num(glm::cross(glm_v2value(x), glm_v2value(y))));
        return 1;
      }
      return luaL_typeerror(L, 2, GLM_STRING_VECTOR2);
    }
    case LUA_VVECTOR3: {
#if defined(LUAGLM_FORCE_HIGHP)  // @GCCHack
      if (ttypetag(y) == LUA_VQUAT) {
        const glm::vec<3, glm_Float, glm::qualifier::highp> v(glm_v3value(x));
        const glm::qua<glm_Float, glm::qualifier::highp> q(glm_qvalue(y));
        return glm_pushvec3(L, glm::vec<3, glm_Float>(glm::cross(v, q)));
      }
      else if (ttypetag(y) == LUA_VVECTOR3) {
        const glm::vec<3, glm_Float, glm::qualifier::highp> v(glm_v3value(x));
        const glm::vec<3, glm_Float, glm::qualifier::highp> v2(glm_v3value(y));
        return glm_pushvec3(L, glm::vec<3, glm_Float>(glm::cross(v, v2)));
      }
#else
      if (ttypetag(y) == LUA_VQUAT)
        return glm_pushvec3(L, glm::cross(glm_v3value(x), glm_qvalue(y)));
      if (ttypetag(y) == LUA_VVECTOR3)
        return glm_pushvec3(L, glm::cross(glm_v3value(x), glm_v3value(y)));
#endif
      return luaL_typeerror(L, 2, GLM_STRING_VECTOR3 " or " GLM_STRING_QUATERN);
    }
    case LUA_VQUAT: {
#if defined(LUAGLM_FORCE_HIGHP)  // @GCCHack
      if (ttypetag(y) == LUA_VQUAT) {
        const glm::qua<glm_Float, glm::qualifier::highp> q(glm_qvalue(x));
        const glm::qua<glm_Float, glm::qualifier::highp> q2(glm_qvalue(y));
        return glm_pushquat(L, glm::qua<glm_Float>(glm::cross(q, q2)));
      }
      else if (ttypetag(y) == LUA_VVECTOR3) {
        const glm::qua<glm_Float, glm::qualifier::highp> q(glm_qvalue(x));
        const glm::vec<3, glm_Float, glm::qualifier::highp> v(glm_v3value(y));
        return glm_pushvec3(L, glm::vec<3, glm_Float>(glm::cross(q, v)));
      }
#else
      if (ttypetag(y) == LUA_VQUAT)
        return glm_pushquat(L, glm::cross(glm_qvalue(x), glm_qvalue(y)));
      if (ttypetag(y) == LUA_VVECTOR3)
        return glm_pushvec3(L, glm::cross(glm_qvalue(x), glm_v3value(y)));
#endif
      return luaL_typeerror(L, 2, GLM_STRING_VECTOR3 " or " GLM_STRING_QUATERN);
    }
    default: {
      break;
    }
  }
  return luaL_typeerror(L, 1, GLM_STRING_VECTOR2 ", " GLM_STRING_VECTOR3 ", or " GLM_STRING_QUATERN);
}

LUA_API int glmVec_inverse(lua_State *L) {
  const TValue *x = glm_index2value(L, 1);
  if (ttisquat(x))
    return glm_pushquat(L, glm::inverse(glm_qvalue(x)));
  else if (ttismatrix(x)) {
    lua_lock(L);
    const glmMatrix /*&*/m = glm_mvalue(x);
    lua_unlock(L);
    switch (m.dimensions) {
      case LUAGLM_MATRIX_2x2: return glm_pushmat2x2(L, glm::inverse(m.m22));
      case LUAGLM_MATRIX_3x3: return glm_pushmat3x3(L, glm::inverse(m.m33));
      case LUAGLM_MATRIX_4x4: return glm_pushmat4x4(L, glm::inverse(m.m44));
      default: {
        break;
      }
    }
  }
  return luaL_typeerror(L, 1, GLM_STRING_QUATERN " or " GLM_STRING_SYMMATRIX);
}

LUA_API int glmVec_normalize(lua_State *L) {
  const TValue *x = glm_index2value(L, 1);
  switch (ttypetag(x)) {
    case LUA_VVECTOR2: return glm_pushvec2(L, glm::normalize(glm_v2value(x)));
    case LUA_VVECTOR3: return glm_pushvec3(L, glm::normalize(glm_v3value(x)));
    case LUA_VVECTOR4: return glm_pushvec4(L, glm::normalize(glm_v4value(x)));
    case LUA_VQUAT: return glm_pushquat(L, glm::normalize(glm_qvalue(x)));
    default: {
      break;
    }
  }
  return luaL_typeerror(L, 1, GLM_STRING_VECTOR " or " GLM_STRING_QUATERN);
}

LUA_API int glmVec_slerp(lua_State *L) {
  const TValue *x = glm_index2value(L, 1);
  const TValue *y = glm_index2value(L, 2);
  const TValue *a = glm_index2value(L, 3);
  if (ttypetag(x) == ttypetag(y) && ttype(a) == LUA_TNUMBER) {
    const glm_Float t = glm_castfloat(nvalue(a));
    switch (ttypetag(x)) {
      case LUA_VVECTOR2: return glm_pushvec2(L, glm::__objslerp(glm_v2value(x), glm_v2value(y), t));
      case LUA_VVECTOR3: return glm_pushvec3(L, glm::__objslerp(glm_v3value(x), glm_v3value(y), t));
      case LUA_VVECTOR4: return glm_pushvec4(L, glm::__objslerp(glm_v4value(x), glm_v4value(y), t));
      case LUA_VQUAT: return glm_pushquat(L, glm::slerp(glm_qvalue(x), glm_qvalue(y), t));
      default: {
        break;
      }
    }
  }
  return luaL_error(L, "slerp(v, v, a) or slerp(q, q, a) expected");
}

LUA_API int glmVec_clamp(lua_State *L) {
  const TValue *x = glm_index2value(L, 1);
  const TValue *y = glm_index2value(L, 2);
  const TValue *z = glm_index2value(L, 3);
  if (ttypetag(x) == ttypetag(y) && ttypetag(y) == ttypetag(z)) {
    switch (ttypetag(x)) {
      case LUA_VNUMINT: /* grit-lua implementation will cast integers */
      case LUA_VNUMFLT: lua_pushnumber(L, glm::clamp(nvalue(x), nvalue(y), nvalue(z))); return 1;
      case LUA_VVECTOR2: return glm_pushvec2(L, glm::clamp(glm_v2value(x), glm_v2value(y), glm_v2value(z)));
      case LUA_VVECTOR3: return glm_pushvec3(L, glm::clamp(glm_v3value(x), glm_v3value(y), glm_v3value(z)));
      case LUA_VVECTOR4: return glm_pushvec4(L, glm::clamp(glm_v4value(x), glm_v4value(y), glm_v4value(z)));
      default: {
        break;
      }
    }
  }
  /* Extensions to grit-lua implementation: */
  else if (ttisnumber(x) && ttisnumber(y) && ttisnumber(z)) {
    lua_pushnumber(L, glm::clamp(nvalue(x), nvalue(y), nvalue(z)));
    return 1;
  }
  else if (ttisnumber(x) && ttisnil(y) && ttisnil(z)) {
    lua_pushnumber(L, glm::clamp(nvalue(x), lua_Number(0), lua_Number(1)));
    return 1;
  }
  return luaL_error(L, GLM_STRING_NUMBER " or " GLM_STRING_VECTOR " expected");
}

LUA_API lua_Integer lua_ToHash(lua_State *L, int idx, int ignore_case) {
  return glm_tohash(L, idx, ignore_case);
}

/* }================================================================== */

/*
** {==================================================================
** LuaGLM C-API
** ===================================================================
*/

// Placeholder representing invalid matrix dimension (packed) value.
#define INVALID_PACKED_DIM glm::length_t(-1)

/// <summary>
/// Unpack a tagged value into a vector "v" starting at offset "v_idx"
/// </summary>
template<typename T>
static glm::length_t PopulateVector(lua_State *L, int idx, glm::vec<4, T> &vec, glm::length_t v_idx, glm::length_t v_desired, const TValue *value) {
  lua_assert(v_desired > 0);
  lua_assert(v_idx >= 0 && v_idx < v_desired);
  if (glm_castvalue(value, vec[v_idx]))  // Primitive type: cast & store it.
    return 1;
  else if (ttisvector(value)) {  // Vector: concatenate components values.
    // To handle (not) GLM_FORCE_QUAT_DATA_XYZW it is much easier to force an
    // explicit length rule for quaternion types. For other vector variants,
    // copy the vector or a subset to satisfy 'v_desired'
    const glmVector &v = glm_vvalue(value);
    if (ttisquat(value)) {
      if ((v_idx + 4) > v_desired) {
        return luaL_argerror(L, idx, "invalid " GLM_STRING_VECTOR " dimensions");
      }
      vec[v_idx++] = static_cast<T>(v.q.x);
      vec[v_idx++] = static_cast<T>(v.q.y);
      vec[v_idx++] = static_cast<T>(v.q.z);
      vec[v_idx++] = static_cast<T>(v.q.w);
      return 4;
    }
    else {
      const glm::length_t dims = glm_dimensions(ttypetag(value));
      const glm::length_t length = glm::min(dims, v_desired - v_idx);
      for (glm::length_t i = 0; i < length; ++i)
        vec[v_idx++] = static_cast<T>(v.v4[i]);
      return length;
    }
  }
  else if (ttistable(value)) {  // Array: concatenate values.
    const glm::length_t array_len = static_cast<glm::length_t>(luaH_getn(hvalue(value)));
    const glm::length_t length = glm::min(array_len, v_desired - v_idx);
    for (glm::length_t i = 1; i <= length; ++i) {
      const TValue *t_val = luaH_getint(hvalue(value), static_cast<lua_Integer>(i));
      if (!glm_castvalue(t_val, vec[v_idx++])) {  // Primitive type: cast & store it.
        return luaL_argerror(L, idx, INVALID_VECTOR_TYPE);
      }
    }
    return length;
  }
  return luaL_argerror(L, idx, INVALID_VECTOR_TYPE);
}

/// <summary>
/// Generic matrix population/construction function. Iterate over the current
/// Lua stack and produce a matrix type according to the rules:
///   1. If the first and only object is a number or vector: populate the
///     diagonal of a matrix.
///   2. If the first and only object is a quaternion: cast it to the
///     arbitrarily sized matrix. This logic follows glm::toMat3 and glm::toMat4
///     and uses constructors to down/up-cast the matrix.
///   3. If the first object is a matrix: down/up-cast it.
///   4. Otherwise, expect a column vector for each dimension of the matrix.
///
/// A "desired" or "expected" dimension may be specified within 'm'. Otherwise,
/// this function will infer the dimensions of matrix according to supplied
/// columns vectors and their dimensions. @TODO: Optimize.
/// </summary>
static bool PopulateMatrix(lua_State *L, int idx, int top, bool fixed_size, glmMatrix &m) {
  const int stack_count = (top - idx + 1);
  const TValue *o = glm_index2value(L, idx);
  if (stack_count == 1 && ttisnumber(o)) {
    m.m44 = glm::mat<4, 4, glm_Float>(glm_castfloat(nvalue(o)));
    return true;
  }
  else if (stack_count == 1 && ttisquat(o)) {
    m.m44 = glm::mat4_cast<glm_Float, glm::defaultp>(glm_qvalue(o));
    return true;
  }
  else if (stack_count == 1 && ttisvector(o)) {
    const lua_Float4 &v = vvalue(o);
    const grit_length_t dims = glm_dimensions(ttypetag(o));
    m.m44 = glm::mat<4, 4, glm_Float>(glm_Float(0));
    m.m44[0][0] = v.raw[0];
    m.m44[1][1] = v.raw[1];
    m.m44[2][2] = (dims > 2) ? v.raw[2] : glm_Float(0);
    m.m44[3][3] = (dims > 3) ? v.raw[3] : glm_Float(0);
    m.dimensions = fixed_size ? m.dimensions : dims;
    return true;
  }
  else if (stack_count == 1 && ttismatrix(o)) {
    const glmMatrix &_m = glm_mvalue(o);
    switch (_m.dimensions) {
      case LUAGLM_MATRIX_2x2: m.m44 = glm::mat<4, 4, glm_Float>(_m.m22); break;
      case LUAGLM_MATRIX_2x3: m.m44 = glm::mat<4, 4, glm_Float>(_m.m23); break;
      case LUAGLM_MATRIX_2x4: m.m44 = glm::mat<4, 4, glm_Float>(_m.m24); break;
      case LUAGLM_MATRIX_3x2: m.m44 = glm::mat<4, 4, glm_Float>(_m.m32); break;
      case LUAGLM_MATRIX_3x3: m.m44 = glm::mat<4, 4, glm_Float>(_m.m33); break;
      case LUAGLM_MATRIX_3x4: m.m44 = glm::mat<4, 4, glm_Float>(_m.m34); break;
      case LUAGLM_MATRIX_4x2: m.m44 = glm::mat<4, 4, glm_Float>(_m.m42); break;
      case LUAGLM_MATRIX_4x3: m.m44 = glm::mat<4, 4, glm_Float>(_m.m43); break;
      case LUAGLM_MATRIX_4x4: m.m44 = _m.m44; break;
      default: {
        return false;
      }
    }
    m.dimensions = fixed_size ? m.dimensions : _m.dimensions;
    return true;
  }

  // Otherwise, parse column vectors
  const glm::length_t mat_size = LUAGLM_MATRIX_COLS(m.dimensions);
  const glm::length_t mat_secondary = LUAGLM_MATRIX_ROWS(m.dimensions);
  if (!fixed_size || mat_size <= static_cast<glm::length_t>(stack_count)) {
    glm::length_t size = 0;
    glm::length_t secondary = fixed_size ? mat_secondary : 0;
    for (; size < static_cast<glm::length_t>(stack_count); ++size) {
      const TValue *value = glm_index2value(L, idx);  // Parse the column/row vector.
      const glm::length_t vec_size = PopulateVector(L, idx++, m.m44[size], 0, mat_secondary, value);
      if (fixed_size && vec_size != mat_secondary)
        return false;  // No/not-enough columns have been parsed
      else if (vec_size > 0 && secondary > 0 && secondary != vec_size)
        return false;  // Inconsistent dimensions
      else if (vec_size == 0)
        break;  // At least one non-fixed column has been parsed.
      else {
        secondary = vec_size;  // Update populated matrix size if non-fixed.
      }
    }

    if (size >= 2 && size <= 4 && secondary >= 2 && secondary <= 4) {
      m.dimensions = LUAGLM_MATRIX_TYPE(size, secondary);
      return true;
    }
  }
  return false;
}

template<typename T>
static LUA_INLINE int glm_pushvalue(lua_State *L, const T &v) {
  GLM_IF_CONSTEXPR(std::is_same<T, bool>::value)
    lua_pushboolean(L, static_cast<int>(v));
  else GLM_IF_CONSTEXPR(std::is_integral<T>::value)
    lua_pushinteger(L, static_cast<lua_Integer>(v));
  else
    lua_pushnumber(L, static_cast<lua_Number>(v));
  return 1;
}

/// <summary>
/// Generic vector population/construction function.
///
/// This function will iterate over the current Lua stack and unpack its values;
/// returning the number of components of the vector populated and zero on
/// failure (e.g., exceeding row size or attempting to populate the vector with
/// an invalid type).
///
/// Unpacking Rules:
///   (1) A primitive type (float, int, bool) will be stored at v[X];
///   (2) A vector (of N dimensions) will have its contents stored at v[X],
///       v[X + 1], ..., v[X + N]; following x, y, z, w ordering (same applies
///       to quaternions);
///   (3) An array (of length N) will have contents started at v[X], v[X + 1], etc.
///   (4) Otherwise, a lua_error is thrown.
/// </summary>
template<typename T>
static int glm_createVector(lua_State *L, glm::length_t desiredSize = 0) {
  glm::vec<4, T> v(T(0));
  glm::length_t v_len = 0;
  const int top = _gettop(L);
  // Hotpath no argument and single argument parameters
  if (top == 0 && desiredSize > 0)  // No arguments; return zero vector
    return glm_pushvec(L, glmVector(v), desiredSize);
  // Fixed size and only one non-table argument
  else if (top == 1 && glm_castvalue(glm_index2value(L, 1), v.x)) {
    if (desiredSize == 1)  // @ImplicitVec
      return glm_pushvalue<T>(L, v.x);
    else if (desiredSize > 1) {
      v.y = v.z = v.w = v.x;
      return glm_pushvec(L, glmVector(v), desiredSize);
    }
  }

  // Maximum number of stack values to parse, starting from "idx"
  lua_lock(L);
  const glm::length_t v_max = desiredSize == 0 ? 4 : desiredSize;
  for (int i = 1; i <= top && v_len < v_max; ++i)
    v_len += PopulateVector(L, i, v, v_len, v_max, glm_index2value(L, i));
  lua_unlock(L);

  if (desiredSize == 0 && v_len == 0)
    return luaL_error(L, GLM_STRING_VECTOR " requires 1 to 4 values");
  else if (desiredSize != 0 && v_len != desiredSize)
    return luaL_error(L, GLM_STRING_VECTOR "%d requires 0, 1, or %d values", cast_int(desiredSize), cast_int(desiredSize));
  else if (v_len == 1)  // @ImplicitVec
    return glm_pushvalue<T>(L, v.x);
  return glm_pushvec(L, glmVector(v), v_len);
}

/// <summary>
/// Generalized create matrix.
/// </summary>
template<typename T>
static int glm_createMatrix(lua_State *L, glm::length_t dimensions) {
  glmMatrix result;
  result.dimensions = dimensions != INVALID_PACKED_DIM ? dimensions : LUAGLM_MATRIX_4x4;

  const int top = _gettop(L);
  if (top == 0) {  // If there are no elements, return the identity matrix
    switch (LUAGLM_MATRIX_ROWS(result.dimensions)) {
      case 2: result.m42 = glm::identity<glm::mat<4, 2, T>>(); break;
      case 3: result.m43 = glm::identity<glm::mat<4, 3, T>>(); break;
      case 4: result.m44 = glm::identity<glm::mat<4, 4, T>>(); break;
      default: {
        break;
      }
    }
    return glm_pushmat(L, result);
  }
  else {  // Parse the contents of the stack and populate 'result'
    const TValue *o = glm_index2value(L, 1);
    const bool recycle = top > 1 && ttismatrix(o);
    lua_lock(L);
    if (PopulateMatrix(L, recycle ? 2 : 1, top, dimensions != INVALID_PACKED_DIM, result)) {
      // Realign column-vectors, ensuring the matrix can be faithfully
      // represented by its m.mCR union value.
      switch (LUAGLM_MATRIX_ROWS(result.dimensions)) {
        case 2: result.m42 = glm::mat<4, 2, glm_Float>(result.m44); break;
        case 3: result.m43 = glm::mat<4, 3, glm_Float>(result.m44); break;
        case 4: /* result.m44 = glm::mat<4, 4, glm_Float>(result.m44); */ break;
        default: {
          break;
        }
      }

      // The first argument was a 'matrix' intended to be recycled. The stack
      // *should* be untouched during PopulateMatrix so using 'o' should be safe
      if (recycle) {
        glm_mat_boundary(mvalue_ref(o)) = result;
        lua_unlock(L);
        lua_pushvalue(L, 1);
        return 1;
      }
      lua_unlock(L);
      return glm_pushmat(L, result);
    }
    lua_unlock(L);
  }
  return luaL_error(L, "invalid " GLM_STRING_MATRIX " structure");
}

LUA_API int glmVec_vec(lua_State *L) { return glm_createVector<glm_Float>(L); }
LUA_API int glmVec_vec1(lua_State *L) { return glm_createVector<glm_Float>(L, 1); }
LUA_API int glmVec_vec2(lua_State *L) { return glm_createVector<glm_Float>(L, 2); }
LUA_API int glmVec_vec3(lua_State *L) { return glm_createVector<glm_Float>(L, 3); }
LUA_API int glmVec_vec4(lua_State *L) { return glm_createVector<glm_Float>(L, 4); }

LUA_API int glmVec_ivec(lua_State *L) { return glm_createVector<glm_Integer>(L); }
LUA_API int glmVec_ivec1(lua_State *L) { return glm_createVector<glm_Integer>(L, 1); }
LUA_API int glmVec_ivec2(lua_State *L) { return glm_createVector<glm_Integer>(L, 2); }
LUA_API int glmVec_ivec3(lua_State *L) { return glm_createVector<glm_Integer>(L, 3); }
LUA_API int glmVec_ivec4(lua_State *L) { return glm_createVector<glm_Integer>(L, 4); }

LUA_API int glmVec_bvec(lua_State *L) { return glm_createVector<bool>(L); }
LUA_API int glmVec_bvec1(lua_State *L) { return glm_createVector<bool>(L, 1); }
LUA_API int glmVec_bvec2(lua_State *L) { return glm_createVector<bool>(L, 2); }
LUA_API int glmVec_bvec3(lua_State *L) { return glm_createVector<bool>(L, 3); }
LUA_API int glmVec_bvec4(lua_State *L) { return glm_createVector<bool>(L, 4); }

LUA_API int glmMat_mat2x2(lua_State *L) { return glm_createMatrix<glm_Float>(L, LUAGLM_MATRIX_2x2); }
LUA_API int glmMat_mat2x3(lua_State *L) { return glm_createMatrix<glm_Float>(L, LUAGLM_MATRIX_2x3); }
LUA_API int glmMat_mat2x4(lua_State *L) { return glm_createMatrix<glm_Float>(L, LUAGLM_MATRIX_2x4); }
LUA_API int glmMat_mat3x2(lua_State *L) { return glm_createMatrix<glm_Float>(L, LUAGLM_MATRIX_3x2); }
LUA_API int glmMat_mat3x3(lua_State *L) { return glm_createMatrix<glm_Float>(L, LUAGLM_MATRIX_3x3); }
LUA_API int glmMat_mat3x4(lua_State *L) { return glm_createMatrix<glm_Float>(L, LUAGLM_MATRIX_3x4); }
LUA_API int glmMat_mat4x2(lua_State *L) { return glm_createMatrix<glm_Float>(L, LUAGLM_MATRIX_4x2); }
LUA_API int glmMat_mat4x3(lua_State *L) { return glm_createMatrix<glm_Float>(L, LUAGLM_MATRIX_4x3); }
LUA_API int glmMat_mat4x4(lua_State *L) { return glm_createMatrix<glm_Float>(L, LUAGLM_MATRIX_4x4); }
LUA_API int glmMat_mat(lua_State *L) { return glm_createMatrix<glm_Float>(L, INVALID_PACKED_DIM); }

/// <summary>
/// Function written to bypass API overheads;
/// </summary>
LUA_API int glmVec_qua(lua_State *L) {
  const TValue *o = glm_index2value(L, 1);
  if (o == &G(L)->nilvalue)  // No arguments: return the identity.
    return glm_pushquat(L, glm::identity<glm::qua<glm_Float>>());
  else if (ttisnumber(o)) {
    const TValue *o2 = glm_index2value(L, 2);
    if (ttisvector3(o2))  // <angle, axis>, degrees for grit-lua compatibility
      return glm_pushquat(L, glm::angleAxis(glm_castfloat(glm::radians(nvalue(o))), glm_v3value(o2)));
    else if (ttisnumber(o2)) {  // <w, x, y, z>
      const glm_Float w = glm_castfloat(nvalue(o));
      const glm_Float x = glm_castfloat(nvalue(o2));
      const glm_Float y = glm_castfloat(luaL_checknumber(L, 3));
      const glm_Float z = glm_castfloat(luaL_checknumber(L, 4));
      return glm_pushquat(L, glm::qua<glm_Float>(
#if defined(GLM_FORCE_QUAT_DATA_XYZW)  // @LUAGLM_QUAT_WXYZ
        x, y, z, w
#else
        w, x, y, z
#endif
      ));
    }
    return luaL_error(L, "{w, x, y, z} or {angle, axis} expected");
  }
  else if (ttisvector3(o)) {
    const TValue *o2 = glm_index2value(L, 2);
    if (!_isvalid(L, o2))  // <euler>
      return glm_pushquat(L, glm::qua<glm_Float>(glm_v3value(o)));
    else if (ttisnumber(o2))  // <xyz, w>
      return glm_pushquat(L, glm::qua<glm_Float>(glm_castfloat(nvalue(o2)), glm_v3value(o)));
    else if (ttisvector3(o2))  // <from, to>
      return glm_pushquat(L, glm::qua<glm_Float>(glm_v3value(o), glm_v3value(o2)));
    return luaL_error(L, "{euler}, {from, to}, or {xyz, w} expected");
  }
  else if (ttisquat(o)) {
    lua_pushvalue(L, 1);
    return 1;
  }
  else if (ttismatrix(o)) {
    lua_lock(L);
    const glmMatrix /*&*/m = glm_mvalue(o);
    lua_unlock(L);
    switch (m.dimensions) {
      case LUAGLM_MATRIX_3x3: return glm_pushquat(L, glm::qua<glm_Float>(m.m33));
      case LUAGLM_MATRIX_4x4: return glm_pushquat(L, glm::qua<glm_Float>(m.m44));
      default: {
        return luaL_typeerror(L, 1, GLM_STRING_MATRIX "3x3 or " GLM_STRING_MATRIX "4x4");
      }
    }
  }
  return luaL_typeerror(L, 1, GLM_STRING_NUMBER ", " GLM_STRING_VECTOR3 ", or " GLM_STRING_MATRIX);
}

LUA_API const char *glm_typename(lua_State *L, int idx) {
  const TValue *o = glm_index2value(L, idx);
  switch (ttypetag(o)) {
    case LUA_VNUMFLT: return GLM_STRING_NUMBER;
    case LUA_VNUMINT: return GLM_STRING_INTEGER;
    case LUA_VVECTOR2: return GLM_STRING_VECTOR2;
    case LUA_VVECTOR3: return GLM_STRING_VECTOR3;
    case LUA_VVECTOR4: return GLM_STRING_VECTOR4;
    case LUA_VQUAT: return GLM_STRING_QUATERN;
    case LUA_VMATRIX: return GLM_STRING_MATRIX;
    default: {
      return "Unknown GLM Type";
    }
  }
}

LUA_API const char *glm_pushstring(lua_State *L, int idx) {
  const TValue *o = glm_index2value(L, idx);
  if (ttisinteger(o))
    return lua_pushfstring(L, LUA_INTEGER_FMT, ivalue(o));
  else if (ttisfloat(o))
    return lua_pushfstring(L, LUA_NUMBER_FMT, static_cast<LUAI_UACNUMBER>(lua_tonumber(L, idx)));
  else if (ttisvector(o)) {
    char buff[GLM_STRING_BUFFER];
    const int len = glmVec_tostr(o, buff, GLM_STRING_BUFFER);
    return lua_pushlstring(L, buff, cast_sizet(len < 0 ? 0 : len));
  }
  else if (ttismatrix(o)) {
    char buff[GLM_STRING_BUFFER];
    lua_lock(L);
    const int len = glmMat_tostr(o, buff, GLM_STRING_BUFFER);
    lua_unlock(L);
    return lua_pushlstring(L, buff, cast_sizet(len < 0 ? 0 : len));
  }
  return lua_pushliteral(L, "nil");
}

LUA_API int glm_unpack_vector(lua_State *L, int idx) {
  luaL_checkstack(L, 4, "vector fields"); // Ensure stack-space
  const TValue *o = glm_index2value(L, idx);
  switch (ttypetag(o)) {
    case LUA_VVECTOR2:
      lua_pushnumber(L, cast_num(vecvalue(o).raw[0]));
      lua_pushnumber(L, cast_num(vecvalue(o).raw[1]));
      return 2;
    case LUA_VVECTOR3:
      lua_pushnumber(L, cast_num(vecvalue(o).raw[0]));
      lua_pushnumber(L, cast_num(vecvalue(o).raw[1]));
      lua_pushnumber(L, cast_num(vecvalue(o).raw[2]));
      return 3;
    case LUA_VVECTOR4:
      lua_pushnumber(L, cast_num(vecvalue(o).raw[0]));
      lua_pushnumber(L, cast_num(vecvalue(o).raw[1]));
      lua_pushnumber(L, cast_num(vecvalue(o).raw[2]));
      lua_pushnumber(L, cast_num(vecvalue(o).raw[3]));
      return 4;
    case LUA_VQUAT:
      lua_pushnumber(L, cast_num(glm_qvalue(o).w));
      lua_pushnumber(L, cast_num(glm_qvalue(o).x));
      lua_pushnumber(L, cast_num(glm_qvalue(o).y));
      lua_pushnumber(L, cast_num(glm_qvalue(o).z));
      return 4;
    default: {
      lua_pushvalue(L, idx);
      return 1;
    }
  }
}

LUA_API int glm_unpack_matrix(lua_State *L, int idx) {
  luaL_checkstack(L, 4, "matrix unpack");
  const TValue *o = glm_index2value(L, idx);
  if (ttismatrix(o)) {
    lua_lock(L);
    const glmMatrix /*&*/m = glm_mvalue(o);
    lua_unlock(L);
    for (glm::length_t i = 0; i < LUAGLM_MATRIX_COLS(m.dimensions); ++i) {
      switch (LUAGLM_MATRIX_ROWS(m.dimensions)) {
        case 2: glm_pushvec2(L, m.m42[i]); break;
        case 3: glm_pushvec3(L, m.m43[i]); break;
        case 4: glm_pushvec4(L, m.m44[i]); break;
        default: {
          lua_pushnil(L);
          break;
        }
      }
    }
    return cast_int(LUAGLM_MATRIX_COLS(m.dimensions));
  }
  return 0;
}

LUA_API lua_Integer glm_tohash(lua_State *L, int idx, int ignore_case) {
  const TValue *o = glm_index2value(L, idx);
  if (ttisstring(o))
    return luaO_HashString(svalue(o), vslen(o), ignore_case);
  else if (ttisboolean(o))
    return ttistrue(o) ? 1 : 0;
  else if (ttisnumber(o)) {
    lua_Integer res = 0;
    return tointeger(o, &res) ? res : 0;
  }
  return 0;
}

/* }================================================================== */


/*
** {==================================================================
** @DEPRECATED: grit-lua API
** ===================================================================
*/

#define VECTOR_PARSE_TABLE 0x1 /* Parse table values as vectors. */
#define VECTOR_PARSE_NUMBER 0x2 /* Ignore @ImplicitVec */
#define VECTOR_DEFAULT VECTOR_PARSE_NUMBER

/* Workaround for GCC 4.8.X warning; GLM supports GCC 4.6 and higher */
#if defined(__GNUC__) && __GNUC__ < 5
  #define FLOAT4_INIT() { {0, 0, 0, 0} }
#else
  #define FLOAT4_INIT() {}
#endif

/* Helper for grit-lua: lua_checkvectorX */
#define checkvector(L, I, T, ERR)                     \
  lua_Float4 f4 = FLOAT4_INIT();                      \
  do {                                                \
    if (l_unlikely(tovector((L), (I), &f4) != (T))) { \
      luaL_typeerror((L), (I), (ERR));                \
      return;                                         \
    }                                                 \
  } while (0)

/// <summary>
/// Parse the table object as a vector type, i.e., check the table for numeric
/// and consecutive 'x', 'y', 'z', and 'w' fields. With 'v' as an optional
/// vector pointer that is populated with the contents from the table.
///
/// Returning the number of dimensions of the resultant vector; zero on failure.
///
/// @NOTE: Function considered deprecated. The previous idea that tables can be
/// implicit vector types does not "mesh" well with the glm binding library.
/// </summary>
static int glmH_tovector(lua_State *L, const TValue *o, glmVector *v) {
  static const char *const dims[] = { "x", "y", "z", "w" };

  int count = 0;
  Table *t = hvalue(o);
  for (int i = 0; i < 4; ++i) {
    TString *key = luaS_newlstr(L, dims[i], 1);  // luaS_newliteral
    const TValue *slot = luaH_getstr(t, key);
    if (ttisnumber(slot)) {
      if (v != GLM_NULLPTR) {
        v->v4[i] = glm_castfloat(nvalue(slot));
      }
      count++;
    }
    else {
      break;
    }
  }
  return count;
}

template<int Flags = VECTOR_DEFAULT>
static lu_byte isvector(lua_State *L, int idx) {
  lua_lock(L);
  lu_byte variant = 0;
  const TValue *o = glm_index2value(L, idx);
  if (l_likely(ttisvector(o)))
    variant = ttypetag(o);
  else if ((Flags & VECTOR_PARSE_NUMBER) != 0 && ttisnumber(o))
    variant = LUA_VVECTOR1;  // @ImplicitVec
  else if ((Flags & VECTOR_PARSE_TABLE) != 0 && ttistable(o)) {
    const int length = glmH_tovector(L, o, GLM_NULLPTR);
    if (length == 1)
      variant = LUA_VVECTOR1;  // @ImplicitVec
    else if (length >= 2 && length <= 4)
      variant = glm_variant(length);
  }
  lua_unlock(L);
  return variant;
}

template<int Flags = VECTOR_DEFAULT>
static int tovector(lua_State *L, int idx, lua_Float4 *f4) {
  lu_byte variant = LUA_TNIL;
  glmVector v(glm::vec<4, glm_Float>(0));

  lua_lock(L);
  const TValue *o = glm_index2value(L, idx);
  if (l_likely(ttisvector(o))) {
    v = glm_vvalue(o);
    variant = ttypetag(o);
  }
  else if ((Flags & VECTOR_PARSE_NUMBER) != 0 && ttisnumber(o))  // @ImplicitVec
    variant = glm_castvalue(o, v.v4.x) ? LUA_VVECTOR1 : LUA_TNIL;
  else if ((Flags & VECTOR_PARSE_TABLE) != 0 && ttistable(o)) {
    const int length = glmH_tovector(L, o, &v);
    if (length == 1)  // @ImplicitVec
      variant = LUA_VVECTOR1;
    else if (length >= 2 && length <= 4)
      variant = glm_variant(length);
  }
  lua_unlock(L);

  if (f4 != GLM_NULLPTR) {
    if (novariant(variant) == LUA_TVECTOR) {
  #if LUAGLM_QUAT_WXYZ
      f4->raw[0] = ((variant == LUA_VQUAT) ? v.q.x : v.v4.x);
      f4->raw[1] = ((variant == LUA_VQUAT) ? v.q.y : v.v4.y);
      f4->raw[2] = ((variant == LUA_VQUAT) ? v.q.z : v.v4.z);
      f4->raw[3] = ((variant == LUA_VQUAT) ? v.q.w : v.v4.w);
  #else
      f4->raw[0] = v.v4.x;
      f4->raw[1] = v.v4.y;
      f4->raw[2] = v.v4.z;
      f4->raw[3] = v.v4.w;
  #endif
    }
    else if (variant == LUA_VVECTOR1) {  // @ImplicitVec
      f4->raw[0] = v.v4.x;
    }
  }

  return variant;
}

LUA_API int lua_isvector2(lua_State *L, int idx) { return isvector(L, idx) == LUA_VVECTOR2; }
LUA_API int lua_isvector3(lua_State *L, int idx) { return isvector(L, idx) == LUA_VVECTOR3; }
LUA_API int lua_isvector4(lua_State *L, int idx) { return isvector(L, idx) == LUA_VVECTOR4; }
LUA_API int lua_isquat(lua_State *L, int idx) { return isvector(L, idx) == LUA_VQUAT; }

LUA_API void lua_checkvector2(lua_State *L, int idx, lua_VecF *x, lua_VecF *y) {
  checkvector(L, idx, LUA_VVECTOR2, GLM_STRING_VECTOR2);
  if (x != GLM_NULLPTR) *x = f4.raw[0];
  if (y != GLM_NULLPTR) *y = f4.raw[1];
}

LUA_API void lua_checkvector3(lua_State *L, int idx, lua_VecF *x, lua_VecF *y, lua_VecF *z) {
  checkvector(L, idx, LUA_VVECTOR3, GLM_STRING_VECTOR3);
  if (x != GLM_NULLPTR) *x = f4.raw[0];
  if (y != GLM_NULLPTR) *y = f4.raw[1];
  if (z != GLM_NULLPTR) *z = f4.raw[2];
}

LUA_API void lua_checkvector4(lua_State *L, int idx, lua_VecF *x, lua_VecF *y, lua_VecF *z, lua_VecF *w) {
  checkvector(L, idx, LUA_VVECTOR4, GLM_STRING_VECTOR4);
  if (x != GLM_NULLPTR) *x = f4.raw[0];
  if (y != GLM_NULLPTR) *y = f4.raw[1];
  if (z != GLM_NULLPTR) *z = f4.raw[2];
  if (w != GLM_NULLPTR) *w = f4.raw[3];
}

LUA_API void lua_checkquat(lua_State *L, int idx, lua_VecF *w, lua_VecF *x, lua_VecF *y, lua_VecF *z) {
  checkvector(L, idx, LUA_VQUAT, GLM_STRING_QUATERN);
  if (w != GLM_NULLPTR) *w = f4.raw[3];
  if (x != GLM_NULLPTR) *x = f4.raw[0];
  if (y != GLM_NULLPTR) *y = f4.raw[1];
  if (z != GLM_NULLPTR) *z = f4.raw[2];
}

LUA_API void lua_pushvector2(lua_State *L, lua_VecF x, lua_VecF y) {
  lua_pushvector(L, lua_Float4{ { x, y, 0, 0 } }, LUA_VVECTOR2);
}

LUA_API void lua_pushvector3(lua_State *L, lua_VecF x, lua_VecF y, lua_VecF z) {
  lua_pushvector(L, lua_Float4{ { x, y, z, 0 } }, LUA_VVECTOR3);
}

LUA_API void lua_pushvector4(lua_State *L, lua_VecF x, lua_VecF y, lua_VecF z, lua_VecF w) {
  lua_pushvector(L, lua_Float4{ { x, y, z, w } }, LUA_VVECTOR4);
}

LUA_API void lua_pushquat(lua_State *L, lua_VecF w, lua_VecF x, lua_VecF y, lua_VecF z) {
  lua_pushvector(L, lua_Float4{ { x, y, z, w } }, LUA_VQUAT);
}

/* }================================================================== */


/*
** {==================================================================
** @DEPRECATED: Extended grit-lua API
** ===================================================================
*/

LUA_API int lua_isvector(lua_State *L, int idx) {
  return isvector(L, idx);
}

LUA_API int lua_tovector(lua_State *L, int idx, lua_Float4 *f4) {
  return tovector(L, idx, f4);
}

LUA_API void lua_pushvector(lua_State *L, lua_Float4 f4, int variant) {
  if (l_likely(novariant(variant) == LUA_TVECTOR)) {
#if LUAGLM_QUAT_WXYZ  // quaternion has WXYZ layout
    if (variant == LUA_VQUAT)
      f4 = lua_Float4{ { f4.raw[3], f4.raw[0], f4.raw[1], f4.raw[2] } };
#endif
    lua_lock(L);
    setvvalue(s2v(L->top), f4, cast_byte(withvariant(variant)));
    api_incr_top(L);
    lua_unlock(L);
  }
  else if (variant == LUA_VVECTOR1)  // @ImplicitVec
    lua_pushnumber(L, cast_num(f4.raw[0]));
  else {
#if defined(LUA_USE_APICHECK)
    luaG_runerror(L, INVALID_VECTOR_TYPE);
#else
    lua_pushnil(L);
#endif
  }
}

LUA_API void lua_pushquatf4(lua_State *L, lua_Float4 f4) {
#if LUAGLM_QUAT_WXYZ  // quaternion has WXYZ layout
  f4 = lua_Float4{ { f4.raw[3], f4.raw[0], f4.raw[1], f4.raw[2] } };
#endif
  lua_lock(L);
  setvvalue(s2v(L->top), f4, LUA_VQUAT);
  api_incr_top(L);
  lua_unlock(L);
}

LUA_API int lua_ismatrix(lua_State *L, int idx, int *dimensions) {
  int result = 0;
  lua_lock(L);
  const TValue *o = glm_index2value(L, idx);
  if (l_likely(ttismatrix(o))) {
    result = 1;
    if (dimensions != GLM_NULLPTR) {
      *dimensions = cast_int(mvalue_dims(o));
    }
  }
  lua_unlock(L);
  return result;
}

LUA_API int lua_tomatrix(lua_State *L, int idx, lua_Mat4 *matrix) {
  int result = 0;
  lua_lock(L);
  const TValue *o = glm_index2value(L, idx);
  if (l_likely(ttismatrix(o) && matrix != GLM_NULLPTR)) {
    result = 1;
    *matrix = mvalue(o);
  }
  lua_unlock(L);
  return result;
}

LUA_API void lua_pushmatrix(lua_State *L, const lua_Mat4 *matrix) {
  if (l_unlikely(matrix == GLM_NULLPTR)) {
#if defined(LUA_USE_APICHECK)
    luaG_runerror(L, INVALID_MATRIX_DIMENSIONS);
#endif
    return;
  }

#if defined(LUA_USE_APICHECK)
  const int m_rows = LUAGLM_MATRIX_ROWS(matrix->dimensions);
  const int m_cols = LUAGLM_MATRIX_COLS(matrix->dimensions);
  if (!(m_cols >= 2 && m_cols <= 4 && m_rows >= 2 && m_rows <= 4)) {
    luaG_runerror(L, INVALID_MATRIX_DIMENSIONS);
  }
#endif
  glm_pushmat(L, glmMatrixBoundary(*matrix).glm);
}

/* }================================================================== */


/*
** {==================================================================
** Metamethod implementations. Ugly.
**
** @GLMIndependent: Operation done only on vec4/mat4x4. Used as an optimization
** as the function is independently applied to each component of the structure.
** Also, if enabled, allow SSE operations on all matrix and vector structures.
** ===================================================================
*/

/*
** Create a new matrix collectible and set it to the stack.
**
** A dimension override is included to simplify the below logic for operations
** that operate on a per-value basis. Allowing the use of more generalized
** operations instead of logic for all nine matrix types.
*/
#define glm_newmvalue(L, obj, x, dims)  \
  LUA_MLM_BEGIN                         \
  GCMatrix *mat = glmMat_new(L);        \
  glm_mat_boundary(&(mat->mat4)) = (x); \
  mat->mat4.dimensions = dims;          \
  glm_setmvalue2s(L, obj, mat);         \
  luaC_checkGC(L);                      \
  LUA_MLM_END

/*
** Operations on integer vectors (or floating-point vectors that are int-casted).
*/
#define INT_VECTOR_OPERATION(F, res, v, p2, t1, t2) \
  LUA_MLM_BEGIN                                     \
  if ((t1) == (t2)) { /* @GLMIndependent */         \
    const glmVector &v2 = glm_vvalue(p2);           \
    glm_setvvalue2s(res, F(                         \
      glm::vec<4, lua_Integer, LUAGLM_Q>((v).v4),   \
      glm::vec<4, lua_Integer, LUAGLM_Q>((v2).v4)   \
    ), (t1));                                       \
    return 1;                                       \
  }                                                 \
  else if ((t2) == LUA_VNUMINT) {                   \
    glm_setvvalue2s(res, F(                         \
      glm::vec<4, lua_Integer, LUAGLM_Q>((v).v4),   \
      ivalue(p2)                                    \
    ), (t1));                                       \
    return 1;                                       \
  }                                                 \
  LUA_MLM_END

/*
@@ LUAGLM_MUL_DIRECTION Define how the runtime handles: `TM_MUL(mat4x4, vec3)`,
** i.e., transform the vec3 as a 'direction' or a 'position'.
*/
#if defined(LUAGLM_MUL_DIRECTION)
  #define MAT_VEC3_W 0 /* Transforms the given vector by: M * (x, y, z, 0) */
#else
  #define MAT_VEC3_W 1 /* Transforms the given vector by: M * (x, y, z, 1) */
#endif

#define MDIMS(A, B) m##A##B
#define MATRIX_MULTIPLICATION_OPERATION(L, res, m1, m2, C, R)                                                       \
  LUA_MLM_BEGIN                                                                                                     \
  switch (LUAGLM_MATRIX_COLS(m2.dimensions)) {                                                                      \
    case 2: glm_newmvalue(L, res, (operator*(m1.MDIMS(C, R), m2.MDIMS(2, C))), LUAGLM_MATRIX_TYPE(2, R)); return 1; \
    case 3: glm_newmvalue(L, res, (operator*(m1.MDIMS(C, R), m2.MDIMS(3, C))), LUAGLM_MATRIX_TYPE(3, R)); return 1; \
    case 4: glm_newmvalue(L, res, (operator*(m1.MDIMS(C, R), m2.MDIMS(4, C))), LUAGLM_MATRIX_TYPE(4, R)); return 1; \
    default:                                                                                                        \
      break;                                                                                                        \
  }                                                                                                                 \
  LUA_MLM_END

static int num_trybinTM(lua_State *L, const TValue *p1, const TValue *p2, StkId res, TMS event) {
  const glm_Float s = glm_toflt(p1);
  switch (event) {
    case TM_ADD: {
      switch (ttype(p2)) {
        case LUA_TVECTOR:
          glm_setvvalue2s(res, operator+(s, glm_v4value(p2)), ttypetag(p2));
          return 1;
        case LUA_TMATRIX: {
          // GLM only supports operator+(T, mat...) on symmetric matrices. This
          // expands that functionality.
          const glmMatrix &m2 = glm_mvalue(p2);
          glm_newmvalue(L, res, operator+(s, m2.m44), m2.dimensions);
          return 1;
        }
        default: {
          break;
        }
      }
      break;
    }
    case TM_SUB: {  // @GLMIndependent
      switch (ttype(p2)) {
        case LUA_TVECTOR:
          glm_setvvalue2s(res, operator-(s, glm_v4value(p2)), ttypetag(p2));
          return 1;
        case LUA_TMATRIX: {
          const glmMatrix &m2 = glm_mvalue(p2);
          glm_newmvalue(L, res, operator-(s, m2.m44), m2.dimensions);
          return 1;
        }
        default: {
          break;
        }
      }
      break;
    }
    case TM_MUL: {  // @GLMIndependent
      switch (ttypetag(p2)) {
        case LUA_VVECTOR2:
        case LUA_VVECTOR3:
        case LUA_VVECTOR4:
          glm_setvvalue2s(res, operator*(s, glm_v4value(p2)), ttypetag(p2));
          return 1;
        case LUA_VQUAT:
          glm_setvvalue2s(res, operator*(s, glm_qvalue(p2)), LUA_VQUAT);
          return 1;
        case LUA_VMATRIX: {
          const glmMatrix &m2 = glm_mvalue(p2);
          glm_newmvalue(L, res, operator*(s, m2.m44), m2.dimensions);
          return 1;
        }
        default: {
          break;
        }
      }
      break;
    }
    case TM_DIV: {  // @GLMIndependent
      switch (ttypetag(p2)) {
        case LUA_VVECTOR2:
        case LUA_VVECTOR3:
        case LUA_VVECTOR4:
        case LUA_VQUAT:
          glm_setvvalue2s(res, operator/(s, glm_v4value(p2)), ttypetag(p2));
          return 1;
        case LUA_VMATRIX: {
          const glmMatrix &m2 = glm_mvalue(p2);
          glm_newmvalue(L, res, operator/(s, m2.m44), m2.dimensions);
          return 1;
        }
        default: {
          break;
        }
      }
      break;
    }
    default: {
      break;
    }
  }
  return 0;
}

static int vec_trybinTM(lua_State *L, const TValue *p1, const TValue *p2, StkId res, TMS event) {
  UNUSED(L);
  const glmVector &v = glm_vvalue(p1);
  const lu_byte tt_p1 = ttypetag(p1);
  switch (event) {
    case TM_ADD: {  // @GLMIndependent
      if (tt_p1 == ttypetag(p2)) {
        glm_setvvalue2s(res, operator+(v.v4, glm_v4value(p2)), tt_p1);
        return 1;
      }
      else if (ttype(p2) == LUA_TNUMBER) {
        glm_setvvalue2s(res, operator+(v.v4, glm_toflt(p2)), tt_p1);
        return 1;
      }
      break;
    }
    case TM_SUB: {  // @GLMIndependent
      if (tt_p1 == ttypetag(p2)) {
        glm_setvvalue2s(res, operator-(v.v4, glm_v4value(p2)), tt_p1);
        return 1;
      }
      else if (ttype(p2) == LUA_TNUMBER) {
        glm_setvvalue2s(res, operator-(v.v4, glm_toflt(p2)), tt_p1);
        return 1;
      }
      break;
    }
    case TM_MUL: {  // @GLMIndependent
      const lu_byte tt_p2 = ttypetag(p2);
      if (tt_p1 == tt_p2) {
        glm_setvvalue2s(res, operator*(v.v4, glm_v4value(p2)), tt_p1);
        return 1;
      }
      else if (ttype(p2) == LUA_TNUMBER) {
        glm_setvvalue2s(res, operator*(v.v4, glm_toflt(p2)), tt_p1);
        return 1;
      }
      else if (tt_p2 == LUA_VQUAT) {
        switch (tt_p1) {
#if defined(LUAGLM_FORCE_HIGHP)  // @GCCHack
          case LUA_VVECTOR3: {
            const glm::vec<3, glm_Float, glm::qualifier::highp> vx(v.v3);
            const glm::qua<glm_Float, glm::qualifier::highp> qy(glm_qvalue(p2));
            const glm::vec<3, glm_Float> result(vx * qy);
            glm_setvvalue2s(res, result, LUA_VVECTOR3);
            return 1;
          }
#else
          case LUA_VVECTOR3: {
            glm_setvvalue2s(res, v.v3 * glm_qvalue(p2), LUA_VVECTOR3);
            return 1;
          }
#endif
#if defined(LUAGLM_ALIGNED)  // @QuatHack
          case LUA_VVECTOR4: {
            const glm::vec<4, glm_Float, glm::qualifier::highp> vx(v.v4);
            const glm::qua<glm_Float, glm::qualifier::highp> qy(glm_qvalue(p2));
            const glm::vec<4, glm_Float> result(vx * qy);
            glm_setvvalue2s(res, result, LUA_VVECTOR4);
            return 1;
          }
#else
          case LUA_VVECTOR4: {
            glm_setvvalue2s(res, operator*(v.v4, glm_qvalue(p2)), LUA_VVECTOR4);
            return 1;
          }
#endif
          default: {
            break;
          }
        }
      }
      else if (tt_p2 == LUA_VMATRIX) {
        const glmMatrix &m2 = glm_mvalue(p2);
        if (LUAGLM_MATRIX_ROWS(m2.dimensions) == glm_dimensions(tt_p1)) {
          switch (m2.dimensions) {
            case LUAGLM_MATRIX_2x2: glm_setvvalue2s(res, operator*(v.v2, m2.m22), LUA_VVECTOR2); return 1;
            case LUAGLM_MATRIX_2x3: glm_setvvalue2s(res, operator*(v.v3, m2.m23), LUA_VVECTOR2); return 1;
            case LUAGLM_MATRIX_2x4: glm_setvvalue2s(res, operator*(v.v4, m2.m24), LUA_VVECTOR2); return 1;
            case LUAGLM_MATRIX_3x2: glm_setvvalue2s(res, operator*(v.v2, m2.m32), LUA_VVECTOR3); return 1;
            case LUAGLM_MATRIX_3x3: glm_setvvalue2s(res, operator*(v.v3, m2.m33), LUA_VVECTOR3); return 1;
            case LUAGLM_MATRIX_3x4: glm_setvvalue2s(res, operator*(v.v4, m2.m34), LUA_VVECTOR3); return 1;
            case LUAGLM_MATRIX_4x2: glm_setvvalue2s(res, operator*(v.v2, m2.m42), LUA_VVECTOR4); return 1;
            case LUAGLM_MATRIX_4x3: glm_setvvalue2s(res, operator*(v.v3, m2.m43), LUA_VVECTOR4); return 1;
            case LUAGLM_MATRIX_4x4: glm_setvvalue2s(res, operator*(v.v4, m2.m44), LUA_VVECTOR4); return 1;
            default: {
              break;
            }
          }
        }
      }
      break;
    }
    case TM_MOD: {  // @GLMIndependent; Using fmod for the same reasons described in llimits.h
      if (tt_p1 == ttypetag(p2)) {
        glm_setvvalue2s(res, glm::fmod(v.v4, glm_v4value(p2)), tt_p1);
        return 1;
      }
      else if (ttype(p2) == LUA_TNUMBER) {
        glm_setvvalue2s(res, glm::fmod(v.v4, glm_toflt(p2)), tt_p1);
        return 1;
      }
      break;
    }
    case TM_POW: {  // @GLMIndependent
      if (tt_p1 == ttypetag(p2)) {
        glm_setvvalue2s(res, glm::pow(v.v4, glm_v4value(p2)), tt_p1);
        return 1;
      }
      else if (ttype(p2) == LUA_TNUMBER) {
        glm_setvvalue2s(res, glm::pow(v.v4, decltype(v.v4)(glm_toflt(p2))), tt_p1);
        return 1;
      }
      break;
    }
    case TM_DIV: {  // @GLMIndependent
      const lu_byte tt_p2 = ttypetag(p2);
      if (tt_p1 == tt_p2) {
        glm_setvvalue2s(res, operator/(v.v4, glm_v4value(p2)), tt_p1);
        return 1;
      }
      else if (ttype(p2) == LUA_TNUMBER) {
        glm_setvvalue2s(res, operator/(v.v4, glm_toflt(p2)), tt_p1);
        return 1;
      }
      else if (tt_p2 == LUA_VMATRIX) {
        const glmMatrix &m2 = glm_mvalue(p2);
        const grit_length_t cols = LUAGLM_MATRIX_COLS(m2.dimensions);
        if (cols == LUAGLM_MATRIX_ROWS(m2.dimensions) && tt_p1 == glm_variant(cols)) {
          switch (tt_p1) {
            case LUA_VVECTOR2: glm_setvvalue2s(res, operator/(v.v2, m2.m22), LUA_VVECTOR2); return 1;
            case LUA_VVECTOR3: glm_setvvalue2s(res, operator/(v.v3, m2.m33), LUA_VVECTOR3); return 1;
            case LUA_VVECTOR4: glm_setvvalue2s(res, operator/(v.v4, m2.m44), LUA_VVECTOR4); return 1;
            default: {
              break;
            }
          }
          break;
        }
      }
      break;
    }
    case TM_IDIV: {  // @GLMIndependent
      if (tt_p1 == ttypetag(p2)) {
        glm_setvvalue2s(res, glm::floor(v.v4 / glm_v4value(p2)), tt_p1);
        return 1;
      }
      else if (ttype(p2) == LUA_TNUMBER) {
        glm_setvvalue2s(res, glm::floor(v.v4 / glm_toflt(p2)), tt_p1);
        return 1;
      }
      break;
    }
    case TM_BAND:  // @GLMIndependent
      INT_VECTOR_OPERATION(operator&, res, v, p2, tt_p1, ttypetag(p2));
      break;
    case TM_BOR:  // @GLMIndependent
      INT_VECTOR_OPERATION(operator|, res, v, p2, tt_p1, ttypetag(p2));
      break;
    case TM_BXOR:  // @GLMIndependent
      INT_VECTOR_OPERATION(operator^, res, v, p2, tt_p1, ttypetag(p2));
      break;
    case TM_SHL:  // @GLMIndependent
      INT_VECTOR_OPERATION(operator<<, res, v, p2, tt_p1, ttypetag(p2));
      break;
    case TM_SHR:  // @GLMIndependent
      INT_VECTOR_OPERATION(operator>>, res, v, p2, tt_p1, ttypetag(p2));
      break;
    case TM_UNM:  // @GLMIndependent
      glm_setvvalue2s(res, operator-(v.v4), tt_p1);
      return 1;
    case TM_BNOT:  // @GLMIndependent
      glm_setvvalue2s(res, operator~(glm::vec<4, lua_Integer, LUAGLM_Q>(v.v4)), tt_p1);
      return 1;
    default: {
      break;
    }
  }
  return 0;
}

static int quat_trybinTM(lua_State *L, const TValue *p1, const TValue *p2, StkId res, TMS event) {
  UNUSED(L);
  const glmVector &v = glm_vvalue(p1);
  switch (event) {
    case TM_ADD: {
      if (ttypetag(p2) == LUA_VQUAT) {
        glm_setvvalue2s(res, operator+(glm_qvalue(p1), glm_qvalue(p2)), LUA_VQUAT);
        return 1;
      }
      else if (ttype(p2) == LUA_TNUMBER) {
        // @GLMIndependent; Not supported by GLM but allow vector semantics.
        glm_setvvalue2s(res, operator+(v.v4, glm_toflt(p2)), LUA_VQUAT);
        return 1;
      }
      break;
    }
    case TM_SUB: {
      if (ttypetag(p2) == LUA_VQUAT) {
#if defined(LUAGLM_ALIGNED)  // @QuatHack
        const glm::qua<glm_Float, glm::qualifier::highp> qx(glm_qvalue(p1));
        const glm::qua<glm_Float, glm::qualifier::highp> qy(glm_qvalue(p2));
        glm_setvvalue2s(res, glm::qua<glm_Float>(qx - qy), LUA_VQUAT);
        return 1;
#else
        glm_setvvalue2s(res, operator-(glm_qvalue(p1), glm_qvalue(p2)), LUA_VQUAT);
        return 1;
#endif
      }
      else if (ttype(p2) == LUA_TNUMBER) {
        // @GLMIndependent; Not supported by GLM but allow vector semantics.
        glm_setvvalue2s(res, operator-(v.v4, glm_toflt(p2)), LUA_VQUAT);
        return 1;
      }
      break;
    }
    case TM_MUL: {
      switch (ttypetag(p2)) {
        case LUA_VNUMINT:
          glm_setvvalue2s(res, operator*(v.q, glm_castfloat(ivalue(p2))), LUA_VQUAT);
          return 1;
        case LUA_VNUMFLT:
          glm_setvvalue2s(res, operator*(v.q, glm_castfloat(fltvalue(p2))), LUA_VQUAT);
          return 1;
#if defined(LUAGLM_FORCE_HIGHP)  // @GCCHack
        case LUA_VVECTOR3: {
          const glm::qua<glm_Float, glm::qualifier::highp> qx(glm_qvalue(p1));
          const glm::vec<3, glm_Float, glm::qualifier::highp> vy(glm_v3value(p2));
          const glm::vec<3, glm_Float> result(qx * vy);
          glm_setvvalue2s(res, result, LUA_VVECTOR3);
          return 1;
        }
#else
        case LUA_VVECTOR3:
          glm_setvvalue2s(res, operator*(v.q, glm_v3value(p2)), LUA_VVECTOR3);
          return 1;
#endif
#if defined(LUAGLM_ALIGNED)  // @QuatHack
        case LUA_VVECTOR4: {
          const glm::qua<glm_Float, glm::qualifier::highp> qx(glm_qvalue(p1));
          const glm::vec<4, glm_Float, glm::qualifier::highp> vy(glm_v4value(p2));
          const glm::vec<4, glm_Float> result(qx * vy);
          glm_setvvalue2s(res, result, LUA_VVECTOR4);
          return 1;
        }
#else
        case LUA_VVECTOR4:
          glm_setvvalue2s(res, operator*(v.q, glm_v4value(p2)), LUA_VVECTOR4);
          return 1;
#endif
        case LUA_VQUAT:
          glm_setvvalue2s(res, operator*(v.q, glm_qvalue(p2)), LUA_VQUAT);
          return 1;
        default: {
          break;
        }
      }
      break;
    }
    case TM_POW: {
      if (ttype(p2) == LUA_TNUMBER) {
        glm_setvvalue2s(res, glm::pow(v.q, glm_toflt(p2)), LUA_VQUAT);
        return 1;
      }
      break;
    }
    case TM_DIV: {
      if (ttype(p2) == LUA_TNUMBER) {
        const glm_Float s = glm_toflt(p2);
        glm::qua<glm_Float> result = glm::identity<glm::qua<glm_Float>>();
        if (glm::notEqual(s, glm_Float(0), glm::epsilon<glm_Float>())) {
          result = v.q / s;
        }

        glm_setvvalue2s(res, result, LUA_VQUAT);
        return 1;
      }
      break;
    }
    case TM_UNM:
      glm_setvvalue2s(res, operator-(v.q), LUA_VQUAT);
      return 1;
    default: {
      break;
    }
  }
  return 0;
}

static int mat_trybinTM(lua_State *L, const TValue *p1, const TValue *p2, StkId res, TMS event) {
  const glmMatrix &m = glm_mvalue(p1);
  const grit_length_t cols = LUAGLM_MATRIX_COLS(m.dimensions);
  switch (event) {
    case TM_ADD: {  // @GLMIndependent
      if (ttypetag(p2) == LUA_VMATRIX && m.dimensions == mvalue_dims(p2)) {
        const glmMatrix &m2 = glm_mvalue(p2);
        glm_newmvalue(L, res, operator+(m.m44, m2.m44), m.dimensions);
        return 1;
      }
      else if (ttype(p2) == LUA_TNUMBER) {
        glm_newmvalue(L, res, operator+(m.m44, glm_toflt(p2)), m.dimensions);
        return 1;
      }
      break;
    }
    case TM_SUB: {  // @GLMIndependent
      if (ttypetag(p2) == LUA_VMATRIX && m.dimensions == mvalue_dims(p2)) {
        const glmMatrix &m2 = glm_mvalue((p2));
        glm_newmvalue(L, res, operator-(m.m44, m2.m44), m.dimensions);
        return 1;
      }
      else if (ttype(p2) == LUA_TNUMBER) {
        glm_newmvalue(L, res, operator-(m.m44, glm_toflt(p2)), m.dimensions);
        return 1;
      }
      break;
    }
    case TM_MUL: {
      const lu_byte tt_p2 = ttypetag(p2);
      if (tt_p2 == LUA_VMATRIX) {
        const glmMatrix &m2 = glm_mvalue(p2);
        if (cols == LUAGLM_MATRIX_ROWS(m2.dimensions)) {
          switch (m.dimensions) {
            case LUAGLM_MATRIX_2x2: MATRIX_MULTIPLICATION_OPERATION(L, res, m, m2, 2, 2); return 1;
            case LUAGLM_MATRIX_2x3: MATRIX_MULTIPLICATION_OPERATION(L, res, m, m2, 2, 3); return 1;
            case LUAGLM_MATRIX_2x4: MATRIX_MULTIPLICATION_OPERATION(L, res, m, m2, 2, 4); return 1;
            case LUAGLM_MATRIX_3x2: MATRIX_MULTIPLICATION_OPERATION(L, res, m, m2, 3, 2); return 1;
            case LUAGLM_MATRIX_3x3: MATRIX_MULTIPLICATION_OPERATION(L, res, m, m2, 3, 3); return 1;
            case LUAGLM_MATRIX_3x4: MATRIX_MULTIPLICATION_OPERATION(L, res, m, m2, 3, 4); return 1;
            case LUAGLM_MATRIX_4x2: MATRIX_MULTIPLICATION_OPERATION(L, res, m, m2, 4, 2); return 1;
            case LUAGLM_MATRIX_4x3: MATRIX_MULTIPLICATION_OPERATION(L, res, m, m2, 4, 3); return 1;
            case LUAGLM_MATRIX_4x4: MATRIX_MULTIPLICATION_OPERATION(L, res, m, m2, 4, 4); return 1;
            default: {
              break;
            }
          }
        }
      }
      else if (tt_p2 == glm_variant(cols)) {
        const glmVector &v2 = glm_vvalue(p2);
        switch (m.dimensions) {
          case LUAGLM_MATRIX_2x2: glm_setvvalue2s(res, operator*(m.m22, v2.v2), LUA_VVECTOR2); return 1;
          case LUAGLM_MATRIX_2x3: glm_setvvalue2s(res, operator*(m.m23, v2.v2), LUA_VVECTOR3); return 1;
          case LUAGLM_MATRIX_2x4: glm_setvvalue2s(res, operator*(m.m24, v2.v2), LUA_VVECTOR4); return 1;
          case LUAGLM_MATRIX_3x2: glm_setvvalue2s(res, operator*(m.m32, v2.v3), LUA_VVECTOR2); return 1;
          case LUAGLM_MATRIX_3x3: glm_setvvalue2s(res, operator*(m.m33, v2.v3), LUA_VVECTOR3); return 1;
          case LUAGLM_MATRIX_3x4: glm_setvvalue2s(res, operator*(m.m34, v2.v3), LUA_VVECTOR4); return 1;
          case LUAGLM_MATRIX_4x2: glm_setvvalue2s(res, operator*(m.m42, v2.v4), LUA_VVECTOR2); return 1;
          case LUAGLM_MATRIX_4x3: glm_setvvalue2s(res, operator*(m.m43, v2.v4), LUA_VVECTOR3); return 1;
          case LUAGLM_MATRIX_4x4: glm_setvvalue2s(res, operator*(m.m44, v2.v4), LUA_VVECTOR4); return 1;
          default: {
            break;
          }
        }
      }
      // Special case for handling mat4x4 * vec3 and mat4x3 * vec3; see LUAGLM_MUL_DIRECTION.
      else if (tt_p2 == LUA_VVECTOR3) {
        const glm::mat<4, 4, glm_Float>::col_type p(glm_v3value(p2), MAT_VEC3_W);
        switch (m.dimensions) {
          case LUAGLM_MATRIX_4x3: glm_setvvalue2s(res, operator*(m.m43, p), LUA_VVECTOR3); return 1;
          case LUAGLM_MATRIX_4x4: glm_setvvalue2s(res, operator*(m.m44, p), LUA_VVECTOR3); return 1;
          default:
            break;
        }
        break;
      }
      else if (ttype(p2) == LUA_TNUMBER) {  // @GLMIndependent
        glm_newmvalue(L, res, operator*(m.m44, glm_toflt(p2)), m.dimensions);
        return 1;
      }
      break;
    }
    case TM_DIV: {
      const lu_byte tt_p2 = ttypetag(p2);
      if (tt_p2 == LUA_VMATRIX) {  // operator/(matNxN, matNxN)
        const glmMatrix &m2 = glm_mvalue(p2);
        if (m.dimensions == m2.dimensions && cols == LUAGLM_MATRIX_ROWS(m.dimensions)) {
          switch (m.dimensions) {
            case LUAGLM_MATRIX_2x2: glm_newmvalue(L, res, operator/(m.m22, m2.m22), LUAGLM_MATRIX_2x2); return 1;
            case LUAGLM_MATRIX_3x3: glm_newmvalue(L, res, operator/(m.m33, m2.m33), LUAGLM_MATRIX_3x3); return 1;
            case LUAGLM_MATRIX_4x4: glm_newmvalue(L, res, operator/(m.m44, m2.m44), LUAGLM_MATRIX_4x4); return 1;
            default: {
              break;
            }
          }
        }
      }
      else if (tt_p2 == glm_variant(cols)) {  // operator/(matrix, vector)
        const glmVector &v2 = glm_vvalue(p2);
        switch (cols) {
          case 2: glm_setvvalue2s(res, operator/(m.m22, v2.v2), LUA_VVECTOR2); return 1;
          case 3: glm_setvvalue2s(res, operator/(m.m33, v2.v3), LUA_VVECTOR3); return 1;
          case 4: glm_setvvalue2s(res, operator/(m.m44, v2.v4), LUA_VVECTOR4); return 1;
          default: {
            break;
          }
        }
      }
      else if (ttype(p2) == LUA_TNUMBER) {  // @GLMIndependent
        glm_newmvalue(L, res, operator/(m.m44, glm_toflt(p2)), m.dimensions);
        return 1;
      }
      break;
    }
    case TM_UNM:  // @GLMIndependent
      glm_newmvalue(L, res, operator-(m.m44), m.dimensions);
      return 1;
    default: {
      break;
    }
  }
  return 0;
}

/* }================================================================== */
