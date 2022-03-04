/*
** $Id: geom.hpp $
** Geometric Structures
**
** See Copyright Notice in lua.h
*/
#ifndef BINDING_GEOM_HPP
#define BINDING_GEOM_HPP

#include "lua.hpp"
#include "lglm.hpp"
#include "lglm_core.h"

#include "allocator.hpp"
#include "bindings.hpp"
#include "iterators.hpp"

#include "ext/geom/setup.hpp"
#include "ext/geom/aabb.hpp"
#include "ext/geom/line.hpp"
#include "ext/geom/linesegment.hpp"
#include "ext/geom/ray.hpp"
#include "ext/geom/triangle.hpp"
#include "ext/geom/sphere.hpp"
#include "ext/geom/plane.hpp"
#include "ext/geom/polygon.hpp"

/*
** {==================================================================
** Traits
** ===================================================================
*/

/// <summary>
/// Relative position along a line, segment, ray for casting.
/// </summary>
template<bool isNear, bool isRelative, typename T = glm_Float>
struct gLuaParametric : gLuaAbstractTrait<T, T> {
  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() {
    return "RelativePosition";
  }

  LUA_BIND_QUALIFIER LUA_CONSTEXPR_STATEMENT T Zero() {
    LUA_IF_CONSTEXPR(isNear)
      return isRelative ? T(0) : -std::numeric_limits<T>::infinity();
    return isRelative ? T(1) : std::numeric_limits<T>::infinity();
  }

  LUA_BIND_QUALIFIER bool Is(lua_State *L_, int idx) {
    return gLuaBase::isnoneornil(L_, idx) || gLuaTrait<T>::Is(L_, idx);
  }

  LUA_BIND_QUALIFIER T Next(lua_State *L, int &idx) {
    if (gLuaBase::isnoneornil(L, idx)) {
      idx++;  // Skip the argument
      return Zero();
    }
    return gLuaTrait<T>::Next(L, idx);
  }
};

template<glm::length_t L, typename T, glm::qualifier Q, bool FastPath>
struct gLuaTrait<glm::AABB<L, T, Q>, FastPath> : gLuaAbstractTrait<glm::AABB<L, T, Q>> {
  template<typename Type = T>
  using as_type = gLuaTrait<glm::AABB<L, Type, Q>>;  // @CastBinding

  /// <summary>
  /// @PointBinding: Type trait equivalent to glm::Structure::point_type
  /// </summary>
  using point_trait = gLuaTrait<typename glm::AABB<L, T, Q>::point_type>;
  static const LUA_CONSTEXPR int stack_size = (2 * point_trait::stack_size);  // @StackSize

  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() {
    return "AABB";
  }

  LUA_BIND_QUALIFIER bool Is(lua_State *L_, int idx) {
    return point_trait::Is(L_, idx) && point_trait::Is(L_, idx + point_trait::stack_size);
  }

  LUA_BIND_QUALIFIER glm::AABB<L, T, Q> Next(lua_State *L_, int &idx) {
    glm::AABB<L, T, Q> result;
    result.minPoint = point_trait::Next(L_, idx);
    result.maxPoint = point_trait::Next(L_, idx);
    return result;
  }

  LUA_BIND_QUALIFIER int Push(const gLuaBase &LB, const glm::AABB<L, T, Q> &v) {
    point_trait::Push(LB, v.minPoint);
    point_trait::Push(LB, v.maxPoint);
    return 2;
  }
};

template<glm::length_t L, typename T, glm::qualifier Q, bool FastPath>
struct gLuaTrait<glm::Line<L, T, Q>, FastPath> : gLuaAbstractTrait<glm::Line<L, T, Q>> {
  template<typename Type = T>
  using as_type = gLuaTrait<glm::Line<L, Type, Q>>;  // @CastBinding
  using point_trait = gLuaTrait<typename glm::Line<L, T, Q>::point_type>;  // @PointBinding
  static const LUA_CONSTEXPR int stack_size = (2 * point_trait::stack_size);  // @StackSize

  /// <summary>
  /// @RelativeZero: Lua type trait representing the relative negative-inf/zero
  /// coordinate of the object.
  /// </summary>
  using zero_trait = gLuaParametric<true, false, T>;

  /// <summary>
  /// @RelativeOne: Lua type trait representing the relative inf/one coordinate
  /// of the object.
  /// </summary>
  using one_trait = gLuaParametric<false, false, T>;

  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() {
    return "Line";
  }

  LUA_BIND_QUALIFIER bool Is(lua_State *L_, int idx) {
    return point_trait::Is(L_, idx) && point_trait::Is(L_, idx + point_trait::stack_size);
  }

  LUA_BIND_QUALIFIER glm::Line<L, T, Q> Next(lua_State *L_, int &idx) {
    glm::Line<L, T, Q> result;
    result.pos = point_trait::Next(L_, idx);
    result.dir = point_trait::Next(L_, idx);
    return result;
  }

  LUA_BIND_QUALIFIER int Push(const gLuaBase &LB, const glm::Line<L, T, Q> &l) {
    point_trait::Push(LB, l.pos);
    point_trait::Push(LB, l.dir);
    return 2;
  }
};

template<glm::length_t L, typename T, glm::qualifier Q, bool FastPath>
struct gLuaTrait<glm::LineSegment<L, T, Q>, FastPath> : gLuaAbstractTrait<glm::LineSegment<L, T, Q>> {
  template<typename Type = T>
  using as_type = gLuaTrait<glm::LineSegment<L, Type, Q>>;  // @CastBinding
  using point_trait = gLuaTrait<typename glm::LineSegment<L, T, Q>::point_type>;  // @PointBinding
  using zero_trait = gLuaParametric<true, true, T>;  // @RelativeZero
  using one_trait = gLuaParametric<false, true, T>;  // @RelativeOne
  static const LUA_CONSTEXPR int stack_size = (2 * point_trait::stack_size);  // @StackSize

  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() {
    return "Segment";
  }

  LUA_BIND_QUALIFIER bool Is(lua_State *L_, int idx) {
    return point_trait::Is(L_, idx) && point_trait::Is(L_, idx + point_trait::stack_size);
  }

  LUA_BIND_QUALIFIER glm::LineSegment<L, T, Q> Next(lua_State *L_, int &idx) {
    glm::LineSegment<L, T, Q> result;
    result.a = point_trait::Next(L_, idx);
    result.b = point_trait::Next(L_, idx);
    return result;
  }

  LUA_BIND_QUALIFIER int Push(const gLuaBase &LB, const glm::LineSegment<L, T, Q> &l) {
    point_trait::Push(LB, l.a);
    point_trait::Push(LB, l.b);
    return 2;
  }
};

template<glm::length_t L, typename T, glm::qualifier Q, bool FastPath>
struct gLuaTrait<glm::Ray<L, T, Q>, FastPath> : gLuaAbstractTrait<glm::Ray<L, T, Q>> {
  template<typename Type = T>
  using as_type = gLuaTrait<glm::Ray<L, Type, Q>>;  // @CastBinding
  using point_trait = gLuaTrait<typename glm::Ray<L, T, Q>::point_type>;  // @PointBinding
  using zero_trait = gLuaParametric<true, true, T>;  // @RelativeZero
  using one_trait = gLuaParametric<false, false, T>;  // @RelativeOne
  static const LUA_CONSTEXPR int stack_size = (2 * point_trait::stack_size);  // @StackSize

  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() {
    return "Ray";
  }

  LUA_BIND_QUALIFIER bool Is(lua_State *L_, int idx) {
    return point_trait::Is(L_, idx) && point_trait::Is(L_, idx + point_trait::stack_size);
  }

  LUA_BIND_QUALIFIER glm::Ray<L, T, Q> Next(lua_State *L_, int &idx) {
    glm::Ray<L, T, Q> result;
    result.pos = point_trait::Next(L_, idx);
    result.dir = point_trait::Next(L_, idx);
    return result;
  }

  LUA_BIND_QUALIFIER int Push(const gLuaBase &LB, const glm::Ray<L, T, Q> &r) {
    point_trait::Push(LB, r.pos);
    point_trait::Push(LB, r.dir);
    return 2;
  }
};

template<glm::length_t L, typename T, glm::qualifier Q, bool FastPath>
struct gLuaTrait<glm::Triangle<L, T, Q>, FastPath> : gLuaAbstractTrait<glm::Triangle<L, T, Q>> {
  template<typename Type = T>
  using as_type = gLuaTrait<glm::Triangle<L, Type, Q>>;  // @CastBinding
  using point_trait = gLuaTrait<typename glm::Triangle<L, T, Q>::point_type>;  // @PointBinding
  static const LUA_CONSTEXPR int stack_size = (3 * point_trait::stack_size);  // @StackSize

  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() {
    return "Triangle";
  }

  LUA_BIND_QUALIFIER bool Is(lua_State *L_, int idx) {
    return point_trait::Is(L_, idx)
           && point_trait::Is(L_, idx + point_trait::stack_size)
           && point_trait::Is(L_, idx + 2 * point_trait::stack_size);
  }

  LUA_BIND_QUALIFIER glm::Triangle<L, T, Q> Next(lua_State *L_, int &idx) {
    glm::Triangle<L, T, Q> result;
    result.a = point_trait::Next(L_, idx);
    result.b = point_trait::Next(L_, idx);
    result.c = point_trait::Next(L_, idx);
    return result;
  }

  LUA_BIND_QUALIFIER int Push(const gLuaBase &LB, const glm::Triangle<L, T, Q> &t) {
    point_trait::Push(LB, t.a);
    point_trait::Push(LB, t.b);
    point_trait::Push(LB, t.c);
    return 3;
  }
};

template<glm::length_t L, typename T, glm::qualifier Q, bool FastPath>
struct gLuaTrait<glm::Sphere<L, T, Q>, FastPath> : gLuaAbstractTrait<glm::Sphere<L, T, Q>> {
  template<typename Type = T>
  using as_type = gLuaTrait<glm::Sphere<L, Type, Q>>;  // @CastBinding
  using point_trait = gLuaTrait<typename glm::Sphere<L, T, Q>::point_type>;  // @PointBinding
  static const LUA_CONSTEXPR int stack_size = (point_trait::stack_size + point_trait::value_trait::stack_size);  // @StackSize

  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() {
    return "Sphere";
  }

  LUA_BIND_QUALIFIER bool Is(lua_State *L_, int idx) {
    return point_trait::Is(L_, idx) && gLuaTrait<T>::Is(L_, idx + point_trait::stack_size);
  }

  LUA_BIND_QUALIFIER glm::Sphere<L, T, Q> Next(lua_State *L_, int &idx) {
    glm::Sphere<L, T, Q> result;
    result.pos = point_trait::Next(L_, idx);
    result.r = point_trait::value_trait::Next(L_, idx);
    return result;
  }

  LUA_BIND_QUALIFIER int Push(const gLuaBase &LB, const glm::Sphere<L, T, Q> &s) {
    point_trait::Push(LB, s.pos);
    point_trait::value_trait::Push(LB, s.r);
    return 2;
  }
};

template<glm::length_t L, typename T, glm::qualifier Q, bool FastPath>
struct gLuaTrait<glm::Plane<L, T, Q>, FastPath> : gLuaAbstractTrait<glm::Plane<L, T, Q>> {
  template<typename Type = T>
  using as_type = gLuaTrait<glm::Plane<L, Type, Q>>;  // @CastBinding
  using point_trait = gLuaTrait<typename glm::Plane<L, T, Q>::point_type>;  // @PointBinding
  static const LUA_CONSTEXPR int stack_size = (point_trait::stack_size + point_trait::value_trait::stack_size);  // @StackSize

  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() {
    return "Plane";
  }

  LUA_BIND_QUALIFIER bool Is(lua_State *L_, int idx) {
    return point_trait::Is(L_, idx) && gLuaTrait<T>::Is(L_, idx + point_trait::stack_size);
  }

  LUA_BIND_QUALIFIER glm::Plane<L, T, Q> Next(lua_State *L_, int &idx) {
    glm::Plane<L, T, Q> result;
    result.normal = point_trait::Next(L_, idx);
    result.d = point_trait::value_trait::Next(L_, idx);
    return result;
  }

  LUA_BIND_QUALIFIER int Push(const gLuaBase &LB, const glm::Plane<L, T, Q> &p) {
    point_trait::Push(LB, p.normal);
    point_trait::value_trait::Push(LB, p.d);
    return 2;
  }
};

/// <summary>
/// An (explicitly three dimensional) polygon trait.
/// </summary>
template<typename T, glm::qualifier Q, bool FastPath>
struct gLuaTrait<glm::Polygon<3, T, Q>, FastPath> : gLuaAbstractTrait<glm::Polygon<3, T, Q>> {
  template<typename Type = T>
  using as_type = gLuaTrait<glm::Polygon<3, Type, Q>>;  // @CastBinding
  using point_trait = gLuaTrait<typename glm::Polygon<3, T, Q>::point_type>;  // @PointBinding

  /// <summary>
  /// Global metatable name for Polygon userdata
  /// </summary>
  static LUA_CONSTEXPR const char *Metatable() {
    return "GLM_POLYGON";
  }

  LUA_BIND_DECL LUA_CONSTEXPR const char *Label() {
    return "Polygon";
  }

  LUA_BIND_QUALIFIER GLM_CONSTEXPR glm::Polygon<3, T, Q> Zero() {
    return glm::Polygon<3, T, Q>(GLM_NULLPTR);
  }

  LUA_BIND_QUALIFIER bool Is(lua_State *L, int idx) {
    return luaL_testudata(L, idx, Metatable()) != GLM_NULLPTR;
  }

  LUA_BIND_QUALIFIER glm::Polygon<3, T, Q> Next(lua_State *L_, int &idx) {
    void *ptr = GLM_NULLPTR;
    if ((ptr = luaL_checkudata(L_, idx, Metatable())) != GLM_NULLPTR) {
      glm::Polygon<3, T, Q> result = *(static_cast<glm::Polygon<3, T, Q> *>(ptr));
      result.stack_idx = idx++;
      result.p->validate(L_);
      return result;
    }
    else {
      gLuaBase::error(L_, "Invalid polygon userdata");
    }
    return glm::Polygon<3, T, Q>();
  }

  LUA_BIND_QUALIFIER int Push(const gLuaBase &LB, const glm::Polygon<3, T, Q> &p) {
    // All operations mutate the referenced Polygon userdata; push it back onto
    // the Lua stack.
    if (l_likely(p.stack_idx >= 1)) {
      lua_pushvalue(LB.L, p.stack_idx);
      return 1;
    }

    // This code-path is not implemented for the time being. All polygons must
    // already exist on the Lua stack, otherwise, polygon_new will need to be
    // duplicated/called here.
    return LUAGLM_ERROR(LB.L, "not implemented");
  }
};

template<glm::length_t L = 3, typename T = glm_Float, glm::qualifier Q = LUAGLM_BINDING_QUAL>
using gLuaAABB = gLuaTrait<glm::AABB<L, T, Q>>;

template<glm::length_t L = 3, typename T = glm_Float, glm::qualifier Q = LUAGLM_BINDING_QUAL>
using gLuaLine = gLuaTrait<glm::Line<L, T, Q>>;

template<glm::length_t L = 3, typename T = glm_Float, glm::qualifier Q = LUAGLM_BINDING_QUAL>
using gLuaSegment = gLuaTrait<glm::LineSegment<L, T, Q>>;

template<glm::length_t L = 3, typename T = glm_Float, glm::qualifier Q = LUAGLM_BINDING_QUAL>
using gLuaRay = gLuaTrait<glm::Ray<L, T, Q>>;

template<glm::length_t L = 3, typename T = glm_Float, glm::qualifier Q = LUAGLM_BINDING_QUAL>
using gLuaTriangle = gLuaTrait<glm::Triangle<L, T, Q>>;

template<glm::length_t L = 3, typename T = glm_Float, glm::qualifier Q = LUAGLM_BINDING_QUAL>
using gLuaSphere = gLuaTrait<glm::Sphere<L, T, Q>>;

template<glm::length_t L = 3, typename T = glm_Float, glm::qualifier Q = LUAGLM_BINDING_QUAL>
using gLuaPlane = gLuaTrait<glm::Plane<L, T, Q>>;

template<typename T = glm_Float, glm::qualifier Q = LUAGLM_BINDING_QUAL>
using gLuaPolygon = gLuaTrait<glm::Polygon<3, T, Q>>;

/* }================================================================== */

/*
** {==================================================================
** Trait Layouts
** ===================================================================
*/

/* All geometric objects adhere to the glm::equal/glm::notEqual API. */
#define GEOM_EQUALS(LB, F, Tr, ...) \
  LAYOUT_GENERIC_EQUAL(LB, F, Tr, Tr::point_trait::fast)

