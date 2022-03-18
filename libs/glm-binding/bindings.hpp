/*
** $Id: bindings.hpp $
**
** Template system designed to relate the various glm types (e.g., scalars, vec,
** quaternions, and matrices) and their functions to Lua operations. Designed to
** be compatible with g++, clang++, and MSVC (prior to https://docs.microsoft.com/en-us/cpp/preprocessor/preprocessor-experimental-overview?view=vs-2019)
**
** See Copyright Notice in lua.h
*/
#ifndef BINDING_LUA_BINDINGS_HPP
#define BINDING_LUA_BINDINGS_HPP

/* Ensure linkage specification is defined; compensate for unity builds. */
#if !defined(LUA_API_LINKAGE)
  #if defined(LUA_C_LINKAGE)
    #define LUA_API_LINKAGE "C"
  #else
    #define LUA_API_LINKAGE "C++"
  #endif
#endif

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/gtc/integer.hpp>
#include <glm/gtx/type_trait.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/scalar_relational.hpp>
#include "ext/matrix_extensions.hpp"  // glm::fixed_mat

#include <lua.hpp>
#include <lglm.hpp>
extern LUA_API_LINKAGE {
#include "lgrit_lib.h"
#include "lapi.h"
#include "lobject.h"
#include "lgc.h"
#include "lstate.h"
#include "lvm.h"
#include "lglm_core.h"
}

/*
@@ LUA_HAS_ATTRIBUTE __has_attribute wrapper.
*/
#if !defined(LUA_HAS_ATTRIBUTE)
#if defined(__has_attribute)
  #define LUA_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
  #define LUA_HAS_ATTRIBUTE(x) 0
#endif
#endif

/*
@@ LUA_HAS_CPP_ATTRIBUTE __has_cpp_attribute wrapper.
*/
#if !defined(LUA_HAS_CPP_ATTRIBUTE)
#if defined(__has_cpp_attribute)
  #define LUA_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
  #define LUA_HAS_CPP_ATTRIBUTE(x) 0
#endif
#endif

/*
@@ LUA_NOEXCEPT noexcept wrapper
*/
#if !defined(LUA_NOEXCEPT)
#if defined(GLM_CXX11_NOEXCEPT)
  #define LUA_NOEXCEPT noexcept
#else
  #define LUA_NOEXCEPT
#endif
#endif

/*
@@ LUA_FALLTHROUGH Macro for informing the compiler a fallthrough is intentional.
*/
#if !defined(LUA_FALLTHROUGH)
#if LUA_HAS_CPP_ATTRIBUTE(fallthrough) && !defined(__INTEL_COMPILER)
  #define LUA_FALLTHROUGH [[fallthrough]]
#elif LUA_HAS_ATTRIBUTE(__fallthrough__)
  #define LUA_FALLTHROUGH __attribute__((__fallthrough__))
#else
  #define LUA_FALLTHROUGH \
    do {                  \
    } while (0) /* FALLTHROUGH */
#endif
#endif

/*
@@ LUA_NO_RETURN non-return type
*/
#if !defined(LUA_NORETURN)
#if defined(__GNUC__) || LUA_HAS_ATTRIBUTE(__noreturn__)
  #define LUA_NORETURN void __attribute__((noreturn))
#elif defined(_MSC_VER) && _MSC_VER >= 1200
  #define LUA_NORETURN void __declspec(noreturn)
#else
  #define LUA_NORETURN void
#endif
#endif

/*
@@ LUA_UNREACHABLE Unreachable code path reached.
*/
#if !defined(LUA_UNREACHABLE)
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
  #define LUA_UNREACHABLE() __builtin_unreachable()
#elif defined(__clang__) || defined(__INTEL_COMPILER)
  #define LUA_UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
  #define LUA_UNREACHABLE() __assume(false)
#else
  #define LUA_UNREACHABLE() lua_assert(false)
#endif
#endif

/*
** GLM disables constexpr use when compiler SIMD intrinsics are enabled. This
** library redefines these macros for usage for non-SIMD/intrinsic code paths.
**
@@ LUA_HAS_CONSTEXPR GLM_HAS_CONSTEXPR analogue
*/
#if (GLM_LANG & GLM_LANG_CXX11_FLAG)
  #define LUA_HAS_CONSTEXPR 1
#elif (GLM_COMPILER & GLM_COMPILER_CLANG)
  #define LUA_HAS_CONSTEXPR __has_feature(cxx_relaxed_constexpr
#else
  #define LUA_HAS_CONSTEXPR ((GLM_LANG & GLM_LANG_CXX0X_FLAG) && GLM_HAS_INITIALIZER_LISTS && (((GLM_COMPILER & GLM_COMPILER_INTEL) && (GLM_COMPILER >= GLM_COMPILER_INTEL17)) || ((GLM_COMPILER & GLM_COMPILER_VC) && (GLM_COMPILER >= GLM_COMPILER_VC15))))
#endif

/*
@@ LUA_CONSTEXPR GLM_CONSTEXPR analogue
*/
#if LUA_HAS_CONSTEXPR
  #define LUA_CONSTEXPR constexpr
#else
  #define LUA_CONSTEXPR
#endif

/*
@@ LUA_CONSTEXPR_STATEMENT Avoid 'statement in constexpr function' wrapper for
** icc, etc.
*/
#if !(GLM_COMPILER & GLM_COMPILER_INTEL) && (GLM_LANG & GLM_LANG_CXX14_FLAG)
  #define LUA_CONSTEXPR_STATEMENT LUA_CONSTEXPR
#else
  #define LUA_CONSTEXPR_STATEMENT
#endif

/*
@@ LUA_HAS_IF_CONSTEXPR GLM_HAS_IF_CONSTEXPR analogue
*/
#if LUA_HAS_CONSTEXPR
  #if (GLM_LANG & GLM_LANG_CXX17_FLAG)
    #define LUA_HAS_IF_CONSTEXPR 1
  #elif (GLM_COMPILER & GLM_COMPILER_CLANG)
    #if __has_feature(cxx_if_constexpr)
      #define LUA_HAS_IF_CONSTEXPR 1
    #else
      #define LUA_HAS_IF_CONSTEXPR 0
    #endif
  #else
    #define LUA_HAS_IF_CONSTEXPR 0
  #endif
#else
  #define LUA_HAS_IF_CONSTEXPR 0
#endif

/*
@@ LUA_IF_CONSTEXPR GLM_IF_CONSTEXPR analogue
*/
#if LUA_HAS_IF_CONSTEXPR
  #define LUA_IF_CONSTEXPR if constexpr
#else
  #define LUA_IF_CONSTEXPR if
#endif

/*
@@ LUAGLM_INCLUDE_IEEE Include IEEE-754 specific bindings; otherwise presume
** long double operations are enabled.
*/
#define LUAGLM_INCLUDE_IEEE (LUA_FLOAT_TYPE != LUA_FLOAT_LONGDOUBLE)

/*
@@ LUAGLM_ALIGNED SIMD is enabled by default and compensate for some SIMD-based
** implementations being broken in GLM.
*/
#if !defined(LUAGLM_ALIGNED)
#if GLM_CONFIG_ALIGNED_GENTYPES == GLM_ENABLE && defined(GLM_FORCE_DEFAULT_ALIGNED_GENTYPES)
  #define LUAGLM_ALIGNED 1
#else
  #define LUAGLM_ALIGNED 0
#endif
#endif

/*
@@ LUAGLM_BINDING_REALIGN A mark denoting the alignment requirements of the Lua
** runtime and binding library are different. Vectors, quats, and matrices
** require explicit load/stores when interfacing with the Lua stack.
**
** @GLMFix: Vectors and quaternions have constructors which can implicitly
** convert these types; matrices require the glm::fixed_mat .
*/
#if LUAGLM_ALIGNED && !defined(LUAGLM_FORCES_ALIGNED_GENTYPES)
  #define glm_mat_cast(M, C, R, T, P) glm::fixed_mat<C, R, T, P>((M))
  #define LUAGLM_BINDING_REALIGN 1
  #if GLM_MESSAGES == GLM_ENABLE
    #pragma message("LuaGLM: binding and runtime compiled with different alignments")
  #endif
#else
  #define glm_mat_cast(M, C, R, T, P) ((M))
  #define LUAGLM_BINDING_REALIGN 0
  #if GLM_MESSAGES == GLM_ENABLE
    #pragma message("LuaGLM: binding and runtime compiled with same alignment")
  #endif

  #if !LUAGLM_ALIGNED && defined(LUAGLM_FORCES_ALIGNED_GENTYPES)
    #error "Runtime is compiled with aligned types and so should the binding..."
  #endif
#endif

/*
@@ LUAGLM_BINDING_QUAL Default alignment qualifier for binding operations.
**
** Future proofing to allow configuration independent of GLM_FORCE_DEFAULT_ALIGNED_GENTYPES,
** e.g., a unity build where the binding library is compiled with SIMD while the
** runtime is not; glm::defaultp cannot be used explicitly in this case.
*/
#define LUAGLM_BINDING_QUAL glm::defaultp

/*
** {==================================================================
** Inlined Lua functions
** ===================================================================
*/

/* lua_gettop */
#if !defined(_gettop)
#define _gettop(L) (cast_int((L)->top - ((L)->ci->func + 1)))
#endif

/* test for a valid index (one that is not the 'nilvalue') */
#if !defined(_isvalid)
#define _isvalid(L, o) (!ttisnil(o) || o != &G(L)->nilvalue)
#endif

/* object accessors */
#if !defined(glm_vvalue)
#if defined(LUAGLM_HALF_STORAGE)
#define _A(o, i) (i < std::decay<decltype(o)>::type::length() ? (o)[i] : 0)
#define glm_vvalue(o) glmVector(f4_loadf4(vvalue(o)))
#define glm_setvvalue2s(s, x, o) \
  setvvalue(s2v(s), f4_cstore(_A(x, 0), _A(x, 1), _A(x, 2), _A(x, 3)), (o))
#else
#define glm_vvalue(o) glm_constvec_boundary(vvalue_ref(o))
#define glm_setvvalue2s(s, x, o)        \
  LUA_MLM_BEGIN                         \
  TValue *io = s2v(s);                  \
  glm_vec_boundary(&vvalue_(io)) = (x); \
  settt_(io, (o));                      \
  LUA_MLM_END
#endif

/* glm::type vector references */
#define glm_v2value(o) glm_vvalue(o).v2
#define glm_v3value(o) glm_vvalue(o).v3
#define glm_v4value(o) glm_vvalue(o).v4
#define glm_qvalue(o) glm_vvalue(o).q

/* glm::type matrix references */
#define glm_mvalue(o) glm_constmat_boundary(mvalue_ref(o))
#endif

