/*
** $Id: lglmlib.cpp $
** GLM binding library
**
** Porting GLM-specific code between luaglm and cpp should be painless (or as
** painless as possible). With key differences being:
**  1. Static versus Dynamic typing (or 'auto' versus 'local' syntax);
**  2. Namespace resolution (::) versus table access (.) syntax. However, the
**    'LUAGLM_EXT_SCOPE_RESOLUTION' build option removes this case at the cost
**     of 'goto' no longer being a language feature.
**  3. Constants in luaglm are stored by value, e.g. glm.pi. However in cpp,
**    they are templated constant expressions, e.g., glm::pi<float>().
**  4. Floating-point literals (1.0f) vs. lua_Number (1.0).
**
** See Copyright Notice in lua.h
*/

#define lglmlib_cpp
#if !defined(LUA_LIB)
  #define LUA_LIB
#endif

#include <algorithm>
#include <functional>

#include <lua.hpp>
#include <lglm.hpp>
#include "lglmlib.hpp"

#include "api.hpp"
#if defined(LUAGLM_INCLUDE_GEOM)
  #include "geom.hpp"
#endif

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#if GLM_VERSION >= 996 /* @COMPAT: Added in 0.9.9.6 */
  #include <glm/ext/scalar_integer.hpp>
#endif
#if GLM_VERSION >= 991 /* @COMPAT: Changed to ext/scale_constants in 0.9.9.1 */
  #include <glm/ext/scalar_constants.hpp>
#endif

/* @TEMP: The GLM library will be force-installed by default */
#if !defined(LUAGLM_INSTALL_METATABLES)
  #define LUAGLM_INSTALL_METATABLES
#endif

/*
** {==================================================================
** Library
** ===================================================================
*/

/* Create a "{ function, glm_function }" registry instance */
#define REG_STR(C) #C
#define GLM_LUA_REG(NAME) \
  { "" REG_STR(NAME), GLM_NAME(NAME) }

/* glm::Name */
#define GLM_NAMESPACE(NAME) glm::NAME

/* Set a GLM constant value into the table on top of the stack. */
#define GLM_CONSTANT(L, Name)                             \
  LUA_MLM_BEGIN                                           \
  lua_pushnumber((L), GLM_NAMESPACE(Name)<lua_Number>()); \
  lua_setfield((L), -2, "" REG_STR(Name));                \
  LUA_MLM_END

/// <summary>
/// Pushes onto the stack the value GLM[k], where GLM is the binding library
/// stored as an upvalue to this metamethod.
/// </summary>
static int glm_libraryindex(lua_State *L) {
  lua_settop(L, 2);
  if (lua_rawget(L, lua_upvalueindex(1)) != LUA_TFUNCTION) {  // Only functions can be accessed
    lua_pop(L, 1);
    lua_pushnil(L);
  }
  return 1;
}

#if defined(LUAGLM_INCLUDE_GEOM)
/// <summary>
/// Helper function for creating meta/library tables.
/// </summary>
static void glm_newmetatable(lua_State *L, const char *meta_name, const char *lib_name, const luaL_Reg *lib) {
  if (luaL_newmetatable(L, meta_name)) {
    luaL_setfuncs(L, lib, 0);
    lua_setfield(L, -2, lib_name);  // Push the library into the GLM library table
  }
  else {
    lua_pop(L, 1);
  }
}
#endif