/*
** Generic project-to-axis definition; returning the parametric min & max of the
** axis projection.
*/
#define GEOM_PROJECTION(LB, F, A, B)  \
  LUA_MLM_BEGIN                       \
  A::value_type outMin, outMax;       \
  const A::type a = (LB).Next<A>();   \
  const B::type b = (LB).Next<B>();   \
  F(a, b, outMin, outMax);            \
  BIND_PUSH(LB, outMin, outMax);      \
  LUA_MLM_END

/* Single parametric output */
#define LAYOUT_GEOM_PARAMETRIC_U(LB, F, A, B, U) \
  LUA_MLM_BEGIN                                  \
  U::type t;                                     \
  const A::type a = (LB).Next<A>();              \
  const B::type b = (LB).Next<B>();              \
  const auto _r = F(a, b, t);                    \
  BIND_PUSH(LB, _r, t);                          \
  LUA_MLM_END

/* Multiple parametric outputs */
#define LAYOUT_GEOM_PARAMETRIC_UV(LB, F, A, B, U, V) \
  LUA_MLM_BEGIN                                      \
  U::type n; V::type f;                              \
  const A::type a = (LB).Next<A>();                  \
  const B::type b = (LB).Next<B>();                  \
  const auto _r = F(a, b, n, f);                     \
  BIND_PUSH(LB, _r, n, f);                           \
  LUA_MLM_END

/* Multiple parametric inputs & outputs */
#define LAYOUT_GEOM_PARAMETRIC_UV_ARGS(LB, F, A, B, U, V) \
  LUA_MLM_BEGIN                                           \
  const A::type a = (LB).Next<A>();                       \
  const B::type b = (LB).Next<B>();                       \
  U::type n = (LB).Next<U>();                             \
  V::type f = (LB).Next<V>();                             \
  const auto _r = F(a, b, n, f);                          \
  BIND_PUSH(LB, _r, n, f);                                \
  LUA_MLM_END

/* Multiple parametric outputs */
#define LAYOUT_GEOM_PARAMETRIC_UVW(LB, F, A, B, U, V, W) \
  LUA_MLM_BEGIN                                          \
  U::type u; V::type v; W::type w;                       \
  const A::type a = (LB).Next<A>();                      \
  const B::type b = (LB).Next<B>();                      \
  const auto _r = F(a, b, u, v, w);                      \
  BIND_PUSH(LB, _r, u, v, w);                            \
  LUA_MLM_END

/*
** Generic distance definition: returning the distance between a geometric
** object and point-of-interest along with the parametric distances of intersection
*/
#define GEOM_DISTANCE(LB, F, A, B) LAYOUT_GEOM_PARAMETRIC_U(LB, F, A, B, A::value_trait)

/* Generic near + far glm::distance definition */
#define GEOM_DISTANCE_UV(LB, F, A, B) LAYOUT_GEOM_PARAMETRIC_UV(LB, F, A, B, A::value_trait, A::value_trait)

/*
** Generic intersects definition where the line/ray/segment is the first
** parameter being tested against the structure passed as the second parameter.
** Returning the point of intersection and relative location along each object.
*/
#define GEOM_INTERSECTS(LB, F, A, B) LAYOUT_GEOM_PARAMETRIC_UV(LB, F, A, B, A::value_trait, A::value_trait)

/*
** The line/ray/segment is the left-hand or right-hand parameter being tested
** against the AABB passed as the first opposite.
*/
#define GEOM_INTERSECTS_AABB_LH(LB, F, A, B) LAYOUT_GEOM_PARAMETRIC_UV_ARGS(LB, F, A, B, A::zero_trait, A::one_trait)
#define GEOM_INTERSECTS_AABB_RH(LB, F, A, B) LAYOUT_GEOM_PARAMETRIC_UV_ARGS(LB, F, A, B, B::zero_trait, B::one_trait)

/*
** Intersection test with a result (e.g., boolean), UV coordinates, and a
** distance along the object
*/
#define GEOM_INTERSECTS_TRIANGLE(LB, F, A, B) LAYOUT_GEOM_PARAMETRIC_UVW(LB, F, A, B, A::value_trait, A::value_trait, A::value_trait)

/* Generic glm::closestPoint definition */
#define GEOM_CLOSEST(LB, F, A, B) LAYOUT_GEOM_PARAMETRIC_UV(LB, F, A, B, A::value_trait, A::value_trait)

/* Generic glm::closestPoint definition */
#define GEOM_CLOSEST_TRIANGLE(LB, F, A, B) LAYOUT_GEOM_PARAMETRIC_UVW(LB, F, A, B, A::value_trait, A::value_trait, A::value_trait)

/* }================================================================== */

/*
** {==================================================================
** AABB
** ===================================================================
*/

/// <summary>
/// Create a new AABB that encloses all coordinates on the Lua stack (or within
/// a table if it is the first argument)
/// </summary>
GLM_BINDING_QUALIFIER(aabb_new) {
  GLM_BINDING_BEGIN
  luaL_checktype(L, LB.idx, LUA_TTABLE);
  using value_type = gLuaAABB<>::point_trait::value_type;
  using Iterator = gLuaArray<gLuaAABB<>::point_trait>::Iterator;
  gLuaArray<gLuaAABB<>::point_trait> lArray(LB.L, LB.idx);
  return gLuaBase::Push(LB, glm::minimalEnclosingAABB<Iterator, 3, value_type>(lArray.begin(), lArray.end()));
  GLM_BINDING_END
}

/* Create an AABB from a coordinate & radius. */
LAYOUT_DEFN(aabb_fromCenterAndSize, glm::aabbFromCenterAndSize, LAYOUT_BINARY_OPTIONAL, gLuaAABB<>::point_trait)
BIND_DEFN(aabb_fromSphere, glm::aabbFromSphere, gLuaSphere<>)
BIND_DEFN(aabb_operator_negate, operator-, gLuaAABB<>)
BIND_DEFN(aabb_operator_equals, operator==, gLuaAABB<>, gLuaAABB<>)
BIND_DEFN(aabb_operator_add, operator+, gLuaAABB<>, gLuaAABB<>::point_trait)
BIND_DEFN(aabb_operator_sub, operator-, gLuaAABB<>, gLuaAABB<>::point_trait)
ROTATION_MATRIX_DEFN(aabb_operator_mul, operator*, LAYOUT_UNARY, gLuaAABB<>::as_type<gLuaQuat<>::value_type>)
LAYOUT_DEFN(aabb_equal, glm::equal, GEOM_EQUALS, gLuaAABB<>)
LAYOUT_DEFN(aabb_notEqual, glm::notEqual, GEOM_EQUALS, gLuaAABB<>)
BIND_DEFN(aabb_isinf, glm::isinf, gLuaAABB<>)
BIND_DEFN(aabb_isnan, glm::isnan, gLuaAABB<>)
BIND_DEFN(aabb_isfinite, glm::isfinite, gLuaAABB<>)
BIND_DEFN(aabb_isDegenerate, glm::isDegenerate, gLuaAABB<>)
BIND_DEFN(aabb_centerPoint, glm::centerPoint, gLuaAABB<>)
BIND_DEFN(aabb_pointInside, glm::pointInside, gLuaAABB<>, gLuaAABB<>::point_trait)
BIND_DEFN(aabb_minimalEnclosingSphere, glm::minimalEnclosingSphere, gLuaAABB<>)
BIND_DEFN(aabb_maximalContainedSphere, glm::maximalContainedSphere, gLuaAABB<>)
BIND_DEFN(aabb_edge, glm::edge, gLuaAABB<>, gLuaTrait<int>)
BIND_DEFN(aabb_cornerPoint, glm::cornerPoint, gLuaAABB<>, gLuaTrait<int>)
BIND_DEFN(aabb_extremePoint, glm::extremePoint, gLuaAABB<>, gLuaAABB<>::point_trait)
BIND_DEFN(aabb_pointOnEdge, glm::pointOnEdge, gLuaAABB<>, gLuaTrait<int>, gLuaAABB<>::value_trait)
BIND_DEFN(aabb_faceCenterPoint, glm::faceCenterPoint, gLuaAABB<>, gLuaTrait<int>)
BIND_DEFN(aabb_facePoint, glm::facePoint, gLuaAABB<>, gLuaTrait<int>, gLuaAABB<>::value_trait, gLuaAABB<>::value_trait)
BIND_DEFN(aabb_faceNormal, glm::faceNormalAABB<gLuaFloat::value_type>, gLuaTrait<int>)
BIND_DEFN(aabb_facePlane, glm::facePlane, gLuaAABB<>, gLuaTrait<int>)
BIND_DEFN(aabb_size, glm::size, gLuaAABB<>)
BIND_DEFN(aabb_halfSize, glm::halfSize, gLuaAABB<>)
BIND_DEFN(aabb_volume, glm::volume, gLuaAABB<>)
BIND_DEFN(aabb_surfaceArea, glm::surfaceArea, gLuaAABB<>)
BIND_DEFN(aabb_scale, glm::scale, gLuaAABB<>, gLuaAABB<>::point_trait, gLuaAABB<>::value_trait)
BIND_DEFN(aabb_closestPoint, glm::closestPoint, gLuaAABB<>, gLuaAABB<>::point_trait)
BIND_DEFN(aabb_distance, glm::distance, gLuaAABB<>, gLuaAABB<>::point_trait)
BIND_DEFN(aabb_distanceSphere, glm::distance, gLuaAABB<>, gLuaSphere<>)
BIND_DEFN(aabb_contains, glm::contains, gLuaAABB<>, gLuaAABB<>::point_trait)
BIND_DEFN(aabb_containsAABB, glm::contains, gLuaAABB<>, gLuaAABB<>)
BIND_DEFN(aabb_containsSegment, glm::contains, gLuaAABB<>, gLuaSegment<>)
BIND_DEFN(aabb_containsTriangle, glm::contains, gLuaAABB<>, gLuaTriangle<>)
BIND_DEFN(aabb_containsSphere, glm::contains, gLuaAABB<>, gLuaSphere<>)
BIND_DEFN(aabb_containsPolygon, glm::contains, gLuaAABB<>, gLuaPolygon<>)
BIND_DEFN(aabb_grow, glm::grow, gLuaAABB<>, gLuaAABB<>::value_trait)
BIND_DEFN(aabb_enclose, glm::enclose, gLuaAABB<>, gLuaAABB<>::point_trait)
BIND_DEFN(aabb_encloseSegment, glm::enclose, gLuaAABB<>, gLuaSegment<>)
BIND_DEFN(aabb_encloseTriangle, glm::enclose, gLuaAABB<>, gLuaTriangle<>)
BIND_DEFN(aabb_encloseSphere, glm::enclose, gLuaAABB<>, gLuaSphere<>)
BIND_DEFN(aabb_encloseAABB, glm::enclose, gLuaAABB<>, gLuaAABB<>)
BIND_DEFN(aabb_enclosePolygon, glm::enclose, gLuaAABB<>, gLuaPolygon<>)
BIND_DEFN(aabb_expand, glm::expand, gLuaAABB<>, gLuaAABB<>::point_trait)
BIND_DEFN(aabb_clamp, glm::clamp, gLuaAABB<>, gLuaAABB<>)
BIND_DEFN(aabb_intersectsAABB, glm::intersects, gLuaAABB<>, gLuaAABB<>)
BIND_DEFN(aabb_intersectsSphere, glm::intersects, gLuaAABB<>, gLuaSphere<>)
BIND_DEFN(aabb_intersectsPlane, glm::intersects, gLuaAABB<>, gLuaPlane<>)
// BIND_DEFN(aabb_intersectsTriangle, glm::intersects, gLuaAABB<>, gLuaTriangle<>)
LAYOUT_DEFN(aabb_intersectsLine, glm::intersects, GEOM_INTERSECTS_AABB_RH, gLuaAABB<>, gLuaLine<>)
LAYOUT_DEFN(aabb_intersectsSegment, glm::intersects, GEOM_INTERSECTS_AABB_RH, gLuaAABB<>, gLuaSegment<>)
LAYOUT_DEFN(aabb_intersectsRay, glm::intersects, GEOM_INTERSECTS_AABB_RH, gLuaAABB<>, gLuaRay<>)
BIND_DEFN(aabb_intersection, glm::intersection, gLuaAABB<>, gLuaAABB<>)
BIND_DEFN(aabb_slabs, glm::slabs, gLuaAABB<>, gLuaRay<>)
LAYOUT_DEFN(aabb_projectToAxis, glm::projectToAxis, GEOM_PROJECTION, gLuaAABB<>, gLuaAABB<>::point_trait)

static const luaL_Reg luaglm_aabblib[] = {
  { "new", GLM_NAME(aabb_new) },
  { "fromCenterAndSize", GLM_NAME(aabb_fromCenterAndSize) },
  { "aabbFromSphere", GLM_NAME(aabb_fromSphere) },
  { "operator_negate", GLM_NAME(aabb_operator_negate) },
  { "operator_equals", GLM_NAME(aabb_operator_equals) },
  { "operator_add", GLM_NAME(aabb_operator_add) },
  { "operator_sub", GLM_NAME(aabb_operator_sub) },
  { "operator_mul", GLM_NAME(aabb_operator_mul) },
  { "equal", GLM_NAME(aabb_equal) },
  { "notEqual", GLM_NAME(aabb_notEqual) },
  //{ "tostring", GLM_NAME(aabb_tostring) },
  { "isinf", GLM_NAME(aabb_isinf) },
  { "isnan", GLM_NAME(aabb_isnan) },
  { "isfinite", GLM_NAME(aabb_isfinite) },
  { "isDegenerate", GLM_NAME(aabb_isDegenerate) },
  { "centerPoint", GLM_NAME(aabb_centerPoint) },
  { "centroid", GLM_NAME(aabb_centerPoint) },
  { "pointInside", GLM_NAME(aabb_pointInside) },
  { "minimalEnclosingSphere", GLM_NAME(aabb_minimalEnclosingSphere) },
  { "maximalContainedSphere", GLM_NAME(aabb_maximalContainedSphere) },
  { "edge", GLM_NAME(aabb_edge) },
  { "cornerPoint", GLM_NAME(aabb_cornerPoint) },
  { "extremePoint", GLM_NAME(aabb_extremePoint) },
  { "pointOnEdge", GLM_NAME(aabb_pointOnEdge) },
  { "faceCenterPoint", GLM_NAME(aabb_faceCenterPoint) },
  { "facePoint", GLM_NAME(aabb_facePoint) },
  { "faceNormal", GLM_NAME(aabb_faceNormal) },
  { "facePlane", GLM_NAME(aabb_facePlane) },
  { "size", GLM_NAME(aabb_size) },
  { "halfSize", GLM_NAME(aabb_halfSize) },
  { "diagonal", GLM_NAME(aabb_size) },
  { "halfDiagonal", GLM_NAME(aabb_halfSize) },
  { "volume", GLM_NAME(aabb_volume) },
  { "surfaceArea", GLM_NAME(aabb_surfaceArea) },
  { "scale", GLM_NAME(aabb_scale) },
  { "closestPoint", GLM_NAME(aabb_closestPoint) },
  { "distance", GLM_NAME(aabb_distance) },
  { "distanceSphere", GLM_NAME(aabb_distanceSphere) },
  { "contains", GLM_NAME(aabb_contains) },
  { "containsAABB", GLM_NAME(aabb_containsAABB) },
  { "containsSegment", GLM_NAME(aabb_containsSegment) },
  { "containsTriangle", GLM_NAME(aabb_containsTriangle) },
  { "containsSphere", GLM_NAME(aabb_containsSphere) },
  { "containsPolygon", GLM_NAME(aabb_containsPolygon) },
  { "grow", GLM_NAME(aabb_grow) },
  { "enclose", GLM_NAME(aabb_enclose) },
  { "encloseSegment", GLM_NAME(aabb_encloseSegment) },
  { "encloseTriangle", GLM_NAME(aabb_encloseTriangle) },
  { "encloseSphere", GLM_NAME(aabb_encloseSphere) },
  { "encloseAABB", GLM_NAME(aabb_encloseAABB) },
  { "enclosePolygon", GLM_NAME(aabb_enclosePolygon) },
  { "expand", GLM_NAME(aabb_expand) },
  { "clamp", GLM_NAME(aabb_clamp) },
  { "intersectsAABB", GLM_NAME(aabb_intersectsAABB) },
  { "intersectsSphere", GLM_NAME(aabb_intersectsSphere) },
  { "intersectsPlane", GLM_NAME(aabb_intersectsPlane) },
  //{ "intersectsTriangle", GLM_NAME(aabb_intersectsTriangle) },
  { "intersectsLine", GLM_NAME(aabb_intersectsLine) },
  { "intersectsSegment", GLM_NAME(aabb_intersectsSegment) },
  { "intersectsRay", GLM_NAME(aabb_intersectsRay) },
  { "intersection", GLM_NAME(aabb_intersection) },
  { "slabs", GLM_NAME(aabb_slabs) },
  { "projectToAxis", GLM_NAME(aabb_projectToAxis) },
  // @DEPRECATED: intersectsObject
  { "intersectAABB", GLM_NAME(aabb_intersectsAABB) },
  { "intersectSphere", GLM_NAME(aabb_intersectsSphere) },
  { "intersectPlane", GLM_NAME(aabb_intersectsPlane) },
  { "intersectLine", GLM_NAME(aabb_intersectsLine) },
  { "intersectSegment", GLM_NAME(aabb_intersectsSegment) },
  { "intersectRay", GLM_NAME(aabb_intersectsRay) },
#if defined(LUAGLM_ALIASES_O3DE)
  //{ "ToString", GLM_NAME() }, // @O3DEAlias
  //{ "CreateNull", GLM_NAME() }, // @O3DEAlias
  //{ "IsValid", GLM_NAME() }, // @O3DEAlias: !isDegenerate
  //{ "CreateFromPoint", GLM_NAME() }, // @O3DEAlias
  //{ "CreateFromMinMax", GLM_NAME() }, // @O3DEAlias
  //{ "CreateFromMinMaxValues", GLM_NAME() }, // @O3DEAlias
  { "CreateCenterHalfExtents", GLM_NAME(aabb_fromCenterAndSize) },
  { "CreateCenterRadius", GLM_NAME(aabb_fromSphere) },
  //{ "CreateFromObb", GLM_NAME() }, // @O3DEAlias
  { "GetExtents", GLM_NAME(aabb_size) },
  //{ "GetXExtent", GLM_NAME() }, // @O3DEAlias
  //{ "GetYExtent", GLM_NAME() }, // @O3DEAlias
  //{ "GetZExtent", GLM_NAME() }, // @O3DEAlias
  { "GetCenter", GLM_NAME(aabb_centerPoint) },
  { "GetAsSphere", GLM_NAME(aabb_minimalEnclosingSphere) },
  { "Contains", GLM_NAME(aabb_containsAABB) },  // @O3DEAlias: const Vector3& or const Aabb&
  { "ContainsVector3", GLM_NAME(aabb_contains) },
  { "Overlaps", GLM_NAME(aabb_intersectsAABB) },  // @O3DEAlias: Handles boundaries differently.
  //{ "Disjoint", GLM_NAME() }, // @O3DEAlias
  { "GetExpanded", GLM_NAME(aabb_expand) },  // Expand
  { "AddPoint", GLM_NAME(aabb_enclose) },
  { "AddAabb", GLM_NAME(aabb_encloseAABB) },
  { "GetDistance", GLM_NAME(aabb_distance) },
  { "GetClamped", GLM_NAME(aabb_clamp) },  // Clamp
  { "Translate", GLM_NAME(aabb_operator_add) },
  { "GetTranslated", GLM_NAME(aabb_operator_add) },
  { "GetSurfaceArea", GLM_NAME(aabb_surfaceArea) },
  //{ "GetTransformedObb", GLM_NAME() }, // @O3DEAlias
  { "GetTransformedAabb", GLM_NAME(aabb_operator_mul) },
  { "ApplyTransform", GLM_NAME(aabb_operator_mul) },
  //{ "Clone", GLM_NAME() }, // @O3DEAlias
  { "IsFinite", GLM_NAME(aabb_isfinite) },
  { "Equal", GLM_NAME(aabb_equal) },
  //{ "SetNull", GLM_NAME() }, // Pointless; immutable arguments
  //{ "Set", GLM_NAME() }, // Pointless; immutable arguments
#endif
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* }================================================================== */