/*
** index2value ported from lapi.c. Simplified to only operate on positive stack
** indices; see related function index2stack.
*/
static LUA_INLINE const TValue *glm_i2v(const lua_State *L, int idx) {
  const StkId o = L->ci->func + idx;
  api_check(L, idx > 0, "positive indices only");
  api_check(L, idx <= L->ci->top - (L->ci->func + 1), "invalid index");
  return (o < L->top) ? s2v(o) : &G(L)->nilvalue;
}

/// <summary>
/// RAII for lua_lock/lua_unlock
/// </summary>
class lua_LockScope {
private:
  lua_State *L;
public:
  lua_LockScope(lua_State *L_) : L(L_) { lua_lock(L); }
  ~lua_LockScope() { Unlock(); }
  inline void Unlock() {
    if (L != nullptr) {
      lua_unlock(L);
      L = nullptr;
    }
  }
};

/* }================================================================== */

/*
** {==================================================================
** Stack Iterator
** ===================================================================
*/

/* Base Lua binding declaration */
#define LUA_BIND_DECL static

/* Extended Lua binding declaration (always inline when possible) */
#define LUA_BIND_QUALIFIER LUA_BIND_DECL GLM_INLINE

/*
** Extended Lua binding declaration (never inline).
**
** Prevent the compiler from inlining the 'heavier' interface declarations.
** For instance, tointegerx and tonumberx are relatively heavy, will interfere
** with otherwise-nice codegen, and used in most binding functions (satisfying
** the lmathlib superset requirement).
*/
#define LUA_BIND_QUALIFIER_NIL LUA_BIND_DECL GLM_NEVER_INLINE

/* Wrapper for LUAGLM_TYPE_SANITIZE */
#if defined(LUAGLM_TYPE_SANITIZE)
  #define LUA_NARROW_CAST(L, T, V) gLuaBase::narrow_cast<T>((L), (V))
#else
  #define LUA_NARROW_CAST(L, T, V) static_cast<T>((V))
#endif

/// <summary>
/// Forward declaration of the Lua type trait: the interface that interacts with
/// a stack iterator that converts sequences of Lua objects into function
/// parameters/results and vice-versa.
///
/// The relationship of Lua objects to function parameters may not be one-to-one
/// and multiple Lua objects may correspond to a single parameter (and vice-versa).
/// </summary>
template<typename T, bool FastPath = false>
struct gLuaTrait;

/// <summary>
/// A structure that iterates over an active Lua state.
/// </summary>
struct gLuaBase {
  lua_State *L;  // Current lua state.
  int idx;  // Iteration pointer.

  /// <summary>
  /// Lazy cache of lua_gettop: used for object-recycling and to prevent
  /// clobbering, i.e., writing to the same object twice.
  /// </summary>
  int recycle_top;

  gLuaBase(lua_State *baseL, int baseIdx = 1)
    : L(baseL), idx(baseIdx), recycle_top(0) {
    lua_assert(baseIdx >= 1);
  }

  /// <summary>
  /// Inlined equivalent of lua_gettop.
  /// </summary>
  LUA_INLINE int top() const {
    return _gettop(L);
  }

  /// <summary>
  /// Reset the iterator
  /// </summary>
  LUA_INLINE gLuaBase &reset() {
    idx = 1;
    return *this;
  }

  /// <summary>
  /// Invalid the iterator index, i.e., set it greater than top().
  /// </summary>
  LUA_INLINE void invalidate() {
    idx = top() + 1;
  }

  /// <summary>
  /// Returns lua_gettop ensuring the value is cached for object recycling.
  /// </summary>
  LUA_INLINE int top_for_recycle() {
    return (recycle_top == 0) ? (recycle_top = _gettop(L)) : recycle_top;
  }

  /// <summary>
  /// Return true if the current iteration pointer references a valid, and
  /// recyclable, data structure.
  /// </summary>
  LUA_INLINE bool can_recycle() {
#if defined(LUAGLM_RECYCLE)
    lua_assert(idx >= 1);
    return idx <= top_for_recycle();
#else
    return false;
#endif
  }

  /*
  ** std::random_device analogue using math.random
  **
  ** For performance reasons, this implementation requires lua_upvalueindex(1)
  ** to reference math.random.
  */

  using result_type = lua_Unsigned;

  /// <summary>
  /// Return the smallest possible value in the output range.
  /// </summary>
  static LUA_CONSTEXPR result_type min() {
    return 0;
  }

  /// <summary>
  /// Return the largest possible value in the output range.
  /// </summary>
  static LUA_CONSTEXPR result_type max() {
    return static_cast<result_type>(LUA_MAXINTEGER);
  }

  /// <summary>
  /// Return the entropy estimate for the non-deterministic random number generator.
  /// </summary>
  double entropy() const LUA_NOEXCEPT {
    return 32.0;
  }

  /// <summary>
  /// Advances the engine's state and returns the generated value.
  /// </summary>
  result_type operator()() {
    result_type result = 0;
    lua_checkstack(L, 2);
    lua_pushvalue(L, lua_upvalueindex(1));  // [..., rand_func]
    if (lua_isfunction(L, -1)) {
      lua_pushinteger(L, LUA_MAXINTEGER);  // [..., rand_func, upper limit]
      lua_call(L, 1, 1);  // [..., result]
      result = static_cast<result_type>(lua_tointegerx(L, -1, GLM_NULLPTR));
    }
    else {  // Otherwise, fallback to std::rand if lmathlib has not been cached.
      result = static_cast<result_type>(cast_num(max()) * (cast_num(std::rand()) / cast_num(RAND_MAX)));
    }
    lua_pop(L, 1);  // [...]
    return result;
  }

  /* Lua Exception Wrappers */

  /// <summary>
  /// @HACK: luaL_typeerror that conveys noreturn information to the compiler.
  /// </summary>
  static LUA_NORETURN typeerror(lua_State *L_, int arg, const char *tname) {
    luaL_typeerror(L_, arg, tname);

    // This code should never be reached given that a lngjmp or try/catch is
    // hidden underneath luaL_typeerror.
    LUA_UNREACHABLE();
  }

  /// <summary>
  /// @HACK: See typeerror
  /// </summary>
  static LUA_NORETURN argerror(lua_State *L_, int arg, const char *extrams) {
    luaL_argerror(L_, arg, extrams);
    LUA_UNREACHABLE();
  }

  /// <summary>
  /// @HACK: See typeerror
  /// </summary>
  static LUA_NORETURN error(lua_State *L_, const char *msg) {
    lua_pushstring(L_, msg);
    lua_error(L_);
    LUA_UNREACHABLE();
  }

  /* Binding Helpers */

  template<class T, class U>
  LUA_BIND_QUALIFIER T narrow_cast(lua_State *L_, U u) {
    LUA_IF_CONSTEXPR(std::is_same<T, U>::value) return static_cast<T>(u);
    const T t = static_cast<T>(u);
    if (static_cast<U>(t) != u  // however, allow sign mismatch...
        /* || (std::is_signed<T>::value != std::is_signed<U>::value && ((t < T{}) != (u < U{}))) */) {
      error(L_, "integer value out of range");
    }
    return t;
  }

  /// <summary>
  /// lua_isnoneornil
  /// </summary>
  LUA_BIND_QUALIFIER bool isnoneornil(lua_State *L_, int idx_) {
    const TValue *o = glm_i2v(L_, idx_);
    return !_isvalid(L_, o);
  }

  /// <summary>
  /// lua_tointeger with additional rules for casting booleans.
  /// </summary>
  template<typename T = lua_Integer>
  LUA_BIND_QUALIFIER_NIL T tointegerx(lua_State *L_, int idx_) {
    const TValue *o = glm_i2v(L_, idx_);
    switch (ttypetag(o)) {
      case LUA_VTRUE: return static_cast<T>(1);
      case LUA_VFALSE: return static_cast<T>(0);
      case LUA_VNUMINT: return LUA_NARROW_CAST(L_, T, ivalue(o));
      case LUA_VNUMFLT: return static_cast<T>(fltvalue(o));
      default: {
#if defined(LUAGLM_TYPE_COERCION)
        return static_cast<T>(luaL_checkinteger(L_, idx_));
#else
        gLuaBase::typeerror(L_, idx_, LUAGLM_STRING_INTEGER);
        return T(0);
#endif
      }
    }
  }

  /// <summary>
  /// lua_tonumber with additional rules for casting booleans.
  ///
  /// @MathlibCompat: string coercion must exist to be a superset of lmathlib.
  /// As much of the luaL_checknumber logic is redundant, and could be
  /// optimized, luaV_tonumber_ is not an exported function.
  /// </summary>
  template<typename T = lua_Number>
  LUA_BIND_QUALIFIER_NIL T tonumberx(lua_State *L_, int idx_) {
    const TValue *o = glm_i2v(L_, idx_);
    switch (ttypetag(o)) {
      case LUA_VTRUE: return static_cast<T>(1);
      case LUA_VFALSE: return static_cast<T>(0);
      case LUA_VNUMINT: return static_cast<T>(ivalue(o));
      case LUA_VNUMFLT: return static_cast<T>(fltvalue(o));
      default: {
#if defined(LUAGLM_TYPE_COERCION)
        return static_cast<T>(luaL_checknumber(L_, idx_));
#else
        gLuaBase::typeerror(L_, idx_, LUAGLM_STRING_NUMBER);
        return T(0);
#endif
      }
    }
  }

  /// <summary>
  /// glm_i2v wrapper.
  /// </summary>
  LUA_INLINE const TValue *i2v(int offset = 0) {
    return glm_i2v(L, idx + offset);
  }

  /// <summary>
  /// Tr::Is() wrapper.
  /// </summary>
  template<class Tr>
  LUA_INLINE bool Is(int offset = 0) {
    return Tr::Is(L, idx + offset);
  }

  /// <summary>
  /// Tr::Next() wrapper.
  /// </summary>
  template<class Tr>
  LUA_INLINE typename Tr::type Next() {
    return Tr::Next(L, idx);
  }

  /// <summary>
  /// Next<gLuaTrait<T>>() wrapper.
  /// </summary>
  template<typename T>
  LUA_INLINE T IsNextType() {
    return Is<gLuaTrait<T>>();
  }

  /// <summary>
  /// Next<gLuaTrait<T>>() wrapper.
  /// </summary>
  template<typename T>
  LUA_INLINE T AsNextType() {
    return Next<gLuaTrait<T>>();
  }

  /// <summary>
  /// Pushes a nil value onto the stack.
  /// </summary>
  LUA_BIND_QUALIFIER int Push(const gLuaBase &LB) {
    luaL_pushfail(LB.L);
    return 1;
  }

  /// <summary>
  /// Push a trait value onto the stack.
  /// </summary>
  template<typename T>
  LUA_BIND_QUALIFIER int Push(const gLuaBase &LB, const T &v) {
    return gLuaTrait<typename std::decay<T>::type>::Push(LB, v);
  }

