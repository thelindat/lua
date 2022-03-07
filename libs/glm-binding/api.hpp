/*
** $Id: api.hpp $
** External Lua/GLM binding API.
**
** Missing Headers:
**  glm/gtx/associated_min_max.hpp
**
** See Copyright Notice in lua.h
*/
#ifndef BINDING_API_HPP
#define BINDING_API_HPP

/* Ensure experimental headers are included with glm/ext.hpp */
#if !defined(GLM_ENABLE_EXPERIMENTAL)
  #define GLM_ENABLE_EXPERIMENTAL
#endif

/*
** Flags used by the binding API.
**
** Functions:
@@ LUAGLM_INCLUDE_GTC Include glm/gtc/+
@@ LUAGLM_INCLUDE_GTX Include glm/gtx/+
@@ LUAGLM_INCLUDE_EXT Include glm/ext/+
@@ LUAGLM_INCLUDE_ALL Include all GLM headers.
@@ LUAGLM_ALIASES Include aliases (e.g., length vs. magnitude) to functions.
**
** Features:
@@ LUAGLM_INSTALL_METATABLES Update the global metatables for vector and matrix,
**  types, if ones are not already defined, with lglmlib on load.
@@ LUAGLM_TYPE_COERCION Enable string-to-number type coercion when parsing
**  arguments from the Lua stack. For this binding library to be a superset of
**  lmathlib, e.g., math = require('glm'), this flag must be enabled.
@@ LUAGLM_RECYCLE Enable object recycling: trailing/unused parameters in a
**  function call, e.g., matrix objects, are used as a result store.
@@ LUAGLM_FORCED_RECYCLE Disable this library from allocating memory, i.e.,
**  force use of LUAGLM_RECYCLE. Note, polygons other geom/ structures ignore
**  this flag (this may change).
@@ LUAGLM_SAFELIB Enable a general try/catch wrapper for API/binding functions.
**  This should not be enabled.
**
** Experimental:
@@ LUAGLM_NUMBER_ARGS Control where the lua_Number-to-float typecasting occurs
**  when binding to *float-only* functions:
**    1. parsing parameters (default);
**    2. pushing result, e.g., glm::eulerAngleXYZ.
@@ LUAGLM_DRIFT Implicitly normalize parameters that expect direction vectors
**  and quaternions.
@@ LUAGLM_TYPE_SANITIZE Enable narrow_cast for lua_Integer (to-and-from). At the
** moment this value will be sanitized after the fact, e.g., ensure the casted
** value is between 1 and size (inclusive).
*/
#include "bindings.hpp"
#include "iterators.hpp"

#include <glm/glm.hpp>
#include <glm/fwd.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/common.hpp>
#include <glm/gtx/easing.hpp>
#include <glm/gtx/exterior_product.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/matrix_factorisation.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <glm/gtx/scalar_relational.hpp>
#include <glm/gtx/texture.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <glm/detail/_vectorize.hpp>
/* @COMPAT: introduced in 0.9.9.4 and missing from ext.hpp until 0.9.9.8 */
#if GLM_VERSION >= 994 && GLM_VERSION <= 997
  #include <glm/ext/matrix_common.hpp>
#endif
#if GLM_HAS_CXX11_STL
  #include <glm/gtx/hash.hpp>
#endif

#include "ext/scalar_extensions.hpp"
#include "ext/vector_extensions.hpp"
#include "ext/matrix_extensions.hpp"
#include "ext/quat_extensions.hpp"

#include <lua.hpp>

/*
** {==================================================================
** Header Selection
** ===================================================================
*/

#define COMMON_HPP
#define CONSTANTS_HPP
#define EXPONENTIAL_HPP
#define GEOMETRIC_HPP
#define INTEGER_HPP
#define MATRIX_HPP
#define TRIGONOMETRIC_HPP
#define VECTOR_RELATIONAL_HPP
#if defined(LUAGLM_INCLUDE_ALL)
  #define PACKING_HPP
  #define STD_RANDOM
#endif

#if defined(LUAGLM_INCLUDE_ALL) || defined(LUAGLM_INCLUDE_EXT)
  #define EXT_MATRIX_CLIP_SPACE_HPP
  #define EXT_MATRIX_COMMON_HPP
  #define EXT_MATRIX_PROJECTION_HPP
  #define EXT_MATRIX_RELATIONAL_HPP
  #define EXT_MATRIX_TRANSFORM_HPP
  #define EXT_QUATERNION_COMMON_HPP
  #define EXT_QUATERNION_EXPONENTIAL_HPP
  #define EXT_QUATERNION_GEOMETRIC_HPP
  #define EXT_QUATERNION_RELATIONAL_HPP
  #define EXT_QUATERNION_TRIGONOMETRIC_HPP
  #define EXT_SCALAR_COMMON_HPP
  #define EXT_SCALAR_CONSTANTS_HPP
  #define EXT_SCALAR_INTEGER_HPP
  #define EXT_SCALAR_RELATIONAL_HPP
  #define EXT_SCALAR_ULP_HPP
  #define EXT_VECTOR_COMMON_HPP
  #define EXT_VECTOR_INTEGER_HPP
  #define EXT_VECTOR_RELATIONAL_HPP
  #define EXT_VECTOR_ULP_HPP
#endif

#if defined(LUAGLM_INCLUDE_ALL) || defined(LUAGLM_INCLUDE_GTC)
  #define GTC_BITFIELD_HPP
  #define GTC_COLOR_SPACE_HPP
  #define GTC_EPSILON_HPP
  #define GTC_INTEGER_HPP
  #define GTC_MATRIX_ACCESS_HPP
  #define GTC_MATRIX_INVERSE_HPP
  #define GTC_NOISE_HPP
  #define GTC_QUATERNION_HPP
  #define GTC_RANDOM_HPP
  #define GTC_RECIPROCAL_HPP
  #define GTC_ROUND_HPP
  #define GTC_TYPE_PRECISION_HPP
  #define GTC_ULP_HPP
#endif

#if defined(LUAGLM_INCLUDE_ALL) || defined(LUAGLM_INCLUDE_GTX)
  #if !defined(GLM_ENABLE_EXPERIMENTAL)
    #error "GLM_ENABLE_EXPERIMENTAL not enabled!"
  #endif
  #define GTX_ASSOCIATED_MIN_MAX_HPP
  #define GTX_BIT_HPP
  #define GTX_CLOSEST_POINT_HPP
  #define GTX_COLOR_ENCODING_HPP
  #define GTX_COLOR_SPACE_HPP
  #define GTX_COLOR_SPACE_YCOCG_HPP
  #define GTX_COMMON_HPP
  #define GTX_COMPATIBILITY_HPP
  #define GTX_COMPONENT_WISE_HPP
  #define GTX_EASING_HPP
  #define GTX_EULER_ANGLES_HPP
  #define GTX_EXTEND_HPP
  #define GTX_EXTERIOR_PRODUCT_HPP
  #define GTX_FAST_EXPONENTIAL_HPP
  #define GTX_FAST_SQUARE_ROOT_HPP
  #define GTX_FAST_TRIGONOMETRY_HPP
  #define GTX_FUNCTIONS_HPP
  #define GTX_GRADIENT_PAINT_HPP
  #define GTX_HANDED_COORDINATE_SPACE_HPP
  #define GTX_INTEGER_HPP
  #define GTX_INTERSECT_HPP
  #define GTX_LOG_BASE_HPP
  #define GTX_LOG_BASE_HPP
  #define GTX_MATRIX_CROSS_PRODUCT_HPP
  #define GTX_MATRIX_DECOMPOSE_HPP
  #define GTX_MATRIX_FACTORISATION_HPP
  #define GTX_MATRIX_INTERPOLATION_HPP
  #define GTX_MATRIX_MAJOR_STORAGE_HPP
  #define GTX_MATRIX_OPERATION_HPP
  #define GTX_MATRIX_QUERY_HPP
  #define GTX_MATRIX_TRANSFORM_2D_HPP
  #define GTX_MIXED_PRODUCT_HPP
  #define GTX_NORMALIZE_DOT_HPP
  #define GTX_NORMAL_HPP
  #define GTX_NORM_HPP
  #define GTX_OPTIMUM_POW_HPP
  #define GTX_ORTHONORMALIZE_HPP
  #define GTX_PCA_HPP
  #define GTX_PERPENDICULAR_HPP
  #define GTX_POLAR_COORDINATES_HPP
  #define GTX_PROJECTION_HPP
  #define GTX_QUATERNION_HPP
  #define GTX_QUATERNION_TRANSFORM_HPP
  #define GTX_RANGE_HPP
  #define GTX_ROTATE_NORMALIZED_AXIS_HPP
  #define GTX_ROTATE_VECTOR_HPP
  #define GTX_SPLINE_HPP
  #define GTX_TEXTURE_HPP
  #define GTX_TRANSFORM2_HPP
  #define GTX_TRANSFORM_HPP
  #define GTX_VECTOR_ANGLE_HPP
  #define GTX_VECTOR_QUERY_HPP
  #define GTX_WRAP_HPP
#endif

/*
** @WIP Macros that ensure backwards compatibility.
**
** Temporary change until an improved feature-flag system is developed to reduce
** the size of the binding library (if ever?).
*/
#if GLM_VERSION < 999
  #undef GTX_PCA_HPP  // @COMPAT: pca.hpp introduced in 0.9.9.9
  #if GLM_VERSION < 994
    #undef EXT_MATRIX_COMMON_HPP  // @COMPAT: ext/matrix_common.hpp introduced in 0.9.9.4
  #endif
#endif

/* }================================================================== */

/*
** {==================================================================
** Object Properties
** ===================================================================
*/

/* Template for matrix equals/not-equals */
#define LAYOUT_MATRIX_EQUAL(LB, F, Tr, ...) \
  LAYOUT_GENERIC_EQUAL(LB, F, Tr, Tr::row_type)

/*
** Definition for generalized equals/not-equals function.
**
** Missing: "vec<L, int, Q> const& ULPs". The current design makes it impossible
** to differentiate between a vector of eps values and ULP values.
*/
#define EQUAL_DEFN(Name, F, ...)                                                                 \
  GLM_BINDING_QUALIFIER(Name) {                                                                  \
    GLM_BINDING_BEGIN                                                                            \
    const TValue *o = LB.i2v();                                                                  \
    switch (ttypetag(o)) {                                                                       \
      case LUA_VNUMINT: {                                                                        \
        if (LB.Is<gLuaInteger>(gLuaInteger::stack_size))                                         \
          VA_CALL(BIND_FUNC, LB, F, gLuaInteger::fast, gLuaInteger);                             \
      } LUA_FALLTHROUGH;                                                                         \
      case LUA_VFALSE: case LUA_VTRUE: /* @BoolCoercion */                                       \
      case LUA_VSHRSTR: case LUA_VLNGSTR: /* @StringCoercion */                                  \
      case LUA_VNUMFLT: LAYOUT_GENERIC_EQUAL(LB, F, gLuaNumber, gLuaNumber); break;              \
      case LUA_VVECTOR2: LAYOUT_GENERIC_EQUAL(LB, F, gLuaVec2<>::fast, gLuaVec2<>::fast); break; \
      case LUA_VVECTOR3: LAYOUT_GENERIC_EQUAL(LB, F, gLuaVec3<>::fast, gLuaVec3<>::fast); break; \
      case LUA_VVECTOR4: LAYOUT_GENERIC_EQUAL(LB, F, gLuaVec4<>::fast, gLuaVec4<>::fast); break; \
      case LUA_VQUAT: LAYOUT_GENERIC_EQUAL(LB, F, gLuaQuat<>::fast, gLuaVec4<>::fast); break;    \
      case LUA_VMATRIX: PARSE_MATRIX(LB, mvalue_dims(o), F, LAYOUT_MATRIX_EQUAL); break;         \
      default:                                                                                   \
        break;                                                                                   \
    }                                                                                            \
    return LUAGLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_VECTOR " or " GLM_STRING_QUATERN);         \
    GLM_BINDING_END                                                                              \
  }

/* glm/gtx/string_cast.hpp */
GLM_BINDING_QUALIFIER(to_string) {
  GLM_BINDING_BEGIN
  const int n = LB.top();
  for (int i = LB.idx; i <= n; ++i)
    lua_tostring(LB.L, i);
  return n;
  GLM_BINDING_END
}

/* glm/ext/scalar_relational.hpp, glm/ext/vector_common.hpp, glm/ext/vector_relational.hpp, glm/ext/quaternion_relational.hpp, glm/ext/matrix_relational.hpp */
EQUAL_DEFN(equal, glm::equal)
EQUAL_DEFN(notEqual, glm::notEqual)
EQUAL_DEFN(all_equal, glm::all_equal)  // @GLMVectorExtensions
EQUAL_DEFN(any_notequal, glm::any_notequal)  // @GLMVectorExtensions
#if GLM_HAS_CXX11_STL
#define LAYOUT_HASH(LB, F, Tr, ...) \
  BIND_PUSH_V(LB, F<Tr::type>()((LB).Next<Tr>()))

GLM_BINDING_QUALIFIER(hash) { /* glm/gtx/hash.hpp */
  GLM_BINDING_BEGIN
  const int n = LB.top();
  while (LB.idx <= n) {
    const TValue *o = LB.i2v();
    switch (ttypetag(o)) {
      case LUA_VTRUE:
      case LUA_VFALSE: LAYOUT_HASH(LB, std::hash, gLuaTrait<bool>::fast); break;
      case LUA_VSHRSTR:
  #if defined(LUA_VBLOBSTR)
      case LUA_VBLOBSTR:
  #endif
      case LUA_VLNGSTR: LAYOUT_HASH(LB, std::hash, gLuaTrait<const char *>::fast); break;
      case LUA_VNUMINT: LAYOUT_HASH(LB, std::hash, gLuaInteger::fast); break;
      case LUA_VNUMFLT: LAYOUT_HASH(LB, std::hash, gLuaNumber::fast); break;
      case LUA_VVECTOR2: LAYOUT_HASH(LB, std::hash, gLuaVec2<>::fast); break;
      case LUA_VVECTOR3: LAYOUT_HASH(LB, std::hash, gLuaVec3<>::fast); break;
      case LUA_VVECTOR4: LAYOUT_HASH(LB, std::hash, gLuaVec4<>::fast); break;
      case LUA_VQUAT: LAYOUT_HASH(LB, std::hash, gLuaQuat<>); break;
      case LUA_VMATRIX: PARSE_MATRIX(LB, mvalue_dims(o), std::hash, LAYOUT_HASH); break;
      default: {
        return LUAGLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_VECTOR " or " GLM_STRING_QUATERN " or " GLM_STRING_MATRIX);
      }
    }
  }
  return LB.top() - n;
  GLM_BINDING_END
}
#endif

GLM_BINDING_QUALIFIER(unpack) {
  GLM_BINDING_BEGIN
  const int n = LB.top_for_recycle();
  for (; LB.idx <= n; ++LB.idx) {
    switch (ttype(LB.i2v())) {
      case LUA_TVECTOR:
      case LUA_TMATRIX:
        luaglm_unpack(LB.L, LB.idx);
        break;
      default: {
        lua_pushvalue(LB.L, LB.idx);
        break;
      }
    }
  }
  return LB.top() - n;
  GLM_BINDING_END
}

BIND_DEFN(up, glm::unit::up<glm_Float>)  // @GLMVectorExtensions
BIND_DEFN(right, glm::unit::right<glm_Float>)
BIND_DEFN(forward, glm::unit::forward<glm_Float>)
BIND_DEFN(forwardLH, glm::unit::forwardLH<glm_Float>)
BIND_DEFN(forwardRH, glm::unit::forwardRH<glm_Float>)

/* }================================================================== */

/*
** {==================================================================
** Arithmetic operators: Function wrappers that allow the use of LUAGLM_RECYCLE.
** @TODO: Better
** ===================================================================
*/

/* Layout for object addition: object + object || object + number */
#define LAYOUT_ADDITION_OP(LB, F, Tr, ...)                         \
  LUA_MLM_BEGIN                                                    \
  if (LB.Is<Tr::value_trait>(Tr::stack_size))                      \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::value_trait, ##__VA_ARGS__); \
  VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, ##__VA_ARGS__);          \
  LUA_MLM_END