/*
** {==================================================================
** AABB2D
** ===================================================================
*/

GLM_BINDING_QUALIFIER(aabb2d_new) {
  GLM_BINDING_BEGIN
  luaL_checktype(L, LB.idx, LUA_TTABLE);
  using value_type = gLuaAABB<2>::point_trait::value_type;
  using Iterator = gLuaArray<gLuaAABB<2>::point_trait>::Iterator;
  gLuaArray<gLuaAABB<2>::point_trait> lArray(LB.L, LB.idx);
  return gLuaBase::Push(LB, glm::minimalEnclosingAABB<Iterator, 2, value_type>(lArray.begin(), lArray.end()));
  GLM_BINDING_END
}

/* Create an AABB from a coordinate & radius. */
LAYOUT_DEFN(aabb2d_fromCenterAndSize, glm::aabbFromCenterAndSize, LAYOUT_BINARY_OPTIONAL, gLuaAABB<2>::point_trait)
BIND_DEFN(aabb2d_fromSphere, glm::aabbFromSphere, gLuaSphere<2>)
BIND_DEFN(aabb2d_operator_negate, operator-, gLuaAABB<2>)
BIND_DEFN(aabb2d_operator_equals, operator==, gLuaAABB<2>, gLuaAABB<2>)
BIND_DEFN(aabb2d_operator_add, operator+, gLuaAABB<2>, gLuaAABB<2>::point_trait)
BIND_DEFN(aabb2d_operator_sub, operator-, gLuaAABB<2>, gLuaAABB<2>::point_trait)
ROTATION_MATRIX_DEFN(aabb2d_operator_mul, operator*, LAYOUT_UNARY, gLuaAABB<2>::as_type<gLuaQuat<>::value_type>)
LAYOUT_DEFN(aabb2d_equal, glm::equal, GEOM_EQUALS, gLuaAABB<2>)
LAYOUT_DEFN(aabb2d_notEqual, glm::notEqual, GEOM_EQUALS, gLuaAABB<2>)
BIND_DEFN(aabb2d_isinf, glm::isinf, gLuaAABB<2>)
BIND_DEFN(aabb2d_isnan, glm::isnan, gLuaAABB<2>)
BIND_DEFN(aabb2d_isfinite, glm::isfinite, gLuaAABB<2>)
BIND_DEFN(aabb2d_isDegenerate, glm::isDegenerate, gLuaAABB<2>)
BIND_DEFN(aabb2d_centerPoint, glm::centerPoint, gLuaAABB<2>)
BIND_DEFN(aabb2d_pointInside, glm::pointInside, gLuaAABB<2>, gLuaAABB<2>::point_trait)
BIND_DEFN(aabb2d_edge, glm::edge, gLuaAABB<2>, gLuaTrait<int>)
BIND_DEFN(aabb2d_cornerPoint, glm::cornerPoint, gLuaAABB<2>, gLuaTrait<int>)
BIND_DEFN(aabb2d_extremePoint, glm::extremePoint, gLuaAABB<2>, gLuaAABB<2>::point_trait)
BIND_DEFN(aabb2d_size, glm::size, gLuaAABB<2>)
BIND_DEFN(aabb2d_halfSize, glm::halfSize, gLuaAABB<2>)
BIND_DEFN(aabb2d_volume, glm::volume, gLuaAABB<2>)
BIND_DEFN(aabb2d_scale, glm::scale, gLuaAABB<2>, gLuaAABB<2>::point_trait, gLuaAABB<2>::value_trait)
BIND_DEFN(aabb2d_closestPoint, glm::closestPoint, gLuaAABB<2>, gLuaAABB<2>::point_trait)
BIND_DEFN(aabb2d_distance, glm::distance, gLuaAABB<2>, gLuaAABB<2>::point_trait)
BIND_DEFN(aabb2d_distanceSphere, glm::distance, gLuaAABB<2>, gLuaSphere<2>)
BIND_DEFN(aabb2d_contains, glm::contains, gLuaAABB<2>, gLuaAABB<2>::point_trait)
BIND_DEFN(aabb2d_containsAABB, glm::contains, gLuaAABB<2>, gLuaAABB<2>)
BIND_DEFN(aabb2d_containsSegment, glm::contains, gLuaAABB<2>, gLuaSegment<2>)
BIND_DEFN(aabb2d_containsSphere, glm::contains, gLuaAABB<2>, gLuaSphere<2>)
BIND_DEFN(aabb2d_grow, glm::grow, gLuaAABB<2>, gLuaAABB<2>::value_trait)
BIND_DEFN(aabb2d_enclose, glm::enclose, gLuaAABB<2>, gLuaAABB<2>::point_trait)
BIND_DEFN(aabb2d_encloseSegment, glm::enclose, gLuaAABB<2>, gLuaSegment<2>)
BIND_DEFN(aabb2d_encloseSphere, glm::enclose, gLuaAABB<2>, gLuaSphere<2>)
BIND_DEFN(aabb2d_encloseAABB, glm::enclose, gLuaAABB<2>, gLuaAABB<2>)
BIND_DEFN(aabb2d_intersection, glm::intersection, gLuaAABB<2>, gLuaAABB<2>)
BIND_DEFN(aabb2d_intersectsAABB, glm::intersects, gLuaAABB<2>, gLuaAABB<2>)
LAYOUT_DEFN(aabb2d_intersectsLine, glm::intersects, GEOM_INTERSECTS_AABB_RH, gLuaAABB<2>, gLuaLine<2>)
LAYOUT_DEFN(aabb2d_intersectsSegment, glm::intersects, GEOM_INTERSECTS_AABB_RH, gLuaAABB<2>, gLuaSegment<2>)
LAYOUT_DEFN(aabb2d_intersectsRay, glm::intersects, GEOM_INTERSECTS_AABB_RH, gLuaAABB<2>, gLuaRay<2>)
LAYOUT_DEFN(aabb2d_projectToAxis, glm::projectToAxis, GEOM_PROJECTION, gLuaAABB<2>, gLuaAABB<2>::point_trait)

static const luaL_Reg luaglm_aabb2dlib[] = {
  { "new", GLM_NAME(aabb2d_new) },
  { "fromCenterAndSize", GLM_NAME(aabb2d_fromCenterAndSize) },
  { "aabbFromSphere", GLM_NAME(aabb2d_fromSphere) },
  { "operator_negate", GLM_NAME(aabb2d_operator_negate) },
  { "operator_equals", GLM_NAME(aabb2d_operator_equals) },
  { "operator_add", GLM_NAME(aabb2d_operator_add) },
  { "operator_sub", GLM_NAME(aabb2d_operator_sub) },
  { "operator_mul", GLM_NAME(aabb2d_operator_mul) },
  { "equal", GLM_NAME(aabb2d_equal) },
  { "notEqual", GLM_NAME(aabb2d_notEqual) },
  //{ "tostring", GLM_NAME(aabb2d_tostring) },
  { "isinf", GLM_NAME(aabb2d_isinf) },
  { "isnan", GLM_NAME(aabb2d_isnan) },
  { "isfinite", GLM_NAME(aabb2d_isfinite) },
  { "isDegenerate", GLM_NAME(aabb2d_isDegenerate) },
  { "centerPoint", GLM_NAME(aabb2d_centerPoint) },
  { "centroid", GLM_NAME(aabb2d_centerPoint) },
  { "pointInside", GLM_NAME(aabb2d_pointInside) },
  { "edge", GLM_NAME(aabb2d_edge) },
  { "cornerPoint", GLM_NAME(aabb2d_cornerPoint) },
  { "extremePoint", GLM_NAME(aabb2d_extremePoint) },
  { "size", GLM_NAME(aabb2d_size) },
  { "halfSize", GLM_NAME(aabb2d_halfSize) },
  { "diagonal", GLM_NAME(aabb2d_size) },
  { "halfDiagonal", GLM_NAME(aabb2d_halfSize) },
  { "volume", GLM_NAME(aabb2d_volume) },
  { "scale", GLM_NAME(aabb2d_scale) },
  { "closestPoint", GLM_NAME(aabb2d_closestPoint) },
  { "distance", GLM_NAME(aabb2d_distance) },
  { "distanceSphere", GLM_NAME(aabb2d_distanceSphere) },
  { "contains", GLM_NAME(aabb2d_contains) },
  { "containsAABB", GLM_NAME(aabb2d_containsAABB) },
  { "containsSegment", GLM_NAME(aabb2d_containsSegment) },
  { "containsSphere", GLM_NAME(aabb2d_containsSphere) },
  { "grow", GLM_NAME(aabb2d_grow) },
  { "enclose", GLM_NAME(aabb2d_enclose) },
  { "encloseSegment", GLM_NAME(aabb2d_encloseSegment) },
  { "encloseSphere", GLM_NAME(aabb2d_encloseSphere) },
  { "encloseAABB", GLM_NAME(aabb2d_encloseAABB) },
  { "intersectsAABB", GLM_NAME(aabb2d_intersectsAABB) },
  { "intersectsLine", GLM_NAME(aabb2d_intersectsLine) },
  { "intersectsSegment", GLM_NAME(aabb2d_intersectsSegment) },
  { "intersectsRay", GLM_NAME(aabb2d_intersectsRay) },
  { "intersection", GLM_NAME(aabb2d_intersection) },
  { "projectToAxis", GLM_NAME(aabb2d_projectToAxis) },
  // @DEPRECATED: intersectsObject
  { "intersectAABB", GLM_NAME(aabb2d_intersectsAABB) },
  { "intersectLine", GLM_NAME(aabb2d_intersectsLine) },
  { "intersectSegment", GLM_NAME(aabb2d_intersectsSegment) },
  { "intersectRay", GLM_NAME(aabb2d_intersectsRay) },
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* }================================================================== */

/*
** {==================================================================
** Line
** ===================================================================
*/

BIND_DEFN(line_operator_negate, operator-, gLuaLine<>)
BIND_DEFN(line_operator_equals, operator==, gLuaLine<>, gLuaLine<>)
BIND_DEFN(line_operator_add, operator+, gLuaLine<>, gLuaLine<>::point_trait)
BIND_DEFN(line_operator_sub, operator-, gLuaLine<>, gLuaLine<>::point_trait)
ROTATION_MATRIX_DEFN(line_operator_mul, operator*, LAYOUT_UNARY, gLuaLine<>::as_type<gLuaQuat<>::value_type>)
LAYOUT_DEFN(line_equal, glm::equal, GEOM_EQUALS, gLuaLine<>)
LAYOUT_DEFN(line_notEqual, glm::notEqual, GEOM_EQUALS, gLuaLine<>)
BIND_DEFN(line_to_segment, glm::toLineSegment, gLuaLine<>, gLuaLine<>::value_trait)
BIND_DEFN(line_isinf, glm::isinf, gLuaLine<>)
BIND_DEFN(line_isnan, glm::isnan, gLuaLine<>)
BIND_DEFN(line_isfinite, glm::isfinite, gLuaLine<>)
BIND_DEFN(line_getpoint, glm::getPoint, gLuaLine<>, gLuaLine<>::value_trait)
LAYOUT_DEFN(line_closest, glm::closestPoint, GEOM_DISTANCE, gLuaLine<>, gLuaLine<>::point_trait)
LAYOUT_DEFN(line_closestRay, glm::closestPoint, GEOM_CLOSEST, gLuaLine<>, gLuaRay<>)
LAYOUT_DEFN(line_closestLine, glm::closestPoint, GEOM_CLOSEST, gLuaLine<>, gLuaLine<>)
LAYOUT_DEFN(line_closestSegment, glm::closestPoint, GEOM_CLOSEST, gLuaLine<>, gLuaSegment<>)
LAYOUT_DEFN(line_closestTriangle, glm::closestPoint, GEOM_CLOSEST_TRIANGLE, gLuaLine<>, gLuaTriangle<>)
BIND_DEFN(line_contains, glm::contains, gLuaLine<>, gLuaLine<>::point_trait, gLuaLine<>::eps_trait)
BIND_DEFN(line_containsRay, glm::contains, gLuaLine<>, gLuaRay<>, gLuaLine<>::eps_trait)
BIND_DEFN(line_containsSegment, glm::contains, gLuaLine<>, gLuaSegment<>, gLuaLine<>::eps_trait)
LAYOUT_DEFN(line_distance, glm::distance, GEOM_DISTANCE, gLuaLine<>, gLuaLine<>::point_trait)
LAYOUT_DEFN(line_distanceRay, glm::distance, GEOM_DISTANCE_UV, gLuaLine<>, gLuaRay<>)
LAYOUT_DEFN(line_distanceLine, glm::distance, GEOM_DISTANCE_UV, gLuaLine<>, gLuaLine<>)
LAYOUT_DEFN(line_distanceSegment, glm::distance, GEOM_DISTANCE_UV, gLuaLine<>, gLuaSegment<>)
BIND_DEFN(line_distanceSphere, glm::distance, gLuaLine<>, gLuaSphere<>)
LAYOUT_DEFN(line_intersectsAABB, glm::intersects, GEOM_INTERSECTS_AABB_LH, gLuaLine<>, gLuaAABB<>)
LAYOUT_DEFN(line_intersectsSphere, glm::intersects, GEOM_INTERSECTS, gLuaLine<>, gLuaSphere<>)
LAYOUT_DEFN(line_intersectsPlane, glm::intersects, GEOM_DISTANCE, gLuaLine<>, gLuaPlane<>)
LAYOUT_DEFN(line_intersectsTriangle, glm::intersects, GEOM_INTERSECTS_TRIANGLE, gLuaLine<>, gLuaTriangle<>)
LAYOUT_DEFN(line_projectToAxis, glm::projectToAxis, GEOM_PROJECTION, gLuaLine<>, gLuaLine<>::point_trait)