  /// <summary>
  /// Push a trait value onto the stack, potentially updating top_for_recycle.
  /// </summary>
  template<typename T>
  LUA_BIND_QUALIFIER int Push(gLuaBase &LB, const T &v) {
    return gLuaTrait<typename std::decay<T>::type>::Push(LB, v);
  }

  /// <summary>
  /// Attempt to push the number as an integer; falling back to number otherwise
  /// </summary>
  LUA_BIND_QUALIFIER int PushNumInt(const gLuaBase &LB, lua_Number d) {
    lua_Integer n = 0;
    if (lua_numbertointeger(d, &n)) /* does 'd' fit in an integer? */
      lua_pushinteger(LB.L, n); /* result is integer */
    else
      lua_pushnumber(LB.L, d); /* result is float */
    return 1;
  }

  /// <summary>
  /// Attempt to push the vector as an glm::ivec; falling back to glm::vec otherwise.
  /// </summary>
  template<glm::length_t L, typename T, glm::qualifier Q>
  LUA_BIND_QUALIFIER int PushNumInt(const gLuaBase &LB, const glm::vec<L, T, Q> &v) {
    return gLuaTrait<glm::vec<L, T, Q>>::Push(LB, v);
  }

  /// <summary>
  /// Pull(gLuaBase) wrapper
  /// </summary>
  template<typename T>
  LUA_BIND_QUALIFIER int Pull(lua_State *L_, int idx_, T &v) {
    if (gLuaTrait<T>::Is(L_, idx_)) {
      v = gLuaTrait<T>::Next(L_, idx_);
      return 1;
    }
    return 0;
  }

  /// <summary>
  /// Push(gLuaBase) wrapper
  /// </summary>
  template<typename T>
  LUA_BIND_QUALIFIER int Push(lua_State *L, const T &v) {
    gLuaBase base(L, _gettop(L) + 1);
    return gLuaBase::Push(base, v);
  }
};

/* }================================================================== */

/*
** {==================================================================
** Type Traits and Constraints
** ===================================================================
*/

/// <summary>
/// Forward declaration of epsilon (floating point thresholds, error types, etc).
/// </summary>
template<typename T, bool FastPath = false, bool DefaultZero = false>
struct gEpsilon;

/// <summary>
/// Shared properties for parsing types from the Lua stack.
/// </summary>
template<typename T, typename ValueType = typename T::value_type>
struct gLuaAbstractTrait : glm::type<T> {

  /// <summary>
  /// Base type.
  /// </summary>
  using type = T;

  /// <summary>
  /// Base primitive type: equivalent to ::value_type in glm::vec and glm::mat
  /// structures.
  /// </summary>
  using value_type = ValueType;

  /// <summary>
  /// Trait of the primitive type.
  /// </summary>
  using value_trait = gLuaTrait<value_type>;

  /// <summary>
  /// Epsilon of value_type.
  /// </summary>
  using eps_trait = gEpsilon<value_type>;

  /// <summary>
  /// @CastBinding: Cast this trait to a new primitive.
  /// </summary>
  template<typename Type = T>
  using as_type = gLuaTrait<Type>;

  /// <summary>
  /// @SafeBinding: Cast this trait to one that type-checks its value on Next.
  /// </summary>
  using safe = gLuaTrait<T, false>;

  /// <summary>
  /// @UnsafeBinding: Cast this trait to one that pulls the value without
  /// additional type checking.
  ///
  /// @NOTE: The idiom 'Tr::fast::Is' + 'Tr::fast::Next' may only be used when
  /// its guaranteed the trait does not coerce types, e.g., string-to-float or
  /// table-to-vector/matrix.
  /// </summary>
  using fast = gLuaTrait<T, true>;

  /// <summary>
  /// @StackSize: Default number of elements on the stack required by the type-trait.
  /// This value should primarily be used by trait iterators.
  /// </summary>
  static const LUA_CONSTEXPR int stack_size = 1;

  /// <summary>
  /// Return a descriptive parameter literal for debugging/error messaging.
  /// </summary>
  /// LUA_BIND_DECL LUA_CONSTEXPR const char *Label()

  /// <summary>
  /// Zero (or default) initialize
  /// </summary>
  LUA_BIND_QUALIFIER GLM_CONSTEXPR T Zero() {
    return T();
  }

  /// <summary>
  /// Return true if the value starting at "idx" on the Lua stack corresponds
  /// to this type.
  /// </summary>
  /// LUA_BIND_QUALIFIER bool Is(lua_State *L, int idx)

  /// <summary>
  /// Given a current stack state, create a GLM object corresponding to the
  /// "type" this trait is capturing.
  /// </summary>
  /// LUA_BIND_QUALIFIER T Next(lua_State *L, int &idx)
};

/// <summary>
/// gLuaTrait optimized for primitive types.
/// </summary>
template<typename T, bool FastPath = false>
struct gLuaPrimitive : gLuaAbstractTrait<T, T> {
  LUA_BIND_DECL LUA_CONSTEXPR_STATEMENT const char *Label() {
    LUA_IF_CONSTEXPR(std::is_same<T, bool>::value) return "bool";
    LUA_IF_CONSTEXPR(std::is_integral<T>::value) return LUAGLM_STRING_INTEGER;
    LUA_IF_CONSTEXPR(std::is_floating_point<T>::value) return LUAGLM_STRING_NUMBER;
    return "Unknown_Type";
  }

  LUA_BIND_QUALIFIER bool Is(lua_State *L, int idx) {
    const TValue *o = glm_i2v(L, idx);
    LUA_IF_CONSTEXPR(std::is_same<T, bool>::value) return ttisboolean(o);  // lua_isboolean(LB.L, idx);
    LUA_IF_CONSTEXPR(std::is_integral<T>::value) return ttisinteger(o) || ttisboolean(o);  // lua_isinteger(LB.L, idx)
    LUA_IF_CONSTEXPR(std::is_floating_point<T>::value) return lua_isnumber(L, idx);  // @TODO: isboolean
    lua_assert(false);
    return false;
  }

  LUA_BIND_QUALIFIER T Next(lua_State *L, int &idx) {
    LUA_IF_CONSTEXPR(FastPath) {
      const TValue *o = glm_i2v(L, idx++);
      LUA_IF_CONSTEXPR(std::is_same<T, bool>::value) return static_cast<T>(!l_isfalse(o));
      LUA_IF_CONSTEXPR(std::is_integral<T>::value) return LUA_NARROW_CAST(L, T, ivalue(o));
      LUA_IF_CONSTEXPR(std::is_floating_point<T>::value) return static_cast<T>(fltvalue(o));
    }
    else {
      LUA_IF_CONSTEXPR(std::is_same<T, bool>::value) { const TValue *o = glm_i2v(L, idx++); return !l_isfalse(o); }
      LUA_IF_CONSTEXPR(std::is_integral<T>::value) return gLuaBase::tointegerx<T>(L, idx++);
      LUA_IF_CONSTEXPR(std::is_floating_point<T>::value) return gLuaBase::tonumberx<T>(L, idx++);
    }
    lua_assert(false);
    return T(0);
  }

  template<typename U>
  LUA_BIND_QUALIFIER_NIL typename std::enable_if<std::is_integral<U>::value && !std::is_same<U, bool>::value, int>::type PushPrimitive(const gLuaBase &LB, U v) {
    lua_LockScope _lock(LB.L);
    setivalue(s2v(LB.L->top), LUA_NARROW_CAST(LB.L, lua_Integer, v));
    api_incr_top(LB.L);
    return 1;
  }

  template<typename U>
  LUA_BIND_QUALIFIER_NIL typename std::enable_if<std::is_floating_point<U>::value, int>::type PushPrimitive(const gLuaBase &LB, U v) {
    lua_LockScope _lock(LB.L);
    setfltvalue(s2v(LB.L->top), static_cast<lua_Number>(v));
    api_incr_top(LB.L);
    return 1;
  }

  LUA_BIND_QUALIFIER_NIL int PushPrimitive(const gLuaBase &LB, bool b) {
    lua_LockScope _lock(LB.L);
    settt_(s2v(LB.L->top), b ? LUA_VTRUE : LUA_VFALSE);
    api_incr_top(LB.L);
    return 1;
  }

  /// <summary>
  /// Push a primitive type onto the Lua stack.
  /// </summary>
  LUA_BIND_QUALIFIER int Push(const gLuaBase &LB, T v) {
    return PushPrimitive(LB, v);
  }
};

/// <summary>
/// Abstraction (shared properties) for glm::vec traits.
/// </summary>
template<glm::length_t D, typename T, glm::qualifier Q>
struct gLuaAbstractVector : gLuaAbstractTrait<glm::vec<D, T, Q>> {
  template<typename Type = T>
  using as_type = gLuaTrait<glm::vec<D, Type, Q>>;  // @CastBinding

  /// <summary>
  /// Cast the vector trait to a compatible matrix type.
  ///
  /// This type is mainly a helper for matrix-multiplication operators.
  /// </summary>
  template<glm::length_t R>
  using rhs_mat_type = gLuaTrait<glm::mat<R, D, T, Q>>;

  /// <summary>
  /// Alternative name: lhs_mat_type
  /// </summary>
  using row_type = gLuaTrait<glm::vec<D, T, Q>>;

  /// <summary>
  /// </summary>
  using gLuaAbstractTrait<glm::vec<D, T, Q>>::Zero;

  LUA_BIND_QUALIFIER bool Is(lua_State *L, int idx) {
    const TValue *o = glm_i2v(L, idx);
    return checktag((o), glm_variant(D));
  }

  /// <summary>
  /// Push a glm::vec onto the Lua stack
  /// </summary>
  LUA_BIND_QUALIFIER int Push(const gLuaBase &LB, const glm::vec<D, T, Q> &v) {
    //GLM_STATIC_ASSERT(D >= 2 && D <= 4, "invalid vector specialization");
    lua_LockScope _lock(LB.L);
    glm_setvvalue2s(LB.L->top, v, glm_variant(D));  // May use explicit copy constructor
    api_incr_top(LB.L);
    return 1;
  }
};

/// <summary>
/// Generic type trait for all types: POD/Structs/Classes.
/// </summary>
template<typename T, bool FastPath> struct gLuaTrait : gLuaAbstractTrait<T, T> { };