/* Layout for F(number, matrix) operations */
#define LAYOUT_RH_MULTIPLICATION_OP(LB, F, Tr, ...)              \
  LUA_MLM_BEGIN                                                  \
  VA_CALL(BIND_FUNC, LB, F, Tr::value_trait, Tr, ##__VA_ARGS__); \
  LUA_MLM_END

/* Layout for generic matrix multiplication. */
#define LAYOUT_MULTIPLICATION_OP(LB, F, Tr, ...)                                            \
  LUA_MLM_BEGIN                                                                             \
  const TValue *_tv2 = LB.i2v(Tr::stack_size);                                              \
  switch (ttype(_tv2)) {                                                                    \
    case LUA_TNUMBER: VA_CALL(BIND_FUNC, LB, F, Tr, Tr::value_trait, ##__VA_ARGS__); break; \
    case LUA_TVECTOR: VA_CALL(BIND_FUNC, LB, F, Tr, Tr::row_type, ##__VA_ARGS__); break;    \
    case LUA_TMATRIX: {                                                                     \
      switch (LUAGLM_MATRIX_COLS(mvalue_dims(_tv2))) {                                      \
        case 2: VA_CALL(BIND_FUNC, LB, F, Tr, Tr::rhs_mat_type<2>, ##__VA_ARGS__); break;   \
        case 3: VA_CALL(BIND_FUNC, LB, F, Tr, Tr::rhs_mat_type<3>, ##__VA_ARGS__); break;   \
        case 4: VA_CALL(BIND_FUNC, LB, F, Tr, Tr::rhs_mat_type<4>, ##__VA_ARGS__); break;   \
        default:                                                                            \
          break;                                                                            \
      }                                                                                     \
      break;                                                                                \
    }                                                                                       \
    default:                                                                                \
      break;                                                                                \
  }                                                                                         \
  break;                                                                                    \
  LUA_MLM_END

/// <summary>
/// A matrix functions intended to allow the recycling of preallocated matrix
/// structures. Any operations that result in vector/numeric types should use
/// built-in Lua operators.
/// </summary>
MATRIX_DEFN(mat_add, operator+, LAYOUT_ADDITION_OP);
MATRIX_DEFN(mat_sub, operator-, LAYOUT_ADDITION_OP);
MATRIX_DEFN(mat_negate, operator-, LAYOUT_UNARY);
GLM_BINDING_QUALIFIER(mat_mul) {  // @TODO: Reduce bloat!
  GLM_BINDING_BEGIN
  const TValue *o = LB.i2v();
  switch (ttypetag(o)) {
    case LUA_VNUMINT:
    case LUA_VNUMFLT: {
      const TValue *o2 = LB.i2v(1);
      if (l_likely(ttismatrix(o2)))  // operator*(num, matrix)
        PARSE_MATRIX(LB, mvalue_dims(o2), operator*, LAYOUT_RH_MULTIPLICATION_OP);
      break;
    }
    case LUA_VVECTOR2: LAYOUT_MULTIPLICATION_OP(LB, operator*, gLuaVec2<>::fast); break;
    case LUA_VVECTOR3: LAYOUT_MULTIPLICATION_OP(LB, operator*, gLuaVec3<>::fast); break;
    case LUA_VVECTOR4: LAYOUT_MULTIPLICATION_OP(LB, operator*, gLuaVec4<>::fast); break;
    // @TODO: Special case for handling mat4x4 * vec3 and mat4x3 * vec3; see LUAGLM_MUL_DIRECTION.
    case LUA_VMATRIX: PARSE_MATRIX(LB, mvalue_dims(o), operator*, LAYOUT_MULTIPLICATION_OP); break;
    default: {
      break;
    }
  }
  return LUAGLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_NUMBER " or " GLM_STRING_VECTOR " or " GLM_STRING_MATRIX);
  GLM_BINDING_END
}

/* }================================================================== */

/*
** {==================================================================
** Scalar Specific
** ===================================================================
*/

#if defined(INTEGER_HPP)
#define LAYOUT_ADD_CARRY(LB, F, Tr, ...)    \
  LUA_MLM_BEGIN                             \
  Tr::type carry;                           \
  const Tr::type a = (LB).Next<Tr>();       \
  const Tr::type b = (LB).Next<Tr::safe>(); \
  const auto _r = F(a, b, carry);           \
  BIND_PUSH(LB, _r, carry);                 \
  LUA_MLM_END

#define LAYOUT_MUL_EXTENDED(LB, F, Tr, ...) \
  LUA_MLM_BEGIN                             \
  Tr::type lsb, msb;                        \
  const Tr::type a = (LB).Next<Tr>();       \
  const Tr::type b = (LB).Next<Tr::safe>(); \
  F(a, b, lsb, msb);                        \
  BIND_PUSH(LB, lsb, msb);                  \
  LUA_MLM_END

#if LUA_INT_TYPE != LUA_INT_INT || !LUAGLM_ALIGNED
INTEGER_VECTOR_DEFN(bitCount, glm::bitCount, lua_Unsigned, LAYOUT_UNARY)
#endif
INTEGER_VECTOR_DEFN(bitfieldExtract, glm::bitfieldExtract, lua_Unsigned, LAYOUT_UNARY, gLuaTrait<int>, gLuaTrait<int>)
INTEGER_VECTOR_DEFN(bitfieldInsert, glm::bitfieldInsert, lua_Unsigned, LAYOUT_BINARY, gLuaTrait<int>, gLuaTrait<int>)
#if LUA_INT_TYPE != LUA_INT_INT || !LUAGLM_ALIGNED
INTEGER_VECTOR_DEFN(bitfieldReverse, glm::bitfieldReverse, lua_Unsigned, LAYOUT_UNARY)
#endif
INTEGER_VECTOR_DEFN(findLSB, glm::findLSB, lua_Unsigned, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(findMSB, glm::findMSB, lua_Unsigned, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(imulExtended, glm::imulExtended, int, LAYOUT_MUL_EXTENDED)
INTEGER_VECTOR_DEFN(uaddCarry, glm::uaddCarry, glm::uint, LAYOUT_ADD_CARRY)
INTEGER_VECTOR_DEFN(umulExtended, glm::umulExtended, glm::uint, LAYOUT_MUL_EXTENDED)
INTEGER_VECTOR_DEFN(usubBorrow, glm::usubBorrow, glm::uint, LAYOUT_ADD_CARRY)
#endif

#if defined(EXT_SCALAR_INTEGER_HPP) || defined(EXT_VECTOR_INTEGER_HPP)
#if GLM_VERSION >= 996  // @COMPAT: Added in 0.9.9.6
INTEGER_VECTOR_DEFN(findNSB, glm::findNSB, lua_Unsigned, LAYOUT_BINARY_AS_INT)
#endif
#endif

#if defined(GTC_BITFIELD_HPP)
BIND_DEFN(bitfieldDeinterleave, glm::bitfieldDeinterleave, gLuaTrait<glm::uint64>)
INTEGER_VECTOR_DEFN(bitfieldFillOne, glm::bitfieldFillOne, lua_Unsigned, LAYOUT_UNARY, gLuaTrait<int>, gLuaTrait<int>)
INTEGER_VECTOR_DEFN(bitfieldFillZero, glm::bitfieldFillZero, lua_Unsigned, LAYOUT_UNARY, gLuaTrait<int>, gLuaTrait<int>)
INTEGER_VECTOR_DEFN(bitfieldRotateLeft, glm::bitfieldRotateLeft, lua_Unsigned, LAYOUT_UNARY, gLuaTrait<int>)
INTEGER_VECTOR_DEFN(bitfieldRotateRight, glm::bitfieldRotateRight, lua_Unsigned, LAYOUT_UNARY, gLuaTrait<int>)
GLM_BINDING_QUALIFIER(bitfieldInterleave) {
  GLM_BINDING_BEGIN
  switch (LB.top()) {
    case 2: BIND_FUNC(LB, glm::bitfieldInterleave, gLuaTrait<glm::uint32_t>, gLuaTrait<glm::uint32_t>); break;
    case 3: BIND_FUNC(LB, glm::bitfieldInterleave, gLuaTrait<glm::uint32_t>, gLuaTrait<glm::uint32_t>, gLuaTrait<glm::uint32_t>); break;
    case 4: BIND_FUNC(LB, glm::bitfieldInterleave, gLuaTrait<glm::uint16_t>, gLuaTrait<glm::uint16_t>, gLuaTrait<glm::uint16_t>, gLuaTrait<glm::uint16_t>); break;
    default: {
      break;
    }
  }
  return LUAGLM_ERROR(LB.L, "uint32_tx2, uint32_tx3, or uint16_tx4 expected");
  GLM_BINDING_END
}
INTEGER_VECTOR_DEFN(mask, glm::mask, lua_Unsigned, LAYOUT_UNARY)
#endif

#if defined(GTX_BIT_HPP)
INTEGER_VECTOR_DEFN(highestBitValue, glm::highestBitValue, lua_Integer, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(lowestBitValue, glm::lowestBitValue, lua_Integer, LAYOUT_UNARY)
// GLM_BINDING_DECL(powerOfTwoAbove);  // @DEPRECATED
// GLM_BINDING_DECL(powerOfTwoBelow);  // @DEPRECATED
// GLM_BINDING_DECL(powerOfTwoNearest);  // @DEPRECATED
#endif

#if defined(PACKING_HPP)
BIND_DEFN(packUnorm2x16, glm::packUnorm2x16, gLuaVec2<float>)
BIND_DEFN(unpackUnorm2x16, glm::unpackUnorm2x16, gLuaTrait<glm::uint>)
BIND_DEFN(packSnorm2x16, glm::packSnorm2x16, gLuaVec2<float>)
BIND_DEFN(unpackSnorm2x16, glm::unpackSnorm2x16, gLuaTrait<glm::uint>)
BIND_DEFN(packUnorm4x8, glm::packUnorm4x8, gLuaVec4<float>)
BIND_DEFN(unpackUnorm4x8, glm::unpackUnorm4x8, gLuaTrait<glm::uint>)
BIND_DEFN(packSnorm4x8, glm::packSnorm4x8, gLuaVec4<float>)
BIND_DEFN(unpackSnorm4x8, glm::unpackSnorm4x8, gLuaTrait<glm::uint>)
BIND_DEFN(packDouble2x32, glm::packDouble2x32, gLuaVec2<glm::uint>)
BIND_DEFN(unpackDouble2x32, glm::unpackDouble2x32, gLuaTrait<double>)
BIND_DEFN(packHalf2x16, glm::packHalf2x16, gLuaVec2<float>)
BIND_DEFN(unpackHalf2x16, glm::unpackHalf2x16, gLuaTrait<glm::uint>)
#endif

#if defined(GTC_TYPE_PRECISION_HPP)
BIND_DEFN(packUnorm1x8, glm::packUnorm1x8, gLuaTrait<float>)
BIND_DEFN(unpackUnorm1x8, glm::unpackUnorm1x8, gLuaTrait<glm::uint8>)
BIND_DEFN(packUnorm2x8, glm::packUnorm2x8, gLuaVec2<float>)
BIND_DEFN(unpackUnorm2x8, glm::unpackUnorm2x8, gLuaTrait<glm::uint16>)
BIND_DEFN(packSnorm1x8, glm::packSnorm1x8, gLuaTrait<float>)
BIND_DEFN(unpackSnorm1x8, glm::unpackSnorm1x8, gLuaTrait<glm::uint8>)
BIND_DEFN(packSnorm2x8, glm::packSnorm2x8, gLuaVec2<float>)
BIND_DEFN(unpackSnorm2x8, glm::unpackSnorm2x8, gLuaTrait<glm::uint16>)
BIND_DEFN(packUnorm1x16, glm::packUnorm1x16, gLuaTrait<float>)
BIND_DEFN(unpackUnorm1x16, glm::unpackUnorm1x16, gLuaTrait<glm::uint16>)
BIND_DEFN(packUnorm4x16, glm::packUnorm4x16, gLuaVec4<float>)
BIND_DEFN(unpackUnorm4x16, glm::unpackUnorm4x16, gLuaTrait<glm::uint64>)
BIND_DEFN(packSnorm1x16, glm::packSnorm1x16, gLuaTrait<float>)
BIND_DEFN(unpackSnorm1x16, glm::unpackSnorm1x16, gLuaTrait<glm::uint16>)
BIND_DEFN(packSnorm4x16, glm::packSnorm4x16, gLuaVec4<float>)
BIND_DEFN(unpackSnorm4x16, glm::unpackSnorm4x16, gLuaTrait<glm::uint64>)
BIND_DEFN(packHalf1x16, glm::packHalf1x16, gLuaTrait<float>)
BIND_DEFN(unpackHalf1x16, glm::unpackHalf1x16, gLuaTrait<glm::uint16>)
BIND_DEFN(packHalf4x16, glm::packHalf4x16, gLuaVec4<float>)
BIND_DEFN(unpackHalf4x16, glm::unpackHalf4x16, gLuaTrait<glm::uint64>)
BIND_DEFN(packI3x10_1x2, glm::packI3x10_1x2, gLuaVec4<int>)
BIND_DEFN(unpackI3x10_1x2, glm::unpackI3x10_1x2, gLuaTrait<glm::uint32>)
BIND_DEFN(packU3x10_1x2, glm::packU3x10_1x2, gLuaVec4<unsigned>)
BIND_DEFN(unpackU3x10_1x2, glm::unpackU3x10_1x2, gLuaTrait<glm::uint32>)
BIND_DEFN(packSnorm3x10_1x2, glm::packSnorm3x10_1x2, gLuaVec4<float>)
BIND_DEFN(unpackSnorm3x10_1x2, glm::unpackSnorm3x10_1x2, gLuaTrait<glm::uint32>)
BIND_DEFN(packUnorm3x10_1x2, glm::packUnorm3x10_1x2, gLuaVec4<float>)
BIND_DEFN(unpackUnorm3x10_1x2, glm::unpackUnorm3x10_1x2, gLuaTrait<glm::uint32>)
BIND_DEFN(packF2x11_1x10, glm::packF2x11_1x10, gLuaVec3<float>)
BIND_DEFN(unpackF2x11_1x10, glm::unpackF2x11_1x10, gLuaTrait<glm::uint32>)
BIND_DEFN(packF3x9_E1x5, glm::packF3x9_E1x5, gLuaVec3<float>)
BIND_DEFN(unpackF3x9_E1x5, glm::unpackF3x9_E1x5, gLuaTrait<glm::uint32>)
BIND_DEFN(packRGBM, glm::packRGBM, gLuaVec3<>)
BIND_DEFN(unpackRGBM, glm::unpackRGBM, gLuaVec4<>)
INTEGER_VECTOR_DEFN(packHalf, glm::packHalf, float, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(unpackHalf, glm::unpackHalf, glm::uint16, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(packUnorm, glm::packUnorm<glm::uint16>, float, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(unpackUnorm, glm::unpackUnorm<float>, glm::uint16, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(packSnorm, glm::packSnorm<glm::int16>, float, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(unpackSnorm, glm::unpackSnorm<float>, glm::int16, LAYOUT_UNARY)
BIND_DEFN(packUnorm2x4, glm::packUnorm2x4, gLuaVec2<float>)
BIND_DEFN(unpackUnorm2x4, glm::unpackUnorm2x4, gLuaTrait<glm::uint8>)
BIND_DEFN(packUnorm4x4, glm::packUnorm4x4, gLuaVec4<float>)
BIND_DEFN(unpackUnorm4x4, glm::unpackUnorm4x4, gLuaTrait<glm::uint16>)
BIND_DEFN(packUnorm1x5_1x6_1x5, glm::packUnorm1x5_1x6_1x5, gLuaVec3<float>)
BIND_DEFN(unpackUnorm1x5_1x6_1x5, glm::unpackUnorm1x5_1x6_1x5, gLuaTrait<glm::uint16>)
BIND_DEFN(packUnorm3x5_1x1, glm::packUnorm3x5_1x1, gLuaVec4<float>)
BIND_DEFN(unpackUnorm3x5_1x1, glm::unpackUnorm3x5_1x1, gLuaTrait<glm::uint16>)
BIND_DEFN(packUnorm2x3_1x2, glm::packUnorm2x3_1x2, gLuaVec3<float>)
BIND_DEFN(unpackUnorm2x3_1x2, glm::unpackUnorm2x3_1x2, gLuaTrait<glm::uint8>)
BIND_DEFN(packInt2x8, glm::packInt2x8, gLuaVec2<glm::i8>)
BIND_DEFN(unpackInt2x8, glm::unpackInt2x8, gLuaTrait<glm::int16>)
BIND_DEFN(packUint2x8, glm::packUint2x8, gLuaVec2<glm::u8>)
BIND_DEFN(unpackUint2x8, glm::unpackUint2x8, gLuaTrait<glm::uint16>)
BIND_DEFN(packInt4x8, glm::packInt4x8, gLuaVec4<glm::i8>)
BIND_DEFN(unpackInt4x8, glm::unpackInt4x8, gLuaTrait<glm::int32>)
BIND_DEFN(packUint4x8, glm::packUint4x8, gLuaVec4<glm::u8>)
BIND_DEFN(unpackUint4x8, glm::unpackUint4x8, gLuaTrait<glm::uint32>)
BIND_DEFN(packInt2x16, glm::packInt2x16, gLuaVec2<glm::i16>)
BIND_DEFN(unpackInt2x16, glm::unpackInt2x16, gLuaTrait<int>)
BIND_DEFN(packInt4x16, glm::packInt4x16, gLuaVec4<glm::i16>)
BIND_DEFN(unpackInt4x16, glm::unpackInt4x16, gLuaTrait<glm::int64>)
BIND_DEFN(packUint2x16, glm::packUint2x16, gLuaVec2<glm::u16>)
BIND_DEFN(unpackUint2x16, glm::unpackUint2x16, gLuaTrait<glm::uint>)
BIND_DEFN(packUint4x16, glm::packUint4x16, gLuaVec4<glm::u16>)
BIND_DEFN(unpackUint4x16, glm::unpackUint4x16, gLuaTrait<glm::uint64>)
BIND_DEFN(packInt2x32, glm::packInt2x32, gLuaVec2<glm::i32>)
BIND_DEFN(unpackInt2x32, glm::unpackInt2x32, gLuaTrait<glm::int64>)
BIND_DEFN(packUint2x32, glm::packUint2x32, gLuaVec2<glm::u32>)
BIND_DEFN(unpackUint2x32, glm::unpackUint2x32, gLuaTrait<glm::uint64>)
#endif

#if (defined(GTC_ULP_HPP) || defined(EXT_SCALAR_ULP_HPP)) && LUAGLM_INCLUDE_IEEE
#if GLM_VERSION >= 993  // @COMPAT: float_distance incorrectly declared until 0.9.9.3
NUMBER_VECTOR_DEFN(float_distance, glm::float_distance, LAYOUT_BINARY)
#endif
#endif

#if (defined(GTC_ULP_HPP) || defined(EXT_SCALAR_ULP_HPP) || defined(EXT_VECTOR_ULP_HPP)) && LUAGLM_INCLUDE_IEEE
#define LAYOUT_NEXT_FLOAT(LB, F, Tr, ...) /* @GLMAssert: assert(ULPs >= 0); */         \
  LUA_MLM_BEGIN                                                                        \
  if (gLuaBase::isnoneornil((LB).L, (LB).idx + Tr::stack_size))                        \
    VA_CALL(BIND_FUNC, LB, F, Tr, ##__VA_ARGS__);                                      \
  else if (LB.Is<gLuaTrait<int>>(Tr::stack_size))                                      \
    VA_CALL(BIND_FUNC, LB, F, Tr, gPositiveConstraint<gLuaTrait<int>>, ##__VA_ARGS__); \
  VA_CALL(BIND_FUNC, LB, F, Tr, gPositiveConstraint<Tr::as_type<int>>, ##__VA_ARGS__); \
  LUA_MLM_END

#if GLM_VERSION >= 993  // @COMPAT: vector support added in 0.9.9.3
NUMBER_VECTOR_DEFN(next_float, glm::next_float, LAYOUT_NEXT_FLOAT)
NUMBER_VECTOR_DEFN(prev_float, glm::prev_float, LAYOUT_NEXT_FLOAT)
#else
LAYOUT_DEFN(next_float, glm::next_float, LAYOUT_NEXT_FLOAT, gLuaNumber)
LAYOUT_DEFN(prev_float, glm::prev_float, LAYOUT_NEXT_FLOAT, gLuaNumber)
#endif
#endif

/* }================================================================== */

/*
** {==================================================================
** Quaternion Specific
** ===================================================================
*/

#if defined(EXT_QUATERNION_COMMON_HPP)
BIND_DEFN(conjugate, glm::conjugate, gLuaQuat<>)
#endif

#if defined(EXT_QUATERNION_COMMON_HPP) || defined(MATRIX_HPP)
GLM_BINDING_QUALIFIER(inverse) {
  GLM_BINDING_BEGIN
  const TValue *o = LB.i2v();
  if (ttismatrix(o))
    PARSE_SYMMETRIC_MATRIX(LB, mvalue_dims(o), glm::inverse, LAYOUT_UNARY);
  PARSE_NUMBER_VECTOR_QUAT(LB, glm::inverse, LAYOUT_UNARY, LAYOUT_UNARY, LAYOUT_UNARY);
  GLM_BINDING_END
}
SYMMETRIC_MATRIX_DEFN(invertible, glm::invertible, LAYOUT_UNARY)  // @GLMMatrixExtensions
ROTATION_MATRIX_DEFN(inverse_transform, glm::inverse_transform, LAYOUT_UNARY)
#endif

#if defined(EXT_QUATERNION_TRIGONOMETRIC_HPP)
BIND_DEFN(axis, glm::axis, gLuaQuat<>)
BIND_DEFN(angleAxis, glm::angleAxis, gLuaVec3<>::value_trait, gLuaDir3<>)
#endif

#if defined(GTC_QUATERNION_HPP)
BIND_DEFN(eulerAngles, glm::eulerAngles, gLuaQuat<>)
BIND_DEFN(mat3_cast, glm::mat3_cast, gLuaQuat<>)
BIND_DEFN(mat4_cast, glm::mat4_cast, gLuaQuat<>)
BIND_DEFN(pitch, glm::pitch, gLuaQuat<>)
BIND_DEFN(roll, glm::roll, gLuaQuat<>)
BIND_DEFN(yaw, glm::yaw, gLuaQuat<>)
BIND_DEFN(quatLookAt, glm::quatLookAt, gLuaDir3<>, gLuaDir3<>)
BIND_DEFN(quatLookAtLH, glm::quatLookAtLH, gLuaDir3<>, gLuaDir3<>)
BIND_DEFN(quatLookAtRH, glm::quatLookAtRH, gLuaDir3<>, gLuaDir3<>)
BIND_DEFN(quatbillboard, glm::quatbillboard, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>, gLuaDir3<>)  // @GLMQuatExtensions
BIND_DEFN(quatbillboardRH, glm::quatbillboardRH, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>, gLuaDir3<>)
BIND_DEFN(quatbillboardLH, glm::quatbillboardLH, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>, gLuaDir3<>)
BIND_DEFN(quatFromBasis, glm::fromBasis, gLuaDir3<>, gLuaDir3<>, gLuaDir3<>)
#endif

#if defined(GTX_QUATERNION_HPP)
BIND_DEFN(extractRealComponent, glm::extractRealComponent, gLuaQuat<>)
BIND_DEFN(fastMix, glm::fastMix, gLuaQuat<>, gLuaQuat<>, gLuaQuat<>::value_trait)
BIND_DEFN(intermediate, glm::intermediate, gLuaQuat<>, gLuaQuat<>, gLuaQuat<>)
BIND_DEFN(shortMix, glm::shortMix, gLuaQuat<>, gLuaQuat<>, gLuaQuat<>::value_trait)
BIND_DEFN(toMat3, glm::toMat3, gLuaQuat<>)
BIND_DEFN(toMat4, glm::toMat4, gLuaQuat<>)
BIND_DEFN(squad, glm::squad, gLuaQuat<>, gLuaQuat<>, gLuaQuat<>, gLuaQuat<>, gLuaQuat<>::value_trait)
BIND_DEFN(rotation, glm::rotation, gLuaVec3<>, gLuaVec3<>)
ROTATION_MATRIX_DEFN(quat_cast, glm::quat_cast, LAYOUT_UNARY)
BIND_DEFN(quat_identity, glm::identity<gLuaQuat<>::type>)
#endif

#if defined(GTX_ROTATE_NORMALIZED_AXIS_HPP)
ROTATION_MATRIX_DEFN(rotateNormalizedAxis, glm::rotateNormalizedAxis, LAYOUT_UNARY, gLuaFloat, gLuaDir3<>)
#endif

/* }================================================================== */

/*
** {==================================================================
** Matrix Specific
** ===================================================================
*/

#if defined(MATRIX_HPP)
#define LAYOUT_OUTERPRODUCT(LB, F, Tr, ...)                                           \
  LUA_MLM_BEGIN                                                                       \
  switch (ttypetag((LB).i2v(Tr::stack_size))) {                                       \
    case LUA_VVECTOR2: BIND_FUNC(LB, glm::outerProduct, Tr, gLuaVec2<>::fast); break; \
    case LUA_VVECTOR3: BIND_FUNC(LB, glm::outerProduct, Tr, gLuaVec3<>::fast); break; \
    case LUA_VVECTOR4: BIND_FUNC(LB, glm::outerProduct, Tr, gLuaVec4<>::fast); break; \
    default: {                                                                        \
      break;                                                                          \
    }                                                                                 \
  }                                                                                   \
  LUA_MLM_END

SYMMETRIC_MATRIX_DEFN(determinant, glm::determinant, LAYOUT_UNARY)
MATRIX_DEFN(matrixCompMult, glm::__matrixCompMult, LAYOUT_BINARY)  // @GLMFix
MATRIX_DEFN(transpose, glm::transpose, LAYOUT_UNARY)
GLM_BINDING_QUALIFIER(outerProduct) {
  GLM_BINDING_BEGIN
  switch (ttypetag(LB.i2v())) {
    case LUA_VVECTOR2: LAYOUT_OUTERPRODUCT(LB, glm::outerProduct, gLuaVec2<>::fast); break;
    case LUA_VVECTOR3: LAYOUT_OUTERPRODUCT(LB, glm::outerProduct, gLuaVec3<>::fast); break;
    case LUA_VVECTOR4: LAYOUT_OUTERPRODUCT(LB, glm::outerProduct, gLuaVec4<>::fast); break;
    default: {
      break;
    }
  }
  return LUAGLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_VECTOR);
  GLM_BINDING_END
}
#endif

#if defined(EXT_MATRIX_CLIP_SPACE_HPP)
using gAspect = gPositiveConstraint<gLuaNumCoT, false, true>;  // @GLMAssert: assert(abs(aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));
using gFov = gPositiveConstraint<gLuaNumCoT, false, false>;  // @GLMAssert: assert(fov > static_cast<T>(0));
using gHeight = gFov;  // @GLMAssert: assert(height > static_cast<T>(0));
using gWidth = gFov;  // @GLMAssert: assert(width > static_cast<T>(0));
BIND_DEFN(frustum, glm::frustum, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(frustumLH, glm::frustumLH, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(frustumLH_NO, glm::frustumLH_NO, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(frustumLH_ZO, glm::frustumLH_ZO, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(frustumNO, glm::frustumNO, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(frustumRH, glm::frustumRH, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(frustumRH_NO, glm::frustumRH_NO, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(frustumRH_ZO, glm::frustumRH_ZO, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(frustumZO, glm::frustumZO, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(infinitePerspective, glm::infinitePerspective, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(infinitePerspectiveLH, glm::infinitePerspectiveLH, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(infinitePerspectiveRH, glm::infinitePerspectiveRH, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
GLM_BINDING_QUALIFIER(ortho) {
  GLM_BINDING_BEGIN
  if (LB.top() > 4)
    BIND_FUNC(LB, glm::ortho, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT);
  BIND_FUNC(LB, glm::ortho, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT);
  GLM_BINDING_END
}
BIND_DEFN(orthoLH, glm::orthoLH, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(orthoLH_NO, glm::orthoLH_NO, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(orthoLH_ZO, glm::orthoLH_ZO, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(orthoNO, glm::orthoNO, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(orthoRH, glm::orthoRH, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(orthoRH_NO, glm::orthoRH_NO, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(orthoRH_ZO, glm::orthoRH_ZO, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(orthoZO, glm::orthoZO, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspective, glm::perspective, gLuaNumCoT, gAspect, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspectiveLH, glm::perspectiveLH, gLuaNumCoT, gAspect, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspectiveLH_NO, glm::perspectiveLH_NO, gLuaNumCoT, gAspect, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspectiveLH_ZO, glm::perspectiveLH_ZO, gLuaNumCoT, gAspect, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspectiveNO, glm::perspectiveNO, gLuaNumCoT, gAspect, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspectiveRH, glm::perspectiveRH, gLuaNumCoT, gAspect, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspectiveRH_NO, glm::perspectiveRH_NO, gLuaNumCoT, gAspect, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspectiveRH_ZO, glm::perspectiveRH_ZO, gLuaNumCoT, gAspect, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspectiveZO, glm::perspectiveZO, gLuaNumCoT, gAspect, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspectiveFov, glm::perspectiveFov, gFov, gWidth, gHeight, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspectiveFovLH, glm::perspectiveFovLH, gFov, gWidth, gHeight, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspectiveFovLH_NO, glm::perspectiveFovLH_NO, gFov, gWidth, gHeight, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspectiveFovLH_ZO, glm::perspectiveFovLH_ZO, gFov, gWidth, gHeight, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspectiveFovNO, glm::perspectiveFovNO, gFov, gWidth, gHeight, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspectiveFovRH, glm::perspectiveFovRH, gFov, gWidth, gHeight, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspectiveFovRH_NO, glm::perspectiveFovRH_NO, gFov, gWidth, gHeight, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspectiveFovRH_ZO, glm::perspectiveFovRH_ZO, gFov, gWidth, gHeight, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(perspectiveFovZO, glm::perspectiveFovZO, gFov, gWidth, gHeight, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(tweakedInfinitePerspective, glm::tweakedInfinitePerspective, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT::eps_trait)
#endif

#if defined(EXT_MATRIX_TRANSFORM_HPP) || defined(GTX_MATRIX_TRANSFORM_2D_HPP)
GLM_BINDING_QUALIFIER(identity) {
  GLM_BINDING_BEGIN
  const lua_Integer size = LB.AsNextType<lua_Integer>();
  const lua_Integer secondary = LB.AsNextType<lua_Integer>();
  switch (LUAGLM_MATRIX_TYPE(size, secondary)) {
    case LUAGLM_MATRIX_2x2: BIND_RESULT(LB, glm::identity<gLuaMat2x2<>::type>());
    case LUAGLM_MATRIX_2x3: BIND_RESULT(LB, glm::identity<gLuaMat2x3<>::type>());
    case LUAGLM_MATRIX_2x4: BIND_RESULT(LB, glm::identity<gLuaMat2x4<>::type>());
    case LUAGLM_MATRIX_3x2: BIND_RESULT(LB, glm::identity<gLuaMat3x2<>::type>());
    case LUAGLM_MATRIX_3x3: BIND_RESULT(LB, glm::identity<gLuaMat3x3<>::type>());
    case LUAGLM_MATRIX_3x4: BIND_RESULT(LB, glm::identity<gLuaMat3x4<>::type>());
    case LUAGLM_MATRIX_4x2: BIND_RESULT(LB, glm::identity<gLuaMat4x2<>::type>());
    case LUAGLM_MATRIX_4x3: BIND_RESULT(LB, glm::identity<gLuaMat4x3<>::type>());
    case LUAGLM_MATRIX_4x4: BIND_RESULT(LB, glm::identity<gLuaMat4x4<>::type>());
    default: {
      break;
    }
  }
  return LUAGLM_ERROR(LB.L, GLM_INVALID_MAT_DIMENSIONS);
  GLM_BINDING_END
}

BIND_DEFN(lookAt, glm::lookAt, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>)
BIND_DEFN(lookAtLH, glm::lookAtLH, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>)
BIND_DEFN(lookAtRH, glm::lookAtRH, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>)
BIND_DEFN(lookRotation, glm::lookRotation, gLuaDir3<>, gLuaDir3<>)  // @GLMMatrixExtensions
BIND_DEFN(lookRotationRH, glm::lookRotationRH, gLuaDir3<>, gLuaDir3<>)
BIND_DEFN(lookRotationLH, glm::lookRotationLH, gLuaDir3<>, gLuaDir3<>)
BIND_DEFN(billboard, glm::billboard, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>, gLuaDir3<>)
BIND_DEFN(billboardRH, glm::billboardRH, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>, gLuaDir3<>)
BIND_DEFN(billboardLH, glm::billboardLH, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>, gLuaDir3<>)
#endif

#if defined(EXT_MATRIX_PROJECTION_HPP)
// @GLMAssert: assert(delta.x > static_cast<T>(0) && delta.y > static_cast<T>(0));
// glm::pickMatrix also sanitizes the parameters without asserts; a bit redundant.
using gPickDeltaValue = gPositiveConstraint<gLuaVec2<>, false>;
BIND_DEFN(pickMatrix, glm::pickMatrix, gLuaVec2<>, gPickDeltaValue, gLuaVec4<>)
BIND_DEFN(project, glm::project, gLuaVec3<>, gLuaMat4x4<>, gLuaMat4x4<>, gLuaVec4<>)
BIND_DEFN(projectNO, glm::projectNO, gLuaVec3<>, gLuaMat4x4<>, gLuaMat4x4<>, gLuaVec4<>)
BIND_DEFN(projectZO, glm::projectZO, gLuaVec3<>, gLuaMat4x4<>, gLuaMat4x4<>, gLuaVec4<>)
BIND_DEFN(unProject, glm::unProject, gLuaVec3<>, gLuaMat4x4<>, gLuaMat4x4<>, gLuaVec4<>)
BIND_DEFN(unProjectNO, glm::unProjectNO, gLuaVec3<>, gLuaMat4x4<>, gLuaMat4x4<>, gLuaVec4<>)
BIND_DEFN(unProjectZO, glm::unProjectZO, gLuaVec3<>, gLuaMat4x4<>, gLuaMat4x4<>, gLuaVec4<>)
BIND_DEFN(rayPicking, glm::rayPicking, gLuaVec3<>, gLuaVec3<>, gLuaFloat, gLuaFloat, gLuaFloat, gLuaFloat, gLuaFloat, gLuaFloat)  // @GLMVectorExtensions
BIND_DEFN(containsProjection, glm::containsProjection, gLuaMat4x4<>, gLuaMat4x4<>::eps_trait)  // @GLMMatrixExtensions
#endif

#if defined(GTC_MATRIX_ACCESS_HPP)  // @NOTE: These GLM functions are zero-based
#define LAYOUT_MATRIX_ACCESS_COLUMN(LB, F, Tr, ...) \
  LAYOUT_MATRIX_ACCESS(LB, F, Tr, Tr::col_type, Tr::row_type, gLuaTrait<glm::length_t>, ##__VA_ARGS__)
#define LAYOUT_MATRIX_ACCESS_ROW(LB, F, Tr, ...) \
  LAYOUT_MATRIX_ACCESS(LB, F, Tr, Tr::row_type, Tr::col_type, gLuaTrait<glm::length_t>, ##__VA_ARGS__)
#define LAYOUT_MATRIX_ACCESS(LB, F, Tr, TrComp, TrDim, TrIdx, ...)                  \
  LUA_MLM_BEGIN                                                                     \
  const lua_Integer _idx = gLuaBase::tointegerx((LB).L, (LB).idx + Tr::stack_size); \
  if (_idx < 0 || _idx >= cast(lua_Integer, TrDim::type::length()))                 \
    return LUAGLM_ARG_ERROR((LB).L, (LB).idx + Tr::stack_size, "matrix index");     \
  else if (LB.Is<TrComp>(Tr::stack_size + TrIdx::stack_size))                       \
    VA_CALL(BIND_FUNC, LB, F, Tr, TrIdx, TrComp, ##__VA_ARGS__); /* Set */          \
  VA_CALL(BIND_FUNC, LB, F, Tr, TrIdx, ##__VA_ARGS__); /* Get */                    \
  LUA_MLM_END

MATRIX_DEFN(column, glm::column, LAYOUT_MATRIX_ACCESS_COLUMN)
MATRIX_DEFN(row, glm::row, LAYOUT_MATRIX_ACCESS_ROW)
#endif

#if defined(GTC_MATRIX_INVERSE_HPP)
BINARY_LAYOUT_DEFN(affineInverse, glm::affineInverse, LAYOUT_UNARY, gLuaMat3x3<>, gLuaMat4x4<>)
SYMMETRIC_MATRIX_DEFN(inverseTranspose, glm::inverseTranspose, LAYOUT_UNARY)
#endif

#if defined(GTX_EULER_ANGLES_HPP)
#define LAYOUT_EULER_DECOMPOSE(LB, F, Tr, ...) \
  LUA_MLM_BEGIN                                \
  Tr::value_type a, b, c;                      \
  F((LB).Next<Tr>(), a, b, c);                 \
  BIND_PUSH(LB, a, b, c);                      \
  LUA_MLM_END

BIND_DEFN(derivedEulerAngleX, glm::derivedEulerAngleX, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(derivedEulerAngleY, glm::derivedEulerAngleY, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(derivedEulerAngleZ, glm::derivedEulerAngleZ, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleX, glm::eulerAngleX, gLuaNumCoT)
BIND_DEFN(eulerAngleXY, glm::eulerAngleXY, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleXYX, glm::eulerAngleXYX, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleXYZ, glm::eulerAngleXYZ, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleXZ, glm::eulerAngleXZ, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleXZX, glm::eulerAngleXZX, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleXZY, glm::eulerAngleXZY, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleY, glm::eulerAngleY, gLuaNumCoT)
BIND_DEFN(eulerAngleYX, glm::eulerAngleYX, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleYXY, glm::eulerAngleYXY, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleYXZ, glm::eulerAngleYXZ, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleYZ, glm::eulerAngleYZ, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleYZX, glm::eulerAngleYZX, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleYZY, glm::eulerAngleYZY, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleZ, glm::eulerAngleZ, gLuaNumCoT)
BIND_DEFN(eulerAngleZX, glm::eulerAngleZX, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleZXY, glm::eulerAngleZXY, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleZXZ, glm::eulerAngleZXZ, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleZY, glm::eulerAngleZY, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleZYX, glm::eulerAngleZYX, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(eulerAngleZYZ, glm::eulerAngleZYZ, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(orientate2, glm::orientate2, gLuaNumCoT)
BINARY_LAYOUT_DEFN(orientate3, glm::orientate3, LAYOUT_UNARY, gLuaFloat, gLuaVec3<>::fast)
BIND_DEFN(orientate4, glm::orientate4, gLuaVec3<>)
BIND_DEFN(yawPitchRoll, glm::yawPitchRoll, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
ROTATION_MATRIX_DEFN(extractEulerAngleXYX, glm::extractEulerAngleXYX, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleXYZ, glm::extractEulerAngleXYZ, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleXZX, glm::extractEulerAngleXZX, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleXZY, glm::extractEulerAngleXZY, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleYXY, glm::extractEulerAngleYXY, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleYXZ, glm::extractEulerAngleYXZ, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleYZX, glm::extractEulerAngleYZX, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleYZY, glm::extractEulerAngleYZY, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleZXY, glm::extractEulerAngleZXY, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleZXZ, glm::extractEulerAngleZXZ, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleZYX, glm::extractEulerAngleZYX, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleZYZ, glm::extractEulerAngleZYZ, LAYOUT_EULER_DECOMPOSE)
BIND_DEFN(quatEulerAngleX, glm::quatEulerAngleX, gLuaNumCoT)  // @GLMQuatExtensions
BIND_DEFN(quatEulerAngleXY, glm::quatEulerAngleXY, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(quatEulerAngleXYX, glm::quatEulerAngleXYX, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(quatEulerAngleXYZ, glm::quatEulerAngleXYZ, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(quatEulerAngleXZ, glm::quatEulerAngleXZ, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(quatEulerAngleXZX, glm::quatEulerAngleXZX, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(quatEulerAngleXZY, glm::quatEulerAngleXZY, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(quatEulerAngleY, glm::quatEulerAngleY, gLuaNumCoT)
BIND_DEFN(quatEulerAngleYX, glm::quatEulerAngleYX, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(quatEulerAngleYXY, glm::quatEulerAngleYXY, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(quatEulerAngleYXZ, glm::quatEulerAngleYXZ, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(quatEulerAngleYZ, glm::quatEulerAngleYZ, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(quatEulerAngleYZX, glm::quatEulerAngleYZX, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(quatEulerAngleYZY, glm::quatEulerAngleYZY, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(quatEulerAngleZ, glm::quatEulerAngleZ, gLuaNumCoT)
BIND_DEFN(quatEulerAngleZX, glm::quatEulerAngleZX, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(quatEulerAngleZXY, glm::quatEulerAngleZXY, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(quatEulerAngleZXZ, glm::quatEulerAngleZXZ, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(quatEulerAngleZY, glm::quatEulerAngleZY, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(quatEulerAngleZYX, glm::quatEulerAngleZYX, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
BIND_DEFN(quatEulerAngleZYZ, glm::quatEulerAngleZYZ, gLuaNumCoT, gLuaNumCoT, gLuaNumCoT)
#endif

#if defined(GTX_MATRIX_CROSS_PRODUCT_HPP)
BIND_DEFN(matrixCross3, glm::matrixCross3, gLuaVec3<>)
BIND_DEFN(matrixCross4, glm::matrixCross4, gLuaVec3<>)
#endif

#if defined(GTX_MATRIX_DECOMPOSE_HPP)
GLM_BINDING_QUALIFIER(decompose) {
  GLM_BINDING_BEGIN
  gLuaQuat<>::type orientation;
  gLuaVec4<>::type perspective;
  gLuaVec3<>::type scale, translation, skew;
  if (glm::decompose(LB.Next<gLuaMat4x4<>>(), scale, orientation, translation, skew, perspective))
    BIND_PUSH(LB, scale, orientation, translation, skew, perspective);
  return gLuaBase::Push(LB);
  GLM_BINDING_END
}
#endif

#if defined(GTX_MATRIX_FACTORISATION_HPP)
#define LAYOUT_QRDECOMPOSE(LB, F, Tr, ...) \
  LUA_MLM_BEGIN                            \
  Tr::q_type::type q; Tr::r_type::type r;  \
  F((LB).Next<Tr>(), q, r);                \
  BIND_PUSH(LB, q, r);                     \
  LUA_MLM_END

MATRIX_DEFN(fliplr, glm::fliplr, LAYOUT_UNARY)
MATRIX_DEFN(flipud, glm::flipud, LAYOUT_UNARY)
MATRIX_DEFN(qr_decompose, glm::qr_decompose, LAYOUT_QRDECOMPOSE)
MATRIX_DEFN(rq_decompose, glm::rq_decompose, LAYOUT_QRDECOMPOSE)
#endif

#if defined(GTX_MATRIX_INTERPOLATION_HPP)
#define LAYOUT_AXIS_ANGLE(LB, F, Tr, ...) \
  LUA_MLM_BEGIN                           \
  gLuaVec3<>::type axis;                  \
  gLuaVec3<>::value_type angle;           \
  F((LB).Next<Tr>(), axis, angle);        \
  BIND_PUSH(LB, axis, angle);             \
  LUA_MLM_END

BIND_DEFN(axisAngleMatrix, glm::axisAngleMatrix, gLuaDir3<>, gLuaFloat)
BIND_DEFN(extractMatrixRotation, glm::extractMatrixRotation, gLuaMat4x4<>)
BIND_DEFN(interpolate, glm::interpolate, gLuaMat4x4<>, gLuaMat4x4<>, gLuaFloat)
ROTATION_MATRIX_DEFN(axisAngle, glm::__axisAngle, LAYOUT_AXIS_ANGLE)
#endif

#if defined(GTX_MATRIX_MAJOR_STORAGE_HPP)
#define MATRIX_MAJOR_DEFN(Name, F, ArgLayout, Tr) \
  GLM_BINDING_QUALIFIER(Name) {                   \
    GLM_BINDING_BEGIN                             \
    if (LB.Is<Tr::col_type>())                    \
      ArgLayout(LB, F, Tr::col_type);             \
    BIND_RESULT(LB, F(LB.Next<Tr>()));            \
    GLM_BINDING_END                               \
  }

#define MATRIX_GENERAL_MAJOR_DEFN(Name, F)                                              \
  GLM_BINDING_QUALIFIER(Name) {                                                         \
    GLM_BINDING_BEGIN                                                                   \
    const TValue *o = LB.i2v();                                                         \
    switch (ttypetag(o)) {                                                              \
      case LUA_VVECTOR2: LAYOUT_BINARY(LB, F##2, gLuaVec2<>::fast); break;              \
      case LUA_VVECTOR3: LAYOUT_TERNARY(LB, F##3, gLuaVec3<>::fast); break;             \
      case LUA_VVECTOR4: LAYOUT_QUATERNARY(LB, F##4, gLuaVec4<>::fast); break;          \
      case LUA_VMATRIX: {                                                               \
        switch (mvalue_dims(o)) {                                                       \
          case LUAGLM_MATRIX_2x2: BIND_FUNC(LB, F##2, gLuaMat2x2<>::fast);              \
          case LUAGLM_MATRIX_3x3: BIND_FUNC(LB, F##3, gLuaMat3x3<>::fast);              \
          case LUAGLM_MATRIX_4x4: BIND_FUNC(LB, F##4, gLuaMat4x4<>::fast);              \
          default:                                                                      \
            break;                                                                      \
        }                                                                               \
        break;                                                                          \
      }                                                                                 \
      default:                                                                          \
        break;                                                                          \
    }                                                                                   \
    return LUAGLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_VECTOR " or " GLM_STRING_MATRIX); \
    GLM_BINDING_END                                                                     \
  }

MATRIX_MAJOR_DEFN(colMajor2, glm::colMajor2, LAYOUT_BINARY, gLuaMat2x2<>)
MATRIX_MAJOR_DEFN(colMajor3, glm::colMajor3, LAYOUT_TERNARY, gLuaMat3x3<>)
MATRIX_MAJOR_DEFN(colMajor4, glm::colMajor4, LAYOUT_QUATERNARY, gLuaMat4x4<>)
MATRIX_MAJOR_DEFN(rowMajor2, glm::rowMajor2, LAYOUT_BINARY, gLuaMat2x2<>)
MATRIX_MAJOR_DEFN(rowMajor3, glm::rowMajor3, LAYOUT_TERNARY, gLuaMat3x3<>)
MATRIX_MAJOR_DEFN(rowMajor4, glm::rowMajor4, LAYOUT_QUATERNARY, gLuaMat4x4<>)
MATRIX_GENERAL_MAJOR_DEFN(colMajor, glm::colMajor)  // @GLMMatrixExtensions
MATRIX_GENERAL_MAJOR_DEFN(rowMajor, glm::rowMajor)
#endif

#if defined(GTX_MATRIX_OPERATION_HPP)
#if GLM_VERSION >= 993  // @COMPAT: Added in 0.9.9.3
SYMMETRIC_MATRIX_DEFN(adjugate, glm::adjugate, LAYOUT_UNARY)
#endif
BIND_DEFN(diagonal2x2, glm::diagonal2x3, gLuaVec2<>)
BIND_DEFN(diagonal2x3, glm::diagonal2x3, gLuaVec2<>)
BIND_DEFN(diagonal2x4, glm::diagonal2x4, gLuaVec2<>)
BIND_DEFN(diagonal3x2, glm::diagonal3x2, gLuaVec2<>)
BIND_DEFN(diagonal3x3, glm::diagonal3x3, gLuaVec3<>)
BIND_DEFN(diagonal3x4, glm::diagonal3x4, gLuaVec3<>)
BIND_DEFN(diagonal4x2, glm::diagonal4x2, gLuaVec2<>)
BIND_DEFN(diagonal4x3, glm::diagonal4x3, gLuaVec3<>)
BIND_DEFN(diagonal4x4, glm::diagonal4x4, gLuaVec4<>)
MATRIX_DEFN(diagonal, glm::diagonal, LAYOUT_UNARY)  // @GLMMatrixExtensions
#endif

#if defined(GTX_MATRIX_QUERY_HPP)
MATRIX_DEFN(isIdentity, glm::_isIdentity, LAYOUT_BINARY_EPS)
MATRIX_DEFN(isOrthogonal, glm::isOrthogonal, LAYOUT_BINARY_EPS)
ROTATION_MATRIX_DEFN(extractScale, glm::extractScale, LAYOUT_UNARY)  // @GLMMatrixExtension
ROTATION_MATRIX_DEFN(hasUniformScale, glm::hasUniformScale, LAYOUT_BINARY_EPS)
#endif

#if defined(GTX_TRANSFORM2_HPP)
BIND_DEFN(proj2D, glm::proj2D, gLuaMat3x3<>, gLuaVec3<>)
BIND_DEFN(proj3D, glm::proj3D, gLuaMat4x4<>, gLuaVec3<>)
BIND_DEFN(shearX2D, glm::shearX2D, gLuaMat3x3<>, gLuaFloat)
BIND_DEFN(shearX3D, glm::shearX3D, gLuaMat4x4<>, gLuaFloat, gLuaFloat)
BIND_DEFN(shearY2D, glm::shearY2D, gLuaMat3x3<>, gLuaFloat)
BIND_DEFN(shearY3D, glm::shearY3D, gLuaMat4x4<>, gLuaFloat, gLuaFloat)
BIND_DEFN(shearZ3D, glm::shearZ3D, gLuaMat4x4<>, gLuaFloat, gLuaFloat)
GLM_BINDING_QUALIFIER(scaleBias) {
  GLM_BINDING_BEGIN
  if (LB.Is<gLuaMat4x4<>>())
    BIND_FUNC(LB, glm::__scaleBias, gLuaMat4x4<>::fast, gLuaFloat, gLuaFloat);
  BIND_FUNC(LB, glm::__scaleBias, gLuaNumCoT, gLuaNumCoT);
  GLM_BINDING_END
}
#endif

#if defined(GTX_MATRIX_TRANSFORM_2D_HPP)
BIND_DEFN(shearX, glm::shearX, gLuaMat3x3<>, gLuaMat3x3<>::value_trait)
BIND_DEFN(shearY, glm::shearY, gLuaMat3x3<>, gLuaMat3x3<>::value_trait)
#endif

#if defined(GTX_PCA_HPP)  // @COMPAT: pca.hpp introduced in 0.9.9.9
#include <glm/gtx/pca.hpp>
#define LAYOUT_FIND_EIGEN(LB, F, Tr, ...)                                         \
  LUA_MLM_BEGIN                                                                   \
  Tr::type outVectors = Tr::Zero();                                               \
  Tr::col_type::type outValues = Tr::col_type::Zero();                            \
  glm::length_t count = glm::length_t(F((LB).Next<Tr>(), outValues, outVectors)); \
  if (outValues.length() == count)                                                \
    glm::sortEigenvalues(outValues, outVectors);                                  \
  BIND_PUSH(LB, count, outValues, outVectors);                                    \
  LUA_MLM_END

#define LAYOUT_COMPUTE_COVARIANCE(LB, F, Mat, Cols, ...)                                                  \
  LUA_MLM_BEGIN                                                                                           \
  using Vec = Mat::col_type;                                                                              \
  gLuaArray<Vec> lArray((LB).L, (LB).idx++);                                                              \
  if (LB.Is<Vec>())                                                                                       \
    return gLuaBase::Push(LB, F<Cols, Mat::value_type, LUAGLM_BINDING_QUAL>(lArray.begin(), lArray.end(), (LB).Next<Vec::fast>())); \
  return gLuaBase::Push(LB, F<Cols, Mat::value_type, LUAGLM_BINDING_QUAL>(lArray.begin(), lArray.end())); \
  LUA_MLM_END

SYMMETRIC_MATRIX_DEFN(findEigenvaluesSymReal, glm::findEigenvaluesSymReal, LAYOUT_FIND_EIGEN)
GLM_BINDING_QUALIFIER(computeCovarianceMatrix) {
  GLM_BINDING_BEGIN
  luaL_checktype(L, LB.idx, LUA_TTABLE);
  lua_rawgeti(LB.L, LB.idx, 1);  // Determine array dimensions
  const glm::length_t dimensions = glm_vector_length(LB.L, -1);
  lua_pop(L, 1);

  switch (dimensions) {
    case 2: LAYOUT_COMPUTE_COVARIANCE(LB, glm::computeCovarianceMatrix, gLuaMat2x2<>, 2); break;
    case 3: LAYOUT_COMPUTE_COVARIANCE(LB, glm::computeCovarianceMatrix, gLuaMat3x3<>, 3); break;
    case 4: LAYOUT_COMPUTE_COVARIANCE(LB, glm::computeCovarianceMatrix, gLuaMat4x4<>, 4); break;
    default: {
      break;
    }
  }
  return LUAGLM_TYPE_ERROR(LB.L, LB.idx, "vector array");
  GLM_BINDING_END
}
#endif

/* }================================================================== */

/*
** {==================================================================
** OpenGL Mathematics API (everything else)
** ===================================================================
*/

#if defined(COMMON_HPP)
#define LAYOUT_FREXP(LB, F, Tr, ...)          \
  LUA_MLM_BEGIN                               \
  Tr::as_type<int>::type v2;                  \
  const Tr::type v3 = F((LB).Next<Tr>(), v2); \
  BIND_PUSH(LB, v3, v2);                      \
  LUA_MLM_END

#define LAYOUT_MODF(LB, F, Tr, ...)            \
  LUA_MLM_BEGIN                                \
  Tr::type v2;                                 \
  const Tr::type v3 = F((LB).Next<Tr>(), v2);  \
  const int _a = gLuaBase::PushNumInt(LB, v2); \
  return _a + BIND_PUSH_V(LB, v3);             \
  LUA_MLM_END

#define LAYOUT_UNARY_TOINT(LB, _, Tr, ...) \
  return gLuaBase::Push(LB, LB.Next<Tr::as_type<glm_Integer>>())
#define LAYOUT_UNARY_NUMINT(LB, F, Tr, ...) \
  return gLuaBase::PushNumInt(LB, F(LB.Next<Tr>()))

INTEGER_NUMBER_VECTOR_DEFN(abs, glm::abs, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fract, glm::fract, LAYOUT_UNARY)
#if GLM_HAS_CXX11_STL
BIND_DEFN(fma, glm::fma, gLuaNumber, gLuaNumber, gLuaNumber)
#else
NUMBER_VECTOR_DEFN(fma, glm::fma, LAYOUT_TERNARY)
#endif
INTEGER_VECTOR_DEFN(floatBitsToInt, glm::floatBitsToInt, float, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(floatBitsToUint, glm::floatBitsToUint, float, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(intBitsToFloat, glm::intBitsToFloat, int, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(uintBitsToFloat, glm::uintBitsToFloat, unsigned, LAYOUT_UNARY)
NUMBER_VECTOR_QUAT_DEFN(isinf, glm::isinf, LAYOUT_UNARY) /* glm/ext/quaternion_common.hpp */
NUMBER_VECTOR_QUAT_DEFN(isnan, glm::isnan, LAYOUT_UNARY) /* glm/ext/quaternion_common.hpp */
NUMBER_VECTOR_DEFN(round, glm::round, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(roundEven, glm::roundEven, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(smoothstep, glm::smoothstep, LAYOUT_TERNARY)
NUMBER_VECTOR_DEFN(step, glm::step, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(trunc, glm::trunc, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(ldexp, glm::ldexp, LAYOUT_BINARY_AS_INT)
NUMBER_VECTOR_DEFN(frexp, glm::frexp, LAYOUT_FREXP, int)
NUMBER_VECTOR_DEFN(reverse, glm::reverse, LAYOUT_UNARY)  // @GLMVectorExtensions
INTEGER_NUMBER_VECTOR_DEFN(mod, glm::imod, LAYOUT_MODULO)  // @MathlibCompat
INTEGER_NUMBER_VECTOR_DEFNS(ceil, glm::iceil, LAYOUT_UNARY, LAYOUT_UNARY_NUMINT, LAYOUT_UNARY)
INTEGER_NUMBER_VECTOR_DEFNS(floor, glm::ifloor, LAYOUT_UNARY, LAYOUT_UNARY_NUMINT, LAYOUT_UNARY)
#if GLM_HAS_CXX11_STL
NUMBER_VECTOR_DEFN(fdim, glm::fdim, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(hypot, glm::hypot, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(isnormal, glm::isnormal, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(isunordered, glm::isunordered, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(nearbyint, glm::nearbyint, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(nextafter, glm::nextafter, LAYOUT_BINARY)
#if LUA_FLOAT_TYPE == LUA_FLOAT_LONGDOUBLE
BIND_DEFN(nexttoward, std::nexttoward, gLuaNumber, gLuaTrait<long double>)
#endif
NUMBER_VECTOR_DEFN(remainder, glm::remainder, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(scalbn, glm::scalbn, LAYOUT_BINARY_AS_INT)
#endif

GLM_BINDING_QUALIFIER(modf) { /* @MathlibCompat */
  GLM_BINDING_BEGIN
  if (lua_isinteger(LB.L, LB.idx)) {
    lua_pushvalue(LB.L, LB.idx); /* number is its own integer part */
    lua_pushnumber(L, 0); /* no fractional part */
    return 2;
  }
  PARSE_NUMBER_VECTOR(LB, glm::modf, LAYOUT_MODF, LAYOUT_MODF);
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(toint) { /* @MathlibCompat */
  GLM_BINDING_BEGIN
  const TValue *o = LB.i2v();
  switch (ttypetag(o)) {
    case LUA_VVECTOR2: LAYOUT_UNARY_TOINT(LB, void, gLuaVec2<>);
    case LUA_VVECTOR3: LAYOUT_UNARY_TOINT(LB, void, gLuaVec3<>);
    case LUA_VVECTOR4: LAYOUT_UNARY_TOINT(LB, void, gLuaVec4<>);
    //case LUA_VMATRIX: PARSE_MATRIX(LB, mvalue_dims(o), void, LAYOUT_UNARY_TOINT); break;
    default: {
      int valid = 0;
      const lua_Integer n = lua_tointegerx(LB.L, LB.idx, &valid);
      if (l_likely(valid))
        lua_pushinteger(LB.L, n);
      else {
        luaL_checkany(LB.L, 1);
        luaL_pushfail(LB.L); /* value is not convertible to integer */
      }
      return 1;
    }
  }
  GLM_BINDING_END
}
#endif

#if defined(COMMON_HPP) || defined(EXT_SCALAR_COMMON_HPP) || defined(EXT_VECTOR_COMMON_HPP)
/* Accumulation for min/max functions, where arguments can be Tr or a primitive */
#define LAYOUT_MINMAX(LB, F, Tr, ...)               \
  LUA_MLM_BEGIN                                     \
  const int _n = (LB).top();                        \
  Tr::type base = (LB).Next<Tr>();                  \
  while ((LB).idx <= _n) {                          \
    if (LB.Is<Tr::value_trait>())                   \
      base = F(base, (LB).Next<Tr::value_trait>()); \
    else                                            \
      base = F(base, (LB).Next<Tr::safe>());        \
  }                                                 \
  return gLuaBase::Push(LB, base);                  \
  LUA_MLM_END

#define LAYOUT_CLAMP(LB, F, Tr, ...) /* glm::clamp */                                                                           \
  LUA_MLM_BEGIN /* <Tr, 0, 1>, <Tr, minVal, maxVal>, <Tr, TrMin, TrMax> */                                                      \
  if (gLuaBase::isnoneornil((LB).L, (LB).idx + Tr::stack_size) && gLuaBase::isnoneornil((LB).L, (LB).idx + 2 * Tr::stack_size)) \
    VA_CALL(BIND_FUNC, LB, F, Tr, ##__VA_ARGS__);                                                                               \
  else if (LB.Is<Tr::value_trait>(Tr::stack_size) && LB.Is<Tr::value_trait>(Tr::stack_size + Tr::value_trait::stack_size))      \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::value_trait, Tr::value_trait, ##__VA_ARGS__);                                             \
  VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::safe, ##__VA_ARGS__);                                                             \
  LUA_MLM_END

NUMBER_VECTOR_DEFN(fmin, glm::fmin, LAYOUT_MINMAX)
NUMBER_VECTOR_DEFN(fmax, glm::fmax, LAYOUT_MINMAX)
NUMBER_VECTOR_DEFN(fclamp, glm::fclamp, LAYOUT_CLAMP)
GLM_BINDING_QUALIFIER(clamp) {
  GLM_BINDING_BEGIN
  if (LB.Is<gLuaInteger>()) { /* support int-only values */
    if (LB.top() == 1)
      BIND_FUNC(LB, glm::clamp, gLuaInteger);
    else if (LB.Is<gLuaInteger>(gLuaInteger::stack_size) && LB.Is<gLuaInteger>(2 * gLuaInteger::stack_size))
      BIND_FUNC(LB, glm::clamp, gLuaInteger, gLuaInteger, gLuaInteger);
  }
  PARSE_NUMBER_VECTOR(LB, glm::clamp, LAYOUT_CLAMP, LAYOUT_CLAMP);
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(min) { /* @MathlibCompat */
  GLM_BINDING_BEGIN
  const int n = LB.top_for_recycle(); /* number of arguments */
  luaL_argcheck(L, n >= 1, 1, "value expected");

  const TValue *o = LB.i2v();
  if (ttisnumber(o) || cvt2num(o)) {
    int imin = 1; /* index of current minimum value */
    for (int i = 2; i <= n; i++) {
      if (lua_compare(L, i, imin, LUA_OPLT))
        imin = i;
    }
    lua_pushvalue(L, imin);
    return 1;
  }
  PARSE_NUMBER_VECTOR(LB, glm::min, LAYOUT_MINMAX, LAYOUT_MINMAX);
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(max) { /* @MathlibCompat */
  GLM_BINDING_BEGIN
  const int n = LB.top_for_recycle(); /* number of arguments */
  luaL_argcheck(L, n >= 1, 1, "value expected");

  const TValue *o = LB.i2v();
  if (ttisnumber(o) || cvt2num(o)) {
    int imax = 1; /* index of current maximum value */
    for (int i = 2; i <= n; i++) {
      if (lua_compare(L, imax, i, LUA_OPLT))
        imax = i;
    }
    lua_pushvalue(L, imax);
    return 1;
  }
  PARSE_NUMBER_VECTOR(LB, glm::max, LAYOUT_MINMAX, LAYOUT_MINMAX);
  GLM_BINDING_END
}
#endif

#if defined(COMMON_HPP) || defined(EXT_MATRIX_COMMON_HPP)
#define LAYOUT_MIX(LB, F, Tr, ...)                                           \
  LUA_MLM_BEGIN                                                              \
  if (LB.Is<gLuaTrait<bool>>(2 * Tr::stack_size))                            \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, gLuaTrait<bool>, ##__VA_ARGS__); \
  else if (LB.Is<Tr::value_trait>(2 * Tr::stack_size))                       \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::value_trait, ##__VA_ARGS__); \
  VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::safe, ##__VA_ARGS__);          \
  LUA_MLM_END

GLM_BINDING_QUALIFIER(mix) {
  GLM_BINDING_BEGIN
#if GLM_VERSION >= 994  // @COMPAT: ext/matrix_common.hpp introduced in 0.9.9.4
  const TValue *o = LB.i2v();
  if (ttismatrix(o))
    PARSE_SYMMETRIC_MATRIX(LB, mvalue_dims(o), glm::__mix, LAYOUT_TERNARY_OPTIONAL);
  else
#endif
  PARSE_NUMBER_VECTOR_QUAT(LB, glm::mix, LAYOUT_MIX, LAYOUT_MIX, LAYOUT_TERNARY_SCALAR);
  GLM_BINDING_END
}
#endif

#if defined(COMMON_HPP) || defined(GTX_LOG_BASE_HPP)
INTEGER_NUMBER_VECTOR_DEFN(sign, glm::sign, LAYOUT_UNARY)
INTEGER_NUMBER_VECTOR_DEFN(signP, glm::signP, LAYOUT_UNARY)  // @GLMVectorExtensions
INTEGER_NUMBER_VECTOR_DEFN(signN, glm::signN, LAYOUT_UNARY)
#if GLM_HAS_CXX11_STL
NUMBER_VECTOR_DEFN(copysign, glm::copysign, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(fpclassify, glm::fpclassify, LAYOUT_UNARY)
#endif
#endif

#if defined(EXPONENTIAL_HPP)
NUMBER_VECTOR_DEFN(exp2, glm::exp2, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(inversesqrt, glm::inversesqrt, LAYOUT_UNARY)
#if GLM_HAS_CXX11_STL
NUMBER_VECTOR_DEFN(expm1, glm::expm1, LAYOUT_UNARY)
#endif
NUMBER_VECTOR_DEFN(logistic, glm::logistic, LAYOUT_UNARY)  // @GLMVectorExtensions
#endif

#if defined(EXPONENTIAL_HPP) || defined(EXT_QUATERNION_EXPONENTIAL_HPP) || defined(GTX_LOG_BASE_HPP)
NUMBER_VECTOR_DEFN(log2, glm::log2, LAYOUT_UNARY)
NUMBER_VECTOR_QUAT_DEFN(exp, glm::exp, LAYOUT_UNARY)
NUMBER_VECTOR_QUAT_DEFN(sqrt, glm::sqrt, LAYOUT_UNARY)
NUMBER_VECTOR_QUAT_DEFNS(log, glm::log, LAYOUT_UNARY_OR_BINARY, LAYOUT_UNARY_OR_BINARY, LAYOUT_UNARY)
GLM_BINDING_QUALIFIER(pow) {
  GLM_BINDING_BEGIN
  if (LB.Is<gLuaInteger>() && LB.Is<gLuaTrait<unsigned>>(gLuaInteger::stack_size))
    BIND_FUNC(LB, glm::pow, gLuaInteger, gLuaTrait<unsigned>);
  PARSE_NUMBER_VECTOR_QUAT(LB, glm::pow, LAYOUT_BINARY_SCALAR, LAYOUT_BINARY_OPTIONAL, LAYOUT_BINARY_SCALAR);
  GLM_BINDING_END
}

#if GLM_HAS_CXX11_STL
NUMBER_VECTOR_DEFN(cbrt, glm::cbrt, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(log10, glm::log10, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(log1p, glm::log1p, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(logb, glm::logb, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(ilogb, glm::ilogb, LAYOUT_UNARY)
#endif
#endif

#if defined(EXPONENTIAL_HPP) || defined(GTX_EXTERIOR_PRODUCT_HPP) || defined(EXT_QUATERNION_GEOMETRIC_HPP)
GLM_BINDING_QUALIFIER(cross) {
  GLM_BINDING_BEGIN
  switch (ttypetag(LB.i2v())) {
    case LUA_VVECTOR2: BIND_FUNC(LB, glm::cross, gLuaVec2<>::fast, gLuaVec2<>); break; /* glm/gtx/exterior_product.hpp */
    case LUA_VVECTOR3: {
      if (LB.Is<gLuaQuat<>>(gLuaVec3<>::stack_size))
        BIND_FUNC(LB, glm::cross, gLuaVec3<>::fast, gLuaQuat<>::fast); /* glm/gtx/quaternion.hpp */
      BIND_FUNC(LB, glm::cross, gLuaVec3<>::fast, gLuaVec3<>); /* glm/geometric.hpp */
      break;
    }
    case LUA_VQUAT: { /* glm/gtx/quaternion.hpp */
      if (LB.Is<gLuaQuat<>>(gLuaQuat<>::stack_size))
        BIND_FUNC(LB, glm::cross, gLuaQuat<>::fast, gLuaQuat<>::fast);
      BIND_FUNC(LB, glm::cross, gLuaQuat<>::fast, gLuaVec3<>);
      break;
    }
    default: {
      break;
    }
  }
  return LUAGLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_VECTOR " or " GLM_STRING_QUATERN);
  GLM_BINDING_END
}
BIND_DEFN(crossXAxis, glm::crossXAxis, gLuaVec3<>)  // @GLMVectorExtensions
BIND_DEFN(crossYAxis, glm::crossYAxis, gLuaVec3<>)
BIND_DEFN(crossZAxis, glm::crossZAxis, gLuaVec3<>)
BIND_DEFN(xAxisCross, glm::xAxisCross, gLuaVec3<>)
BIND_DEFN(yAxisCross, glm::yAxisCross, gLuaVec3<>)
BIND_DEFN(zAxisCross, glm::zAxisCross, gLuaVec3<>)
#endif

#if defined(GEOMETRIC_HPP)
NUMBER_VECTOR_DEFN(distance, glm::distance, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(faceforward, glm::faceforward, LAYOUT_TERNARY)
NUMBER_VECTOR_DEFN(reflect, glm::reflect, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(refract, glm::refract, LAYOUT_TERNARY_SCALAR)
#endif

#if defined(GEOMETRIC_HPP) || defined(EXT_QUATERNION_GEOMETRIC_HPP)
NUMBER_VECTOR_QUAT_DEFN(dot, glm::dot, LAYOUT_BINARY)
NUMBER_VECTOR_QUAT_DEFN(length, glm::length, LAYOUT_UNARY)
NUMBER_VECTOR_QUAT_DEFN(normalize, glm::normalize, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(clampLength, glm::clampLength, LAYOUT_BINARY_SCALAR)  // @GLMVectorExtensions
NUMBER_VECTOR_DEFN(scaleLength, glm::scaleLength, LAYOUT_BINARY_SCALAR)
NUMBER_VECTOR_DEFN(direction, glm::direction, LAYOUT_BINARY)
BIND_DEFN(homogenize, glm::homogenize, gLuaVec4<>)
BIND_DEFN(dot3, glm::dot, gLuaVec4<>, gLuaVec3<>)
#endif

#if defined(VECTOR_RELATIONAL_HPP)
INTEGER_VECTOR_DEFN(all, glm::all, bool, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(any, glm::any, bool, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(not_, glm::not_, bool, LAYOUT_UNARY)
#endif

#if defined(VECTOR_RELATIONAL_HPP) || defined(GTC_QUATERNION_HPP)
NUMBER_VECTOR_QUAT_DEFN(greaterThan, glm::greaterThan, LAYOUT_BINARY)
NUMBER_VECTOR_QUAT_DEFN(greaterThanEqual, glm::greaterThanEqual, LAYOUT_BINARY)
NUMBER_VECTOR_QUAT_DEFN(lessThan, glm::lessThan, LAYOUT_BINARY)
NUMBER_VECTOR_QUAT_DEFN(lessThanEqual, glm::lessThanEqual, LAYOUT_BINARY)
INTEGER_VECTOR_DEFN(ult, glm::lessThan, lua_Unsigned, LAYOUT_BINARY)  // @MathlibCompat
INTEGER_VECTOR_DEFN(ulte, glm::lessThanEqual, lua_Unsigned, LAYOUT_BINARY)
NUMBER_VECTOR_QUAT_DEFN(all_greaterThan, glm::all_greaterThan, LAYOUT_BINARY)
NUMBER_VECTOR_QUAT_DEFN(all_greaterThanEqual, glm::all_greaterThanEqual, LAYOUT_BINARY)
NUMBER_VECTOR_QUAT_DEFN(all_lessThan, glm::all_lessThan, LAYOUT_BINARY)
NUMBER_VECTOR_QUAT_DEFN(all_lessThanEqual, glm::all_lessThanEqual, LAYOUT_BINARY)
#endif

#if defined(TRIGONOMETRIC_HPP)
#define LAYOUT_SINCOS(LB, F, Tr, ...) \
  LUA_MLM_BEGIN                       \
  Tr::type s, c;                      \
  F((LB).Next<Tr>(), s, c);           \
  BIND_PUSH(LB, s, c);                \
  LUA_MLM_END

NUMBER_VECTOR_DEFN(acos, glm::acos, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(acosh, glm::acosh, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(asin, glm::asin, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(asinh, glm::asinh, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(atan, glm::atan, LAYOUT_UNARY_OR_BINARY)
NUMBER_VECTOR_DEFN(atanh, glm::atanh, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(cos, glm::cos, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(cosh, glm::cosh, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(radians, glm::radians, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(degrees, glm::degrees, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(sin, glm::sin, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(sinh, glm::sinh, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(tan, glm::tan, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(tanh, glm::tanh, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(sincos, glm::sincos, LAYOUT_SINCOS)  // @GLMVectorExtensions
BIND_DEFN(fromAngle, glm::fromAngle, gLuaNumCoT)
#endif

#if defined(EXT_SCALAR_INTEGER_HPP) || defined(EXT_VECTOR_INTEGER_HPP)
#if GLM_VERSION >= 999  // @COMPAT: isMultiple fixed in 0.9.9.9
INTEGER_VECTOR_DEFN(isMultiple, glm::isMultiple, lua_Unsigned, LAYOUT_BINARY_SCALAR)
#endif
#if GLM_VERSION >= 996  // @COMPAT: Fixed in 0.9.9.6
INTEGER_VECTOR_DEFN(isPowerOfTwo, glm::isPowerOfTwo, lua_Unsigned, LAYOUT_UNARY)
#else
BIND_DEFN(isPowerOfTwo, glm::isPowerOfTwo, gLuaInteger)
#endif
#if GLM_VERSION >= 996  // @COMPAT: Added in 0.9.9.6
INTEGER_VECTOR_DEFN(nextMultiple, glm::nextMultiple, lua_Unsigned, LAYOUT_BINARY_OPTIONAL)
INTEGER_VECTOR_DEFN(nextPowerOfTwo, glm::nextPowerOfTwo, lua_Unsigned, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(prevMultiple, glm::prevMultiple, lua_Unsigned, LAYOUT_BINARY_OPTIONAL)
INTEGER_VECTOR_DEFN(prevPowerOfTwo, glm::prevPowerOfTwo, lua_Unsigned, LAYOUT_UNARY)
#endif
#endif

#if defined(GTC_EPSILON_HPP)
#define LAYOUT_EPSILON_EQUAL(LB, F, Tr, ...)                             \
  LUA_MLM_BEGIN                                                          \
  if (LB.Is<Tr>(2 * Tr::stack_size))                                     \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::safe, ##__VA_ARGS__);    \
  VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::eps_trait, ##__VA_ARGS__); \
  LUA_MLM_END

NUMBER_VECTOR_QUAT_DEFNS(epsilonEqual, glm::epsilonEqual, LAYOUT_TERNARY_EPS, LAYOUT_EPSILON_EQUAL, LAYOUT_TERNARY_EPS)
NUMBER_VECTOR_QUAT_DEFNS(epsilonNotEqual, glm::epsilonNotEqual, LAYOUT_TERNARY_EPS, LAYOUT_EPSILON_EQUAL, LAYOUT_TERNARY_EPS)
#endif

#if defined(GTC_INTEGER_HPP)
// @GLMAssert: assert(static_cast<genType>(0.0) <= x);
// @GLMAssert: assert(all(lessThanEqual(vec<L, T, Q>(0), x)));
#define LAYOUT_ROUND_BOUNDED(LB, F, Tr, ...) \
  VA_CALL(BIND_FUNC, LB, F, gPositiveConstraint<Tr>, ##__VA_ARGS__)
NUMBER_VECTOR_DEFN(iround, glm::iround, LAYOUT_ROUND_BOUNDED)
NUMBER_VECTOR_DEFN(uround, glm::uround, LAYOUT_ROUND_BOUNDED)
#endif

#if defined(GTC_RANDOM_HPP)
using gRandValue = gPositiveConstraint<gLuaNumber, false>;  // @GLMAssert: assert(Radius > static_cast<T>(0));
NUMBER_VECTOR_DEFN(linearRand, glm::linearRand, LAYOUT_BINARY)
BIND_DEFN(ballRand, glm::ballRand, gRandValue)
BIND_DEFN(circularRand, glm::circularRand, gRandValue)
BIND_DEFN(diskRand, glm::diskRand, gRandValue)
BIND_DEFN(gaussRand, glm::gaussRand, gRandValue, gRandValue)
BIND_DEFN(sphericalRand, glm::sphericalRand, gRandValue)
#endif

#if defined(GTC_RECIPROCAL_HPP)
NUMBER_VECTOR_DEFN(acot, glm::acot, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(acoth, glm::acoth, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(acsc, glm::acsc, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(acsch, glm::acsch, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(asec, glm::asec, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(asech, glm::asech, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(cot, glm::cot, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(coth, glm::coth, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(csc, glm::csc, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(csch, glm::csch, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(sec, glm::sec, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(sech, glm::sech, LAYOUT_UNARY)
#endif

#if defined(GTC_ROUND_HPP)
INTEGER_NUMBER_VECTOR_DEFN(ceilMultiple, glm::ceilMultiple, LAYOUT_MODULO)
INTEGER_NUMBER_VECTOR_DEFN(floorMultiple, glm::floorMultiple, LAYOUT_MODULO)
INTEGER_NUMBER_VECTOR_DEFN(roundMultiple, glm::roundMultiple, LAYOUT_MODULO)
INTEGER_VECTOR_DEFN(ceilPowerOfTwo, glm::ceilPowerOfTwo, lua_Unsigned, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(floorPowerOfTwo, glm::floorPowerOfTwo, lua_Unsigned, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(roundPowerOfTwo, glm::roundPowerOfTwo, lua_Unsigned, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(snap, glm::snap, LAYOUT_BINARY)  // @GLMVectorExtensions
#endif

#if defined(GTC_COLOR_SPACE_HPP) && !defined(GLM_FORCE_XYZW_ONLY)
NUMBER_VECTOR_DEFN(convertLinearToSRGB, glm::convertLinearToSRGB, LAYOUT_UNARY_OPTIONAL)
NUMBER_VECTOR_DEFN(convertSRGBToLinear, glm::convertSRGBToLinear, LAYOUT_UNARY_OPTIONAL)
#endif

#if defined(GTC_NOISE_HPP)
NUMBER_VECTOR_DEFN(perlin, glm::perlin, LAYOUT_UNARY_OR_BINARY)
NUMBER_VECTOR_DEFN(simplex, glm::simplex, LAYOUT_UNARY)
#endif

#if defined(GTX_ASSOCIATED_MIN_MAX_HPP)
#define LAYOUT_ASSOCIATED_OPTIONAL(LB, F, Tr, ...)                                     \
  LUA_MLM_BEGIN                                                                        \
  using S = Tr::safe;                                                                  \
  if (LB.Is<Tr>(6 * Tr::stack_size)) /* f(x,a,y,b,z,c,w,d) */                          \
    VA_CALL(BIND_FUNC, LB, F, Tr, S, S, S, S, S, S, S, ##__VA_ARGS__);                 \
  else if (LB.Is<Tr>(4 * Tr::stack_size)) /* f(x,a,y,b,z,c) */                         \
    VA_CALL(BIND_FUNC, LB, F, Tr, S, S, S, S, S, ##__VA_ARGS__);                       \
  else if (LB.Is<gLuaTrait<bool>>(Tr::stack_size)) /* f(x,a,y,b) */                    \
    VA_CALL(BIND_FUNC, LB, F, Tr, gLuaTrait<bool>, S, gLuaTrait<bool>, ##__VA_ARGS__); \
  else if (LB.Is<Tr::value_trait>(Tr::stack_size))                                     \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::value_trait, S, Tr::value_trait, ##__VA_ARGS__); \
  else                                                                                 \
    VA_CALL(BIND_FUNC, LB, F, Tr, S, S, S, ##__VA_ARGS__);                             \
  LUA_MLM_END

NUMBER_VECTOR_DEFN(associatedMin, glm::associatedMin, LAYOUT_ASSOCIATED_OPTIONAL)
NUMBER_VECTOR_DEFN(associatedMax, glm::associatedMax, LAYOUT_ASSOCIATED_OPTIONAL)
#endif

#if defined(GTX_CLOSEST_POINT_HPP)
NUMBER_VECTOR_DEFN(closestPointOnLine, glm::closestPointOnLine, LAYOUT_TERNARY)
#endif

#if defined(GTX_COLOR_ENCODING_HPP)
BIND_DEFN(convertD65XYZToD50XYZ, glm::convertD65XYZToD50XYZ, gLuaVec3<>)
BIND_DEFN(convertD65XYZToLinearSRGB, glm::convertD65XYZToLinearSRGB, gLuaVec3<>)
BIND_DEFN(convertLinearSRGBToD50XYZ, glm::convertLinearSRGBToD50XYZ, gLuaVec3<>)
BIND_DEFN(convertLinearSRGBToD65XYZ, glm::convertLinearSRGBToD65XYZ, gLuaVec3<>)
#endif

#if defined(GTX_COLOR_SPACE_HPP) && !defined(GLM_FORCE_XYZW_ONLY)
BIND_DEFN(hsvColor, glm::hsvColor, gLuaVec3<float>)
BIND_DEFN(luminosity, glm::luminosity, gLuaVec3<>)
BIND_DEFN(rgbColor, glm::rgbColor, gLuaVec3<>)
GLM_BINDING_QUALIFIER(saturation) {
  GLM_BINDING_BEGIN
  if (LB.Is<gLuaVec3<>>(gLuaFloat::stack_size)) BIND_FUNC(LB, glm::saturation, gLuaFloat, gLuaVec3<>::fast);
  if (LB.Is<gLuaVec4<>>(gLuaFloat::stack_size)) BIND_FUNC(LB, glm::saturation, gLuaFloat, gLuaVec4<>::fast);
  BIND_FUNC(LB, glm::saturation, gLuaNumCoT);
  GLM_BINDING_END
}
#endif

#if defined(GTX_COLOR_SPACE_YCOCG_HPP) && !defined(GLM_FORCE_XYZW_ONLY)
BIND_DEFN(rgb2YCoCg, glm::rgb2YCoCg, gLuaVec3<>)
BIND_DEFN(rgb2YCoCgR, glm::rgb2YCoCgR, gLuaVec3<>)
BIND_DEFN(YCoCg2rgb, glm::YCoCg2rgb, gLuaVec3<>)
BIND_DEFN(YCoCgR2rgb, glm::YCoCgR2rgb, gLuaVec3<>)
#endif

#if defined(GTX_COMMON_HPP)
NUMBER_VECTOR_DEFN(closeBounded, glm::closeBounded, LAYOUT_TERNARY)
NUMBER_VECTOR_DEFN(isdenormal, glm::isdenormal, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(openBounded, glm::openBounded, LAYOUT_TERNARY)
INTEGER_NUMBER_VECTOR_DEFN(fmod, glm::fmod, LAYOUT_MODULO)
#endif

#if defined(GTX_COMPATIBILITY_HPP)
NUMBER_VECTOR_DEFN(isfinite, glm::isfinite, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(atan2, glm::atan2, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(saturate, glm::saturate, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(all_isfinite, glm::all_isfinite, LAYOUT_UNARY)  // @GLMVectorExtensions
#endif

#if defined(GTX_COMPATIBILITY_HPP) || defined(EXT_QUATERNION_COMMON_HPP)
// @GLMAssert: assert(a >= static_cast<T>(0));
// @GLMAssert: assert(a <= static_cast<T>(1));
#define LAYOUT_QUAT_LERP(LB, F, Tr, ...) \
  VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, gRelativeConstraint<Tr::value_trait>, ##__VA_ARGS__)
NUMBER_VECTOR_QUAT_DEFNS(lerp, glm::lerp, LAYOUT_TERNARY_OPTIONAL, LAYOUT_TERNARY_OPTIONAL, LAYOUT_QUAT_LERP)
NUMBER_VECTOR_QUAT_DEFNS(nlerp, glm::nlerp, LAYOUT_TERNARY_OPTIONAL, LAYOUT_TERNARY_OPTIONAL, LAYOUT_TERNARY_SCALAR)
NUMBER_VECTOR_DEFN(lerpinverse, glm::lerpinverse, LAYOUT_TERNARY_OPTIONAL)
#endif

#if defined(GTX_COMPONENT_WISE_HPP)
INTEGER_NUMBER_VECTOR_DEFN(compAdd, glm::compAdd, LAYOUT_UNARY)
INTEGER_NUMBER_VECTOR_DEFN(compMax, glm::compMax, LAYOUT_UNARY)
INTEGER_NUMBER_VECTOR_DEFN(compMin, glm::compMin, LAYOUT_UNARY)
INTEGER_NUMBER_VECTOR_DEFN(compMul, glm::compMul, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(compNormalize, glm::compNormalize<glm_Float>, glm_Integer, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(compScale, glm::compScale<glm_Integer>, LAYOUT_UNARY)
/* glm::compNormalize/glm::compScale for different types, e.g., u8, i8, u16, i16 */
INTEGER_VECTOR_DEFN(compNormalize_i8, glm::compNormalize<glm_Float>, glm::i8, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(compNormalize_u8, glm::compNormalize<glm_Float>, glm::u8, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(compScale_i8, glm::compScale<glm::i8>, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(compScale_u8, glm::compScale<glm::u8>, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(compNormalize_i16, glm::compNormalize<glm_Float>, glm::i16, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(compNormalize_u16, glm::compNormalize<glm_Float>, glm::u16, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(compScale_i16, glm::compScale<glm::i16>, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(compScale_u16, glm::compScale<glm::u16>, LAYOUT_UNARY)
#endif

#if defined(GTX_EASING_HPP)
// @GLMAssert: assert(a >= zero<genType>());
// @GLMAssert: assert(a <= one<genType>());
using gEasingValue = gRelativeConstraint<gLuaNumber, true, true>;
LAYOUT_DEFN(backEaseIn, glm::backEaseIn, LAYOUT_UNARY_OR_BINARY, gEasingValue)
LAYOUT_DEFN(backEaseInOut, glm::backEaseInOut, LAYOUT_UNARY_OR_BINARY, gEasingValue)
LAYOUT_DEFN(backEaseOut, glm::backEaseOut, LAYOUT_UNARY_OR_BINARY, gEasingValue)
BIND_DEFN(bounceEaseIn, glm::bounceEaseIn, gEasingValue)
BIND_DEFN(bounceEaseInOut, glm::bounceEaseInOut, gEasingValue)
BIND_DEFN(bounceEaseOut, glm::bounceEaseOut, gEasingValue)
BIND_DEFN(circularEaseIn, glm::circularEaseIn, gEasingValue)
BIND_DEFN(circularEaseInOut, glm::circularEaseInOut, gEasingValue)
BIND_DEFN(circularEaseOut, glm::circularEaseOut, gEasingValue)
BIND_DEFN(cubicEaseIn, glm::cubicEaseIn, gEasingValue)
BIND_DEFN(cubicEaseInOut, glm::cubicEaseInOut, gEasingValue)
BIND_DEFN(cubicEaseOut, glm::cubicEaseOut, gEasingValue)
BIND_DEFN(elasticEaseIn, glm::elasticEaseIn, gEasingValue)
BIND_DEFN(elasticEaseInOut, glm::elasticEaseInOut, gEasingValue)
BIND_DEFN(elasticEaseOut, glm::elasticEaseOut, gEasingValue)
BIND_DEFN(exponentialEaseIn, glm::exponentialEaseIn, gEasingValue)
BIND_DEFN(exponentialEaseInOut, glm::exponentialEaseInOut, gEasingValue)
BIND_DEFN(exponentialEaseOut, glm::exponentialEaseOut, gEasingValue)
BIND_DEFN(linearInterpolation, glm::linearInterpolation, gEasingValue)
BIND_DEFN(quadraticEaseIn, glm::quadraticEaseIn, gEasingValue)
BIND_DEFN(quadraticEaseInOut, glm::quadraticEaseInOut, gEasingValue)
BIND_DEFN(quadraticEaseOut, glm::quadraticEaseOut, gEasingValue)
BIND_DEFN(quarticEaseIn, glm::quarticEaseIn, gEasingValue)
BIND_DEFN(quarticEaseInOut, glm::quarticEaseInOut, gEasingValue)
BIND_DEFN(quarticEaseOut, glm::quarticEaseOut, gEasingValue)
BIND_DEFN(quinticEaseIn, glm::quinticEaseIn, gEasingValue)
BIND_DEFN(quinticEaseInOut, glm::quinticEaseInOut, gEasingValue)
BIND_DEFN(quinticEaseOut, glm::quinticEaseOut, gEasingValue)
BIND_DEFN(sineEaseIn, glm::sineEaseIn, gEasingValue)
BIND_DEFN(sineEaseInOut, glm::sineEaseInOut, gEasingValue)
BIND_DEFN(sineEaseOut, glm::sineEaseOut, gEasingValue)
#endif

#if defined(GTX_EXTEND_HPP)
NUMBER_VECTOR_DEFN(extend, glm::extend, LAYOUT_TERNARY)
#endif

#if defined(GTX_FAST_EXPONENTIAL_HPP)
NUMBER_VECTOR_DEFN(fastExp, glm::fastExp, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastLog, glm::fastLog, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastPow, glm::fastPow, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(fastExp2, glm::fastExp2, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastLog2, glm::fastLog2, LAYOUT_UNARY)
#endif

#if defined(GTX_FAST_SQUARE_ROOT_HPP)
NUMBER_VECTOR_DEFN(fastDistance, glm::fastDistance, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(fastInverseSqrt, glm::fastInverseSqrt, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastLength, glm::fastLength, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastSqrt, glm::fastSqrt, LAYOUT_UNARY)
#if GLM_VERSION >= 999  // @COMPAT: fastNormalize ambiguity fixed in 0.9.9.9
NUMBER_VECTOR_QUAT_DEFN(fastNormalize, glm::fastNormalize, LAYOUT_UNARY)
#endif
#endif

#if defined(GTX_FAST_TRIGONOMETRY_HPP)
NUMBER_VECTOR_DEFN(fastAcos, glm::fastAcos, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastAsin, glm::fastAsin, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastAtan, glm::fastAtan, LAYOUT_UNARY_OR_BINARY)
NUMBER_VECTOR_DEFN(fastCos, glm::fastCos, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastSin, glm::fastSin, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastTan, glm::fastTan, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(wrapAngle, glm::wrapAngle, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(wrapAngle2, glm::wrapAngle2, LAYOUT_UNARY)  // @GLMVectorExtensions
#endif

#if defined(GTX_FUNCTIONS_HPP)
#define LAYOUT_SMOOTH_DAMP(LB, F, Tr, ...)                \
  LUA_MLM_BEGIN                                           \
  const Tr::type c = (LB).Next<Tr>();                     \
  const Tr::safe::type t = (LB).Next<Tr::safe>();         \
  Tr::safe::type cv = (LB).Next<Tr::safe>();              \
  const Tr::value_type st = (LB).Next<Tr::value_trait>(); \
  const Tr::value_type ms = (LB).Next<Tr::value_trait>(); \
  const Tr::value_type dt = (LB).Next<Tr::value_trait>(); \
  const Tr::type result = F(c, t, cv, st, ms, dt);        \
  BIND_PUSH(LB, result, cv);                              \
  LUA_MLM_END

BINARY_LAYOUT_DEFN(gauss, glm::gauss, LAYOUT_TERNARY, gLuaNumber, gLuaVec2<>::fast)
NUMBER_VECTOR_DEFN(smoothDamp, glm::smoothDamp, LAYOUT_SMOOTH_DAMP)  // @GLMVectorExtensions
NUMBER_VECTOR_DEFN(moveTowards, glm::moveTowards, LAYOUT_TERNARY_SCALAR)
GLM_BINDING_QUALIFIER(rotateTowards) {
  GLM_BINDING_BEGIN
  if (LB.Is<gLuaQuat<>>())
    BIND_FUNC(LB, glm::rotateTowards, gLuaQuat<>::fast, gLuaQuat<>, gLuaFloat);  // @GLMQuatExtensions
  BIND_FUNC(LB, glm::rotateTowards, gLuaVec3<>, gLuaVec3<>, gLuaFloat, gLuaFloat);
  GLM_BINDING_END
}
#if GLM_HAS_CXX11_STL
NUMBER_VECTOR_DEFN(erf, glm::erf, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(erfc, glm::erfc, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(lgamma, glm::lgamma, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(tgamma, glm::tgamma, LAYOUT_UNARY)
#endif
#endif

#if defined(GTX_GRADIENT_PAINT_HPP)
BIND_DEFN(linearGradient, glm::linearGradient, gLuaVec2<>, gLuaVec2<>, gLuaVec2<>)
BIND_DEFN(radialGradient, glm::radialGradient, gLuaVec2<>, gLuaFloat, gLuaVec2<>, gLuaVec2<>)
#endif

#if defined(GTX_HANDED_COORDINATE_SPACE_HPP)
BIND_DEFN(leftHanded, glm::leftHanded, gLuaDir3<>, gLuaDir3<>, gLuaDir3<>)
BIND_DEFN(rightHanded, glm::rightHanded, gLuaDir3<>, gLuaDir3<>, gLuaDir3<>)
#endif

#if defined(GTX_INTEGER_HPP)
BIND_DEFN(factorial, glm::factorial, gLuaInteger)
BIND_DEFN(nlz, glm::nlz, gLuaTrait<unsigned>)
#endif

#if defined(GTX_INTERSECT_HPP)
#define LAYOUT_INTERSECT_LINE_SPHERE(LB, F, Tr, ...)            \
  LUA_MLM_BEGIN                                                 \
  Tr::type v5, v6, v7, v8;                                      \
  const Tr::type v1 = (LB).Next<Tr>();                          \
  const Tr::safe::type v2 = (LB).Next<Tr::safe>();              \
  const Tr::safe::type v3 = (LB).Next<Tr::safe>();              \
  const Tr::value_type v4 = (LB).Next<Tr::value_trait>();       \
  if (glm::intersectLineSphere(v1, v2, v3, v4, v5, v6, v7, v8)) \
    BIND_PUSH(LB, true, v5, v6, v7, v8);                        \
  BIND_PUSH(LB, false);                                         \
  LUA_MLM_END

#define LAYOUT_INTERSECT_RAY_PLANE(LB, F, Tr, ...) \
  LUA_MLM_BEGIN                                    \
  Tr::value_type v5;                               \
  const Tr::type v1 = (LB).Next<Tr>();             \
  const Tr::safe::type v2 = (LB).Next<Tr::safe>(); \
  const Tr::safe::type v3 = (LB).Next<Tr::safe>(); \
  const Tr::safe::type v4 = (LB).Next<Tr::safe>(); \
  if (glm::intersectRayPlane(v1, v2, v3, v4, v5))  \
    BIND_PUSH(LB, true, v5);                       \
  BIND_PUSH(LB, false);                            \
  LUA_MLM_END

#define LAYOUT_INTERSECT_RAY_SPHERE(LB, F, Tr, ...)       \
  LUA_MLM_BEGIN                                           \
  Tr::type v5, v6;                                        \
  const Tr::type v1 = (LB).Next<Tr>();                    \
  const Tr::safe::type v2 = (LB).Next<Tr::safe>();        \
  const Tr::safe::type v3 = (LB).Next<Tr::safe>();        \
  const Tr::value_type v4 = (LB).Next<Tr::value_trait>(); \
  if (glm::intersectRaySphere(v1, v2, v3, v4, v5, v6))    \
    BIND_PUSH(LB, true, v5, v6);                          \
  BIND_PUSH(LB, false);                                   \
  LUA_MLM_END

NUMBER_VECTOR_DEFN(intersectLineSphere, glm::intersectLineSphere, LAYOUT_INTERSECT_LINE_SPHERE)
NUMBER_VECTOR_DEFN(intersectRayPlane, glm::intersectRayPlane, LAYOUT_INTERSECT_RAY_PLANE)
NUMBER_VECTOR_DEFN(intersectRaySphere, glm::intersectRaySphere, LAYOUT_INTERSECT_RAY_SPHERE)
GLM_BINDING_QUALIFIER(intersectLineTriangle) {
  GLM_BINDING_BEGIN
  gLuaVec3<float>::type v6;
  const gLuaVec3<float>::type v1 = LB.Next<gLuaVec3<float>>();
  const gLuaVec3<float>::type v2 = LB.Next<gLuaVec3<float>>();
  const gLuaVec3<float>::type v3 = LB.Next<gLuaVec3<float>>();
  const gLuaVec3<float>::type v4 = LB.Next<gLuaVec3<float>>();
  const gLuaVec3<float>::type v5 = LB.Next<gLuaVec3<float>>();
  if (glm::intersectLineTriangle(v1, v2, v3, v4, v5, v6))
    BIND_PUSH(LB, true, v6);
  BIND_PUSH(LB, false);
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(intersectRayTriangle) {
  GLM_BINDING_BEGIN
  gLuaVec2<>::type baryPosition;
  gLuaVec3<>::value_type distance;
  const gLuaVec3<>::type orig = LB.Next<gLuaVec3<>>();
  const gLuaVec3<>::type dir = LB.Next<gLuaVec3<>>();
  const gLuaVec3<>::type v0 = LB.Next<gLuaVec3<>>();
  const gLuaVec3<>::type v1 = LB.Next<gLuaVec3<>>();
  const gLuaVec3<>::type v2 = LB.Next<gLuaVec3<>>();
  if (glm::intersectRayTriangle(orig, dir, v0, v1, v2, baryPosition, distance))
    BIND_PUSH(LB, true, baryPosition, distance);
  BIND_PUSH(LB, false);
  GLM_BINDING_END
}
#endif

#if defined(GTX_MIXED_PRODUCT_HPP)
BIND_DEFN(mixedProduct, glm::mixedProduct, gLuaVec3<>, gLuaVec3<>, gLuaVec3<>)
#endif

#if defined(GTX_NORM_HPP)
NUMBER_VECTOR_DEFN(distance2, glm::distance2, LAYOUT_BINARY)
LAYOUT_DEFN(l1Norm, glm::l1Norm, LAYOUT_UNARY_OR_BINARY, gLuaVec3<>)
LAYOUT_DEFN(l2Norm, glm::l2Norm, LAYOUT_UNARY_OR_BINARY, gLuaVec3<>)
NUMBER_VECTOR_DEFN(length2, glm::length2, LAYOUT_UNARY) /* glm/gtx/quaternion.hpp */
#if GLM_VERSION >= 996  // @COMPAT: Added in 0.9.9.6
LAYOUT_DEFN(lMaxNorm, glm::lMaxNorm, LAYOUT_UNARY_OR_BINARY, gLuaVec3<>)
#endif
LAYOUT_DEFN(lxNorm, glm::lxNorm, LAYOUT_UNARY_OR_BINARY, gLuaVec3<>, gLuaTrait<unsigned>)
#endif

#if defined(GTX_NORMAL_HPP)
BIND_DEFN(triangleNormal, glm::triangleNormal, gLuaVec3<>, gLuaVec3<>, gLuaVec3<>)
#endif

#if defined(GTX_NORMAL_HPP) || defined(GTX_NORMALIZE_DOT_HPP)
NUMBER_VECTOR_DEFN(fastNormalizeDot, glm::fastNormalizeDot, LAYOUT_BINARY)
#endif

#if defined(GTX_NORMALIZE_DOT_HPP)
NUMBER_VECTOR_DEFN(normalizeDot, glm::normalizeDot, LAYOUT_BINARY)
#endif

#if defined(GTX_OPTIMUM_POW_HPP)
INTEGER_NUMBER_VECTOR_DEFN(pow2, glm::pow2, LAYOUT_UNARY)
INTEGER_NUMBER_VECTOR_DEFN(pow3, glm::pow3, LAYOUT_UNARY)
INTEGER_NUMBER_VECTOR_DEFN(pow4, glm::pow4, LAYOUT_UNARY)
#endif

#if defined(GTX_ORTHONORMALIZE_HPP)
GLM_BINDING_QUALIFIER(orthonormalize) {
  GLM_BINDING_BEGIN
  const TValue *o = LB.i2v();
  if (ttisvector3(o))
    BIND_FUNC(LB, glm::orthonormalize, gLuaVec3<>::fast, gLuaVec3<>);
  else if (ttismatrix(o) && mvalue_dims(o) == LUAGLM_MATRIX_3x3)
    BIND_FUNC(LB, glm::orthonormalize, gLuaMat3x3<>::fast);
  return LUAGLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_VECTOR3 " or " GLM_STRING_MATRIX "3x3");
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(orthonormalize3) {  // @GLMVectorExtensions
  GLM_BINDING_BEGIN
  gLuaVec3<>::type x = LB.Next<gLuaVec3<>>();
  gLuaVec3<>::type y = LB.Next<gLuaVec3<>>();
  if (LB.Is<gLuaVec3<>>()) {
    gLuaVec3<>::fast::type z = LB.Next<gLuaVec3<>::fast>();
    glm::orthonormalize3(x, y, z);
    BIND_PUSH(LB, x, y, z);
  }
  else {
    glm::orthonormalize2(x, y);
    BIND_PUSH(LB, x, y);
  }
  GLM_BINDING_END
}
BIND_DEFN(spherical_encode, glm::spherical_encode, gLuaVec3<>)
BIND_DEFN(spherical_decode, glm::spherical_decode, gLuaVec2<>)
BIND_DEFN(octahedron_encode, glm::octahedron_encode, gLuaVec3<>)
BIND_DEFN(octahedron_decode, glm::octahedron_decode, gLuaVec2<>)
#endif

#if defined(GTX_PERPENDICULAR_HPP)
#define LAYOUT_PERPBASIS(LB, F, Tr, ...) \
  LUA_MLM_BEGIN                          \
  Tr::type u, v;                         \
  F((LB).Next<Tr>(), u, v);              \
  BIND_PUSH(LB, u, v);                   \
  LUA_MLM_END

NUMBER_VECTOR_DEFN(perp, glm::perp, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(isPerpendicular, glm::isPerpendicular, LAYOUT_BINARY)  // @GLMVectorExtensions
LAYOUT_DEFN(perpendicular, glm::perpendicular, LAYOUT_UNARY_OR_TERNARY, gLuaDir3<>)
LAYOUT_DEFN(perpendicular2, glm::perpendicular2, LAYOUT_UNARY_OR_TERNARY, gLuaDir3<>)
LAYOUT_DEFN(perpendicularBasis, glm::perpendicularBasis, LAYOUT_PERPBASIS, gLuaDir3<>)
BINARY_LAYOUT_DEFN(perpendicularFast, glm::perpendicularFast, LAYOUT_UNARY, gLuaVec3<>::fast, gLuaVec2<>::fast)
#endif

#if defined(GTX_POLAR_COORDINATES_HPP)
BIND_DEFN(euclidean, glm::euclidean, gLuaVec2<>)
BIND_DEFN(polar, glm::polar, gLuaVec3<>)
#endif

#if defined(GTX_PROJECTION_HPP)
#define LAYOUT_DECOMPOSE(LB, F, Tr, ...)    \
  LUA_MLM_BEGIN                             \
  Tr::type q, r;                            \
  const Tr::type p = (LB).Next<Tr>();       \
  const Tr::type d = (LB).Next<Tr::safe>(); \
  F(p, d, q, r);                            \
  BIND_PUSH(LB, q, r);                      \
  LUA_MLM_END

NUMBER_VECTOR_DEFN(proj, glm::proj, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(projNorm, glm::projNorm, LAYOUT_BINARY)  // @GLMVectorExtensions
NUMBER_VECTOR_DEFN(projPlane, glm::projPlane, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(projDecompose, glm::projDecompose, LAYOUT_DECOMPOSE)
#endif

#if defined(GLM_HAS_RANGE_FOR) || defined(GTX_RANGE_HPP)
// GLM_BINDING_DECL(begin);
// GLM_BINDING_DECL(end);
GLM_BINDING_QUALIFIER(components) {  // An optimized variant of glm::components
  GLM_BINDING_BEGIN
  const TValue *o = LB.i2v();
  switch (ttype(o)) {
    case LUA_TVECTOR: BIND_RESULT(LB, glm_dimensions(ttypetag(o)));
    case LUA_TMATRIX: {
      gLuaBase::Push(LB, LUAGLM_MATRIX_COLS(mvalue_dims(o)));
      gLuaBase::Push(LB, LUAGLM_MATRIX_ROWS(mvalue_dims(o)));
      return 2;
    }
    default: {
      BIND_RESULT(LB, 1);
    }
  }
  GLM_BINDING_END
}
#endif

#if defined(GTX_ROTATE_VECTOR_HPP)
BIND_DEFN(orientation, glm::orientation, gLuaDir3<>, gLuaDir3<>)
BINARY_LAYOUT_DEFN(rotateX, glm::rotateX, LAYOUT_BINARY_SCALAR, gLuaVec3<>::fast, gLuaVec4<>::fast)
BINARY_LAYOUT_DEFN(rotateY, glm::rotateY, LAYOUT_BINARY_SCALAR, gLuaVec3<>::fast, gLuaVec4<>::fast)
BINARY_LAYOUT_DEFN(rotateZ, glm::rotateZ, LAYOUT_BINARY_SCALAR, gLuaVec3<>::fast, gLuaVec4<>::fast)
#endif

#if defined(GTX_ROTATE_VECTOR_HPP) || defined(EXT_QUATERNION_COMMON_HPP)
#if GLM_VERSION >= 998  // @COMPAT: slerp + 'additional spin count' introduced in 0.9.9.8
  #define LAYOUT_QUAT_SLERP(LB, F, Tr, ...)                                                    \
    LUA_MLM_BEGIN                                                                              \
    if (LB.Is<gLuaTrait<int>>((2 * Tr::stack_size) + Tr::value_trait::stack_size))             \
      VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::value_trait, gLuaTrait<int>, ##__VA_ARGS__); \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::value_trait, ##__VA_ARGS__);                   \
    LUA_MLM_END
#else
  #define LAYOUT_QUAT_SLERP(LB, F, Tr, ...) LAYOUT_TERNARY_SCALAR(LB, F, Tr, ##__VA_ARGS__)
#endif

NUMBER_VECTOR_QUAT_DEFNS(slerp, glm::__slerp, LAYOUT_TERNARY_SCALAR, LAYOUT_TERNARY_SCALAR, LAYOUT_QUAT_SLERP)
NUMBER_VECTOR_QUAT_DEFN(barycentric, glm::barycentric, LAYOUT_BARYCENTRIC)  // @GLMVectorExtensions
#endif

#if defined(GTX_ROTATE_VECTOR_HPP) || defined(EXT_MATRIX_TRANSFORM_HPP) || defined(GTX_MATRIX_TRANSFORM_2D_HPP) || defined(GTX_QUATERNION_TRANSFORM_HPP)
GLM_BINDING_QUALIFIER(rotate) {
  GLM_BINDING_BEGIN
  const TValue *o = LB.i2v();
  switch (ttypetag(o)) {
    case LUA_VFALSE: case LUA_VTRUE:  // @BoolCoercion
    case LUA_VSHRSTR: case LUA_VLNGSTR:  // @StringCoercion
    case LUA_VNUMINT:  // @IntCoercion
    case LUA_VNUMFLT: BIND_FUNC(LB, glm::rotate, gLuaFloat, gLuaVec3<>); break; /* glm/gtx/transform.hpp */
    case LUA_VVECTOR2: BIND_FUNC(LB, glm::rotate, gLuaVec2<>::fast, gLuaVec2<>::value_trait); break;
    case LUA_VVECTOR3: BIND_FUNC(LB, glm::rotate, gLuaVec3<>::fast, gLuaVec3<>::value_trait, gLuaDir3<>); break;
    case LUA_VVECTOR4: BIND_FUNC(LB, glm::rotate, gLuaVec4<>::fast, gLuaVec4<>::value_trait, gLuaDir3<>); break;
    case LUA_VQUAT: { /* glm/ext/quaternion_transform.hpp */
      const TValue *o2 = LB.i2v(gLuaQuat<>::stack_size);
      if (ttisnumber(o2))
        BIND_FUNC(LB, glm::rotate, gLuaQuat<>::fast, gLuaFloat::fast, gLuaDir3<>); /* <quat, angle, axis> */
      else if (ttisvector3(o2)) /* glm/gtx/quaternion.hpp */
        BIND_FUNC(LB, glm::rotate, gLuaQuat<>::fast, gLuaVec3<>::fast);
      else if (ttisvector4(o2)) /* glm/gtx/quaternion.hpp */
        BIND_FUNC(LB, glm::__rotate, gLuaQuat<>::fast, gLuaVec4<>::fast);  // @GLMFix
      return LUAGLM_ERROR(LB.L, "invalid arguments for rotate(glm::qua, ...)");
    }
    case LUA_VMATRIX: {
      switch (mvalue_dims(o)) {
        case LUAGLM_MATRIX_3x3: BIND_FUNC(LB, glm::rotate, gLuaMat3x3<>::fast, gLuaMat3x3<>::value_trait); break;
        case LUAGLM_MATRIX_4x4: BIND_FUNC(LB, glm::rotate, gLuaMat4x4<>::fast, gLuaMat4x4<>::value_trait, gLuaDir3<>); break;
        default: {
          break;
        }
      }
      return LUAGLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_MATRIX "3x3 or " GLM_STRING_MATRIX "4x4");
    }
    default: {
      break;
    }
  }
  return LUAGLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_VECTOR " or " GLM_STRING_QUATERN " or " GLM_STRING_MATRIX);
  GLM_BINDING_END
}

// BIND_DEFN(rotate_slow, glm::rotate_slow, gLuaMat4x4<>, gLuaMat4x4<>::value_trait, gLuaDir3<>)
BIND_DEFN(rotateFromTo, glm::rotateFromTo, gLuaVec3<>, gLuaVec3<>)  // @GLMQuatExtensions
BIND_DEFN(shortest_equivalent, glm::shortest_equivalent, gLuaQuat<>)
ROTATION_MATRIX_DEFN(transformDir, glm::transformDir, LAYOUT_UNARY, gLuaVec3<>)  // @GLMMatrixExtensions
ROTATION_MATRIX_DEFN(transformPos, glm::transformPos, LAYOUT_UNARY, gLuaVec3<>)
BIND_DEFN(transformPosPerspective, glm::transformPosPerspective, gLuaMat4x4<>, gLuaVec3<>)
#endif

#if defined(GTX_SPLINE_HPP)
NUMBER_VECTOR_DEFN(catmullRom, glm::catmullRom, LAYOUT_QUINARY_SCALAR)
NUMBER_VECTOR_DEFN(cubic, glm::cubic, LAYOUT_QUINARY_SCALAR)
NUMBER_VECTOR_DEFN(hermite, glm::hermite, LAYOUT_QUINARY_SCALAR)
#endif

#if defined(GTX_TEXTURE_HPP)
INTEGER_NUMBER_VECTOR_DEFN(levels, glm::levels, LAYOUT_UNARY)
#endif

#if defined(GTX_TRANSFORM_HPP) || defined(EXT_MATRIX_TRANSFORM_HPP)
#define MATRIX_TRANSFORM_DEFN(Name, F, ...)                                                \
  GLM_BINDING_QUALIFIER(Name) {                                                            \
    GLM_BINDING_BEGIN                                                                      \
    const TValue *o = LB.i2v();                                                            \
    switch (ttypetag(o)) {                                                                 \
      case LUA_VVECTOR3: BIND_FUNC(LB, F, gLuaVec3<>::fast); break;                        \
      case LUA_VMATRIX: {                                                                  \
        switch (mvalue_dims(o)) {                                                          \
          case LUAGLM_MATRIX_3x3: BIND_FUNC(LB, F, gLuaMat3x3<>::fast, gLuaVec2<>); break; \
          case LUAGLM_MATRIX_4x4: BIND_FUNC(LB, F, gLuaMat4x4<>::fast, gLuaVec3<>); break; \
          default: {                                                                       \
            break;                                                                         \
          }                                                                                \
        }                                                                                  \
        return LUAGLM_TYPE_ERROR((LB).L, (LB).idx, GLM_STRING_MATRIX);                     \
      }                                                                                    \
      default: {                                                                           \
        break;                                                                             \
      }                                                                                    \
    }                                                                                      \
    return LUAGLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_VECTOR3);                            \
    GLM_BINDING_END                                                                        \
  }

MATRIX_TRANSFORM_DEFN(scale, glm::scale)
MATRIX_TRANSFORM_DEFN(translate, glm::translate)
BIND_DEFN(trs, glm::trs, gLuaVec3<>, gLuaQuat<>, gLuaVec3<>)  // @GLMMatrixExtensions
BIND_DEFN(inverse_world_tensor, glm::inverse_world_tensor, gLuaVec3<>, gLuaMat3x3<>)
#endif

#if defined(GTX_VECTOR_ANGLE_HPP) || defined(EXT_QUATERNION_TRIGONOMETRIC_HPP)
#define ORIENTED_ANGLE_DEFN(Name, F, ...)                                                                           \
  GLM_BINDING_QUALIFIER(Name) {                                                                                     \
    GLM_BINDING_BEGIN                                                                                               \
    switch (ttypetag(LB.i2v())) {                                                                                   \
      case LUA_VVECTOR2: VA_CALL(BIND_FUNC, LB, F, gLuaDir2<>::fast, gLuaDir2<>, ##__VA_ARGS__); break;             \
      case LUA_VVECTOR3: VA_CALL(BIND_FUNC, LB, F, gLuaDir3<>::fast, gLuaDir3<>, gLuaDir3<>, ##__VA_ARGS__); break; \
      case LUA_VQUAT: VA_CALL(BIND_FUNC, LB, F, gLuaQuat<>::fast, gLuaQuat<>, gLuaDir3<>, ##__VA_ARGS__); break;    \
      default:                                                                                                      \
        break;                                                                                                      \
    }                                                                                                               \
    return LUAGLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_VECTOR);                                                      \
    GLM_BINDING_END                                                                                                 \
  }

NUMBER_VECTOR_QUAT_DEFNS(angle, glm::angle, LAYOUT_BINARY, LAYOUT_BINARY, LAYOUT_UNARY_OR_BINARY)
ORIENTED_ANGLE_DEFN(orientedAngle, glm::orientedAngle)
NUMBER_VECTOR_QUAT_DEFNS(angle_atan, glm::__angle, LAYOUT_BINARY, LAYOUT_BINARY, LAYOUT_UNARY_OR_BINARY)  // @GLMVectorExtensions
ORIENTED_ANGLE_DEFN(orientedAngle_atan, glm::__orientedAngle)
#endif

#if defined(GTX_VECTOR_QUERY_HPP)
NUMBER_VECTOR_DEFN(areCollinear, glm::areCollinear, LAYOUT_TERNARY_EPS)
NUMBER_VECTOR_DEFN(areOrthogonal, glm::areOrthogonal, LAYOUT_TERNARY_EPS)
NUMBER_VECTOR_DEFN(areOrthonormal, glm::areOrthonormal, LAYOUT_TERNARY_EPS)
NUMBER_VECTOR_DEFN(isCompNull, glm::isCompNull, LAYOUT_BINARY_EPS)
#endif

#if defined(GTX_VECTOR_QUERY_HPP) || defined(GTX_MATRIX_QUERY_HPP)
#define QUERY_DEFN(Name, F, FMat, ...) /* @GLMFix: use alternate definition for QUERY_HPP */  \
  GLM_BINDING_QUALIFIER(Name) {                                                               \
    GLM_BINDING_BEGIN /* Error message technically incorrect here. */                         \
    const TValue *o = LB.i2v();                                                               \
    if (ttismatrix(o))                                                                        \
      PARSE_MATRIX(LB, mvalue_dims(o), FMat, LAYOUT_BINARY_EPS);                              \
    PARSE_NUMBER_VECTOR_QUAT(LB, F, LAYOUT_BINARY_EPS, LAYOUT_BINARY_EPS, LAYOUT_BINARY_EPS); \
    GLM_BINDING_END                                                                           \
  }

QUERY_DEFN(isNormalized, glm::isNormalized, glm::_isNormalized)
QUERY_DEFN(isNull, glm::isNull, glm::_isNull)
NUMBER_VECTOR_DEFN(isUniform, glm::isUniform, LAYOUT_BINARY_EPS)  // @GLMVectorExtensions
#endif

#if defined(GTX_WRAP_HPP) || defined(EXT_SCALAR_COMMON_HPP)
NUMBER_VECTOR_DEFN(mirrorClamp, glm::mirrorClamp, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(mirrorRepeat, glm::mirrorRepeat, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(repeat, glm::repeat, LAYOUT_UNARY)
BIND_DEFN(deltaAngle, glm::deltaAngle, gLuaNumber, gLuaNumber)  // @GLMVectorExtensions
NUMBER_VECTOR_DEFN(wrap, glm::wrap, LAYOUT_BINARY_OPTIONAL)
NUMBER_VECTOR_DEFN(loopRepeat, glm::loopRepeat, LAYOUT_BINARY_OPTIONAL)
NUMBER_VECTOR_DEFN(pingPong, glm::pingPong, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(lerpAngle, glm::lerpAngle, LAYOUT_TERNARY_OPTIONAL)
#endif

/* }================================================================== */

/*
** {==================================================================
** <random>
** ===================================================================
*/
#if defined(STD_RANDOM) && GLM_HAS_CXX11_STL
#include <random>

#define RAND_TRAIT(LB, F, A, ...) RANDOM_DEVICE(LB, F, A) /* Avoid /Zc:preprocessor- headache */
#define RANDOM_DEVICE(...) VA_CALL(RANDOM_DEVICE, ##__VA_ARGS__) /* Mapping to <random> structures */
#define RANDOM_DEVICE1(LB) BIND_RESULT(LB, 0) /* zero */
#define RANDOM_DEVICE2(LB, F) BIND_RESULT(LB, F()(LB)) /* std::rand_func<>()(LB) */
#define RANDOM_DEVICE3(LB, F, A)            \
  LUA_MLM_BEGIN                             \
  if ((LB).top() > 0)                       \
    BIND_RESULT(LB, F((LB).Next<A>())(LB)); \
  BIND_RESULT(LB, F()(LB));                 \
  LUA_MLM_END

#define RANDOM_DEVICE4(LB, F, A, B)    \
  LUA_MLM_BEGIN                        \
  if ((LB).top() > 0) {                \
    const A::type _a = (LB).Next<A>(); \
    const B::type _b = (LB).Next<B>(); \
    BIND_RESULT(LB, F(_a, _b)(LB));    \
  }                                    \
  BIND_RESULT(LB, F()(LB));            \
  LUA_MLM_END

/* std::rand_func<>(a, b)(LB) | a <= b */
#define RANDOM_UNIFORM(LB, F, A, B, ...)                                         \
  LUA_MLM_BEGIN                                                                  \
  if ((LB).top() > 0) {                                                          \
    const A::type _a = (LB).Next<A>();                                           \
    const B::type _b = (LB).Next<B>();                                           \
    if (_a <= _b && (0 <= _a || _b <= _a + std::numeric_limits<A::type>::max())) \
      BIND_RESULT(LB, F(_a, _b)(LB));                                            \
    return LUAGLM_ERROR(LB.L, "invalid uniform_dist arguments");                 \
  }                                                                              \
  BIND_RESULT(LB, F()(LB));                                                      \
  LUA_MLM_END

using raNum = gLuaNumber;
template<typename T = raNum::type> using raAboveZero = gPositiveConstraint<gLuaTrait<T>, false>;  // 0.0 < _Ax0
template<typename T = raNum::type> using raAboveZeroInc = gPositiveConstraint<gLuaTrait<T>, true>;  // 0.0 <= _Ax0
template<typename T = raNum::type> using raProbability = gRelativeConstraint<gLuaTrait<T>, true, true>;  // 0.0 <= _Ax0 && _Ax0 <= 1.0
template<typename T = raNum::type> using raRelativeGeo = gRelativeConstraint<gLuaTrait<T>, false, false>;  // 0.0 < _Ax0 && _Ax0 < 1.0
template<typename T = raNum::type> using raNegativeBinorm = gRelativeConstraint<gLuaTrait<T>, false, true>;  // 0.0 < _Ax0 && _Ax0 <= 1.0

LAYOUT_DEFN(uniform_int, std::uniform_int_distribution<lua_Integer>, RANDOM_UNIFORM, gLuaInteger, gLuaInteger)
LAYOUT_DEFN(uniform_real, std::uniform_real_distribution<raNum::type>, RANDOM_UNIFORM, raAboveZeroInc<>, gLuaNumber)
LAYOUT_DEFN(bernoulli, std::bernoulli_distribution, RAND_TRAIT, raProbability<double>)
LAYOUT_DEFN(binomial, std::binomial_distribution<lua_Integer>, RANDOM_DEVICE, gPositiveConstraint<gLuaTrait<lua_Integer>>, raProbability<double>)
LAYOUT_DEFN(negative_binomial, std::negative_binomial_distribution<lua_Integer>, RANDOM_DEVICE, raAboveZero<lua_Integer>, raNegativeBinorm<double>)
LAYOUT_DEFN(geometric, std::geometric_distribution<lua_Integer>, RAND_TRAIT, raRelativeGeo<double>)
LAYOUT_DEFN(poisson, std::poisson_distribution<lua_Integer>, RAND_TRAIT, raAboveZero<double>)
LAYOUT_DEFN(exponential, std::exponential_distribution<raNum::type>, RAND_TRAIT, raAboveZero<>)
LAYOUT_DEFN(gamma, std::gamma_distribution<raNum::type>, RANDOM_DEVICE, raAboveZero<>, raAboveZero<>)
LAYOUT_DEFN(weibull, std::weibull_distribution<raNum::type>, RANDOM_DEVICE, raAboveZero<>, raAboveZero<>)
LAYOUT_DEFN(extreme_value, std::extreme_value_distribution<raNum::type>, RANDOM_DEVICE, raNum, raAboveZero<>)
LAYOUT_DEFN(normal, std::normal_distribution<raNum::type>, RANDOM_DEVICE, raNum, raAboveZero<>)
LAYOUT_DEFN(lognormal, std::lognormal_distribution<raNum::type>, RANDOM_DEVICE, raNum, raAboveZero<>)
LAYOUT_DEFN(chi_squared, std::chi_squared_distribution<raNum::type>, RAND_TRAIT, raAboveZero<>)
LAYOUT_DEFN(cauchy, std::cauchy_distribution<raNum::type>, RANDOM_DEVICE, raNum, raAboveZero<>)
LAYOUT_DEFN(fisher_f, std::fisher_f_distribution<raNum::type>, RANDOM_DEVICE, raAboveZero<>, raAboveZero<>)
LAYOUT_DEFN(student_t, std::student_t_distribution<raNum::type>, RAND_TRAIT, raAboveZero<>)
// discrete
// piecewise_constant_distribution
// piecewise_linear_distribution
#endif
/* }================================================================== */

#endif