static const luaL_Reg luaglm_linelib[] = {
  { "operator_negate", GLM_NAME(line_operator_negate) },
  { "operator_equals", GLM_NAME(line_operator_equals) },
  { "operator_add", GLM_NAME(line_operator_add) },
  { "operator_sub", GLM_NAME(line_operator_sub) },
  { "operator_mul", GLM_NAME(line_operator_mul) },
  { "equal", GLM_NAME(line_equal) },
  { "notEqual", GLM_NAME(line_notEqual) },
  //{ "tostring", GLM_NAME(line_tostring) },
  { "to_segment", GLM_NAME(line_to_segment) },
  { "isinf", GLM_NAME(line_isinf) },
  { "isnan", GLM_NAME(line_isnan) },
  { "isfinite", GLM_NAME(line_isfinite) },
  { "getPoint", GLM_NAME(line_getpoint) },
  { "closest", GLM_NAME(line_closest) },
  { "closestRay", GLM_NAME(line_closestRay) },
  { "closestLine", GLM_NAME(line_closestLine) },
  { "closestSegment", GLM_NAME(line_closestSegment) },
  { "closestTriangle", GLM_NAME(line_closestTriangle) },
  { "contains", GLM_NAME(line_contains) },
  { "containsRay", GLM_NAME(line_containsRay) },
  { "containsSegment", GLM_NAME(line_containsSegment) },
  { "distance", GLM_NAME(line_distance) },
  { "distanceRay", GLM_NAME(line_distanceRay) },
  { "distanceLine", GLM_NAME(line_distanceLine) },
  { "distanceSegment", GLM_NAME(line_distanceSegment) },
  { "distanceSphere", GLM_NAME(line_distanceSphere) },
  { "intersectsAABB", GLM_NAME(line_intersectsAABB) },
  { "intersectsSphere", GLM_NAME(line_intersectsSphere) },
  { "intersectsPlane", GLM_NAME(line_intersectsPlane) },
  { "intersectsTriangle", GLM_NAME(line_intersectsTriangle) },
  { "projectToAxis", GLM_NAME(line_projectToAxis) },
  // @DEPRECATED: intersectsObject
  { "intersectAABB", GLM_NAME(line_intersectsAABB) },
  { "intersectSphere", GLM_NAME(line_intersectsSphere) },
  { "intersectPlane", GLM_NAME(line_intersectsPlane) },
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* }================================================================== */

/*
** {==================================================================
** Ray
** ===================================================================
*/

BIND_DEFN(ray_operator_negate, operator-, gLuaRay<>)
BIND_DEFN(ray_operator_equals, operator==, gLuaRay<>, gLuaRay<>)
BIND_DEFN(ray_operator_add, operator+, gLuaRay<>, gLuaRay<>::point_trait)
BIND_DEFN(ray_operator_sub, operator-, gLuaRay<>, gLuaRay<>::point_trait)
ROTATION_MATRIX_DEFN(ray_operator_mul, operator*, LAYOUT_UNARY, gLuaRay<>::as_type<gLuaQuat<>::value_type>)
LAYOUT_DEFN(ray_equal, glm::equal, GEOM_EQUALS, gLuaRay<>)
LAYOUT_DEFN(ray_notEqual, glm::notEqual, GEOM_EQUALS, gLuaRay<>)
BIND_DEFN(ray_isinf, glm::isinf, gLuaRay<>)
BIND_DEFN(ray_isnan, glm::isnan, gLuaRay<>)
BIND_DEFN(ray_isfinite, glm::isfinite, gLuaRay<>)
BIND_DEFN(ray_getPoint, glm::getPoint, gLuaRay<>, gLuaRay<>::value_trait)
LAYOUT_DEFN(ray_closest, glm::closestPoint, GEOM_DISTANCE, gLuaRay<>, gLuaRay<>::point_trait)
LAYOUT_DEFN(ray_closestRay, glm::closestPoint, GEOM_CLOSEST, gLuaRay<>, gLuaRay<>)
LAYOUT_DEFN(ray_closestLine, glm::closestPoint, GEOM_CLOSEST, gLuaRay<>, gLuaLine<>)
LAYOUT_DEFN(ray_closestSegment, glm::closestPoint, GEOM_CLOSEST, gLuaRay<>, gLuaSegment<>)
BIND_DEFN(ray_contains, glm::contains, gLuaRay<>, gLuaRay<>::point_trait, gLuaRay<>::eps_trait)
BIND_DEFN(ray_containsSegment, glm::contains, gLuaRay<>, gLuaSegment<>, gLuaRay<>::eps_trait)
LAYOUT_DEFN(ray_distance, glm::distance, GEOM_DISTANCE, gLuaRay<>, gLuaRay<>::point_trait)
LAYOUT_DEFN(ray_distanceRay, glm::distance, GEOM_DISTANCE_UV, gLuaRay<>, gLuaRay<>)
LAYOUT_DEFN(ray_distanceLine, glm::distance, GEOM_DISTANCE_UV, gLuaRay<>, gLuaLine<>)
LAYOUT_DEFN(ray_distanceSegment, glm::distance, GEOM_DISTANCE_UV, gLuaRay<>, gLuaSegment<>)
BIND_DEFN(ray_distanceSphere, glm::distance, gLuaRay<>, gLuaSphere<>)
LAYOUT_DEFN(ray_intersectsSphere, glm::intersects, GEOM_INTERSECTS, gLuaRay<>, gLuaSphere<>)
LAYOUT_DEFN(ray_intersectsAABB, glm::intersects, GEOM_INTERSECTS_AABB_LH, gLuaRay<>, gLuaAABB<>)
LAYOUT_DEFN(ray_intersectsPlane, glm::intersects, GEOM_DISTANCE, gLuaRay<>, gLuaPlane<>)
LAYOUT_DEFN(ray_intersectsTriangle, glm::intersects, GEOM_INTERSECTS_TRIANGLE, gLuaRay<>, gLuaTriangle<>)
LAYOUT_DEFN(ray_projectToAxis, glm::projectToAxis, GEOM_PROJECTION, gLuaRay<>, gLuaRay<>::point_trait)

static const luaL_Reg luaglm_raylib[] = {
  { "operator_negate", GLM_NAME(ray_operator_negate) },
  { "operator_equals", GLM_NAME(ray_operator_equals) },
  { "operator_add", GLM_NAME(ray_operator_add) },
  { "operator_sub", GLM_NAME(ray_operator_sub) },
  { "operator_mul", GLM_NAME(ray_operator_mul) },
  { "equal", GLM_NAME(ray_equal) },
  { "notEqual", GLM_NAME(ray_notEqual) },
  //{ "tostring", GLM_NAME(ray_tostring) },
  { "isinf", GLM_NAME(ray_isinf) },
  { "isnan", GLM_NAME(ray_isnan) },
  { "isfinite", GLM_NAME(ray_isfinite) },
  { "getPoint", GLM_NAME(ray_getPoint) },
  { "closest", GLM_NAME(ray_closest) },
  { "closestRay", GLM_NAME(ray_closestRay) },
  { "closestLine", GLM_NAME(ray_closestLine) },
  { "closestSegment", GLM_NAME(ray_closestSegment) },
  { "contains", GLM_NAME(ray_contains) },
  { "containsSegment", GLM_NAME(ray_containsSegment) },
  { "distance", GLM_NAME(ray_distance) },
  { "distanceRay", GLM_NAME(ray_distanceRay) },
  { "distanceLine", GLM_NAME(ray_distanceLine) },
  { "distanceSegment", GLM_NAME(ray_distanceSegment) },
  { "distanceSphere", GLM_NAME(ray_distanceSphere) },
  { "intersectsSphere", GLM_NAME(ray_intersectsSphere) },
  { "intersectsAABB", GLM_NAME(ray_intersectsAABB) },
  { "intersectsTriangle", GLM_NAME(ray_intersectsTriangle) },
  { "intersectPlane", GLM_NAME(ray_intersectsPlane) },
  { "projectToAxis", GLM_NAME(ray_projectToAxis) },
  // @DEPRECATED: intersectsObject
  { "intersectSphere", GLM_NAME(ray_intersectsSphere) },
  { "intersectAABB", GLM_NAME(ray_intersectsAABB) },
  { "intersectPlane", GLM_NAME(ray_intersectsPlane) },
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* }================================================================== */

/*
** {==================================================================
** LineSegment
** ===================================================================
*/

BIND_DEFN(segment_operator_negate, operator-, gLuaSegment<>)
BIND_DEFN(segment_operator_equals, operator==, gLuaSegment<>, gLuaSegment<>)
BIND_DEFN(segment_operator_add, operator+, gLuaSegment<>, gLuaSegment<>::point_trait)
BIND_DEFN(segment_operator_sub, operator-, gLuaSegment<>, gLuaSegment<>::point_trait)
ROTATION_MATRIX_DEFN(segment_operator_mul, operator*, LAYOUT_UNARY, gLuaSegment<>::as_type<gLuaQuat<>::value_type>)
LAYOUT_DEFN(segment_equal, glm::equal, GEOM_EQUALS, gLuaSegment<>)
LAYOUT_DEFN(segment_notEqual, glm::notEqual, GEOM_EQUALS, gLuaSegment<>)
BIND_DEFN(segment_length, glm::length, gLuaSegment<>)
BIND_DEFN(segment_length2, glm::length2, gLuaSegment<>)
BIND_DEFN(segment_isfinite, glm::isfinite, gLuaSegment<>)
BIND_DEFN(segment_getPoint, glm::getPoint, gLuaSegment<>, gLuaSegment<>::value_trait)
BIND_DEFN(segment_centerPoint, glm::centerPoint, gLuaSegment<>)
BIND_DEFN(segment_reverse, glm::reverse, gLuaSegment<>)
BIND_DEFN(segment_dir, glm::dir, gLuaSegment<>)
BIND_DEFN(segment_extremePoint, glm::extremePoint, gLuaSegment<>, gLuaSegment<>::point_trait)
LAYOUT_DEFN(segment_closestPoint, glm::closestPoint, GEOM_DISTANCE, gLuaSegment<>, gLuaSegment<>::point_trait)
LAYOUT_DEFN(segment_closestRay, glm::closestPoint, GEOM_CLOSEST, gLuaSegment<>, gLuaRay<>)
LAYOUT_DEFN(segment_closestLine, glm::closestPoint, GEOM_CLOSEST, gLuaSegment<>, gLuaLine<>)
LAYOUT_DEFN(segment_closestSegment, glm::closestPoint, GEOM_CLOSEST, gLuaSegment<>, gLuaSegment<>)
LAYOUT_DEFN(segment_closestTriangle, glm::closestPoint, GEOM_INTERSECTS_TRIANGLE, gLuaSegment<>, gLuaTriangle<>)
BIND_DEFN(segment_containsPoint, glm::contains, gLuaSegment<>, gLuaSegment<>::point_trait, gLuaSegment<>::eps_trait)
BIND_DEFN(segment_containsSegment, glm::contains, gLuaSegment<>, gLuaSegment<>, gLuaSegment<>::eps_trait)
LAYOUT_DEFN(segment_distance2, glm::distance2, GEOM_DISTANCE, gLuaSegment<>, gLuaSegment<>::point_trait)
LAYOUT_DEFN(segment_distanceSegment2, glm::distance2, GEOM_DISTANCE_UV, gLuaSegment<>, gLuaSegment<>)
LAYOUT_DEFN(segment_distance, glm::distance, GEOM_DISTANCE, gLuaSegment<>, gLuaSegment<>::point_trait)
LAYOUT_DEFN(segment_distanceRay, glm::distance, GEOM_DISTANCE_UV, gLuaSegment<>, gLuaRay<>)
LAYOUT_DEFN(segment_distanceLine, glm::distance, GEOM_DISTANCE_UV, gLuaSegment<>, gLuaLine<>)
LAYOUT_DEFN(segment_distanceSegment, glm::distance, GEOM_DISTANCE_UV, gLuaSegment<>, gLuaSegment<>)
BIND_DEFN(segment_distancePlane, glm::distance, gLuaSegment<>, gLuaPlane<>)
LAYOUT_DEFN(segment_intersectsSphere, glm::intersects, GEOM_INTERSECTS, gLuaSegment<>, gLuaSphere<>)
LAYOUT_DEFN(segment_intersectsAABB, glm::intersects, GEOM_INTERSECTS_AABB_LH, gLuaSegment<>, gLuaAABB<>)
BIND_DEFN(segment_intersectsPlane, glm::intersects, gLuaSegment<>, gLuaPlane<>)
LAYOUT_DEFN(segment_intersectsSegment, glm::intersects, GEOM_INTERSECTS, gLuaSegment<>, gLuaSegment<>)
LAYOUT_DEFN(segment_intersectsTriangle, glm::intersects, GEOM_INTERSECTS_TRIANGLE, gLuaSegment<>, gLuaTriangle<>)

static const luaL_Reg luaglm_segmentlib[] = {
  { "operator_negate", GLM_NAME(segment_operator_negate) },
  { "operator_equals", GLM_NAME(segment_operator_equals) },
  { "operator_add", GLM_NAME(segment_operator_add) },
  { "operator_sub", GLM_NAME(segment_operator_sub) },
  { "operator_mul", GLM_NAME(segment_operator_mul) },
  { "equal", GLM_NAME(segment_equal) },
  { "notEqual", GLM_NAME(segment_notEqual) },
  //{ "tostring", GLM_NAME(segment_tostring) },
  { "length", GLM_NAME(segment_length) },
  { "length2", GLM_NAME(segment_length2) },
  { "isfinite", GLM_NAME(segment_isfinite) },
  { "getPoint", GLM_NAME(segment_getPoint) },
  { "centerPoint", GLM_NAME(segment_centerPoint) },
  { "centroid", GLM_NAME(segment_centerPoint) },
  { "reverse", GLM_NAME(segment_reverse) },
  { "dir", GLM_NAME(segment_dir) },
  { "extremePoint", GLM_NAME(segment_extremePoint) },
  { "closestPoint", GLM_NAME(segment_closestPoint) },
  { "closestRay", GLM_NAME(segment_closestRay) },
  { "closestLine", GLM_NAME(segment_closestLine) },
  { "closestSegment", GLM_NAME(segment_closestSegment) },
  { "containsPoint", GLM_NAME(segment_containsPoint) },
  { "containsSegment", GLM_NAME(segment_containsSegment) },
  { "closestTriangle", GLM_NAME(segment_closestTriangle) },
  { "distance2", GLM_NAME(segment_distance2) },
  { "distanceSegment2", GLM_NAME(segment_distanceSegment2) },
  { "distance", GLM_NAME(segment_distance) },
  { "distanceRay", GLM_NAME(segment_distanceRay) },
  { "distanceLine", GLM_NAME(segment_distanceLine) },
  { "distanceSegment", GLM_NAME(segment_distanceSegment) },
  { "distancePlane", GLM_NAME(segment_distancePlane) },
  { "intersectsSphere", GLM_NAME(segment_intersectsSphere) },
  { "intersectsAABB", GLM_NAME(segment_intersectsAABB) },
  { "intersectsPlane", GLM_NAME(segment_intersectsPlane) },
  { "intersectsSegment", GLM_NAME(segment_intersectsSegment) },
  { "intersectsTriangle", GLM_NAME(segment_intersectsTriangle) },
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* }================================================================== */

/*
** {==================================================================
** LineSegment2D
** ===================================================================
*/