// Explicit type trait specializations for primitive types.
template<bool FastPath> struct gLuaTrait<bool, FastPath> : gLuaPrimitive<bool, FastPath> { };
template<bool FastPath> struct gLuaTrait<char, FastPath> : gLuaPrimitive<char, FastPath> { };
template<bool FastPath> struct gLuaTrait<char16_t, FastPath> : gLuaPrimitive<char16_t, FastPath> { };
template<bool FastPath> struct gLuaTrait<char32_t, FastPath> : gLuaPrimitive<char32_t, FastPath> { };
template<bool FastPath> struct gLuaTrait<signed char, FastPath> : gLuaPrimitive<signed char, FastPath> { };
template<bool FastPath> struct gLuaTrait<signed short int, FastPath> : gLuaPrimitive<signed short int, FastPath> { };
template<bool FastPath> struct gLuaTrait<signed int, FastPath> : gLuaPrimitive<signed int, FastPath> { };
template<bool FastPath> struct gLuaTrait<signed long int, FastPath> : gLuaPrimitive<signed long int, FastPath> { };
template<bool FastPath> struct gLuaTrait<signed long long int, FastPath> : gLuaPrimitive<signed long long int, FastPath> { };
template<bool FastPath> struct gLuaTrait<unsigned char, FastPath> : gLuaPrimitive<unsigned char, FastPath> { };
template<bool FastPath> struct gLuaTrait<unsigned short int, FastPath> : gLuaPrimitive<unsigned short int, FastPath> { };
template<bool FastPath> struct gLuaTrait<unsigned int, FastPath> : gLuaPrimitive<unsigned int, FastPath> { };
template<bool FastPath> struct gLuaTrait<unsigned long int, FastPath> : gLuaPrimitive<unsigned long int, FastPath> { };
template<bool FastPath> struct gLuaTrait<unsigned long long int, FastPath> : gLuaPrimitive<unsigned long long int, FastPath> { };
template<bool FastPath> struct gLuaTrait<float, FastPath> : gLuaPrimitive<float, FastPath> { };
template<bool FastPath> struct gLuaTrait<double, FastPath> : gLuaPrimitive<double, FastPath> { };
#if LUA_FLOAT_TYPE == LUA_FLOAT_LONGDOUBLE
template<bool FastPath> struct gLuaTrait<long double, FastPath> : gLuaPrimitive<long double, FastPath> { };
#endif

/// <summary>
/// strings type trait; includes the ability to also fetch the length of a
/// string on Next. As lua_tolstring has the ability to relocate the Lua stack,
/// this implementation will not coerce other types.
/// </summary>
template<bool FastPath>
struct gLuaTrait<const char *, FastPath> : gLuaAbstractTrait<const char *, const char *> {
  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() { return "string"; }

  LUA_BIND_QUALIFIER LUA_CONSTEXPR const char *Zero() {
    return GLM_NULLPTR;
  }

  LUA_BIND_QUALIFIER bool Is(lua_State *L, int idx) {
    const TValue *o = glm_i2v(L, idx);
    return ttisstring(o);
  }

  LUA_BIND_QUALIFIER const char *Next(lua_State *L, int &idx, size_t *len = GLM_NULLPTR) {
    const TValue *o = glm_i2v(L, idx++);
    if (FastPath || l_likely(ttisstring(o))) {
      if (len != GLM_NULLPTR)
        *len = vslen(o);
      return svalue(o);
    }
    gLuaBase::typeerror(L, idx - 1, Label());
    return GLM_NULLPTR;
  }

  LUA_BIND_QUALIFIER int Push(const gLuaBase &LB, const char *s) {
    lua_pushstring(LB.L, s);
    return 1;
  }
};

/// <summary>
/// @ImplicitVec glm::vec<1, T, Q> type trait
/// </summary>
template<typename T, glm::qualifier Q, bool FastPath>
struct gLuaTrait<glm::vec<1, T, Q>, FastPath> : gLuaAbstractVector<1, T, Q> {
  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() { return LUAGLM_STRING_VECTOR1; }

  LUA_BIND_QUALIFIER bool Is(lua_State *L, int idx) {
    return gLuaTrait<T>::Is(L, idx);
  }

  LUA_BIND_QUALIFIER glm::vec<1, T, Q> Next(lua_State *L, int &idx) {
    return glm::vec<1, T, Q>(gLuaTrait<T>::Next(L, idx));
  }

  LUA_BIND_QUALIFIER int Push(const gLuaBase &LB, const glm::vec<1, T, Q> &v) {
    return gLuaTrait<T>::Push(LB, v.x);
  }
};

/// <summary>
/// glm::vec<2, T, Q> type trait.
/// </summary>
template<typename T, glm::qualifier Q, bool FastPath>
struct gLuaTrait<glm::vec<2, T, Q>, FastPath> : gLuaAbstractVector<2, T, Q> {
  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() { return LUAGLM_STRING_VECTOR2; }

  LUA_BIND_QUALIFIER glm::vec<2, T, Q> Next(lua_State *L, int &idx) {
    const TValue *o = glm_i2v(L, idx++);
    if (FastPath || l_likely(ttisvector2(o)))
      return glm_v2value(o);
    else {
      gLuaBase::typeerror(L, idx - 1, Label());
      return glm::vec<2, T, Q>();
    }
  }
};

/// <summary>
/// glm::vec<3, T, Q> type trait.
/// </summary>
template<typename T, glm::qualifier Q, bool FastPath>
struct gLuaTrait<glm::vec<3, T, Q>, FastPath> : gLuaAbstractVector<3, T, Q> {
  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() { return LUAGLM_STRING_VECTOR3; }

  LUA_BIND_QUALIFIER glm::vec<3, T, Q> Next(lua_State *L, int &idx) {
    const TValue *o = glm_i2v(L, idx++);
    if (FastPath || l_likely(ttisvector3(o)))
      return glm_v3value(o);
    else {
      gLuaBase::typeerror(L, idx - 1, Label());
      return glm::vec<3, T, Q>();
    }
  }
};

/// <summary>
/// glm::vec<4, T, Q> type trait.
/// </summary>
template<typename T, glm::qualifier Q, bool FastPath>
struct gLuaTrait<glm::vec<4, T, Q>, FastPath> : gLuaAbstractVector<4, T, Q> {
  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() { return LUAGLM_STRING_VECTOR4; }

  LUA_BIND_QUALIFIER glm::vec<4, T, Q> Next(lua_State *L, int &idx) {
    const TValue *o = glm_i2v(L, idx++);
    if (FastPath || l_likely(ttisvector4(o)))
      return glm_v4value(o);
    else {
      gLuaBase::typeerror(L, idx - 1, Label());
      return glm::vec<4, T, Q>();
    }
  }
};

/// <summary>
/// Trait for glm::qua<T, Q> types.
/// </summary>
template<typename T, glm::qualifier Q, bool FastPath>
struct gLuaTrait<glm::qua<T, Q>, FastPath> : gLuaAbstractTrait<glm::qua<T, Q>> {
  template<typename Type = T>
  using as_type = gLuaTrait<glm::qua<Type, Q>>;  // @CastBinding
  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() { return LUAGLM_STRING_QUATERN; }

  LUA_BIND_QUALIFIER bool Is(lua_State *L, int idx) {
    const TValue *o = glm_i2v(L, idx);
    return ttisquat(o);
  }

  LUA_BIND_QUALIFIER glm::qua<T, Q> Next(lua_State *L, int &idx) {
    const TValue *o = glm_i2v(L, idx++);
    if (FastPath || l_likely(ttisquat(o)))
      return glm_qvalue(o);
    else {
      gLuaBase::typeerror(L, idx - 1, Label());
      return glm::qua<T, Q>();
    }
  }

  LUA_BIND_QUALIFIER int Push(const gLuaBase &LB, const glm::qua<T, Q> &q) {
    lua_LockScope _lock(LB.L);
    glm_setvvalue2s(LB.L->top, q, LUA_VQUAT);  // May use explicit copy constructor
    api_incr_top(LB.L);
    return 1;
  }
};

/// <summary>
/// Trait for glm::mat<C, R, T, Q> types.
///
/// @TODO: As matrix objects are mutable: column-vectors may be added/removed,
/// the matrix object being recycled, etc., lua_LockScope stubs *should* be
/// placed around all matrix object use.
/// </summary>
template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q, bool FastPath>
struct gLuaTrait<glm::mat<C, R, T, Q>, FastPath> : gLuaAbstractTrait<glm::mat<C, R, T, Q>> {
  template<typename Type = T>
  using as_type = gLuaTrait<glm::mat<C, R, Type, Q>>;  // @CastBinding

  /// <summary>
  /// Lua type trait equivalent to glm::mat<C, R, T, Q>::col_type
  /// </summary>
  using col_type = gLuaTrait<typename glm::mat<C, R, T, Q>::col_type>;

  /// <summary>
  /// Lua type trait equivalent to glm::mat<C, R, T, Q>::row_type
  /// </summary>
  using row_type = gLuaTrait<typename glm::mat<C, R, T, Q>::row_type>;

  /// <summary>
  /// Right-hand matrix-operation type.
  /// </summary>
  template<glm::length_t RNext>
  using rhs_mat_type = gLuaTrait<glm::mat<RNext, C, T, Q>>;

  /// <summary>
  /// Helpers for QR decomposition
  /// </summary>
  using q_type = gLuaTrait<glm::mat<(C < R ? C : R), R, T, Q>>;
  using r_type = gLuaTrait<glm::mat<C, (C < R ? C : R), T, Q>>;

  /// <summary>
  /// </summary>
  using gLuaAbstractTrait<glm::mat<C, R, T, Q>>::Zero;