static const luaL_Reg luaglm_lib[] = {
  /* API */
  #include "lglmlib_reg.hpp"
  /* Lua mathlib */
  { "type", GLM_NULLPTR },
  { "random", GLM_NULLPTR },
  { "randomseed", GLM_NULLPTR },
  { "pi", GLM_NULLPTR },
  { "tau", GLM_NULLPTR },
  { "eps", GLM_NULLPTR },
  { "feps", GLM_NULLPTR },
  { "maxinteger", GLM_NULLPTR },
  { "mininteger", GLM_NULLPTR },
  { "huge", GLM_NULLPTR },
  { "FP_INFINITE", GLM_NULLPTR },
  { "FP_NAN", GLM_NULLPTR },
  { "FP_ZERO", GLM_NULLPTR },
  { "FP_SUBNORMAL", GLM_NULLPTR },
  { "FP_NORMAL", GLM_NULLPTR },
  /* Constants */
#if defined(CONSTANTS_HPP) || defined(EXT_SCALAR_CONSTANTS_HPP)
  #if GLM_VERSION >= 997  // @COMPAT: Added in 0.9.9.7
  { "cos_one_over_two", GLM_NULLPTR },
  #endif
  { "e", GLM_NULLPTR },
  { "epsilon", GLM_NULLPTR },
  { "euler", GLM_NULLPTR },
  { "four_over_pi", GLM_NULLPTR },
  { "golden_ratio", GLM_NULLPTR },
  { "half_pi", GLM_NULLPTR },
  { "ln_ln_two", GLM_NULLPTR },
  { "ln_ten", GLM_NULLPTR },
  { "ln_two", GLM_NULLPTR },
  { "one", GLM_NULLPTR },
  { "one_over_pi", GLM_NULLPTR },
  { "one_over_root_two", GLM_NULLPTR },
  { "one_over_two_pi", GLM_NULLPTR },
  //{ "pi", GLM_NULLPTR },
  { "quarter_pi", GLM_NULLPTR },
  { "root_five", GLM_NULLPTR },
  { "root_half_pi", GLM_NULLPTR },
  { "root_ln_four", GLM_NULLPTR },
  { "root_pi", GLM_NULLPTR },
  { "root_three", GLM_NULLPTR },
  { "root_two", GLM_NULLPTR },
  { "root_two_pi", GLM_NULLPTR },
  { "third", GLM_NULLPTR },
  { "three_over_two_pi", GLM_NULLPTR },
  { "two_over_pi", GLM_NULLPTR },
  { "two_over_root_pi", GLM_NULLPTR },
  { "two_pi", GLM_NULLPTR },
  { "two_thirds", GLM_NULLPTR },
  { "zero", GLM_NULLPTR },
  { "epsilon", GLM_NULLPTR },
#endif
  /* Common unicode labels */
  { "subscript_minus", GLM_NULLPTR },
  { "subscript_one", GLM_NULLPTR },
  { "subscript_two", GLM_NULLPTR },
  { "subscript_three", GLM_NULLPTR },
  { "interpunct", GLM_NULLPTR },
  { "unit_velocity", GLM_NULLPTR },
  { "unit_angular_velocity", GLM_NULLPTR },
  /* Metamethods */
  { "__index", GLM_NULLPTR },
  /* RNG API */
  { "distribution", GLM_NULLPTR },
  /* Geometry API */
#if defined(LUAGLM_INCLUDE_GEOM)
  { "aabb", GLM_NULLPTR },
  { "line", GLM_NULLPTR },
  { "ray", GLM_NULLPTR },
  { "triangle", GLM_NULLPTR },
  { "segment", GLM_NULLPTR },
  { "sphere", GLM_NULLPTR },
  { "plane", GLM_NULLPTR },
  { "polygon", GLM_NULLPTR },
  { "aabb2d", GLM_NULLPTR },
  { "segment2d", GLM_NULLPTR },
  { "circle", GLM_NULLPTR },
#endif
  /* Library Details */
  { "_NAME", GLM_NULLPTR },
  { "_VERSION", GLM_NULLPTR },
  { "_COPYRIGHT", GLM_NULLPTR },
  { "_DESCRIPTION", GLM_NULLPTR },
  { "_GLM_VERSION", GLM_NULLPTR },
  { "_GLM_SIMD", GLM_NULLPTR },
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* Functions with math.rand() upvalue */
static const luaL_Reg luaglm_randfuncs[] = {
#if defined(STD_RANDOM) && GLM_HAS_CXX11_STL
  GLM_LUA_REG(uniform_int),
  GLM_LUA_REG(uniform_real),
  GLM_LUA_REG(bernoulli),
  GLM_LUA_REG(binomial),
  GLM_LUA_REG(negative_binomial),
  GLM_LUA_REG(geometric),
  GLM_LUA_REG(poisson),
  GLM_LUA_REG(exponential),
  GLM_LUA_REG(gamma),
  GLM_LUA_REG(weibull),
  GLM_LUA_REG(extreme_value),
  GLM_LUA_REG(normal),
  GLM_LUA_REG(lognormal),
  GLM_LUA_REG(chi_squared),
  GLM_LUA_REG(cauchy),
  GLM_LUA_REG(fisher_f),
  GLM_LUA_REG(student_t),
#endif
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* Functions with a lib-glm upvalue */
static const luaL_Reg luaglm_metamethods[] = {
  { "__index", glm_libraryindex },
  { GLM_NULLPTR, GLM_NULLPTR }
};

extern "C" {
  LUAMOD_API int luaopen_glm(lua_State *L) {
    luaL_newlib(L, luaglm_lib);  // Initialize GLM library
#if defined(LUAGLM_INCLUDE_GEOM)
    luaL_newlib(L, luaglm_aabblib); lua_setfield(L, -2, "aabb");
    luaL_newlib(L, luaglm_linelib); lua_setfield(L, -2, "line");
    luaL_newlib(L, luaglm_raylib); lua_setfield(L, -2, "ray");
    luaL_newlib(L, luaglm_trianglelib); lua_setfield(L, -2, "triangle");
    luaL_newlib(L, luaglm_segmentlib); lua_setfield(L, -2, "segment");
    luaL_newlib(L, luaglm_spherelib); lua_setfield(L, -2, "sphere");
    luaL_newlib(L, luaglm_planelib); lua_setfield(L, -2, "plane");
    luaL_newlib(L, luaglm_aabb2dlib); lua_setfield(L, -2, "aabb2d");
    luaL_newlib(L, luaglm_segment2dlib); lua_setfield(L, -2, "segment2d");
    luaL_newlib(L, luaglm_circlelib); lua_setfield(L, -2, "circle");
    // The "polygon" API doubles as the polygon metatable stored in the registry.
    glm_newmetatable(L, gLuaPolygon<>::Metatable(), "polygon", luaglm_polylib);
#endif
#if defined(CONSTANTS_HPP) || defined(EXT_SCALAR_CONSTANTS_HPP)
  #if GLM_VERSION >= 997  // @COMPAT: Added in 0.9.9.7
    GLM_CONSTANT(L, cos_one_over_two);
  #endif
    GLM_CONSTANT(L, e);
    GLM_CONSTANT(L, epsilon);
    GLM_CONSTANT(L, euler);
    GLM_CONSTANT(L, four_over_pi);
    GLM_CONSTANT(L, golden_ratio);
    GLM_CONSTANT(L, half_pi);
    GLM_CONSTANT(L, ln_ln_two);
    GLM_CONSTANT(L, ln_ten);
    GLM_CONSTANT(L, ln_two);
    GLM_CONSTANT(L, one);
    GLM_CONSTANT(L, one_over_pi);
    GLM_CONSTANT(L, one_over_root_two);
    GLM_CONSTANT(L, one_over_two_pi);
    GLM_CONSTANT(L, quarter_pi);
    GLM_CONSTANT(L, root_five);
    GLM_CONSTANT(L, root_half_pi);
    GLM_CONSTANT(L, root_ln_four);
    GLM_CONSTANT(L, root_pi);
    GLM_CONSTANT(L, root_three);
    GLM_CONSTANT(L, root_two);
    GLM_CONSTANT(L, root_two_pi);
    GLM_CONSTANT(L, third);
    GLM_CONSTANT(L, three_over_two_pi);
    GLM_CONSTANT(L, two_over_pi);
    GLM_CONSTANT(L, two_over_root_pi);
    GLM_CONSTANT(L, two_pi);
    GLM_CONSTANT(L, two_thirds);
    GLM_CONSTANT(L, zero);
    GLM_CONSTANT(L, epsilon);
#endif
    GLM_CONSTANT(L, pi); /* lmathlib */
    lua_pushnumber(L, glm::two_pi<lua_Number>()); lua_setfield(L, -2, "tau");
    lua_pushnumber(L, glm::epsilon<lua_Number>()); lua_setfield(L, -2, "eps");
    lua_pushnumber(L, static_cast<lua_Number>(glm::epsilon<glm_Float>())); lua_setfield(L, -2, "feps");
    lua_pushnumber(L, std::numeric_limits<lua_Number>::infinity()); lua_setfield(L, -2, "huge");
    lua_pushinteger(L, std::numeric_limits<lua_Integer>::max()); lua_setfield(L, -2, "maxinteger");
    lua_pushinteger(L, std::numeric_limits<lua_Integer>::min()); lua_setfield(L, -2, "mininteger");
    lua_pushinteger(L, FP_INFINITE); lua_setfield(L, -2, "FP_INFINITE"); /* c99 */
    lua_pushinteger(L, FP_NAN); lua_setfield(L, -2, "FP_NAN");
    lua_pushinteger(L, FP_ZERO); lua_setfield(L, -2, "FP_ZERO");
    lua_pushinteger(L, FP_SUBNORMAL); lua_setfield(L, -2, "FP_SUBNORMAL");
    lua_pushinteger(L, FP_NORMAL); lua_setfield(L, -2, "FP_NORMAL");

    /* Common unicode labels */
    lua_pushliteral(L, "\xE2\x81\xBB"); lua_setfield(L, -2, "subscript_minus");
    lua_pushliteral(L, "\xC2\xB9"); lua_setfield(L, -2, "subscript_one");
    lua_pushliteral(L, "\xC2\xB2"); lua_setfield(L, -2, "subscript_two");
    lua_pushliteral(L, "\xC2\xB3"); lua_setfield(L, -2, "subscript_three");
    lua_pushliteral(L, "\xC2\xB7"); lua_setfield(L, -2, "interpunct");
    lua_pushliteral(L, "m\xC2\xB7s\xE2\x81\xBB\xC2\xB9"); lua_setfield(L, -2, "unit_velocity");
    lua_pushliteral(L, "rad\xC2\xB7s\xE2\x81\xBB\xC2\xB9"); lua_setfield(L, -2, "unit_angular_velocity");

    /* Metamethods that reference the library as an upvalue */
    lua_pushvalue(L, -1);
    luaL_setfuncs(L, luaglm_metamethods, 1);

    /* Library details */
    lua_pushliteral(L, LUAGLM_NAME); lua_setfield(L, -2, "_NAME");
    lua_pushliteral(L, LUAGLM_VERSION); lua_setfield(L, -2, "_VERSION");
    lua_pushliteral(L, LUAGLM_COPYRIGHT); lua_setfield(L, -2, "_COPYRIGHT");
    lua_pushliteral(L, LUAGLM_DESCRIPTION); lua_setfield(L, -2, "_DESCRIPTION");
    lua_pushinteger(L, GLM_VERSION); lua_setfield(L, -2, "_GLM_VERSION");
#if defined(LUAGLM_ALIGNED)
    lua_pushboolean(L, 1); lua_setfield(L, -2, "_GLM_SIMD");
#else
    lua_pushboolean(L, 0); lua_setfield(L, -2, "_GLM_SIMD");
#endif

    /* Copy lmathlib functions not supported by library. */
    if (lua_getfield(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE) == LUA_TTABLE) {  // [..., glm, load_tab]
      if (lua_getfield(L, -1, LUA_MATHLIBNAME) == LUA_TTABLE) {  // [..., glm, load_tab, math_tab]
        lua_getfield(L, -1, "type"); lua_setfield(L, -4, "type");
        lua_getfield(L, -1, "random"); lua_setfield(L, -4, "random");
        lua_getfield(L, -1, "randomseed"); lua_setfield(L, -4, "randomseed");

        /* distribution functions prefer math.random as an upvalue */
        luaL_newlibtable(L, luaglm_randfuncs);  // [..., glm, load_tab, math_tab, dist_tab]
        if (lua_getfield(L, -2, "random") == LUA_TFUNCTION) {  // [..., glm, load_tab, math_tab, dist_tab, random]
          luaL_setfuncs(L, luaglm_randfuncs, 1);  // [..., glm, load_tab, math_tab, dist_tab]
        }
        else {
          lua_pop(L, 1);  // [..., glm, load_tab, math_tab, dist_tab]
        }
        lua_setfield(L, -4, "distribution");  // [..., glm, load_tab, math_tab]
      }
      lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* Setup default metatables */
#if defined(LUAGLM_INSTALL_METATABLES)
    lua_lock(L);
    if (G(L)->mt[LUA_TVECTOR] == GLM_NULLPTR)
      G(L)->mt[LUA_TVECTOR] = hvalue(s2v(L->top - 1));
    if (G(L)->mt[LUA_TMATRIX] == GLM_NULLPTR)
      G(L)->mt[LUA_TMATRIX] = hvalue(s2v(L->top - 1));
    lua_unlock(L);
#endif

    return 1;
  }
}

/* }================================================================== */