BIND_DEFN(segment2d_operator_negate, operator-, gLuaSegment<2>)
BIND_DEFN(segment2d_operator_equals, operator==, gLuaSegment<2>, gLuaSegment<2>)
BIND_DEFN(segment2d_operator_add, operator+, gLuaSegment<2>, gLuaSegment<2>::point_trait)
BIND_DEFN(segment2d_operator_sub, operator-, gLuaSegment<2>, gLuaSegment<2>::point_trait)
LAYOUT_DEFN(segment2d_equal, glm::equal, GEOM_EQUALS, gLuaSegment<2>)
LAYOUT_DEFN(segment2d_notEqual, glm::notEqual, GEOM_EQUALS, gLuaSegment<2>)
BIND_DEFN(segment2d_length, glm::length, gLuaSegment<2>)
BIND_DEFN(segment2d_length2, glm::length2, gLuaSegment<2>)
BIND_DEFN(segment2d_isfinite, glm::isfinite, gLuaSegment<2>)
BIND_DEFN(segment2d_getPoint, glm::getPoint, gLuaSegment<2>, gLuaSegment<2>::value_trait)
BIND_DEFN(segment2d_centerPoint, glm::centerPoint, gLuaSegment<2>)
BIND_DEFN(segment2d_reverse, glm::reverse, gLuaSegment<2>)
BIND_DEFN(segment2d_dir, glm::dir, gLuaSegment<2>)
BIND_DEFN(segment2d_extremePoint, glm::extremePoint, gLuaSegment<2>, gLuaSegment<2>::point_trait)
LAYOUT_DEFN(segment2d_closestPoint, glm::closestPoint, GEOM_DISTANCE, gLuaSegment<2>, gLuaSegment<2>::point_trait)
LAYOUT_DEFN(segment2d_closestRay, glm::closestPoint, GEOM_CLOSEST, gLuaSegment<2>, gLuaRay<2>)
LAYOUT_DEFN(segment2d_closestLine, glm::closestPoint, GEOM_CLOSEST, gLuaSegment<2>, gLuaLine<2>)
LAYOUT_DEFN(segment2d_closestSegment, glm::closestPoint, GEOM_CLOSEST, gLuaSegment<2>, gLuaSegment<2>)
BIND_DEFN(segment2d_containsPoint, glm::contains, gLuaSegment<2>, gLuaSegment<2>::point_trait, gLuaSegment<2>::eps_trait)
BIND_DEFN(segment2d_containsSegment, glm::contains, gLuaSegment<2>, gLuaSegment<2>, gLuaSegment<2>::eps_trait)
LAYOUT_DEFN(segment2d_distance2, glm::distance2, GEOM_DISTANCE, gLuaSegment<2>, gLuaSegment<2>::point_trait)
LAYOUT_DEFN(segment2d_distanceSegment2, glm::distance2, GEOM_DISTANCE_UV, gLuaSegment<2>, gLuaSegment<2>)
LAYOUT_DEFN(segment2d_distance, glm::distance, GEOM_DISTANCE, gLuaSegment<2>, gLuaSegment<2>::point_trait)
LAYOUT_DEFN(segment2d_distanceRay, glm::distance, GEOM_DISTANCE_UV, gLuaSegment<2>, gLuaRay<2>)
LAYOUT_DEFN(segment2d_distanceLine, glm::distance, GEOM_DISTANCE_UV, gLuaSegment<2>, gLuaLine<2>)
LAYOUT_DEFN(segment2d_distanceSegment, glm::distance, GEOM_DISTANCE_UV, gLuaSegment<2>, gLuaSegment<2>)
BIND_DEFN(segment2d_distancePlane, glm::distance, gLuaSegment<2>, gLuaPlane<2>)
LAYOUT_DEFN(segment2d_intersectsAABB, glm::intersects, GEOM_INTERSECTS_AABB_LH, gLuaSegment<2>, gLuaAABB<2>)
LAYOUT_DEFN(segment2d_intersectsSegment, glm::intersects, GEOM_INTERSECTS, gLuaSegment<2>, gLuaSegment<2>)

static const luaL_Reg luaglm_segment2dlib[] = {
  { "operator_negate", GLM_NAME(segment2d_operator_negate) },
  { "operator_equals", GLM_NAME(segment2d_operator_equals) },
  { "operator_add", GLM_NAME(segment2d_operator_add) },
  { "operator_sub", GLM_NAME(segment2d_operator_sub) },
  { "equal", GLM_NAME(segment2d_equal) },
  { "notEqual", GLM_NAME(segment2d_notEqual) },
  //{ "tostring", GLM_NAME(segment2d_tostring) },
  { "length", GLM_NAME(segment2d_length) },
  { "length2", GLM_NAME(segment2d_length2) },
  { "isfinite", GLM_NAME(segment2d_isfinite) },
  { "getPoint", GLM_NAME(segment2d_getPoint) },
  { "centerPoint", GLM_NAME(segment2d_centerPoint) },
  { "centroid", GLM_NAME(segment2d_centerPoint) },
  { "reverse", GLM_NAME(segment2d_reverse) },
  { "dir", GLM_NAME(segment2d_dir) },
  { "extremePoint", GLM_NAME(segment2d_extremePoint) },
  { "closestPoint", GLM_NAME(segment2d_closestPoint) },
  { "closestRay", GLM_NAME(segment2d_closestRay) },
  { "closestLine", GLM_NAME(segment2d_closestLine) },
  { "closestSegment", GLM_NAME(segment2d_closestSegment) },
  { "containsPoint", GLM_NAME(segment2d_containsPoint) },
  { "containsSegment", GLM_NAME(segment2d_containsSegment) },
  { "distance2", GLM_NAME(segment2d_distance2) },
  { "distanceSegment2", GLM_NAME(segment2d_distanceSegment2) },
  { "distance", GLM_NAME(segment2d_distance) },
  { "distanceRay", GLM_NAME(segment2d_distanceRay) },
  { "distanceLine", GLM_NAME(segment2d_distanceLine) },
  { "distanceSegment", GLM_NAME(segment2d_distanceSegment) },
  { "distancePlane", GLM_NAME(segment2d_distancePlane) },
  { "intersectsAABB", GLM_NAME(segment2d_intersectsAABB) },
  { "intersectsSegment", GLM_NAME(segment2d_intersectsSegment) },
  { GLM_NULLPTR, GLM_NULLPTR }
};

/*
** {==================================================================
** Triangle
** ===================================================================
*/

BIND_DEFN(triangle_operator_negate, operator-, gLuaTriangle<>)
BIND_DEFN(triangle_operator_equals, operator==, gLuaTriangle<>, gLuaTriangle<>)
BIND_DEFN(triangle_operator_add, operator+, gLuaTriangle<>, gLuaTriangle<>::point_trait)
BIND_DEFN(triangle_operator_sub, operator-, gLuaTriangle<>, gLuaTriangle<>::point_trait)
ROTATION_MATRIX_DEFN(triangle_operator_mul, operator*, LAYOUT_UNARY, gLuaTriangle<>::as_type<gLuaQuat<>::value_type>)
LAYOUT_DEFN(triangle_equal, glm::equal, GEOM_EQUALS, gLuaTriangle<>)
LAYOUT_DEFN(triangle_notEqual, glm::notEqual, GEOM_EQUALS, gLuaTriangle<>)
BIND_DEFN(triangle_isinf, glm::isinf, gLuaTriangle<>)
BIND_DEFN(triangle_isnan, glm::isnan, gLuaTriangle<>)
BIND_DEFN(triangle_isfinite, glm::isfinite, gLuaTriangle<>)
BIND_DEFN(triangle_isDegenerate, glm::isDegenerate, gLuaTriangle<>)
BIND_DEFN(triangle_centroid, glm::centroid, gLuaTriangle<>)
BIND_DEFN(triangle_area, glm::area, gLuaTriangle<>)
BIND_DEFN(triangle_signedArea, glm::signedArea, gLuaTriangle<>, gLuaTriangle<>::point_trait)
BIND_DEFN(triangle_perimeter, glm::perimeter, gLuaTriangle<>)
BIND_DEFN(triangle_edge, glm::edge, gLuaTriangle<>, gLuaTrait<int>)
BIND_DEFN(triangle_cornerPoint, glm::cornerPoint, gLuaTriangle<>, gLuaTrait<int>)
BIND_DEFN(triangle_barycentric_uvw, glm::barycentricUVW, gLuaTriangle<>, gLuaTriangle<>::point_trait)
BIND_DEFN(triangle_barycentric_uv, glm::barycentricUV, gLuaTriangle<>, gLuaTriangle<>::point_trait)
BIND_DEFN(triangle_barycentric_inside, glm::barycentricInsideTriangle, gLuaTriangle<>::value_trait, gLuaTriangle<>::value_trait, gLuaTriangle<>::value_trait)
BIND_DEFN(triangle_barycentric_pointuv, glm::barycentricPoint, gLuaTriangle<>, gLuaTriangle<>::value_trait, gLuaTriangle<>::value_trait)
BIND_DEFN(triangle_barycentric_pointuvw, glm::barycentricPoint, gLuaTriangle<>, gLuaTriangle<>::value_trait, gLuaTriangle<>::value_trait, gLuaTriangle<>::value_trait)
BIND_DEFN(triangle_planeCCW, glm::planeCCW, gLuaTriangle<>)
BIND_DEFN(triangle_unnormalizedNormalCCW, glm::unnormalizedNormalCCW, gLuaTriangle<>)
BIND_DEFN(triangle_normalCCW, glm::normalCCW, gLuaTriangle<>)
BIND_DEFN(triangle_planeCW, glm::planeCW, gLuaTriangle<>)
BIND_DEFN(triangle_unnormalizedNormalCW, glm::unnormalizedNormalCW, gLuaTriangle<>)
BIND_DEFN(triangle_normalCW, glm::normalCW, gLuaTriangle<>)
BIND_DEFN(triangle_extremePoint, glm::extremePoint, gLuaTriangle<>, gLuaTriangle<>::point_trait)
BIND_DEFN(triangle_boundingAABB, glm::boundingAABB, gLuaTriangle<>)
BIND_DEFN(triangle_contains, glm::contains, gLuaTriangle<>, gLuaTriangle<>::point_trait, gLuaTriangle<>::eps_trait)
BIND_DEFN(triangle_containsSegment, glm::contains, gLuaTriangle<>, gLuaSegment<>, gLuaTriangle<>::eps_trait)
BIND_DEFN(triangle_containsTriangle, glm::contains, gLuaTriangle<>, gLuaTriangle<>, gLuaTriangle<>::eps_trait)
BIND_DEFN(triangle_closestPoint, glm::closestPoint, gLuaTriangle<>, gLuaTriangle<>::point_trait)
LAYOUT_DEFN(triangle_closestSegment, glm::closestPoint, GEOM_DISTANCE, gLuaTriangle<>, gLuaSegment<>)
LAYOUT_DEFN(triangle_closestLine, glm::closestPoint, GEOM_DISTANCE, gLuaTriangle<>, gLuaLine<>)
BIND_DEFN(triangle_distance, glm::distance, gLuaTriangle<>, gLuaTriangle<>::point_trait)
BIND_DEFN(triangle_distanceSphere, glm::distance, gLuaTriangle<>, gLuaSphere<>)
// BIND_DEFN(triangle_intersectsAABB, glm::intersects, gLuaTriangle<>, gLuaAABB<>)
LAYOUT_DEFN(triangle_intersectsRay, glm::intersects, GEOM_INTERSECTS_TRIANGLE, gLuaTriangle<>, gLuaRay<>)
LAYOUT_DEFN(triangle_intersectsLine, glm::intersects, GEOM_INTERSECTS_TRIANGLE, gLuaTriangle<>, gLuaLine<>)
LAYOUT_DEFN(triangle_intersectsSegment, glm::intersects, GEOM_INTERSECTS_TRIANGLE, gLuaTriangle<>, gLuaSegment<>)
BIND_DEFN(triangle_intersectsPlane, glm::intersects, gLuaTriangle<>, gLuaPlane<>)
BIND_DEFN(triangle_intersectsSphere, glm::intersects, gLuaTriangle<>, gLuaSphere<>)

static const luaL_Reg luaglm_trianglelib[] = {
  { "operator_negate", GLM_NAME(triangle_operator_negate) },
  { "operator_equals", GLM_NAME(triangle_operator_equals) },
  { "operator_add", GLM_NAME(triangle_operator_add) },
  { "operator_sub", GLM_NAME(triangle_operator_sub) },
  { "operator_mul", GLM_NAME(triangle_operator_mul) },
  { "equal", GLM_NAME(triangle_equal) },
  { "notEqual", GLM_NAME(triangle_notEqual) },
  //{ "tostring", GLM_NAME(triangle_tostring) },
  { "isinf", GLM_NAME(triangle_isinf) },
  { "isnan", GLM_NAME(triangle_isnan) },
  { "isfinite", GLM_NAME(triangle_isfinite) },
  { "isDegenerate", GLM_NAME(triangle_isDegenerate) },
  { "centroid", GLM_NAME(triangle_centroid) },
  { "area", GLM_NAME(triangle_area) },
  { "signedArea", GLM_NAME(triangle_signedArea) },
  { "perimeter", GLM_NAME(triangle_perimeter) },
  { "edge", GLM_NAME(triangle_edge) },
  { "cornerPoint", GLM_NAME(triangle_cornerPoint) },
  { "extremePoint", GLM_NAME(triangle_extremePoint) },
  { "boundingAABB", GLM_NAME(triangle_boundingAABB) },
  { "uvw", GLM_NAME(triangle_barycentric_uvw) },
  { "uv", GLM_NAME(triangle_barycentric_uv) },
  { "pointuv", GLM_NAME(triangle_barycentric_pointuv) },
  { "pointuvw", GLM_NAME(triangle_barycentric_pointuvw) },
  { "inside_triangle", GLM_NAME(triangle_barycentric_inside) },
  { "planeCCW", GLM_NAME(triangle_planeCCW) },
  { "planeCW", GLM_NAME(triangle_planeCW) },
  { "unnormalizedNormalCCW", GLM_NAME(triangle_unnormalizedNormalCCW) },
  { "unnormalizedNormalCW", GLM_NAME(triangle_unnormalizedNormalCW) },
  { "normalCCW", GLM_NAME(triangle_normalCCW) },
  { "normalCW", GLM_NAME(triangle_normalCW) },
  { "closestPoint", GLM_NAME(triangle_closestPoint) },
  { "closestSegment", GLM_NAME(triangle_closestSegment) },
  { "closestLine", GLM_NAME(triangle_closestLine) },
  { "contains", GLM_NAME(triangle_contains) },
  { "containsSegment", GLM_NAME(triangle_containsSegment) },
  { "containsTriangle", GLM_NAME(triangle_containsTriangle) },
  { "distance", GLM_NAME(triangle_distance) },
  { "distanceSphere", GLM_NAME(triangle_distanceSphere) },
  //{ "intersectsAABB", GLM_NAME(triangle_intersectsAABB) },
  { "intersectsRay", GLM_NAME(triangle_intersectsRay) },
  { "intersectsLine", GLM_NAME(triangle_intersectsLine) },
  { "intersectsSegment", GLM_NAME(triangle_intersectsSegment) },
  { "intersectsSphere", GLM_NAME(triangle_intersectsSphere) },
  { "intersectsPlane", GLM_NAME(triangle_intersectsPlane) },
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* }================================================================== */

/*
** {==================================================================
** Sphere
** ===================================================================
*/

GLM_BINDING_QUALIFIER(sphere_fitThroughPoints) {
  GLM_BINDING_BEGIN
  using point_trait = gLuaSphere<>::point_trait;
  switch (LB.top()) {
    case 2: BIND_FUNC(LB, glm::fitThroughPoints, point_trait, point_trait); break;
    case 3: BIND_FUNC(LB, glm::fitThroughPoints, point_trait, point_trait, point_trait); break;
    default: {
      BIND_FUNC(LB, glm::fitThroughPoints, point_trait, point_trait, point_trait, point_trait);
      break;
    }
  }
  GLM_BINDING_END
}

// @BloatTodo:
// GLM_BINDING_QUALIFIER(sphere_optimalEnclosingSphere) {
//  GLM_BINDING_BEGIN
//  using point_trait = gLuaSphere<>::point_trait;
//  switch (LB.top()) {
//    case 2: BIND_FUNC(LB, glm::optimalEnclosingSphere, point_trait, point_trait); break;
//    case 3: BIND_FUNC(LB, glm::optimalEnclosingSphere, point_trait, point_trait, point_trait); break;
//    case 4: BIND_FUNC(LB, glm::optimalEnclosingSphere, point_trait, point_trait, point_trait, point_trait); break;
//    default: {
//      luaL_checktype(L, LB.idx, LUA_TTABLE);
//      gLuaArray<point_trait> lArray(LB.L, LB.idx);
//      return gLuaBase::Push(LB, glm::optimalEnclosingSphere<point_trait::value_type, LUAGLM_BINDING_QUAL, gLuaArray<point_trait>>(lArray));
//    }
//  }
//  GLM_BINDING_END
//}