  LUA_BIND_DECL const char *Label() {
    static const LUA_CONSTEXPR glm::length_t D = LUAGLM_MATRIX_TYPE(C, R);
    LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_2x2) return LUAGLM_STRING_MATRIX "2x2";
    LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_2x3) return LUAGLM_STRING_MATRIX "2x3";
    LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_2x4) return LUAGLM_STRING_MATRIX "2x4";
    LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_3x2) return LUAGLM_STRING_MATRIX "3x2";
    LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_3x3) return LUAGLM_STRING_MATRIX "3x3";
    LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_3x4) return LUAGLM_STRING_MATRIX "3x4";
    LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_4x2) return LUAGLM_STRING_MATRIX "4x2";
    LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_4x3) return LUAGLM_STRING_MATRIX "4x3";
    LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_4x4) return LUAGLM_STRING_MATRIX "4x4";
    return LUAGLM_STRING_MATRIX;
  }

  LUA_BIND_QUALIFIER bool Is(lua_State *L, int idx) {
    lua_LockScope _lock(L);
    const TValue *o = glm_i2v(L, idx);
    return ttismatrix(o) && mvalue_dims(o) == LUAGLM_MATRIX_TYPE(C, R);
  }

  LUA_BIND_DECL glm::mat<C, R, T, Q> Next(lua_State *L, int &idx) {
    lua_LockScope _lock(L);
    const TValue *o = glm_i2v(L, idx++);
    if (FastPath || l_likely(ttismatrix(o))) {
      // @TODO: LUA_IF_CONSTEXPR(std::is_same<T, glm_Float>::value && Q == LUAGLM_Q)
      // At the moment this relies on the compiler eliding this cast.
      const glmMatrix &mat = glm_mvalue(o);
      if (FastPath || l_likely(mat.dimensions == LUAGLM_MATRIX_TYPE(C, R))) {
        static const LUA_CONSTEXPR glm::length_t D = LUAGLM_MATRIX_TYPE(C, R);
        LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_2x2) return glm_mat_cast(mat.m22, C, R, T, Q);
        LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_2x3) return glm_mat_cast(mat.m23, C, R, T, Q);
        LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_2x4) return glm_mat_cast(mat.m24, C, R, T, Q);
        LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_3x2) return glm_mat_cast(mat.m32, C, R, T, Q);
        LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_3x3) return glm_mat_cast(mat.m33, C, R, T, Q);
        LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_3x4) return glm_mat_cast(mat.m34, C, R, T, Q);
        LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_4x2) return glm_mat_cast(mat.m42, C, R, T, Q);
        LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_4x3) return glm_mat_cast(mat.m43, C, R, T, Q);
        LUA_IF_CONSTEXPR(D == LUAGLM_MATRIX_4x4) return glm_mat_cast(mat.m44, C, R, T, Q);
      }
    }
    _lock.Unlock();
    gLuaBase::typeerror(L, idx - 1, Label());
    return glm::mat<C, R, T, Q>();
  }

  /// <summary>
  /// Required for '@GLMFix: The glm::mat<2, 3, ...>'
  /// </summary>
  template<bool ConsistentAlignment>
  LUA_BIND_DECL typename std::enable_if<ConsistentAlignment, int>::type PushAligned(gLuaBase &LB, const glm::mat<C, R, T, Q> &m) {
    if (LB.can_recycle()) {
      lua_LockScope _lock(LB.L);
      const TValue *o = glm_i2v(LB.L, LB.idx++);
      if (l_likely(ttismatrix(o))) {  // lua_pushvalue
        glm_mat_boundary(mvalue_ref(o)) = m;
        setobj2s(LB.L, LB.L->top, o);
        api_incr_top(LB.L);
        return 1;
      }
    }

#if defined(LUAGLM_FORCED_RECYCLE)  // This library allocating memory is verboten!
    return LUAGLM_ERROR(LB.L, "library configured to not allocate additional memory; use recycling mechanisms")
#else
    return glm_pushmat(LB.L, glmMatrix(m));
#endif
  }

  template<bool ConsistentAlignment>
  LUA_BIND_DECL typename std::enable_if<!ConsistentAlignment, int>::type PushAligned(gLuaBase &LB, const glm::mat<C, R, T, Q> &m) {
    return gLuaTrait<glm::mat<C, R, glm_Float, LUAGLM_Q>>::Push(LB, glm_mat_cast(m, C, R, glm_Float, LUAGLM_Q));
  }

  LUA_BIND_QUALIFIER int Push(gLuaBase &LB, const glm::mat<C, R, T, Q> &m) {
    return PushAligned<Q == LUAGLM_Q>(LB, m);
  }
};

/// <summary>
/// Specialization for epsilon arguments (and/or default arguments).
/// </summary>
template<typename T, bool FastPath, bool DefaultZero>
struct gEpsilon : gLuaTrait<T, FastPath> {
  using safe = gEpsilon<T, false, DefaultZero>;  // @SafeBinding
  using fast = gEpsilon<T, true, DefaultZero>;  // @UnsafeBinding
  using zero = gEpsilon<T, true, true>;  // @UnsafeBinding
  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() { return "epsilon"; }

  LUA_BIND_QUALIFIER bool Is(lua_State *L, int idx) {
    const TValue *o = glm_i2v(L, idx);
    return !_isvalid(L, o) || gLuaTrait<T>::Is(L, idx);
  }

  LUA_BIND_QUALIFIER T Next(lua_State *L, int &idx) {
    const TValue *o = glm_i2v(L, idx);
    if (!_isvalid(L, o)) {
      idx++;  // Skip the argument
      return DefaultZero ? T(0) : glm::epsilon<T>();
    }
    return gLuaTrait<T, FastPath>::Next(L, idx);
  }
};

/// <summary>
/// @GLMAssert: Specialization for traits bounded at zero. Usage of this trait
/// often reflects an assert clause within the GLM implementation.
/// </summary>
template<class Tr, bool Inclusive = true, bool IncludeEps = false>
struct gPositiveConstraint : gLuaTrait<typename Tr::type, false> {
  using safe = gPositiveConstraint<typename Tr::safe, Inclusive, IncludeEps>;  // @SafeBinding
  using fast = gPositiveConstraint<typename Tr::fast, Inclusive, IncludeEps>;  // @UnsafeBinding
  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() { return Tr::Label(); }

  LUA_BIND_QUALIFIER GLM_CONSTEXPR typename Tr::type Zero() { return Tr::Zero(); }
  LUA_BIND_QUALIFIER bool Is(lua_State *L, int idx) { return Tr::Is(L, idx); }
  LUA_BIND_QUALIFIER typename Tr::type Next(lua_State *L, int &idx) {
    using T = typename Tr::type;
    const T min = T(0) + (IncludeEps ? std::numeric_limits<typename Tr::value_type>::epsilon() : 0);

    const T value = Tr::Next(L, idx);
    const auto minValue = Inclusive ? glm::lessThanEqual(min, value) : glm::lessThan(min, value);
    if (!glm::all(minValue))
      gLuaBase::argerror(L, idx - 1, "argument not in range");
    return value;
  }
};

/// <summary>
/// @GLMAssert: Specialization for arguments between zero and one.
/// </summary>
template<class Tr, bool MinInclusive = true, bool MaxInclusive = true>
struct gRelativeConstraint : gLuaTrait<typename Tr::type, false> {
  using safe = gRelativeConstraint<typename Tr::safe, MinInclusive, MaxInclusive>;  // @SafeBinding
  using fast = gRelativeConstraint<typename Tr::fast, MinInclusive, MaxInclusive>;  // @UnsafeBinding
  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() { return Tr::Label(); }

  LUA_BIND_QUALIFIER GLM_CONSTEXPR typename Tr::type Zero() { return Tr::Zero(); }
  LUA_BIND_QUALIFIER bool Is(lua_State *L, int idx) { return Tr::Is(L, idx); }
  LUA_BIND_QUALIFIER typename Tr::type Next(lua_State *L, int &idx) {
    using T = typename Tr::type;
    const T value = Tr::Next(L, idx);
    const auto minValue = MinInclusive ? glm::lessThanEqual(T(0), value) : glm::lessThan(T(0), value);
    const auto maxValue = MaxInclusive ? glm::greaterThanEqual(T(1), value) : glm::greaterThan(T(1), value);
    if (!glm::all(minValue) || !glm::all(maxValue))
      gLuaBase::argerror(L, idx - 1, "argument not in range");
    return value;
  }
};

/// <summary>
/// @GLMAssert: Specialization that ensures values cannot be zero (fmod)
/// </summary>
template<class Tr>
struct gZeroConstraint : gLuaTrait<typename Tr::type, false> {
  using safe = gZeroConstraint<typename Tr::safe>;  // @SafeBinding
  using fast = gZeroConstraint<typename Tr::fast>;  // @UnsafeBinding
  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() { return Tr::Label(); }

  LUA_BIND_QUALIFIER GLM_CONSTEXPR typename Tr::type Zero() { return Tr::Zero(); }
  LUA_BIND_QUALIFIER bool Is(lua_State *L, int idx) { return Tr::Is(L, idx); }
  LUA_BIND_QUALIFIER typename Tr::type Next(lua_State *L, int &idx) {
    using T = typename Tr::type;
    const T value = Tr::Next(L, idx);
    if (std::is_integral<T>::value && glm::any(glm::equal(T(0), value)))
      gLuaBase::argerror(L, idx - 1, "zero");
    return value;
  }
};

// Specializations

using gLuaFloat = gLuaTrait<glm_Float>;
using gLuaNumber = gLuaTrait<lua_Number>;
using gLuaInteger = gLuaTrait<lua_Integer>;
using gLuaLen_t = gLuaTrait<glm::length_t>;

/// <summary>
/// gLuaNumberCompileTime; See @LUAGLM_NUMBER_ARGS.
/// </summary>
#if defined(LUAGLM_NUMBER_ARGS)
using gLuaNumCoT = gLuaNumber;
#else
using gLuaNumCoT = gLuaFloat;
#endif

template<typename T = glm_Float> using gLuaVec1 = gLuaTrait<glm::vec<1, T, LUAGLM_BINDING_QUAL>>;
template<typename T = glm_Float> using gLuaVec2 = gLuaTrait<glm::vec<2, T, LUAGLM_BINDING_QUAL>>;
template<typename T = glm_Float> using gLuaVec3 = gLuaTrait<glm::vec<3, T, LUAGLM_BINDING_QUAL>>;
template<typename T = glm_Float> using gLuaVec4 = gLuaTrait<glm::vec<4, T, LUAGLM_BINDING_QUAL>>;
template<typename T = glm_Float> using gLuaQuat = gLuaTrait<glm::qua<T, LUAGLM_BINDING_QUAL>>;

template<typename T = glm_Float> using gLuaMat2x2 = gLuaTrait<glm::mat<2, 2, T, LUAGLM_BINDING_QUAL>>;
template<typename T = glm_Float> using gLuaMat2x3 = gLuaTrait<glm::mat<2, 3, T, LUAGLM_BINDING_QUAL>>;
template<typename T = glm_Float> using gLuaMat2x4 = gLuaTrait<glm::mat<2, 4, T, LUAGLM_BINDING_QUAL>>;
template<typename T = glm_Float> using gLuaMat3x2 = gLuaTrait<glm::mat<3, 2, T, LUAGLM_BINDING_QUAL>>;
template<typename T = glm_Float> using gLuaMat3x3 = gLuaTrait<glm::mat<3, 3, T, LUAGLM_BINDING_QUAL>>;
template<typename T = glm_Float> using gLuaMat3x4 = gLuaTrait<glm::mat<3, 4, T, LUAGLM_BINDING_QUAL>>;
template<typename T = glm_Float> using gLuaMat4x2 = gLuaTrait<glm::mat<4, 2, T, LUAGLM_BINDING_QUAL>>;
template<typename T = glm_Float> using gLuaMat4x3 = gLuaTrait<glm::mat<4, 3, T, LUAGLM_BINDING_QUAL>>;
template<typename T = glm_Float> using gLuaMat4x4 = gLuaTrait<glm::mat<4, 4, T, LUAGLM_BINDING_QUAL>>;

/// <summary>
/// See @LUAGLM_DRIFT
/// </summary>
#if defined(LUAGLM_DRIFT)
template<class T, bool FastPath = false>
struct gNormalizeFilter : gLuaTrait<T, FastPath> {
  using safe = gNormalizeFilter<T, false>;  // @SafeBinding
  using fast = gNormalizeFilter<T, true>;  // @UnsafeBinding
  LUA_BIND_QUALIFIER T Next(lua_State *L_, int &idx) {
    return glm::normalize(gLuaTrait<T, FastPath>::Next(L_, idx));
  }
};
template<typename T = glm_Float> using gLuaDir2 = gNormalizeFilter<glm::vec<2, T, LUAGLM_BINDING_QUAL>>;
template<typename T = glm_Float> using gLuaDir3 = gNormalizeFilter<glm::vec<3, T, LUAGLM_BINDING_QUAL>>;
#else
template<typename T = glm_Float> using gLuaDir2 = gLuaTrait<glm::vec<2, T, LUAGLM_BINDING_QUAL>>;
template<typename T = glm_Float> using gLuaDir3 = gLuaTrait<glm::vec<3, T, LUAGLM_BINDING_QUAL>>;
#endif

/* }================================================================== */

/*
** {==================================================================
** Trait Bindings.
** ===================================================================
*/

/*
** Generic Traits Operation:
**  (1) Extract data from Lua stack and convert into some GLM suitable structure
**  (2) Execute the (bound) GLM function: "R = F(...)"
**  (3) Convert the function result back into something suitable for Lua and
**      pushing it onto the stack.
**
** @NOTE: Must consider the order-of-evaluation for function arguments.
*/

#define _VA_NARGS_GLUE(x, y) x y
#define _VA_NARGS_RETURN_COUNT(_1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, count, ...) count
#define _VA_NARGS_EXPAND(args) _VA_NARGS_RETURN_COUNT args
#define _VA_NARGS_COUNT_MAX(...) _VA_NARGS_EXPAND((__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
#define _VA_NARGS_COUNT 10

#define _VA_NARGS_OVERLOAD_MACRO2(Name, count) Name##count
#define _VA_NARGS_OVERLOAD_MACRO1(Name, count) _VA_NARGS_OVERLOAD_MACRO2(Name, count)
#define _VA_NARGS_OVERLOAD_MACRO(Name, count) _VA_NARGS_OVERLOAD_MACRO1(Name, count)
#define VA_CALL(Name, ...) _VA_NARGS_GLUE(_VA_NARGS_OVERLOAD_MACRO(Name, _VA_NARGS_COUNT_MAX(__VA_ARGS__)), (__VA_ARGS__))

/* Mapping Lua stack values to function parameters */
#define BIND_FUNC(...) VA_CALL(BIND_FUNC, __VA_ARGS__)
#define BIND_RESULT(LB, Result) return gLuaBase::Push(LB, (Result))

/* fail */
#define BIND_FUNC1(LB) \
  return gLuaBase::Push(LB)

/* F() */
#define BIND_FUNC2(LB, F) \
  BIND_RESULT(LB, F())

/* F(a) */
#define BIND_FUNC3(LB, F, A)          \
  LUA_MLM_BEGIN                       \
  BIND_RESULT(LB, F((LB).Next<A>())); \
  LUA_MLM_END

/* F(a, b) */
#define BIND_FUNC4(LB, F, A, B)      \
  LUA_MLM_BEGIN                      \
  const A::type _a = (LB).Next<A>(); \
  const B::type _b = (LB).Next<B>(); \
  BIND_RESULT(LB, F(_a, _b));        \
  LUA_MLM_END

/* F(a, b, c)) */
#define BIND_FUNC5(LB, F, A, B, C)   \
  LUA_MLM_BEGIN                      \
  const A::type _a = (LB).Next<A>(); \
  const B::type _b = (LB).Next<B>(); \
  const C::type _c = (LB).Next<C>(); \
  BIND_RESULT(LB, F(_a, _b, _c));    \
  LUA_MLM_END

/* F(a, b, c, d) */
#define BIND_FUNC6(LB, F, A, B, C, D) \
  LUA_MLM_BEGIN                       \
  const A::type _a = (LB).Next<A>();  \
  const B::type _b = (LB).Next<B>();  \
  const C::type _c = (LB).Next<C>();  \
  const D::type _d = (LB).Next<D>();  \
  BIND_RESULT(LB, F(_a, _b, _c, _d)); \
  LUA_MLM_END

/* F(a, b, c, d, e) */
#define BIND_FUNC7(LB, F, A, B, C, D, E)  \
  LUA_MLM_BEGIN                           \
  const A::type _a = (LB).Next<A>();      \
  const B::type _b = (LB).Next<B>();      \
  const C::type _c = (LB).Next<C>();      \
  const D::type _d = (LB).Next<D>();      \
  const E::type _e = (LB).Next<E>();      \
  BIND_RESULT(LB, F(_a, _b, _c, _d, _e)); \
  LUA_MLM_END

/* F(a, b, c, d, e, g) */
#define BIND_FUNC8(LB, F, A, B, C, D, E, G)   \
  LUA_MLM_BEGIN                               \
  const A::type _a = (LB).Next<A>();          \
  const B::type _b = (LB).Next<B>();          \
  const C::type _c = (LB).Next<C>();          \
  const D::type _d = (LB).Next<D>();          \
  const E::type _e = (LB).Next<E>();          \
  const G::type _g = (LB).Next<G>();          \
  BIND_RESULT(LB, F(_a, _b, _c, _d, _e, _g)); \
  LUA_MLM_END

/* F(a, b, c, d, e, g, h) */
#define BIND_FUNC9(LB, F, A, B, C, D, E, G, H)    \
  LUA_MLM_BEGIN                                   \
  const A::type _a = (LB).Next<A>();              \
  const B::type _b = (LB).Next<B>();              \
  const C::type _c = (LB).Next<C>();              \
  const D::type _d = (LB).Next<D>();              \
  const E::type _e = (LB).Next<E>();              \
  const G::type _g = (LB).Next<G>();              \
  const H::type _h = (LB).Next<H>();              \
  BIND_RESULT(LB, F(_a, _b, _c, _d, _e, _g, _h)); \
  LUA_MLM_END

/* F(a, b, c, d, e, g, h, i) */
#define BIND_FUNC10(LB, F, A, B, C, D, E, G, H, I)    \
  LUA_MLM_BEGIN                                       \
  const A::type _a = (LB).Next<A>();                  \
  const B::type _b = (LB).Next<B>();                  \
  const C::type _c = (LB).Next<C>();                  \
  const D::type _d = (LB).Next<D>();                  \
  const E::type _e = (LB).Next<E>();                  \
  const G::type _g = (LB).Next<G>();                  \
  const H::type _h = (LB).Next<H>();                  \
  const I::type _i = (LB).Next<I>();                  \
  BIND_RESULT(LB, F(_a, _b, _c, _d, _e, _g, _h, _i)); \
  LUA_MLM_END

/*
** Place values onto the Lua stack in an order-of-evaluation independent nature;
** returning the number of values placed onto the Lua stack.
*/
#define BIND_PUSH(...) VA_CALL(BIND_PUSH, __VA_ARGS__)
#define BIND_PUSH_V(LB, Result) gLuaBase::Push((LB), (Result))

#define BIND_PUSH1(LB) \
  return gLuaBase::Push((LB))

#define BIND_PUSH2(LB, A) \
  return BIND_PUSH_V((LB), (A))

#define BIND_PUSH3(LB, A, B)              \
  LUA_MLM_BEGIN                           \
  const int _aR = BIND_PUSH_V((LB), (A)); \
  const int _bR = BIND_PUSH_V((LB), (B)); \
  return _aR + _bR;                       \
  LUA_MLM_END

#define BIND_PUSH4(LB, A, B, C)           \
  LUA_MLM_BEGIN                           \
  const int _aR = BIND_PUSH_V((LB), (A)); \
  const int _bR = BIND_PUSH_V((LB), (B)); \
  const int _cR = BIND_PUSH_V((LB), (C)); \
  return _aR + _bR + _cR;                 \
  LUA_MLM_END

#define BIND_PUSH5(LB, A, B, C, D)        \
  LUA_MLM_BEGIN                           \
  const int _aR = BIND_PUSH_V((LB), (A)); \
  const int _bR = BIND_PUSH_V((LB), (B)); \
  const int _cR = BIND_PUSH_V((LB), (C)); \
  const int _dR = BIND_PUSH_V((LB), (D)); \
  return _aR + _bR + _cR + _dR;           \
  LUA_MLM_END

#define BIND_PUSH6(LB, A, B, C, D, E)     \
  LUA_MLM_BEGIN                           \
  const int _aR = BIND_PUSH_V((LB), (A)); \
  const int _bR = BIND_PUSH_V((LB), (B)); \
  const int _cR = BIND_PUSH_V((LB), (C)); \
  const int _dR = BIND_PUSH_V((LB), (D)); \
  const int _eR = BIND_PUSH_V((LB), (E)); \
  return _aR + _bR + _cR + _dR + _eR;     \
  LUA_MLM_END

/* }================================================================== */

/*
** {==================================================================
** GLM Argument Layouts
** ===================================================================
*/

/*
** Argument Layout: In most cases the first argument to a GLM function is
** sufficient in template argument deduction. Moreover, that argument, or type
** trait, is usually repeated.
**
** LAYOUT_*(LB, F, Traits, ...) defined by:
**    LB - the Lua stack iterator.
**    F - the function being wrapped.
**    Tr - the first/deducing argument trait.
**    ... - Any trailing traits (types) consistent across all templates of the
**          same glm::function.
*/

/* repetition */
#define LAYOUT_UNARY(LB, F, Tr, ...) VA_CALL(BIND_FUNC, LB, F, Tr, ##__VA_ARGS__)
#define LAYOUT_BINARY(LB, F, Tr, ...) VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, ##__VA_ARGS__)
#define LAYOUT_TERNARY(LB, F, Tr, ...) VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::safe, ##__VA_ARGS__)
#define LAYOUT_QUATERNARY(LB, F, Tr, ...) VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::safe, Tr::safe, ##__VA_ARGS__)
#define LAYOUT_QUINARY(LB, F, Tr, ...) VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::safe, Tr::safe, Tr::safe, ##__VA_ARGS__)
#define LAYOUT_SENARY(LB, F, Tr, ...) VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::safe, Tr::safe, Tr::safe, Tr::safe, ##__VA_ARGS__)