BIND_DEFN(sphere_operator_negate, operator-, gLuaSphere<>)
BIND_DEFN(sphere_operator_equals, operator==, gLuaSphere<>, gLuaSphere<>)
BIND_DEFN(sphere_operator_add, operator+, gLuaSphere<>, gLuaSphere<>::point_trait)
BIND_DEFN(sphere_operator_sub, operator-, gLuaSphere<>, gLuaSphere<>::point_trait)
ROTATION_MATRIX_DEFN(sphere_operator_mul, operator*, LAYOUT_UNARY, gLuaSphere<>::as_type<gLuaQuat<>::value_type>)
LAYOUT_DEFN(sphere_equal, glm::equal, GEOM_EQUALS, gLuaSphere<>)
LAYOUT_DEFN(sphere_notEqual, glm::notEqual, GEOM_EQUALS, gLuaSphere<>)
BIND_DEFN(sphere_volume, glm::volume, gLuaSphere<>)
BIND_DEFN(sphere_surfaceArea, glm::surfaceArea, gLuaSphere<>)
BIND_DEFN(sphere_isinf, glm::isinf, gLuaSphere<>)
BIND_DEFN(sphere_isnan, glm::isnan, gLuaSphere<>)
BIND_DEFN(sphere_isfinite, glm::isfinite, gLuaSphere<>)
BIND_DEFN(sphere_isDegenerate, glm::isDegenerate, gLuaSphere<>)
BIND_DEFN(sphere_extremePoint, glm::extremePoint, gLuaSphere<>, gLuaSphere<>::point_trait)
BIND_DEFN(sphere_contains, glm::contains, gLuaSphere<>, gLuaSphere<>::point_trait, gLuaSphere<>::eps_trait)
BIND_DEFN(sphere_containsSegment, glm::contains, gLuaSphere<>, gLuaSegment<>)
BIND_DEFN(sphere_containsSphere, glm::contains, gLuaSphere<>, gLuaSphere<>, gLuaSphere<>::eps_trait)
BIND_DEFN(sphere_containsTriangle, glm::contains, gLuaSphere<>, gLuaTriangle<>, gLuaSphere<>::eps_trait)
BIND_DEFN(sphere_containsAABB, glm::contains, gLuaSphere<>, gLuaAABB<>)
BIND_DEFN(sphere_distance, glm::distance, gLuaSphere<>, gLuaSphere<>::point_trait)
BIND_DEFN(sphere_distanceSphere, glm::distance, gLuaSphere<>, gLuaSphere<>)
BIND_DEFN(sphere_distanceAABB, glm::distance, gLuaSphere<>, gLuaAABB<>)
BIND_DEFN(sphere_distanceRay, glm::distance, gLuaSphere<>, gLuaRay<>)
BIND_DEFN(sphere_distanceSegment, glm::distance, gLuaSphere<>, gLuaSegment<>)
BIND_DEFN(sphere_distanceLine, glm::distance, gLuaSphere<>, gLuaLine<>)
BIND_DEFN(sphere_distanceTriangle, glm::distance, gLuaSphere<>, gLuaTriangle<>)
BIND_DEFN(sphere_closestPoint, glm::closestPoint, gLuaSphere<>, gLuaSphere<>::point_trait)
BIND_DEFN(sphere_intersectsSphere, glm::intersects, gLuaSphere<>, gLuaSphere<>)
BIND_DEFN(sphere_intersectsAABB, glm::intersects, gLuaSphere<>, gLuaAABB<>)
BIND_DEFN(sphere_intersectsPlane, glm::intersects, gLuaSphere<>, gLuaPlane<>)
BIND_DEFN(sphere_intersectsTriangle, glm::intersects, gLuaSphere<>, gLuaTriangle<>)
LAYOUT_DEFN(sphere_intersectsLine, glm::intersects, GEOM_INTERSECTS, gLuaSphere<>, gLuaLine<>)
LAYOUT_DEFN(sphere_intersectsSegment, glm::intersects, GEOM_INTERSECTS, gLuaSphere<>, gLuaSegment<>)
LAYOUT_DEFN(sphere_intersectsRay, glm::intersects, GEOM_INTERSECTS, gLuaSphere<>, gLuaRay<>)
BIND_DEFN(sphere_enclose, glm::enclose, gLuaSphere<>, gLuaSphere<>::point_trait)
BIND_DEFN(sphere_encloseSegment, glm::enclose, gLuaSphere<>, gLuaSegment<>)
BIND_DEFN(sphere_encloseSphere, glm::enclose, gLuaSphere<>, gLuaSphere<>)
BIND_DEFN(sphere_encloseAABB, glm::enclose, gLuaSphere<>, gLuaAABB<>)
BIND_DEFN(sphere_encloseTriangle, glm::enclose, gLuaSphere<>, gLuaTriangle<>)
BIND_DEFN(sphere_extendRadiusToContain, glm::extendRadiusToContain, gLuaSphere<>, gLuaSphere<>::point_trait, gLuaSphere<>::eps_trait)
BIND_DEFN(sphere_extendRadiusToContainSphere, glm::extendRadiusToContain, gLuaSphere<>, gLuaSphere<>, gLuaSphere<>::eps_trait)
BIND_DEFN(sphere_maximalContainedAABB, glm::maximalContainedAABB, gLuaSphere<>)
LAYOUT_DEFN(sphere_projectToAxis, glm::projectToAxis, GEOM_PROJECTION, gLuaSphere<>, gLuaSphere<>::point_trait)

static const luaL_Reg luaglm_spherelib[] = {
  { "operator_negate", GLM_NAME(sphere_operator_negate) },
  { "operator_equals", GLM_NAME(sphere_operator_equals) },
  { "operator_add", GLM_NAME(sphere_operator_add) },
  { "operator_sub", GLM_NAME(sphere_operator_sub) },
  { "operator_mul", GLM_NAME(sphere_operator_mul) },
  { "equal", GLM_NAME(sphere_equal) },
  { "notEqual", GLM_NAME(sphere_notEqual) },
  //{ "tostring", GLM_NAME(sphere_tostring) },
  { "volume", GLM_NAME(sphere_volume) },
  { "surfaceArea", GLM_NAME(sphere_surfaceArea) },
  { "isinf", GLM_NAME(sphere_isinf) },
  { "isnan", GLM_NAME(sphere_isnan) },
  { "isfinite", GLM_NAME(sphere_isfinite) },
  { "isDegenerate", GLM_NAME(sphere_isDegenerate) },
  { "extremePoint", GLM_NAME(sphere_extremePoint) },
  { "contains", GLM_NAME(sphere_contains) },
  { "containsSegment", GLM_NAME(sphere_containsSegment) },
  { "containsSphere", GLM_NAME(sphere_containsSphere) },
  { "containsTriangle", GLM_NAME(sphere_containsTriangle) },
  { "containsAABB", GLM_NAME(sphere_containsAABB) },
  { "distance", GLM_NAME(sphere_distance) },
  { "distanceSphere", GLM_NAME(sphere_distanceSphere) },
  { "distanceAABB", GLM_NAME(sphere_distanceAABB) },
  { "distanceRay", GLM_NAME(sphere_distanceRay) },
  { "distanceSegment", GLM_NAME(sphere_distanceSegment) },
  { "distanceLine", GLM_NAME(sphere_distanceLine) },
  { "distanceTriangle", GLM_NAME(sphere_distanceTriangle) },
  { "closestPoint", GLM_NAME(sphere_closestPoint) },
  { "intersectsSphere", GLM_NAME(sphere_intersectsSphere) },
  { "intersectsAABB", GLM_NAME(sphere_intersectsAABB) },
  { "intersectsLine", GLM_NAME(sphere_intersectsLine) },
  { "intersectsSegment", GLM_NAME(sphere_intersectsSegment) },
  { "intersectsRay", GLM_NAME(sphere_intersectsRay) },
  { "intersectsPlane", GLM_NAME(sphere_intersectsPlane) },
  { "intersectsTriangle", GLM_NAME(sphere_intersectsTriangle) },
  { "enclose", GLM_NAME(sphere_enclose) },
  { "encloseSegment", GLM_NAME(sphere_encloseSegment) },
  { "encloseSphere", GLM_NAME(sphere_encloseSphere) },
  { "encloseAABB", GLM_NAME(sphere_encloseAABB) },
  { "encloseTriangle", GLM_NAME(sphere_encloseTriangle) },
  { "extendRadiusToContain", GLM_NAME(sphere_extendRadiusToContain) },
  { "extendRadiusToContainSphere", GLM_NAME(sphere_extendRadiusToContainSphere) },
  { "maximalContainedAABB", GLM_NAME(sphere_maximalContainedAABB) },
  { "fitThroughPoints", GLM_NAME(sphere_fitThroughPoints) },
  //{ "optimalEnclosingSphere", GLM_NAME(sphere_optimalEnclosingSphere) },  // @BloatTodo
  { "projectToAxis", GLM_NAME(sphere_projectToAxis) },
  // @DEPRECATED: intersectsObject
  { "intersectSphere", GLM_NAME(sphere_intersectsSphere) },
  { "intersectAABB", GLM_NAME(sphere_intersectsAABB) },
  { "intersectLine", GLM_NAME(sphere_intersectsLine) },
  { "intersectSegment", GLM_NAME(sphere_intersectsSegment) },
  { "intersectRay", GLM_NAME(sphere_intersectsRay) },
  { "intersectPlane", GLM_NAME(sphere_intersectsPlane) },
#if defined(LUAGLM_ALIASES_O3DE)
  //{ "CreateUnitSphere", GLM_NAME() }, // @O3DEAlias
  //{ "CreateFromAabb", GLM_NAME() }, // @O3DEAlias
  //{ "GetCenter", GLM_NAME() }, // API is functional; not required
  //{ "GetRadius", GLM_NAME() }, // API is functional; not required
  //{ "SetCenter", GLM_NAME() }, // API is functional; not required
  //{ "SetRadius", GLM_NAME() }, // API is functional; not required
#endif
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* }================================================================== */

/*
** {==================================================================
** Circle
** ===================================================================
*/

BIND_DEFN(circle_operator_negate, operator-, gLuaSphere<2>)
BIND_DEFN(circle_operator_equals, operator==, gLuaSphere<2>, gLuaSphere<2>)
BIND_DEFN(circle_operator_add, operator+, gLuaSphere<2>, gLuaSphere<2>::point_trait)
BIND_DEFN(circle_operator_sub, operator-, gLuaSphere<2>, gLuaSphere<2>::point_trait)
LAYOUT_DEFN(circle_equal, glm::equal, GEOM_EQUALS, gLuaSphere<2>)
LAYOUT_DEFN(circle_notEqual, glm::notEqual, GEOM_EQUALS, gLuaSphere<2>)
BIND_DEFN(circle_area, glm::area, gLuaSphere<2>)
BIND_DEFN(circle_isinf, glm::isinf, gLuaSphere<2>)
BIND_DEFN(circle_isnan, glm::isnan, gLuaSphere<2>)
BIND_DEFN(circle_isfinite, glm::isfinite, gLuaSphere<2>)
BIND_DEFN(circle_isDegenerate, glm::isDegenerate, gLuaSphere<2>)
BIND_DEFN(circle_extremePoint, glm::extremePoint, gLuaSphere<2>, gLuaSphere<2>::point_trait)
BIND_DEFN(circle_contains, glm::contains, gLuaSphere<2>, gLuaSphere<2>::point_trait, gLuaSphere<2>::eps_trait)
BIND_DEFN(circle_containsSegment, glm::contains, gLuaSphere<2>, gLuaSegment<2>)
BIND_DEFN(circle_containsCircle, glm::contains, gLuaSphere<2>, gLuaSphere<2>, gLuaSphere<2>::eps_trait)
BIND_DEFN(circle_containsAABB, glm::contains, gLuaSphere<2>, gLuaAABB<2>)
BIND_DEFN(circle_distance, glm::distance, gLuaSphere<2>, gLuaSphere<2>::point_trait)
BIND_DEFN(circle_distanceSphere, glm::distance, gLuaSphere<2>, gLuaSphere<2>)
BIND_DEFN(circle_distanceAABB, glm::distance, gLuaSphere<2>, gLuaAABB<2>)
BIND_DEFN(circle_distanceRay, glm::distance, gLuaSphere<2>, gLuaRay<2>)
BIND_DEFN(circle_distanceSegment, glm::distance, gLuaSphere<2>, gLuaSegment<2>)
BIND_DEFN(circle_distanceLine, glm::distance, gLuaSphere<2>, gLuaLine<2>)
BIND_DEFN(circle_closestPoint, glm::closestPoint, gLuaSphere<2>, gLuaSphere<2>::point_trait)
BIND_DEFN(circle_intersectsCircle, glm::intersects, gLuaSphere<2>, gLuaSphere<2>)
BIND_DEFN(circle_intersectsAABB, glm::intersects, gLuaSphere<2>, gLuaAABB<2>)
BIND_DEFN(circle_intersectsPlane, glm::intersects, gLuaSphere<2>, gLuaPlane<2>)
LAYOUT_DEFN(circle_intersectsLine, glm::intersects, GEOM_INTERSECTS, gLuaSphere<2>, gLuaLine<2>)
LAYOUT_DEFN(circle_intersectsSegment, glm::intersects, GEOM_INTERSECTS, gLuaSphere<2>, gLuaSegment<2>)
LAYOUT_DEFN(circle_intersectsRay, glm::intersects, GEOM_INTERSECTS, gLuaSphere<2>, gLuaRay<2>)
BIND_DEFN(circle_enclose, glm::enclose, gLuaSphere<2>, gLuaSphere<2>::point_trait)
BIND_DEFN(circle_encloseSegment, glm::enclose, gLuaSphere<2>, gLuaSegment<2>)
BIND_DEFN(circle_encloseSphere, glm::enclose, gLuaSphere<2>, gLuaSphere<2>)
BIND_DEFN(circle_encloseAABB, glm::enclose, gLuaSphere<2>, gLuaAABB<2>)
BIND_DEFN(circle_extendRadiusToContain, glm::extendRadiusToContain, gLuaSphere<2>, gLuaSphere<2>::point_trait, gLuaSphere<2>::eps_trait)
BIND_DEFN(circle_extendRadiusToContainCircle, glm::extendRadiusToContain, gLuaSphere<2>, gLuaSphere<2>, gLuaSphere<2>::eps_trait)
BIND_DEFN(circle_maximalContainedAABB, glm::maximalContainedAABB, gLuaSphere<2>)
LAYOUT_DEFN(circle_projectToAxis, glm::projectToAxis, GEOM_PROJECTION, gLuaSphere<2>, gLuaSphere<2>::point_trait)

static const luaL_Reg luaglm_circlelib[] = {
  { "operator_negate", GLM_NAME(circle_operator_negate) },
  { "operator_equals", GLM_NAME(circle_operator_equals) },
  { "operator_add", GLM_NAME(circle_operator_add) },
  { "operator_sub", GLM_NAME(circle_operator_sub) },
  { "equal", GLM_NAME(circle_equal) },
  { "notEqual", GLM_NAME(circle_notEqual) },
  //{ "tostring", GLM_NAME(circle_tostring) },
  { "area", GLM_NAME(circle_area) },
  { "isinf", GLM_NAME(circle_isinf) },
  { "isnan", GLM_NAME(circle_isnan) },
  { "isfinite", GLM_NAME(circle_isfinite) },
  { "isDegenerate", GLM_NAME(circle_isDegenerate) },
  { "extremePoint", GLM_NAME(circle_extremePoint) },
  { "contains", GLM_NAME(circle_contains) },
  { "containsSegment", GLM_NAME(circle_containsSegment) },
  { "containsCircle", GLM_NAME(circle_containsCircle) },
  { "containsAABB", GLM_NAME(circle_containsAABB) },
  { "distance", GLM_NAME(circle_distance) },
  { "distanceSphere", GLM_NAME(circle_distanceSphere) },
  { "distanceAABB", GLM_NAME(circle_distanceAABB) },
  { "distanceRay", GLM_NAME(circle_distanceRay) },
  { "distanceSegment", GLM_NAME(circle_distanceSegment) },
  { "distanceLine", GLM_NAME(circle_distanceLine) },
  { "closestPoint", GLM_NAME(circle_closestPoint) },
  { "intersectsCircle", GLM_NAME(circle_intersectsCircle) },
  { "intersectsAABB", GLM_NAME(circle_intersectsAABB) },
  { "intersectsLine", GLM_NAME(circle_intersectsLine) },
  { "intersectsSegment", GLM_NAME(circle_intersectsSegment) },
  { "intersectsRay", GLM_NAME(circle_intersectsRay) },
  { "intersectsPlane", GLM_NAME(circle_intersectsPlane) },
  { "enclose", GLM_NAME(circle_enclose) },
  { "encloseSegment", GLM_NAME(circle_encloseSegment) },
  { "encloseSphere", GLM_NAME(circle_encloseSphere) },
  { "encloseAABB", GLM_NAME(circle_encloseAABB) },
  { "extendRadiusToContain", GLM_NAME(circle_extendRadiusToContain) },
  { "extendRadiusToContainCircle", GLM_NAME(circle_extendRadiusToContainCircle) },
  { "maximalContainedAABB", GLM_NAME(circle_maximalContainedAABB) },
  { "projectToAxis", GLM_NAME(circle_projectToAxis) },
  // @DEPRECATED: intersectsObject
  { "intersectCircle", GLM_NAME(circle_intersectsCircle) },
  { "intersectAABB", GLM_NAME(circle_intersectsAABB) },
  { "intersectLine", GLM_NAME(circle_intersectsLine) },
  { "intersectSegment", GLM_NAME(circle_intersectsSegment) },
  { "intersectRay", GLM_NAME(circle_intersectsRay) },
  { "intersectPlane", GLM_NAME(circle_intersectsPlane) },
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* }================================================================== */