/* trait + eps op */
#define LAYOUT_BINARY_EPS(LB, F, Tr, ...) \
  VA_CALL(BIND_FUNC, LB, F, Tr, Tr::eps_trait, ##__VA_ARGS__)

/* trait + trait::value_trait op */
#define LAYOUT_BINARY_SCALAR(LB, F, Tr, ...) \
  VA_CALL(BIND_FUNC, LB, F, Tr, Tr::value_trait, ##__VA_ARGS__)

/* trait + trait<int> op */
#define LAYOUT_BINARY_AS_INT(LB, F, Tr, ...) \
  VA_CALL(BIND_FUNC, LB, F, Tr, Tr::as_type<int>, ##__VA_ARGS__)

/* trait + trait + eps op */
#define LAYOUT_TERNARY_EPS(LB, F, Tr, ...) \
  VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::eps_trait, ##__VA_ARGS__)

/* trait + trait + trait::value_trait op */
#define LAYOUT_TERNARY_SCALAR(LB, F, Tr, ...) \
  VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::value_trait, ##__VA_ARGS__)

/* trait + trait + trait + trait + trait::value_trait op */
#define LAYOUT_QUINARY_SCALAR(LB, F, Tr, ...) \
  VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::safe, Tr::safe, Tr::value_trait, ##__VA_ARGS__)

/* trait + trait + trait + trait::value_trait + trait::value_trait op */
#define LAYOUT_BARYCENTRIC(LB, F, Tr, ...) \
  VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::safe, Tr::value_trait, Tr::value_trait, ##__VA_ARGS__)

/* unary or binary operator depending on the state of the Lua stack */
#define LAYOUT_UNARY_OR_BINARY(LB, F, Tr, ...)              \
  LUA_MLM_BEGIN                                             \
  if ((LB).Is<Tr>(Tr::stack_size))                          \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, ##__VA_ARGS__); \
  VA_CALL(BIND_FUNC, LB, F, Tr, ##__VA_ARGS__);             \
  LUA_MLM_END

/* trait + {nil || trait::value_trait} op */
#define LAYOUT_UNARY_OPTIONAL(LB, F, Tr, ...)                    \
  LUA_MLM_BEGIN                                                  \
  if (gLuaBase::isnoneornil((LB).L, (LB).idx + Tr::stack_size))  \
    VA_CALL(BIND_FUNC, LB, F, Tr, ##__VA_ARGS__);                \
  VA_CALL(BIND_FUNC, LB, F, Tr, Tr::value_trait, ##__VA_ARGS__); \
  LUA_MLM_END

/* unary or ternary operator depending on state of Lua stack */
#define LAYOUT_UNARY_OR_TERNARY(LB, F, Tr, ...)                     \
  LUA_MLM_BEGIN                                                     \
  if (gLuaBase::isnoneornil((LB).L, (LB).idx + Tr::stack_size))     \
    VA_CALL(BIND_FUNC, LB, F, Tr, ##__VA_ARGS__);                   \
  VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::safe, ##__VA_ARGS__); \
  LUA_MLM_END

/* trait + {trait || trait::value_trait} op */
#define LAYOUT_BINARY_OPTIONAL(LB, F, Tr, ...)                     \
  LUA_MLM_BEGIN                                                    \
  if ((LB).Is<Tr::value_trait>(Tr::stack_size))                    \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::value_trait, ##__VA_ARGS__); \
  VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, ##__VA_ARGS__);          \
  LUA_MLM_END

/* trait + trait + {trait || trait::value_trait} op */
#define LAYOUT_TERNARY_OPTIONAL(LB, F, Tr, ...)                              \
  LUA_MLM_BEGIN                                                              \
  if (LB.Is<Tr::value_trait>(2 * Tr::stack_size))                            \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::value_trait, ##__VA_ARGS__); \
  VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::safe, ##__VA_ARGS__);          \
  LUA_MLM_END

/* A binary integer layout that sanitizes the second argument (division/modulo zero) */
#define LAYOUT_MODULO(LB, F, Tr, ...)                                               \
  LUA_MLM_BEGIN                                                                     \
  if (LB.Is<Tr::value_trait>(Tr::stack_size))                                       \
    VA_CALL(BIND_FUNC, LB, F, Tr, gZeroConstraint<Tr::value_trait>, ##__VA_ARGS__); \
  VA_CALL(BIND_FUNC, LB, F, Tr, gZeroConstraint<Tr::safe>, ##__VA_ARGS__);          \
  LUA_MLM_END

/* }================================================================== */

/*
** {==================================================================
** GLM Argument Parsers
** ===================================================================
*/

/* Invalid glm structure configurations */
#define GLM_INVALID_MAT_DIMENSIONS ("invalid " LUAGLM_STRING_MATRIX " dimensions")
#define LUAGLM_ARG_ERROR(L, I, S) (gLuaBase::argerror((L), (I), (S)), 0)
#define LUAGLM_TYPE_ERROR(L, I, S) (gLuaBase::typeerror((L), (I), (S)), 0)
#define LUAGLM_ERROR(L, S) (gLuaBase::error((L), (S)), 0)

/*
** Generalized vector parser.
**
** @NOTE: Due to the nature of storing most/all data as floating point types,
**  bitfield operations on vectors may be inconsistent with float -> int -> float
**  casting. Therefore, all INTEGER_VECTOR definitions are considered unsafe
**  when the function isn't explicitly operating on lua_Integer types.
*/
#define PARSE_VECTOR_TYPE(LB, F, IType, FType, VType, ILayout, FLayout, VLayout, ...)     \
  LUA_MLM_BEGIN                                                                           \
  switch (ttypetag((LB).i2v())) {                                                         \
    case LUA_VFALSE: case LUA_VTRUE: /* @BoolCoercion */                                  \
    case LUA_VNUMINT: ILayout(LB, F, gLuaTrait<IType>, ##__VA_ARGS__); break;             \
    case LUA_VSHRSTR: case LUA_VLNGSTR: /* @StringCoercion */                             \
    case LUA_VNUMFLT: FLayout(LB, F, gLuaTrait<FType>, ##__VA_ARGS__); break;             \
    case LUA_VVECTOR2: VLayout(LB, F, gLuaVec2<VType>::fast, ##__VA_ARGS__); break;       \
    case LUA_VVECTOR3: VLayout(LB, F, gLuaVec3<VType>::fast, ##__VA_ARGS__); break;       \
    case LUA_VVECTOR4: VLayout(LB, F, gLuaVec4<VType>::fast, ##__VA_ARGS__); break;       \
    default:                                                                              \
      break;                                                                              \
  }                                                                                       \
  return LUAGLM_TYPE_ERROR((LB).L, (LB).idx, LUAGLM_STRING_NUMBER " or " LUAGLM_STRING_VECTOR); \
  LUA_MLM_END

/* Vector definition where the lua_Number operation takes priority */
#define PARSE_NUMBER_VECTOR(LB, F, FLayout, VLayout, ...) \
  PARSE_VECTOR_TYPE(LB, F, gLuaNumber::value_type, gLuaNumber::value_type, gLuaVec3<>::value_type, FLayout, FLayout, VLayout, ##__VA_ARGS__)

/* Vector definition where lua_Integer and lua_Number operations takes priority */
#define PARSE_INTEGER_NUMBER_VECTOR(LB, F, ILayout, FLayout, VLayout, ...) \
  PARSE_VECTOR_TYPE(LB, F, gLuaInteger::value_type, gLuaNumber::value_type, gLuaVec3<>::value_type, ILayout, FLayout, VLayout, ##__VA_ARGS__)

/* glm::function defined over the vector & quaternion space: vec1, vec2, vec3, vec4, quat */
#define PARSE_NUMBER_VECTOR_QUAT(LB, F, FLayout, VLayout, QLayout, ...)                    \
  LUA_MLM_BEGIN                                                                            \
  switch (ttypetag((LB).i2v())) {                                                          \
    case LUA_VFALSE: case LUA_VTRUE: /* @BoolCoercion */                                   \
    case LUA_VSHRSTR: case LUA_VLNGSTR: /* @StringCoercion */                              \
    case LUA_VNUMINT: /* @IntCoercion */                                                   \
    case LUA_VNUMFLT: FLayout(LB, F, gLuaNumber, ##__VA_ARGS__); break;                    \
    case LUA_VVECTOR2: VLayout(LB, F, gLuaVec2<>::fast, ##__VA_ARGS__); break;             \
    case LUA_VVECTOR3: VLayout(LB, F, gLuaVec3<>::fast, ##__VA_ARGS__); break;             \
    case LUA_VVECTOR4: VLayout(LB, F, gLuaVec4<>::fast, ##__VA_ARGS__); break;             \
    case LUA_VQUAT: QLayout(LB, F, gLuaQuat<>::fast, ##__VA_ARGS__); break;                \
    default:                                                                               \
      break;                                                                               \
  }                                                                                        \
  return LUAGLM_TYPE_ERROR((LB).L, (LB).idx, LUAGLM_STRING_VECTOR " or " LUAGLM_STRING_QUATERN); \
  LUA_MLM_END

/* A GLM function that defined over any NxM matrix */
#define PARSE_MATRIX(LB, MatrixType, F, ArgLayout, ...)                                 \
  LUA_MLM_BEGIN                                                                         \
  switch (MatrixType) {                                                                 \
    case LUAGLM_MATRIX_2x2: ArgLayout(LB, F, gLuaMat2x2<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_2x3: ArgLayout(LB, F, gLuaMat2x3<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_2x4: ArgLayout(LB, F, gLuaMat2x4<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_3x2: ArgLayout(LB, F, gLuaMat3x2<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_3x3: ArgLayout(LB, F, gLuaMat3x3<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_3x4: ArgLayout(LB, F, gLuaMat3x4<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_4x2: ArgLayout(LB, F, gLuaMat4x2<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_4x3: ArgLayout(LB, F, gLuaMat4x3<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_4x4: ArgLayout(LB, F, gLuaMat4x4<>::fast, ##__VA_ARGS__); break; \
    default:                                                                            \
      return LUAGLM_TYPE_ERROR((LB).L, (LB).idx, GLM_INVALID_MAT_DIMENSIONS);           \
  }                                                                                     \
  LUA_MLM_END

/* A GLM function that operates only on NxN matrices */
#define PARSE_SYMMETRIC_MATRIX(LB, MatrixType, F, ArgLayout, ...)                       \
  LUA_MLM_BEGIN                                                                         \
  switch (MatrixType) {                                                                 \
    case LUAGLM_MATRIX_2x2: ArgLayout(LB, F, gLuaMat2x2<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_3x3: ArgLayout(LB, F, gLuaMat3x3<>::fast, ##__VA_ARGS__); break; \
    case LUAGLM_MATRIX_4x4: ArgLayout(LB, F, gLuaMat4x4<>::fast, ##__VA_ARGS__); break; \
    default:                                                                            \
      return LUAGLM_TYPE_ERROR((LB).L, (LB).idx, GLM_INVALID_MAT_DIMENSIONS);           \
  }                                                                                     \
  LUA_MLM_END

/*
** a GLM function that operates on rotation matrices. These operations are
** intended to be an analogue to generic (Affine)Transform wrappers that
** abstract away TRS vs. explicit matrix representation etc.
**
** All geometric objects must support multiplication operations for quats,
** mat3x3, mat3x4, mat4x3, and mat4x4.
*/
#define PARSE_ROTATION_MATRIX(LB, F, ArgLayout, ...)                                        \
  LUA_MLM_BEGIN                                                                             \
  const TValue *_tv = (LB).i2v();                                                           \
  switch (ttypetag(_tv)) {                                                                  \
    case LUA_VQUAT: ArgLayout(LB, F, gLuaQuat<>, ##__VA_ARGS__); break;                     \
    case LUA_VMATRIX: {                                                                     \
      switch (mvalue_dims(_tv)) {                                                           \
        case LUAGLM_MATRIX_3x3: ArgLayout(LB, F, gLuaMat3x3<>::fast, ##__VA_ARGS__); break; \
        case LUAGLM_MATRIX_3x4: ArgLayout(LB, F, gLuaMat3x4<>::fast, ##__VA_ARGS__); break; \
        case LUAGLM_MATRIX_4x3: ArgLayout(LB, F, gLuaMat4x3<>::fast, ##__VA_ARGS__); break; \
        case LUAGLM_MATRIX_4x4: ArgLayout(LB, F, gLuaMat4x4<>::fast, ##__VA_ARGS__); break; \
        default:                                                                            \
          return LUAGLM_TYPE_ERROR((LB).L, (LB).idx, GLM_INVALID_MAT_DIMENSIONS);           \
      }                                                                                     \
    }                                                                                       \
    default:                                                                                \
      break;                                                                                \
  }                                                                                         \
  return LUAGLM_TYPE_ERROR((LB).L, (LB).idx, LUAGLM_STRING_QUATERN " or " LUAGLM_STRING_MATRIX); \
  LUA_MLM_END

/* }================================================================== */

/*
** {==================================================================
** Binding Function Declarations
** ===================================================================
*/

/* Lua library function name. */
#define GLM_NAME(F) glm_##F

/* Common function declaration for all Lua-binded GLM functions. */
#define GLM_BINDING_QUALIFIER(NAME) static int GLM_NAME(NAME)(lua_State *L)

/*
** Exception handling wrapper for generic API functions.
**
** Generic try/catch blocks are used to avoid any lingering std::logic_error and
** std::runtime_error exceptions that could be thrown by GLM.
*/
#if defined(LUAGLM_SAFELIB)
  #include <exception>
  #define GLM_BINDING_BEGIN     \
    gLuaBase LB(L);             \
    const int _stop = LB.top(); \
    try {

  #define GLM_BINDING_END                 \
    }                                     \
    catch (const std::exception &e) {     \
      lua_settop(L, _stop);               \
      lua_pushstring(L, e.what());        \
    }                                     \
    catch (...) {                         \
      lua_settop(L, _stop);               \
      lua_pushstring(L, "GLM exception"); \
    }                                     \
    return lua_error(L);
#else
  #define GLM_BINDING_BEGIN gLuaBase LB(L);
  #define GLM_BINDING_END
#endif

/* GLM function that with one (unique) set of function parameters */
#define BIND_DEFN(Name, F, ...)               \
  GLM_BINDING_QUALIFIER(Name) {               \
    GLM_BINDING_BEGIN                         \
    VA_CALL(BIND_FUNC, LB, F, ##__VA_ARGS__); \
    GLM_BINDING_END                           \
  }

/* A GLM function where the first argument (Tr) is sufficient in template argument deduction */
#define LAYOUT_DEFN(Name, F, ArgLayout, Tr, ...) \
  GLM_BINDING_QUALIFIER(Name) {                  \
    GLM_BINDING_BEGIN                            \
    ArgLayout(LB, F, Tr, ##__VA_ARGS__);         \
    GLM_BINDING_END                              \
  }

/*
** A operation defined for two traits; often 2D/3D or 3D/4D vectors.
**
** Allows @UnsafeBinding when A/B are non-coerced types.
*/
#define BINARY_LAYOUT_DEFN(Name, F, ArgLayout, A, B, ...)                 \
  GLM_BINDING_QUALIFIER(Name) {                                           \
    GLM_BINDING_BEGIN                                                     \
    if (LB.Is<A>()) ArgLayout(LB, F, A, ##__VA_ARGS__);                   \
    if (LB.Is<B>()) ArgLayout(LB, F, B, ##__VA_ARGS__);                   \
    return luaL_error(LB.L, "%s or %s expected", A::Label(), B::Label()); \
    GLM_BINDING_END                                                       \
  }

/* Vector definition where the lua_Number operation takes priority */
#define NUMBER_VECTOR_DEFN(Name, F, ArgLayout, ...)                  \
  GLM_BINDING_QUALIFIER(Name) {                                      \
    GLM_BINDING_BEGIN                                                \
    PARSE_NUMBER_VECTOR(LB, F, ArgLayout, ArgLayout, ##__VA_ARGS__); \
    GLM_BINDING_END                                                  \
  }

/* Vector definition where lua_Integer and lua_Number operations takes priority */
#define INTEGER_NUMBER_VECTOR_DEFN(Name, F, ArgLayout, ...)                             \
  GLM_BINDING_QUALIFIER(Name) {                                                         \
    GLM_BINDING_BEGIN                                                                   \
    PARSE_INTEGER_NUMBER_VECTOR(LB, F, ArgLayout, ArgLayout, ArgLayout, ##__VA_ARGS__); \
    GLM_BINDING_END                                                                     \
  }

#define INTEGER_NUMBER_VECTOR_DEFNS(Name, F, ILayout, FLayout, VLayout, ...)      \
  GLM_BINDING_QUALIFIER(Name) {                                                   \
    GLM_BINDING_BEGIN                                                             \
    PARSE_INTEGER_NUMBER_VECTOR(LB, F, ILayout, FLayout, VLayout, ##__VA_ARGS__); \
    GLM_BINDING_END                                                               \
  }

/* glm::function defined over the vector & quaternion space: vec1, vec2, vec3, vec4, quat */
#define NUMBER_VECTOR_QUAT_DEFN(Name, F, ArgLayout, ...)                             \
  GLM_BINDING_QUALIFIER(Name) {                                                      \
    GLM_BINDING_BEGIN                                                                \
    PARSE_NUMBER_VECTOR_QUAT(LB, F, ArgLayout, ArgLayout, ArgLayout, ##__VA_ARGS__); \
    GLM_BINDING_END                                                                  \
  }

#define NUMBER_VECTOR_QUAT_DEFNS(Name, F, FLayout, VLayout, QLayout, ...)      \
  GLM_BINDING_QUALIFIER(Name) {                                                \
    GLM_BINDING_BEGIN                                                          \
    PARSE_NUMBER_VECTOR_QUAT(LB, F, FLayout, VLayout, QLayout, ##__VA_ARGS__); \
    GLM_BINDING_END                                                            \
  }

/* A glm::function that defined over any NxM matrix */
#define MATRIX_DEFN(Name, F, ArgLayout, ...)                         \
  GLM_BINDING_QUALIFIER(Name) {                                      \
    GLM_BINDING_BEGIN                                                \
    const TValue *o = LB.i2v();                                      \
    if (l_likely(ttismatrix(o)))                                     \
      PARSE_MATRIX(LB, mvalue_dims(o), F, ArgLayout, ##__VA_ARGS__); \
    return LUAGLM_TYPE_ERROR(LB.L, LB.idx, LUAGLM_STRING_MATRIX);    \
    GLM_BINDING_END                                                  \
  }

/* A glm::function that operates only on NxN matrices */
#define SYMMETRIC_MATRIX_DEFN(Name, F, ArgLayout, ...)                         \
  GLM_BINDING_QUALIFIER(Name) {                                                \
    GLM_BINDING_BEGIN                                                          \
    const TValue *o = LB.i2v();                                                \
    if (l_likely(ttismatrix(o)))                                               \
      PARSE_SYMMETRIC_MATRIX(LB, mvalue_dims(o), F, ArgLayout, ##__VA_ARGS__); \
    return LUAGLM_TYPE_ERROR((LB).L, (LB).idx, LUAGLM_STRING_SYMMATRIX);       \
    GLM_BINDING_END                                                            \
  }

/* a glm::function that operates on rotation matrices */
#define ROTATION_MATRIX_DEFN(Name, F, ArgLayout, ...)       \
  GLM_BINDING_QUALIFIER(Name) {                             \
    GLM_BINDING_BEGIN                                       \
    PARSE_ROTATION_MATRIX(LB, F, ArgLayout, ##__VA_ARGS__); \
    GLM_BINDING_END                                         \
  }

/* @TODO: Allow different specializations for lua_Number and vec2/3/4 */
/* Generalized int16_t, int32_t, etc. function definition */
#define INTEGER_VECTOR_DEFN(Name, F, IType, ArgLayout, ...)                                        \
  GLM_BINDING_QUALIFIER(Name) {                                                                    \
    GLM_BINDING_BEGIN                                                                              \
    PARSE_VECTOR_TYPE(LB, F, IType, IType, IType, ArgLayout, ArgLayout, ArgLayout, ##__VA_ARGS__); \
    GLM_BINDING_END                                                                                \
  }

/* }================================================================== */

/*
** {==================================================================
** GLM Layout Helpers
** ===================================================================
*/

/* @COMPAT: max ULPs parameters for scalar numbers introduced in 0.9.9.3 */
#if LUAGLM_INCLUDE_IEEE && GLM_VERSION >= 993
  #define LAYOUT_EQUAL_ULPS(LB, F, A, B, TVal) \
    BIND_RESULT((LB), F((A), (B), gLuaBase::narrow_cast<int>((LB).L, ivalue(TVal))))
#else
  #define LAYOUT_EQUAL_ULPS(LB, F, A, B, TVal) ((void)0)
#endif

/*
** Generic equals layout
**
** Allows @UnsafeBinding when Tr_Row is a non-coerced type.
*/
#define LAYOUT_GENERIC_EQUAL(LB, F, Tr, Tr_Row)                 \
  LUA_MLM_BEGIN                                                 \
  const Tr::type _a = (LB).Next<Tr>();                          \
  const Tr::safe::type _b = (LB).Next<Tr::safe>();              \
  const TValue *_tv3 = (LB).i2v();                              \
  if (ttisfloat(_tv3)) /* <Tr, Tr, eps> */                      \
    BIND_RESULT(LB, F(_a, _b, Tr::value_type(fltvalue(_tv3)))); \
  else if (ttisinteger(_tv3)) /* <Tr, Tr, ULPs> */              \
    LAYOUT_EQUAL_ULPS(LB, F, _a, _b, _tv3);                     \
  else if ((LB).Is<Tr_Row>()) /* <Tr, Tr, vec> */               \
    BIND_RESULT(LB, F(_a, _b, (LB).Next<Tr_Row>()));            \
  BIND_RESULT(LB, F(_a, _b));                                   \
  LUA_MLM_END

/* }================================================================== */

#endif