/*
** {==================================================================
** Plane
** ===================================================================
*/

BIND_DEFN(plane_operator_negate, operator-, gLuaPlane<>)
BIND_DEFN(plane_operator_equals, operator==, gLuaPlane<>, gLuaPlane<>)
BIND_DEFN(plane_operator_add, operator+, gLuaPlane<>, gLuaPlane<>::point_trait)
BIND_DEFN(plane_operator_sub, operator-, gLuaPlane<>, gLuaPlane<>::point_trait)
ROTATION_MATRIX_DEFN(plane_operator_mul, operator*, LAYOUT_UNARY, gLuaPlane<>::as_type<gLuaQuat<>::value_type>)
LAYOUT_DEFN(plane_equal, glm::equal, GEOM_EQUALS, gLuaPlane<>)
LAYOUT_DEFN(plane_notEqual, glm::notEqual, GEOM_EQUALS, gLuaPlane<>)
BIND_DEFN(plane_fromRay, glm::planeFrom, gLuaRay<>, gLuaPlane<>::point_trait)
BIND_DEFN(plane_fromLine, glm::planeFrom, gLuaLine<>, gLuaPlane<>::point_trait)
BIND_DEFN(plane_fromLineSegment, glm::planeFrom, gLuaSegment<>, gLuaPlane<>::point_trait)
BIND_DEFN(plane_fromPointNormal, glm::planeFrom, gLuaPlane<>::point_trait, gLuaPlane<>::point_trait)
BIND_DEFN(plane_fromPoints, glm::planeFrom, gLuaPlane<>::point_trait, gLuaPlane<>::point_trait, gLuaPlane<>::point_trait)
BIND_DEFN(plane_isinf, glm::isinf, gLuaPlane<>)
BIND_DEFN(plane_isnan, glm::isnan, gLuaPlane<>)
BIND_DEFN(plane_isfinite, glm::isfinite, gLuaPlane<>)
BIND_DEFN(plane_isDegenerate, glm::isDegenerate, gLuaPlane<>)
BIND_DEFN(plane_isParallel, glm::isParallel, gLuaPlane<>, gLuaPlane<>, gLuaPlane<>::eps_trait)
BIND_DEFN(plane_areOnSameSide, glm::areOnSameSide, gLuaPlane<>, gLuaPlane<>::point_trait, gLuaPlane<>::point_trait)
BIND_DEFN(plane_examineSide, glm::examineSide, gLuaPlane<>, gLuaTriangle<>, gLuaPlane<>::eps_trait)
BIND_DEFN(plane_isInPositiveDirection, glm::isInPositiveDirection, gLuaPlane<>, gLuaPlane<>::point_trait)
BIND_DEFN(plane_isOnPositiveSide, glm::isOnPositiveSide, gLuaPlane<>, gLuaPlane<>::point_trait)
BIND_DEFN(plane_passesThroughOrigin, glm::passesThroughOrigin, gLuaPlane<>, gLuaPlane<>::eps_trait)
BIND_DEFN(plane_angle, glm::angle, gLuaPlane<>, gLuaPlane<>)
BIND_DEFN(plane_reverseNormal, glm::reverseNormal, gLuaPlane<>)
BIND_DEFN(plane_pointOnPlane, glm::pointOnPlane, gLuaPlane<>)
BIND_DEFN(plane_refract, glm::refract, gLuaPlane<>, gLuaPlane<>::point_trait, gLuaPlane<>::value_trait, gLuaPlane<>::value_trait)
BIND_DEFN(plane_project, glm::project, gLuaPlane<>, gLuaPlane<>::point_trait)
BIND_DEFN(plane_projectLine, glm::project, gLuaPlane<>, gLuaLine<>)
BIND_DEFN(plane_projectSegment, glm::project, gLuaPlane<>, gLuaSegment<>)
BIND_DEFN(plane_projectRay, glm::project, gLuaPlane<>, gLuaRay<>)
BIND_DEFN(plane_projectTriangle, glm::project, gLuaPlane<>, gLuaTriangle<>)
BIND_DEFN(plane_projectToNegativeHalf, glm::projectToNegativeHalf, gLuaPlane<>, gLuaPlane<>::point_trait)
BIND_DEFN(plane_projectToPositiveHalf, glm::projectToPositiveHalf, gLuaPlane<>, gLuaPlane<>::point_trait)
BIND_DEFN(plane_distance, glm::distance, gLuaPlane<>, gLuaPlane<>::point_trait)
BIND_DEFN(plane_distanceSegment, glm::distance, gLuaPlane<>, gLuaSegment<>)
BIND_DEFN(plane_distanceSphere, glm::distance, gLuaPlane<>, gLuaSphere<>)
BIND_DEFN(plane_signedDistance, glm::signedDistance, gLuaPlane<>, gLuaPlane<>::point_trait)
BIND_DEFN(plane_signedDistanceLine, glm::signedDistance, gLuaPlane<>, gLuaLine<>)
BIND_DEFN(plane_signedDistanceSegment, glm::signedDistance, gLuaPlane<>, gLuaSegment<>)
BIND_DEFN(plane_signedDistanceRay, glm::signedDistance, gLuaPlane<>, gLuaRay<>)
BIND_DEFN(plane_signedDistanceAABB, glm::signedDistance, gLuaPlane<>, gLuaAABB<>)
BIND_DEFN(plane_signedDistanceSphere, glm::signedDistance, gLuaPlane<>, gLuaSphere<>)
BIND_DEFN(plane_signedDistanceTriangle, glm::signedDistance, gLuaPlane<>, gLuaTriangle<>)
BIND_DEFN(plane_orthoProjection, glm::orthoProjection, gLuaPlane<>)
BIND_DEFN(plane_mirrorMatrix, glm::mirrorMatrix, gLuaPlane<>)
BIND_DEFN(plane_mirror, glm::mirror, gLuaPlane<>, gLuaPlane<>::point_trait)
BIND_DEFN(plane_closestPointRay, glm::closestPoint, gLuaPlane<>, gLuaRay<>)
BIND_DEFN(plane_closestPointSegment, glm::closestPoint, gLuaPlane<>, gLuaSegment<>)
BIND_DEFN(plane_contains, glm::contains, gLuaPlane<>, gLuaPlane<>::point_trait, gLuaPlane<>::eps_trait)
BIND_DEFN(plane_containsLine, glm::contains, gLuaPlane<>, gLuaLine<>, gLuaPlane<>::eps_trait)
BIND_DEFN(plane_containsRay, glm::contains, gLuaPlane<>, gLuaRay<>, gLuaPlane<>::eps_trait)
BIND_DEFN(plane_containsSegment, glm::contains, gLuaPlane<>, gLuaSegment<>, gLuaPlane<>::eps_trait)
BIND_DEFN(plane_containsTriangle, glm::contains, gLuaPlane<>, gLuaTriangle<>, gLuaPlane<>::eps_trait)
LAYOUT_DEFN(plane_intersectsRay, glm::intersects, GEOM_DISTANCE, gLuaPlane<>, gLuaRay<>)
LAYOUT_DEFN(plane_intersectsLine, glm::intersects, GEOM_DISTANCE, gLuaPlane<>, gLuaLine<>)
LAYOUT_DEFN(plane_intersectsSegment, glm::intersects, GEOM_DISTANCE, gLuaPlane<>, gLuaSegment<>)
BIND_DEFN(plane_intersectsTriangle, glm::intersects, gLuaPlane<>, gLuaTriangle<>)
BIND_DEFN(plane_intersectsSphere, glm::intersects, gLuaPlane<>, gLuaSphere<>)
BIND_DEFN(plane_intersectsAABB, glm::intersects, gLuaPlane<>, gLuaAABB<>)
BIND_DEFN(plane_clipSegment, glm::clip, gLuaPlane<>, gLuaSegment<>)

GLM_BINDING_QUALIFIER(plane_point) {
  GLM_BINDING_BEGIN
  if (LB.top() > 3)
    BIND_FUNC(LB, glm::point, gLuaPlane<>, gLuaPlane<>::value_trait, gLuaPlane<>::value_trait, gLuaPlane<>::point_trait);
  BIND_FUNC(LB, glm::point, gLuaPlane<>, gLuaPlane<>::value_trait, gLuaPlane<>::value_trait);
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(plane_clipLine) {
  GLM_BINDING_BEGIN
  gLuaRay<>::type result;
  const gLuaPlane<>::type plane = LB.Next<gLuaPlane<>>();
  const gLuaLine<>::type line = LB.Next<gLuaLine<>>();
  const int clip_type = glm::clip(plane, line, result);
  BIND_PUSH(LB, clip_type, result);
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(plane_intersectsPlane) {
  GLM_BINDING_BEGIN
  gLuaPlane<>::point_trait::type result;
  const gLuaPlane<>::type a = LB.Next<gLuaPlane<>>();
  const gLuaPlane<>::type b = LB.Next<gLuaPlane<>>();
  const gLuaPlane<>::type c = LB.Next<gLuaPlane<>>();
  if (glm::intersects(a, b, c, result))
    BIND_PUSH(LB, true, result);
  else
    BIND_PUSH(LB, false);
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(plane_clipTriangle) {
  GLM_BINDING_BEGIN
  gLuaTriangle<>::type t1 = gLuaTriangle<>::Zero();
  gLuaTriangle<>::type t2 = gLuaTriangle<>::Zero();
  const gLuaPlane<>::type plane = LB.Next<gLuaPlane<>>();
  const gLuaTriangle<>::type line = LB.Next<gLuaTriangle<>>();
  switch (glm::clip(plane, line, t1, t1)) {
    case 1: BIND_PUSH(LB, t1); break;
    case 2: BIND_PUSH(LB, t1, t2); break;
    default: {
      break;
    }
  }
  return gLuaBase::Push(LB);
  GLM_BINDING_END
}

static const luaL_Reg luaglm_planelib[] = {
  { "operator_negate", GLM_NAME(plane_operator_negate) },
  { "operator_equals", GLM_NAME(plane_operator_equals) },
  { "operator_add", GLM_NAME(plane_operator_add) },
  { "operator_sub", GLM_NAME(plane_operator_sub) },
  { "operator_mul", GLM_NAME(plane_operator_mul) },
  { "equal", GLM_NAME(plane_equal) },
  { "notEqual", GLM_NAME(plane_notEqual) },
  //{ "tostring", GLM_NAME(plane_tostring) },
  { "isinf", GLM_NAME(plane_isinf) },
  { "isnan", GLM_NAME(plane_isnan) },
  { "isfinite", GLM_NAME(plane_isfinite) },
  { "fromRay", GLM_NAME(plane_fromRay) },
  { "fromLine", GLM_NAME(plane_fromLine) },
  { "fromLineSegment", GLM_NAME(plane_fromLineSegment) },
  { "fromPointNormal", GLM_NAME(plane_fromPointNormal) },
  { "fromPoints", GLM_NAME(plane_fromPoints) },
  { "isDegenerate", GLM_NAME(plane_isDegenerate) },
  { "isParallel", GLM_NAME(plane_isParallel) },
  { "areOnSameSide", GLM_NAME(plane_areOnSameSide) },
  { "examineSide", GLM_NAME(plane_examineSide) },
  { "isInPositiveDirection", GLM_NAME(plane_isInPositiveDirection) },
  { "isOnPositiveSide", GLM_NAME(plane_isOnPositiveSide) },
  { "passesThroughOrigin", GLM_NAME(plane_passesThroughOrigin) },
  { "angle", GLM_NAME(plane_angle) },
  { "reverseNormal", GLM_NAME(plane_reverseNormal) },
  { "pointOnPlane", GLM_NAME(plane_pointOnPlane) },
  { "point", GLM_NAME(plane_point) },
  { "refract", GLM_NAME(plane_refract) },
  { "project", GLM_NAME(plane_project) },
  { "projectLine", GLM_NAME(plane_projectLine) },
  { "projectSegment", GLM_NAME(plane_projectSegment) },
  { "projectRay", GLM_NAME(plane_projectRay) },
  { "projectTriangle", GLM_NAME(plane_projectTriangle) },
  { "projectToNegativeHalf", GLM_NAME(plane_projectToNegativeHalf) },
  { "projectToPositiveHalf", GLM_NAME(plane_projectToPositiveHalf) },
  { "distance", GLM_NAME(plane_distance) },
  { "distanceSegment", GLM_NAME(plane_distanceSegment) },
  { "distanceSphere", GLM_NAME(plane_distanceSphere) },
  { "signedDistance", GLM_NAME(plane_signedDistance) },
  { "signedDistanceLine", GLM_NAME(plane_signedDistanceLine) },
  { "signedDistanceSegment", GLM_NAME(plane_signedDistanceSegment) },
  { "signedDistanceRay", GLM_NAME(plane_signedDistanceRay) },
  { "signedDistanceAABB", GLM_NAME(plane_signedDistanceAABB) },
  { "signedDistanceSphere", GLM_NAME(plane_signedDistanceSphere) },
  { "signedDistanceTriangle", GLM_NAME(plane_signedDistanceTriangle) },
  { "orthoProjection", GLM_NAME(plane_orthoProjection) },
  { "mirrorMatrix", GLM_NAME(plane_mirrorMatrix) },
  { "mirror", GLM_NAME(plane_mirror) },
  { "closestPointRay", GLM_NAME(plane_closestPointRay) },
  { "closestPointSegment", GLM_NAME(plane_closestPointSegment) },
  { "contains", GLM_NAME(plane_contains) },
  { "containsLine", GLM_NAME(plane_containsLine) },
  { "containsRay", GLM_NAME(plane_containsRay) },
  { "containsSegment", GLM_NAME(plane_containsSegment) },
  { "containsTriangle", GLM_NAME(plane_containsTriangle) },
  { "intersectsRay", GLM_NAME(plane_intersectsRay) },
  { "intersectsLine", GLM_NAME(plane_intersectsLine) },
  { "intersectsSegment", GLM_NAME(plane_intersectsSegment) },
  { "intersectsTriangle", GLM_NAME(plane_intersectsTriangle) },
  { "intersectsSphere", GLM_NAME(plane_intersectsSphere) },
  { "intersectsAABB", GLM_NAME(plane_intersectsAABB) },
  { "intersectsPlane", GLM_NAME(plane_intersectsPlane) },
  { "intersectsTriangle", GLM_NAME(plane_intersectsTriangle) },
  { "clipSegment", GLM_NAME(plane_clipSegment) },
  { "clipLine", GLM_NAME(plane_clipLine) },
  { "clipTriangle", GLM_NAME(plane_clipTriangle) },
#if defined(LUAGLM_ALIASES_O3DE)
  //{ "ToString", GLM_NAME() }, // @O3DEAlias
  //{ "CreateFromCoefficients", GLM_NAME() }, // @O3DEAlias
  //{ "CreateFromNormalAndPoint", GLM_NAME() }, // @O3DEAlias
  //{ "CreateFromNormalAndDistance", GLM_NAME() }, // @O3DEAlias
  { "CreateFromTriangle", GLM_NAME(plane_fromPoints) },
  { "ApplyTransform", GLM_NAME(plane_operator_mul) },
  { "GetTransform", GLM_NAME(plane_operator_mul) },
  { "GetPointDist", GLM_NAME(plane_distance) },
  { "GetProjected", GLM_NAME(plane_project) },
  { "CastRay", GLM_NAME(plane_intersectsRay) },
  { "IntersectSegment", GLM_NAME(plane_intersectsSegment) },
  { "IsFinite", GLM_NAME(plane_isfinite) },
  //{ "Clone", GLM_NAME() }, // @Pointless
  //{ "GetNormal", GLM_NAME() }, // Pointless
  //{ "GetDistance", GLM_NAME() }, // Pointless
  //{ "SetNormal", GLM_NAME() }, // Immutable arguments
  //{ "SetDistance", GLM_NAME() }, // Immutable arguments
  //{ "GetPlaneEquationCoefficients", GLM_NAME() }, // Pointless?
#endif
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* }================================================================== */

/*
** {==================================================================
** Polygon
** ===================================================================
*/

BIND_DEFN(polygon_operator_negate, operator-, gLuaPolygon<>)
BIND_DEFN(polygon_operator_equals, operator==, gLuaPolygon<>, gLuaPolygon<>)
BIND_DEFN(polygon_operator_add, operator+, gLuaPolygon<>, gLuaPolygon<>::point_trait)
BIND_DEFN(polygon_operator_sub, operator-, gLuaPolygon<>, gLuaPolygon<>::point_trait)
ROTATION_MATRIX_DEFN(polygon_operator_mul, operator*, LAYOUT_UNARY, gLuaPolygon<>::as_type<gLuaQuat<>::value_type>)
BIND_DEFN(polygon_edge, glm::edge, gLuaPolygon<>, gLuaTrait<size_t>)
BIND_DEFN(polygon_edge2d, glm::edge2d, gLuaPolygon<>, gLuaTrait<size_t>)
BIND_DEFN(polygon_diagonal, glm::diagonal, gLuaPolygon<>, gLuaTrait<size_t>, gLuaTrait<size_t>)
BIND_DEFN(polygon_diagonalExists, glm::diagonalExists, gLuaPolygon<>, gLuaTrait<size_t>, gLuaTrait<size_t>)
BIND_DEFN(polygon_basisU, glm::basisU, gLuaPolygon<>)
BIND_DEFN(polygon_basisV, glm::basisV, gLuaPolygon<>)
BIND_DEFN(polygon_mapFrom2D, glm::mapFrom2D, gLuaPolygon<>, gLuaVec2<gLuaPolygon<>::value_type>)
BIND_DEFN(polygon_area, glm::area, gLuaPolygon<>)
BIND_DEFN(polygon_perimeter, glm::perimeter, gLuaPolygon<>)
BIND_DEFN(polygon_centroid, glm::centroid, gLuaPolygon<>)
BIND_DEFN(polygon_isPlanar, glm::isPlanar, gLuaPolygon<>, gLuaPolygon<>::eps_trait)
BIND_DEFN(polygon_isSimple, glm::isSimple, gLuaPolygon<>)
BIND_DEFN(polygon_isNull, glm::isNull, gLuaPolygon<>)
BIND_DEFN(polygon_isfinite, glm::isfinite, gLuaPolygon<>)
BIND_DEFN(polygon_isDegenerate, glm::isDegenerate, gLuaPolygon<>, gLuaPolygon<>::eps_trait)
BIND_DEFN(polygon_isConvex, glm::isConvex, gLuaPolygon<>)
BIND_DEFN(polygon_planeCCW, glm::planeCCW, gLuaPolygon<>)
BIND_DEFN(polygon_normalCCW, glm::normalCCW, gLuaPolygon<>)
BIND_DEFN(polygon_planeCW, glm::planeCW, gLuaPolygon<>)
BIND_DEFN(polygon_normalCW, glm::normalCW, gLuaPolygon<>)
BIND_DEFN(polygon_pointOnEdge, glm::pointOnEdge, gLuaPolygon<>, gLuaPolygon<>::value_trait)
BIND_DEFN(polygon_edgeNormal, glm::edgeNormal, gLuaPolygon<>, gLuaTrait<size_t>)
BIND_DEFN(polygon_edgePlane, glm::edgePlane, gLuaPolygon<>, gLuaTrait<size_t>)
BIND_DEFN(polygon_containsSegment2D, glm::contains2D, gLuaPolygon<>, gLuaSegment<>)
BIND_DEFN(polygon_contains, glm::contains, gLuaPolygon<>, gLuaPolygon<>::point_trait, gLuaPolygon<>::eps_trait)
BIND_DEFN(polygon_containsAbove, glm::containsAbove, gLuaPolygon<>, gLuaPolygon<>::point_trait, gLuaPolygon<>::eps_trait)
BIND_DEFN(polygon_containsBelow, glm::containsBelow, gLuaPolygon<>, gLuaPolygon<>::point_trait, gLuaPolygon<>::eps_trait)
BIND_DEFN(polygon_containsPolygon, glm::contains, gLuaPolygon<>, gLuaPolygon<>, gLuaPolygon<>::eps_trait)
BIND_DEFN(polygon_containsSegment, glm::contains, gLuaPolygon<>, gLuaSegment<>, gLuaPolygon<>::eps_trait)
BIND_DEFN(polygon_containsTriangle, glm::contains, gLuaPolygon<>, gLuaTriangle<>, gLuaPolygon<>::eps_trait)
BIND_DEFN(polygon_minimalEnclosingAABB, glm::minimalEnclosingAABB, gLuaPolygon<>)
BIND_DEFN(polygon_intersectsSegment2D, glm::intersects2D, gLuaPolygon<>, gLuaSegment<>)
BIND_DEFN(polygon_intersectsLine, glm::intersects, gLuaPolygon<>, gLuaLine<>)
BIND_DEFN(polygon_intersectsRay, glm::intersects, gLuaPolygon<>, gLuaRay<>)
BIND_DEFN(polygon_intersectsSegment, glm::intersects, gLuaPolygon<>, gLuaSegment<>)
BIND_DEFN(polygon_intersectsPlane, glm::intersects, gLuaPolygon<>, gLuaPlane<>)
LAYOUT_DEFN(polygon_projectToAxis, glm::projectToAxis, GEOM_PROJECTION, gLuaPolygon<>, gLuaPolygon<>::point_trait)

GLM_BINDING_QUALIFIER(polygon_mapTo2D) {
  GLM_BINDING_BEGIN
  if (LB.Is<gLuaTrait<size_t>>(gLuaPolygon<>::stack_size))
    BIND_FUNC(LB, glm::mapTo2D, gLuaPolygon<>, gLuaTrait<size_t>);
  BIND_FUNC(LB, glm::mapTo2D, gLuaPolygon<>, gLuaPolygon<>::point_trait);
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(polygon_extremePoint) {
  GLM_BINDING_BEGIN
  gLuaPolygon<>::value_type distance(0);
  const gLuaPolygon<>::type polygon = LB.Next<gLuaPolygon<>>();
  const gLuaPolygon<>::point_trait::type direction = LB.Next<gLuaPolygon<>::point_trait>();
  const gLuaPolygon<>::point_trait::type point = glm::extremePoint(polygon, direction, distance);
  BIND_PUSH(LB, point, distance);
  GLM_BINDING_END
}

/* Polygon Metamethods */

/// <summary>
/// Create a new polygon from an array of points.
/// </summary>
GLM_BINDING_QUALIFIER(polygon_new) {
  GLM_BINDING_BEGIN
  const int n = LB.top_for_recycle();
  if (!gLuaBase::isnoneornil(LB.L, LB.idx) && !lua_istable(LB.L, LB.idx)) {
    return LUAGLM_ARG_ERROR(LB.L, LB.idx, lua_typename(LB.L, LUA_TTABLE));
  }

  // Create a new polygon userdata.
  void *ptr = lua_newuserdatauv(LB.L, sizeof(gLuaPolygon<>::type), 0);  // [..., poly]
  gLuaPolygon<>::type *polygon = static_cast<gLuaPolygon<>::type *>(ptr);
  polygon->stack_idx = -1;
  polygon->p = GLM_NULLPTR;

  // Setup metatable.
  if (luaL_getmetatable(LB.L, gLuaPolygon<>::Metatable()) == LUA_TTABLE) {  // [..., poly, meta]
    lua_setmetatable(LB.L, -2);  // [..., poly]
    LuaCrtAllocator<gLuaPolygon<>::point_trait::type> allocator(LB.L);

    // Create a vector backed by the Lua allocator.
    using PolyList = glm::List<gLuaPolygon<>::point_trait::type>;
    PolyList *list = static_cast<PolyList *>(allocator.realloc(GLM_NULLPTR, 0, sizeof(PolyList)));
    if (l_unlikely(list == GLM_NULLPTR)) {
      lua_pop(L, 1);
      return LUAGLM_ERROR(L, "polygon allocation error");
    }

    // Populate the polygon with an array of coordinates, if one exists.
    polygon->p = ::new (list) PolyList(LB.L, allocator);
    if (l_likely(n >= 1 && lua_istable(LB.L, LB.idx))) {
      gLuaArray<gLuaPolygon<>::point_trait> lArray(LB.L, LB.idx);
      const auto e = lArray.end();
      for (auto b = lArray.begin(); b != e; ++b) {
        polygon->p->push_back(*b);
      }
    }

    return 1;
  }

  lua_pop(L, 2);
  return LUAGLM_ERROR(L, "invalid polygon metatable");
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(polygon_to_string) {
  gLuaPolygon<>::type *ud = static_cast<gLuaPolygon<>::type *>(luaL_checkudata(L, 1, gLuaPolygon<>::Metatable()));
  if (l_likely(ud->p != GLM_NULLPTR)) {
    ud->p->validate(L);
    lua_pushfstring(L, "Polygon<%I>", ud->p->size());
    return 1;
  }

  return LUAGLM_ARG_ERROR(L, 1, "Polygon");
}

/// <summary>
/// Garbage collect an allocated polygon userdata.
/// </summary>
GLM_BINDING_QUALIFIER(polygon_gc) {
  gLuaPolygon<>::type *ud = static_cast<gLuaPolygon<>::type *>(luaL_checkudata(L, 1, gLuaPolygon<>::Metatable()));
  if (l_likely(ud->p != GLM_NULLPTR)) {
    LuaCrtAllocator<void> allocator(L);
    ud->p->validate(L);
    ud->p->~LuaVector();  // Invoke destructor.
    allocator.realloc(ud->p, sizeof(glm::List<gLuaPolygon<>::point_trait::type>), 0);  // Free allocation
    ud->p = GLM_NULLPTR;
  }
  return 0;
}

/// <summary>
/// The number of points within a polygon.
/// </summary>
BIND_DEFN(polygon_len, glm::length, gLuaPolygon<>)

/// <summary>
/// Create an array of points.
/// </summary>
GLM_BINDING_QUALIFIER(polygon_call) {
  GLM_BINDING_BEGIN
  const gLuaPolygon<>::type poly = LB.Next<gLuaPolygon<>>();
  lua_createtable(LB.L, static_cast<int>(poly.size()), 0);  // arrow won't resize if overflow
  for (size_t i = 0; i < poly.size(); ++i) {
    if (l_unlikely(gLuaBase::Push(LB, poly[i]) != 1))
      return LUAGLM_ERROR(LB.L, "invalid " GLM_STRING_VECTOR " structure");
    lua_rawseti(LB.L, -2, static_cast<lua_Integer>(i) + 1);
  }
  return 1;
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(polygon_index) {
  GLM_BINDING_BEGIN
  const gLuaPolygon<>::type poly = LB.Next<gLuaPolygon<>>();
  if (LB.IsNextType<size_t>()) {
    const size_t index = LB.AsNextType<size_t>();
    if (1 <= index && index <= poly.size())
      return gLuaBase::Push(LB, poly[index - 1]);
    return gLuaBase::Push(LB);  // nil
  }
  // Attempt to fetch the contents from the polygon library.
  else if (luaL_getmetatable(LB.L, gLuaPolygon<>::Metatable()) == LUA_TTABLE) {
    lua_pushvalue(LB.L, LB.idx);
    lua_rawget(LB.L, -2);
    return 1;  // Have Lua remove the polygon metatable from the stack.
  }

  lua_pop(LB.L, 1);  // Polygon metatable.
  return 0;
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(polygon_newindex) {
  GLM_BINDING_BEGIN
  gLuaPolygon<>::type poly = LB.Next<gLuaPolygon<>>();
  if (l_likely(poly.p != GLM_NULLPTR)) {
    const size_t index = LB.AsNextType<size_t>();
    const gLuaPolygon<>::point_trait::type value = LB.Next<gLuaPolygon<>::point_trait>();

    poly.p->validate(LB.L);
    if (index >= 1 && index <= poly.size())
      poly[index - 1] = value;
    else if (index == poly.size() + 1)
      poly.p->push_back(value);
    else {
      return LUAGLM_ERROR(LB.L, "Invalid polygon index");
    }
  }
  return 0;
  GLM_BINDING_END
}

extern "C" {
  /// <summary>
  /// Iterator function for polygon vertices.
  /// </summary>
  static int polygon_iterator(lua_State *L) {
    GLM_BINDING_BEGIN
    if (!LB.Is<gLuaPolygon<>>()) {
      return LUAGLM_ARG_ERROR(LB.L, LB.idx, gLuaPolygon<>::Label());
    }

    lua_settop(LB.L, LB.idx + gLuaPolygon<>::stack_size);  // create a 2nd argument if there isn't one
    const gLuaPolygon<>::type poly = LB.Next<gLuaPolygon<>>();  // Polygon
    if (LB.IsNextType<size_t>()) {  // Index
      const size_t key = LB.AsNextType<size_t>();
      if (key >= 1 && key < poly.size())
        BIND_PUSH(LB, key + 1, poly[key]);
      return gLuaBase::Push(LB);
    }
    else if (gLuaBase::isnoneornil(LB.L, LB.idx) && poly.size() > 0)  // First index
      BIND_PUSH(LB, size_t(1), poly[0]);
    else
      return gLuaBase::Push(LB);  // Nothing to iterate.
    GLM_BINDING_END
  }
}

GLM_BINDING_QUALIFIER(polygon_pairs) {
  lua_pushcfunction(L, polygon_iterator);  // will return generator,
  lua_pushvalue(L, 1);  // state,
  lua_pushnil(L);  // and initial value
  return 3;
}

static const luaL_Reg luaglm_polylib[] = {
  { "__gc", GLM_NAME(polygon_gc) },
  { "__index", GLM_NAME(polygon_index) },  // Array access
  { "__newindex", GLM_NAME(polygon_newindex) },  // Only allow append
  { "__len", GLM_NAME(polygon_len) },  // # of Points
  { "__call", GLM_NAME(polygon_call) },  // Generate a table.
  { "__pairs", GLM_NAME(polygon_pairs) },
  { "__unm", GLM_NAME(polygon_operator_negate) },  // Negate all points.
  { "__eq", GLM_NAME(polygon_operator_equals) },
  { "__add", GLM_NAME(polygon_operator_add) },
  { "__sub", GLM_NAME(polygon_operator_sub) },
  { "__mul", GLM_NAME(polygon_operator_mul) },
  { "__tostring", GLM_NAME(polygon_to_string) },
  { "new", GLM_NAME(polygon_new) },
  { "operator_negate", GLM_NAME(polygon_operator_negate) },
  { "operator_equals", GLM_NAME(polygon_operator_equals) },
  { "operator_add", GLM_NAME(polygon_operator_add) },
  { "operator_sub", GLM_NAME(polygon_operator_sub) },
  { "operator_mul", GLM_NAME(polygon_operator_mul) },
  { "edge", GLM_NAME(polygon_edge) },
  { "edge2d", GLM_NAME(polygon_edge2d) },
  { "diagonal", GLM_NAME(polygon_diagonal) },
  { "diagonalExists", GLM_NAME(polygon_diagonalExists) },
  { "basisU", GLM_NAME(polygon_basisU) },
  { "basisV", GLM_NAME(polygon_basisV) },
  { "mapTo2D", GLM_NAME(polygon_mapTo2D) },
  { "mapFrom2D", GLM_NAME(polygon_mapFrom2D) },
  { "area", GLM_NAME(polygon_area) },
  { "perimeter", GLM_NAME(polygon_perimeter) },
  { "centroid", GLM_NAME(polygon_centroid) },
  { "isPlanar", GLM_NAME(polygon_isPlanar) },
  { "isSimple", GLM_NAME(polygon_isSimple) },
  { "isNull", GLM_NAME(polygon_isNull) },
  { "isfinite", GLM_NAME(polygon_isfinite) },
  { "isDegenerate", GLM_NAME(polygon_isDegenerate) },
  { "isConvex", GLM_NAME(polygon_isConvex) },
  { "extremePoint", GLM_NAME(polygon_extremePoint) },
  { "projectToAxis", GLM_NAME(polygon_projectToAxis) },
  { "planeCCW", GLM_NAME(polygon_planeCCW) },
  { "normalCCW", GLM_NAME(polygon_normalCCW) },
  { "planeCW", GLM_NAME(polygon_planeCW) },
  { "normalCW", GLM_NAME(polygon_normalCW) },
  { "pointOnEdge", GLM_NAME(polygon_pointOnEdge) },
  { "edgeNormal", GLM_NAME(polygon_edgeNormal) },
  { "edgePlane", GLM_NAME(polygon_edgePlane) },
  { "containsSegment2D", GLM_NAME(polygon_containsSegment2D) },
  { "contains", GLM_NAME(polygon_contains) },
  { "containsAbove", GLM_NAME(polygon_containsAbove) },
  { "containsBelow", GLM_NAME(polygon_containsBelow) },
  { "containsPolygon", GLM_NAME(polygon_containsPolygon) },
  { "containsSegment", GLM_NAME(polygon_containsSegment) },
  { "containsTriangle", GLM_NAME(polygon_containsTriangle) },
  { "minimalEnclosingAABB", GLM_NAME(polygon_minimalEnclosingAABB) },
  { "intersectsSegment2D", GLM_NAME(polygon_intersectsSegment2D) },
  { "intersectsLine", GLM_NAME(polygon_intersectsLine) },
  { "intersectsRay", GLM_NAME(polygon_intersectsRay) },
  { "intersectsSegment", GLM_NAME(polygon_intersectsSegment) },
  { "intersectsPlane", GLM_NAME(polygon_intersectsPlane) },
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* }================================================================== */

#endif
