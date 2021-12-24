/*
** $Id: geom.hpp $
** Geometric Structures
**
** See Copyright Notice in lua.h
*/
#ifndef BINDING_GEOM_HPP
#define BINDING_GEOM_HPP

#include "lglm.hpp"
#include "lglm_core.h"

#include "allocator.hpp"
#include "iterators.hpp"
#include "bindings.hpp"

#include "ext/geom/setup.hpp"
#include "ext/geom/aabb.hpp"
#include "ext/geom/line.hpp"
#include "ext/geom/linesegment.hpp"
#include "ext/geom/ray.hpp"
#include "ext/geom/triangle.hpp"
#include "ext/geom/sphere.hpp"
#include "ext/geom/plane.hpp"
#include "ext/geom/polygon.hpp"

/* All geometric objects adhere to the glm::equal/glm::notEqual API. */
#define GEOM_EQUALS(LB, F, Tr, ...) \
  LAYOUT_GENERIC_EQUAL(LB, F, Tr, Tr::point_trait::fast)

/*
** Generic distance definition: returning the distance between a geometric
** object and point-of-interest along with the parametric distances of intersection
*/
#define GEOM_DISTANCE(LB, F, A, B) \
  LUA_MLM_BEGIN                    \
  const A::type a = A::Next(LB);   \
  const B::type b = B::Next(LB);   \
  A::value_type t(0);              \
  TRAITS_PUSH(LB, F(a, b, t), t);  \
  LUA_MLM_END

/*
** Generic intersects definition where the line/ray/segment is the first
** parameter being tested against the structure passed as the second parameter.
** Returning the point of intersection and relative location along each object.
*/
#define GEOM_INTERSECTS(LB, F, A, B)                   \
  LUA_MLM_BEGIN                                        \
  const A::type a = A::Next(LB);                       \
  const B::type b = B::Next(LB);                       \
  A::zero_trait::type n = A::safe::zero_trait::zero(); \
  A::one_trait::type f = A::safe::one_trait::zero();   \
  TRAITS_PUSH(LB, F(a, b, n, f), n, f);                \
  LUA_MLM_END

/*
** The line/ray/segment is the second parameter being tested against the
** structure passed as the first parameter.
*/
#define GEOM_INTERSECTS_RH(LB, F, A, B)                \
  LUA_MLM_BEGIN                                        \
  const A::type a = A::Next(LB);                       \
  const B::type b = B::Next(LB);                       \
  B::zero_trait::type n = B::safe::zero_trait::zero(); \
  B::one_trait::type f = B::safe::one_trait::zero();   \
  TRAITS_PUSH(LB, F(a, b, n, f), n, f);                \
  LUA_MLM_END

/*
** Intersection test with a result (e.g., boolean), UV coordinates, and a
** distance along the object
*/
#define GEOM_INTERSECTS_UV(LB, F, A, B)       \
  LUA_MLM_BEGIN                               \
  const A::type a = A::Next(LB);              \
  const B::type b = B::Next(LB);              \
  A::value_type x(0), y(0), z(0);             \
  TRAITS_PUSH(LB, F(a, b, x, y, z), x, y, z); \
  LUA_MLM_END

/*
** Intersection test with a result (e.g., boolean) and a single intersection
** object that may also be returned.
*/
#define GEOM_INTERSECTS_PT(LB, F, A, B)             \
  LUA_MLM_BEGIN                                     \
  A::point_trait::type pt = A::point_trait::zero(); \
  const A::type a = A::Next(LB);                    \
  const B::type b = B::Next(LB);                    \
  TRAITS_PUSH(LB, F(a, b, pt), pt);                 \
  LUA_MLM_END

/*
** Generic project-to-axis definition; returning the parametric min & max of the
** axis projection.
*/
#define GEOM_PROJECTION(LB, F, A, B)  \
  LUA_MLM_BEGIN                       \
  const A::type a = A::Next(LB);      \
  const B::type b = B::Next(LB);      \
  A::value_type outMin(0), outMax(0); \
  F(a, b, outMin, outMax);            \
  TRAITS_PUSH(LB, outMin, outMax);    \
  LUA_MLM_END

/// <summary>
/// Relative position along a line, segment, ray for casting.
/// </summary>
template<bool isNear, bool isRelative, typename T = glm_Float>
struct gLuaRelativePosition : gLuaTrait<T> {
  static GLM_CONSTEXPR const char *Label() { return "RelativePosition"; }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR T zero() {
    GLM_IF_CONSTEXPR(isNear)
      return isRelative ? T(0) : -std::numeric_limits<T>::infinity();
    return isRelative ? T(1) : std::numeric_limits<T>::infinity();
  }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L_, int idx) {
    return lua_isnoneornil(L_, idx) || gLuaTrait<T>::Is(L_, idx);
  }

  LUA_TRAIT_QUALIFIER T Next(gLuaBase &LB) {
    if (lua_isnoneornil(LB.L, LB.idx)) {
      LB.idx++;  // Skip the argument
      return zero();
    }

    return gLuaTrait<T>::Next(LB);
  }
};

template<glm::length_t L = 3, typename T = glm_Float>
struct gLuaAABB : gLuaAbstractTrait<glm::AABB<L, T>> {
  template<typename Type = T>
  using as_type = gLuaAABB<L, Type>;  // @CastBinding
  using safe = gLuaAABB;  // @SafeBinding
  using fast = gLuaAABB;  // @UnsafeBinding

  /// <summary>
  /// @PointBinding: Type trait equivalent to glm::Structure::point_type
  /// </summary>
  using point_trait = gLuaTrait<typename glm::AABB<L, T>::point_type>;

  static GLM_CONSTEXPR const char *Label() { return "AABB"; }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR glm::AABB<L, T> zero() {
    return glm::AABB<L, T>();
  }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L_, int idx) {
    return point_trait::Is(L_, idx) && point_trait::Is(L_, idx + 1);
  }

  LUA_TRAIT_QUALIFIER glm::AABB<L, T> Next(gLuaBase &LB) {
    glm::AABB<L, T> result;
    result.minPoint = point_trait::Next(LB);
    result.maxPoint = point_trait::Next(LB);
    return result;
  }
};

template<glm::length_t L = 3, typename T = glm_Float>
struct gLuaLine : gLuaAbstractTrait<glm::Line<L, T>> {
  template<typename Type = T>
  using as_type = gLuaLine<L, Type>;  // @CastBinding
  using safe = gLuaLine;  // @SafeBinding
  using fast = gLuaLine;  // @UnsafeBinding
  using point_trait = gLuaTrait<typename glm::Line<L, T>::point_type>;  // @PointBinding

  /// <summary>
  /// @RelativeZero: Lua type trait representing the relative negative-inf/zero
  /// coordinate of the object.
  /// </summary>
  using zero_trait = gLuaRelativePosition<true, false, T>;

  /// <summary>
  /// @RelativeOne: Lua type trait representing the relative inf/one coordinate
  /// of the object.
  /// </summary>
  using one_trait = gLuaRelativePosition<false, false, T>;

  static GLM_CONSTEXPR const char *Label() { return "Line"; }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR glm::Line<L, T> zero() {
    return glm::Line<L, T>();
  }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L_, int idx) {
    return point_trait::Is(L_, idx) && point_trait::Is(L_, idx + 1);
  }

  LUA_TRAIT_QUALIFIER glm::Line<L, T> Next(gLuaBase &LB) {
    glm::Line<L, T> result;
    result.pos = point_trait::Next(LB);
    result.dir = glm_drift_compensate(point_trait::Next(LB));
    return result;
  }
};

template<glm::length_t L = 3, typename T = glm_Float>
struct gLuaSegment : gLuaAbstractTrait<glm::LineSegment<L, T>> {
  template<typename Type = T>
  using as_type = gLuaSegment<L, Type>;  // @CastBinding
  using safe = gLuaSegment;  // @SafeBinding
  using fast = gLuaSegment;  // @UnsafeBinding
  using point_trait = gLuaTrait<typename glm::LineSegment<L, T>::point_type>;  // @PointBinding
  using zero_trait = gLuaRelativePosition<true, true, T>;  // @RelativeZero
  using one_trait = gLuaRelativePosition<false, true, T>;  // @RelativeOne

  static GLM_CONSTEXPR const char *Label() { return "Segment"; }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR glm::LineSegment<L, T> zero() {
    return glm::LineSegment<L, T>();
  }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L_, int idx) {
    return point_trait::Is(L_, idx) && point_trait::Is(L_, idx + 1);
  }

  LUA_TRAIT_QUALIFIER glm::LineSegment<L, T> Next(gLuaBase &LB) {
    glm::LineSegment<L, T> result;
    result.a = point_trait::Next(LB);
    result.b = point_trait::Next(LB);
    return result;
  }
};

template<glm::length_t L = 3, typename T = glm_Float>
struct gLuaRay : gLuaAbstractTrait<glm::Ray<L, T>> {
  template<typename Type = T>
  using as_type = gLuaRay<L, Type>;  // @CastBinding
  using safe = gLuaRay;  // @SafeBinding
  using fast = gLuaRay;  // @UnsafeBinding
  using point_trait = gLuaTrait<typename glm::Ray<L, T>::point_type>;  // @PointBinding
  using zero_trait = gLuaRelativePosition<true, true, T>;  // @RelativeZero
  using one_trait = gLuaRelativePosition<false, false, T>;  // @RelativeOne

  static GLM_CONSTEXPR const char *Label() { return "Ray"; }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR glm::Ray<L, T> zero() {
    return glm::Ray<L, T>();
  }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L_, int idx) {
    return point_trait::Is(L_, idx) && point_trait::Is(L_, idx + 1);
  }

  LUA_TRAIT_QUALIFIER glm::Ray<L, T> Next(gLuaBase &LB) {
    glm::Ray<L, T> result;
    result.pos = point_trait::Next(LB);
    result.dir = glm_drift_compensate(point_trait::Next(LB));
    return result;
  }
};

template<glm::length_t L = 3, typename T = glm_Float>
struct gLuaTriangle : gLuaAbstractTrait<glm::Triangle<L, T>> {
  template<typename Type = T>
  using as_type = gLuaTriangle<L, Type>;  // @CastBinding
  using safe = gLuaTriangle;  // @SafeBinding
  using fast = gLuaTriangle;  // @UnsafeBinding
  using point_trait = gLuaTrait<typename glm::Triangle<L, T>::point_type>;  // @PointBinding

  static GLM_CONSTEXPR const char *Label() { return "Triangle"; }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR glm::Triangle<L, T> zero() {
    return glm::Triangle<L, T>();
  }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L_, int idx) {
    return point_trait::Is(L_, idx) && point_trait::Is(L_, idx + 1) && point_trait::Is(L_, idx + 2);
  }

  LUA_TRAIT_QUALIFIER glm::Triangle<L, T> Next(gLuaBase &LB) {
    glm::Triangle<L, T> result;
    result.a = point_trait::Next(LB);
    result.b = point_trait::Next(LB);
    result.c = point_trait::Next(LB);
    return result;
  }
};

template<glm::length_t L = 3, typename T = glm_Float>
struct gLuaSphere : gLuaAbstractTrait<glm::Sphere<L, T>> {
  template<typename Type = T>
  using as_type = gLuaSphere<L, Type>;  // @CastBinding
  using safe = gLuaSphere;  // @SafeBinding
  using fast = gLuaSphere;  // @UnsafeBinding
  using point_trait = gLuaTrait<typename glm::Sphere<L, T>::point_type>;  // @PointBinding

  static GLM_CONSTEXPR const char *Label() { return "Sphere"; }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR glm::Plane<L, T> zero() {
    return glm::Plane<L, T>();
  }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L_, int idx) {
    return point_trait::Is(L_, idx) && gLuaTrait<T>::Is(L_, idx + 1);
  }

  LUA_TRAIT_QUALIFIER glm::Sphere<L, T> Next(gLuaBase &LB) {
    glm::Sphere<L, T> result;
    result.pos = point_trait::Next(LB);
    result.r = gLuaSphere::value_trait::Next(LB);
    return result;
  }
};

template<glm::length_t L = 3, typename T = glm_Float>
struct gLuaPlane : gLuaAbstractTrait<glm::Plane<L, T>> {
  template<typename Type = T>
  using as_type = gLuaPlane<L, Type>;  // @CastBinding
  using safe = gLuaPlane;  // @SafeBinding
  using fast = gLuaPlane;  // @UnsafeBinding
  using point_trait = gLuaTrait<typename glm::Plane<L, T>::point_type>;  // @PointBinding

  static GLM_CONSTEXPR const char *Label() { return "Plane"; }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR glm::Plane<L, T> zero() {
    return glm::Plane<L, T>();
  }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L_, int idx) {
    return point_trait::Is(L_, idx) && gLuaTrait<T>::Is(L_, idx + 1);
  }

  LUA_TRAIT_QUALIFIER glm::Plane<L, T> Next(gLuaBase &LB) {
    glm::Plane<L, T> result;
    result.normal = point_trait::Next(LB);
    result.d = gLuaPlane::value_trait::Next(LB);
    return result;
  }
};

/// <summary>
/// An (explicitly three dimensional) polygon trait.
///
/// @TODO: More creative casting rules for generalized polygons, e.g., the
///   userdata also storing the dimensions to each point.
/// </summary>
template<typename T = glm_Float>
struct gLuaPolygon : gLuaAbstractTrait<glm::Polygon<3, T>> {
  template<typename Type = T>
  using as_type = gLuaPolygon<Type>;  // @CastBinding
  using safe = gLuaPolygon;  // @SafeBinding
  using fast = gLuaPolygon;  // @UnsafeBinding
  using point_trait = gLuaTrait<typename glm::Polygon<3, T>::point_type>;  // @PointBinding

  static GLM_CONSTEXPR const char *Label() { return "Polygon"; }
  static GLM_CONSTEXPR const char *Metatable() { return "GLM_POLYGON"; }

  LUA_TRAIT_QUALIFIER GLM_CONSTEXPR glm::Polygon<3, T> zero() {
    return glm::Polygon<3, T>(GLM_NULLPTR);
  }

  LUA_TRAIT_QUALIFIER bool Is(lua_State *L, int idx) {
    return luaL_testudata(L, idx, Metatable()) != GLM_NULLPTR;
  }

  LUA_TRAIT_QUALIFIER glm::Polygon<3, T> Next(gLuaBase &LB) {
    void *ptr = GLM_NULLPTR;
    if ((ptr = luaL_checkudata(LB.L, LB.idx++, Metatable())) != GLM_NULLPTR) {
      glm::Polygon<3, T> result = *(static_cast<glm::Polygon<3, T> *>(ptr));
      result.stack_idx = LB.idx - 1;
      result.p->Validate(LB.L);
      return result;
    }
    else {
      luaL_error(LB.L, "Invalid polygon userdata");
    }
    return glm::Polygon<3, T>();
  }
};

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
  using Iterator = glmLuaArray<gLuaAABB<>::point_trait>::Iterator;
  glmLuaArray<gLuaAABB<>::point_trait> lArray(LB.L, LB.idx);
  return gLuaBase::Push(LB, glm::minimalEnclosingAABB<Iterator, 3, value_type>(lArray.begin(), lArray.end()));
  GLM_BINDING_END
}

/* Create an AABB from a coordinate & radius. */
TRAITS_LAYOUT_DEFN(aabb_fromCenterAndSize, glm::aabbFromCenterAndSize, LAYOUT_BINARY_OPTIONAL, gLuaAABB<>::point_trait)
TRAITS_DEFN(aabb_fromSphere, glm::aabbFromSphere, gLuaSphere<>)
TRAITS_DEFN(aabb_operator_negate, operator-, gLuaAABB<>)
TRAITS_DEFN(aabb_operator_equals, operator==, gLuaAABB<>, gLuaAABB<>)
TRAITS_DEFN(aabb_operator_add, operator+, gLuaAABB<>, gLuaAABB<>::point_trait)
TRAITS_DEFN(aabb_operator_sub, operator-, gLuaAABB<>, gLuaAABB<>::point_trait)
ROTATION_MATRIX_DEFN(aabb_operator_mul, operator*, LAYOUT_UNARY, gLuaAABB<>::as_type<gLuaQuat<>::value_type>)
TRAITS_LAYOUT_DEFN(aabb_equal, glm::equal, GEOM_EQUALS, gLuaAABB<>)
TRAITS_LAYOUT_DEFN(aabb_notEqual, glm::notEqual, GEOM_EQUALS, gLuaAABB<>)
TRAITS_DEFN(aabb_isinf, glm::isinf, gLuaAABB<>)
TRAITS_DEFN(aabb_isnan, glm::isnan, gLuaAABB<>)
TRAITS_DEFN(aabb_isfinite, glm::isfinite, gLuaAABB<>)
TRAITS_DEFN(aabb_isDegenerate, glm::isDegenerate, gLuaAABB<>)
TRAITS_DEFN(aabb_centerPoint, glm::centerPoint, gLuaAABB<>)
TRAITS_DEFN(aabb_pointInside, glm::pointInside, gLuaAABB<>, gLuaAABB<>::point_trait)
TRAITS_DEFN(aabb_minimalEnclosingSphere, glm::minimalEnclosingSphere, gLuaAABB<>)
TRAITS_DEFN(aabb_maximalContainedSphere, glm::maximalContainedSphere, gLuaAABB<>)
TRAITS_DEFN(aabb_edge, glm::edge, gLuaAABB<>, gLuaTrait<int>)
TRAITS_DEFN(aabb_cornerPoint, glm::cornerPoint, gLuaAABB<>, gLuaTrait<int>)
TRAITS_DEFN(aabb_extremePoint, glm::extremePoint, gLuaAABB<>, gLuaAABB<>::point_trait)
TRAITS_DEFN(aabb_pointOnEdge, glm::pointOnEdge, gLuaAABB<>, gLuaTrait<int>, gLuaAABB<>::value_trait)
TRAITS_DEFN(aabb_faceCenterPoint, glm::faceCenterPoint, gLuaAABB<>, gLuaTrait<int>)
TRAITS_DEFN(aabb_facePoint, glm::facePoint, gLuaAABB<>, gLuaTrait<int>, gLuaAABB<>::value_trait, gLuaAABB<>::value_trait)
TRAITS_DEFN(aabb_faceNormal, glm::faceNormalAABB<gLuaFloat::value_type>, gLuaTrait<int>)
TRAITS_DEFN(aabb_facePlane, glm::facePlane, gLuaAABB<>, gLuaTrait<int>)
TRAITS_DEFN(aabb_size, glm::size, gLuaAABB<>)
TRAITS_DEFN(aabb_halfSize, glm::halfSize, gLuaAABB<>)
TRAITS_DEFN(aabb_volume, glm::volume, gLuaAABB<>)
TRAITS_DEFN(aabb_surfaceArea, glm::surfaceArea, gLuaAABB<>)
TRAITS_DEFN(aabb_scale, glm::scale, gLuaAABB<>, gLuaAABB<>::point_trait, gLuaAABB<>::value_trait)
TRAITS_DEFN(aabb_closestPoint, glm::closestPoint, gLuaAABB<>, gLuaAABB<>::point_trait)
TRAITS_DEFN(aabb_distance, glm::distance, gLuaAABB<>, gLuaAABB<>::point_trait)
TRAITS_DEFN(aabb_distanceSphere, glm::distance, gLuaAABB<>, gLuaSphere<>)
TRAITS_DEFN(aabb_contains, glm::contains, gLuaAABB<>, gLuaAABB<>::point_trait)
TRAITS_DEFN(aabb_containsAABB, glm::contains, gLuaAABB<>, gLuaAABB<>)
TRAITS_DEFN(aabb_containsSegment, glm::contains, gLuaAABB<>, gLuaSegment<>)
TRAITS_DEFN(aabb_containsTriangle, glm::contains, gLuaAABB<>, gLuaTriangle<>)
TRAITS_DEFN(aabb_containsSphere, glm::contains, gLuaAABB<>, gLuaSphere<>)
TRAITS_DEFN(aabb_containsPolygon, glm::contains, gLuaAABB<>, gLuaPolygon<>)
TRAITS_DEFN(aabb_grow, glm::grow, gLuaAABB<>, gLuaAABB<>::value_trait)
TRAITS_DEFN(aabb_enclose, glm::enclose, gLuaAABB<>, gLuaAABB<>::point_trait)
TRAITS_DEFN(aabb_encloseSegment, glm::enclose, gLuaAABB<>, gLuaSegment<>)
TRAITS_DEFN(aabb_encloseTriangle, glm::enclose, gLuaAABB<>, gLuaTriangle<>)
TRAITS_DEFN(aabb_encloseSphere, glm::enclose, gLuaAABB<>, gLuaSphere<>)
TRAITS_DEFN(aabb_encloseAABB, glm::enclose, gLuaAABB<>, gLuaAABB<>)
TRAITS_DEFN(aabb_enclosePolygon, glm::enclose, gLuaAABB<>, gLuaPolygon<>)
TRAITS_DEFN(aabb_intersectsAABB, glm::intersects, gLuaAABB<>, gLuaAABB<>)
TRAITS_DEFN(aabb_intersectsSphere, glm::intersects, gLuaAABB<>, gLuaSphere<>)
TRAITS_DEFN(aabb_intersectsPlane, glm::intersects, gLuaAABB<>, gLuaPlane<>)
//TRAITS_DEFN(aabb_intersectsTriangle, glm::intersects, gLuaAABB<>, gLuaTriangle<>)
TRAITS_LAYOUT_DEFN(aabb_intersectsLine, glm::intersects, GEOM_INTERSECTS_RH, gLuaAABB<>, gLuaLine<>)
TRAITS_LAYOUT_DEFN(aabb_intersectsSegment, glm::intersects, GEOM_INTERSECTS_RH, gLuaAABB<>, gLuaSegment<>)
TRAITS_LAYOUT_DEFN(aabb_intersectsRay, glm::intersects, GEOM_INTERSECTS_RH, gLuaAABB<>, gLuaRay<>)
TRAITS_DEFN(aabb_intersection, glm::intersection, gLuaAABB<>, gLuaAABB<>)
TRAITS_DEFN(aabb_slabs, glm::slabs, gLuaAABB<>, gLuaRay<>)
TRAITS_LAYOUT_DEFN(aabb_projectToAxis, glm::projectToAxis, GEOM_PROJECTION, gLuaAABB<>, gLuaAABB<>::point_trait)

static const luaL_Reg luaglm_aabblib[] = {
  { "new", glm_aabb_new },
  { "fromCenterAndSize", glm_aabb_fromCenterAndSize },
  { "aabbFromSphere", glm_aabb_fromSphere },
  { "operator_negate", glm_aabb_operator_negate },
  { "operator_equals", glm_aabb_operator_equals },
  { "operator_add", glm_aabb_operator_add },
  { "operator_sub", glm_aabb_operator_sub },
  { "operator_mul", glm_aabb_operator_mul },
  { "equal", glm_aabb_equal },
  { "notEqual", glm_aabb_notEqual },
  //{ "tostring", glm_aabb_tostring },
  { "isinf", glm_aabb_isinf },
  { "isnan", glm_aabb_isnan },
  { "isfinite", glm_aabb_isfinite },
  { "isDegenerate", glm_aabb_isDegenerate },
  { "centerPoint", glm_aabb_centerPoint },
  { "centroid", glm_aabb_centerPoint },
  { "pointInside", glm_aabb_pointInside },
  { "minimalEnclosingSphere", glm_aabb_minimalEnclosingSphere },
  { "maximalContainedSphere", glm_aabb_maximalContainedSphere },
  { "edge", glm_aabb_edge },
  { "cornerPoint", glm_aabb_cornerPoint },
  { "extremePoint", glm_aabb_extremePoint },
  { "pointOnEdge", glm_aabb_pointOnEdge },
  { "faceCenterPoint", glm_aabb_faceCenterPoint },
  { "facePoint", glm_aabb_facePoint },
  { "faceNormal", glm_aabb_faceNormal },
  { "facePlane", glm_aabb_facePlane },
  { "size", glm_aabb_size },
  { "halfSize", glm_aabb_halfSize },
  { "diagonal", glm_aabb_size },
  { "halfDiagonal", glm_aabb_halfSize },
  { "volume", glm_aabb_volume },
  { "surfaceArea", glm_aabb_surfaceArea },
  { "scale", glm_aabb_scale },
  { "closestPoint", glm_aabb_closestPoint },
  { "distance", glm_aabb_distance },
  { "distanceSphere", glm_aabb_distanceSphere },
  { "contains", glm_aabb_contains },
  { "containsAABB", glm_aabb_containsAABB },
  { "containsSegment", glm_aabb_containsSegment },
  { "containsTriangle", glm_aabb_containsTriangle },
  { "containsSphere", glm_aabb_containsSphere },
  { "containsPolygon", glm_aabb_containsPolygon },
  { "grow", glm_aabb_grow },
  { "enclose", glm_aabb_enclose },
  { "encloseSegment", glm_aabb_encloseSegment },
  { "encloseTriangle", glm_aabb_encloseTriangle },
  { "encloseSphere", glm_aabb_encloseSphere },
  { "encloseAABB", glm_aabb_encloseAABB },
  { "enclosePolygon", glm_aabb_enclosePolygon },
  { "intersectsAABB", glm_aabb_intersectsAABB },
  { "intersectsSphere", glm_aabb_intersectsSphere },
  { "intersectsPlane", glm_aabb_intersectsPlane },
  //{ "intersectsTriangle", glm_aabb_intersectsTriangle },
  { "intersectsLine", glm_aabb_intersectsLine },
  { "intersectsSegment", glm_aabb_intersectsSegment },
  { "intersectsRay", glm_aabb_intersectsRay },
  { "intersection", glm_aabb_intersection },
  { "slabs", glm_aabb_slabs },
  { "projectToAxis", glm_aabb_projectToAxis },
  /* @DEPRECATED: intersectsObject */
  { "intersectAABB", glm_aabb_intersectsAABB },
  { "intersectSphere", glm_aabb_intersectsSphere },
  { "intersectPlane", glm_aabb_intersectsPlane },
  { "intersectLine", glm_aabb_intersectsLine },
  { "intersectSegment", glm_aabb_intersectsSegment },
  { "intersectRay", glm_aabb_intersectsRay },
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
  using Iterator = glmLuaArray<gLuaAABB<2>::point_trait>::Iterator;
  glmLuaArray<gLuaAABB<2>::point_trait> lArray(LB.L, LB.idx);
  return gLuaBase::Push(LB, glm::minimalEnclosingAABB<Iterator, 2, value_type>(lArray.begin(), lArray.end()));
  GLM_BINDING_END
}

/* Create an AABB from a coordinate & radius. */
TRAITS_LAYOUT_DEFN(aabb2d_fromCenterAndSize, glm::aabbFromCenterAndSize, LAYOUT_BINARY_OPTIONAL, gLuaAABB<2>::point_trait)
TRAITS_DEFN(aabb2d_fromSphere, glm::aabbFromSphere, gLuaSphere<2>)
TRAITS_DEFN(aabb2d_operator_negate, operator-, gLuaAABB<2>)
TRAITS_DEFN(aabb2d_operator_equals, operator==, gLuaAABB<2>, gLuaAABB<2>)
TRAITS_DEFN(aabb2d_operator_add, operator+, gLuaAABB<2>, gLuaAABB<2>::point_trait)
TRAITS_DEFN(aabb2d_operator_sub, operator-, gLuaAABB<2>, gLuaAABB<2>::point_trait)
ROTATION_MATRIX_DEFN(aabb2d_operator_mul, operator*, LAYOUT_UNARY, gLuaAABB<2>::as_type<gLuaQuat<>::value_type>)
TRAITS_LAYOUT_DEFN(aabb2d_equal, glm::equal, GEOM_EQUALS, gLuaAABB<2>)
TRAITS_LAYOUT_DEFN(aabb2d_notEqual, glm::notEqual, GEOM_EQUALS, gLuaAABB<2>)
TRAITS_DEFN(aabb2d_isinf, glm::isinf, gLuaAABB<2>)
TRAITS_DEFN(aabb2d_isnan, glm::isnan, gLuaAABB<2>)
TRAITS_DEFN(aabb2d_isfinite, glm::isfinite, gLuaAABB<2>)
TRAITS_DEFN(aabb2d_isDegenerate, glm::isDegenerate, gLuaAABB<2>)
TRAITS_DEFN(aabb2d_centerPoint, glm::centerPoint, gLuaAABB<2>)
TRAITS_DEFN(aabb2d_pointInside, glm::pointInside, gLuaAABB<2>, gLuaAABB<2>::point_trait)
TRAITS_DEFN(aabb2d_edge, glm::edge, gLuaAABB<2>, gLuaTrait<int>)
TRAITS_DEFN(aabb2d_cornerPoint, glm::cornerPoint, gLuaAABB<2>, gLuaTrait<int>)
TRAITS_DEFN(aabb2d_extremePoint, glm::extremePoint, gLuaAABB<2>, gLuaAABB<2>::point_trait)
TRAITS_DEFN(aabb2d_size, glm::size, gLuaAABB<2>)
TRAITS_DEFN(aabb2d_halfSize, glm::halfSize, gLuaAABB<2>)
TRAITS_DEFN(aabb2d_volume, glm::volume, gLuaAABB<2>)
TRAITS_DEFN(aabb2d_scale, glm::scale, gLuaAABB<2>, gLuaAABB<2>::point_trait, gLuaAABB<2>::value_trait)
TRAITS_DEFN(aabb2d_closestPoint, glm::closestPoint, gLuaAABB<2>, gLuaAABB<2>::point_trait)
TRAITS_DEFN(aabb2d_distance, glm::distance, gLuaAABB<2>, gLuaAABB<2>::point_trait)
TRAITS_DEFN(aabb2d_distanceSphere, glm::distance, gLuaAABB<2>, gLuaSphere<2>)
TRAITS_DEFN(aabb2d_contains, glm::contains, gLuaAABB<2>, gLuaAABB<2>::point_trait)
TRAITS_DEFN(aabb2d_containsAABB, glm::contains, gLuaAABB<2>, gLuaAABB<2>)
TRAITS_DEFN(aabb2d_containsSegment, glm::contains, gLuaAABB<2>, gLuaSegment<2>)
TRAITS_DEFN(aabb2d_containsSphere, glm::contains, gLuaAABB<2>, gLuaSphere<2>)
TRAITS_DEFN(aabb2d_grow, glm::grow, gLuaAABB<2>, gLuaAABB<2>::value_trait)
TRAITS_DEFN(aabb2d_enclose, glm::enclose, gLuaAABB<2>, gLuaAABB<2>::point_trait)
TRAITS_DEFN(aabb2d_encloseSegment, glm::enclose, gLuaAABB<2>, gLuaSegment<2>)
TRAITS_DEFN(aabb2d_encloseSphere, glm::enclose, gLuaAABB<2>, gLuaSphere<2>)
TRAITS_DEFN(aabb2d_encloseAABB, glm::enclose, gLuaAABB<2>, gLuaAABB<2>)
TRAITS_DEFN(aabb2d_intersection, glm::intersection, gLuaAABB<2>, gLuaAABB<2>)
TRAITS_DEFN(aabb2d_intersectsAABB, glm::intersects, gLuaAABB<2>, gLuaAABB<2>)
TRAITS_LAYOUT_DEFN(aabb2d_intersectsLine, glm::intersects, GEOM_INTERSECTS_RH, gLuaAABB<2>, gLuaLine<2>)
TRAITS_LAYOUT_DEFN(aabb2d_intersectsSegment, glm::intersects, GEOM_INTERSECTS_RH, gLuaAABB<2>, gLuaSegment<2>)
TRAITS_LAYOUT_DEFN(aabb2d_intersectsRay, glm::intersects, GEOM_INTERSECTS_RH, gLuaAABB<2>, gLuaRay<2>)
TRAITS_LAYOUT_DEFN(aabb2d_projectToAxis, glm::projectToAxis, GEOM_PROJECTION, gLuaAABB<2>, gLuaAABB<2>::point_trait)

static const luaL_Reg luaglm_aabb2dlib[] = {
  { "new", glm_aabb2d_new },
  { "fromCenterAndSize", glm_aabb2d_fromCenterAndSize },
  { "aabbFromSphere", glm_aabb2d_fromSphere },
  { "operator_negate", glm_aabb2d_operator_negate },
  { "operator_equals", glm_aabb2d_operator_equals },
  { "operator_add", glm_aabb2d_operator_add },
  { "operator_sub", glm_aabb2d_operator_sub },
  { "operator_mul", glm_aabb2d_operator_mul },
  { "equal", glm_aabb2d_equal },
  { "notEqual", glm_aabb2d_notEqual },
  //{ "tostring", glm_aabb2d_tostring },
  { "isinf", glm_aabb2d_isinf },
  { "isnan", glm_aabb2d_isnan },
  { "isfinite", glm_aabb2d_isfinite },
  { "isDegenerate", glm_aabb2d_isDegenerate },
  { "centerPoint", glm_aabb2d_centerPoint },
  { "centroid", glm_aabb2d_centerPoint },
  { "pointInside", glm_aabb2d_pointInside },
  { "edge", glm_aabb2d_edge },
  { "cornerPoint", glm_aabb2d_cornerPoint },
  { "extremePoint", glm_aabb2d_extremePoint },
  { "size", glm_aabb2d_size },
  { "halfSize", glm_aabb2d_halfSize },
  { "diagonal", glm_aabb2d_size },
  { "halfDiagonal", glm_aabb2d_halfSize },
  { "volume", glm_aabb2d_volume },
  { "scale", glm_aabb2d_scale },
  { "closestPoint", glm_aabb2d_closestPoint },
  { "distance", glm_aabb2d_distance },
  { "distanceSphere", glm_aabb2d_distanceSphere },
  { "contains", glm_aabb2d_contains },
  { "containsAABB", glm_aabb2d_containsAABB },
  { "containsSegment", glm_aabb2d_containsSegment },
  { "containsSphere", glm_aabb2d_containsSphere },
  { "grow", glm_aabb2d_grow },
  { "enclose", glm_aabb2d_enclose },
  { "encloseSegment", glm_aabb2d_encloseSegment },
  { "encloseSphere", glm_aabb2d_encloseSphere },
  { "encloseAABB", glm_aabb2d_encloseAABB },
  { "intersectsAABB", glm_aabb2d_intersectsAABB },
  { "intersectsLine", glm_aabb2d_intersectsLine },
  { "intersectsSegment", glm_aabb2d_intersectsSegment },
  { "intersectsRay", glm_aabb2d_intersectsRay },
  { "intersection", glm_aabb2d_intersection },
  { "projectToAxis", glm_aabb2d_projectToAxis },
  /* @DEPRECATED: intersectsObject */
  { "intersectAABB", glm_aabb2d_intersectsAABB },
  { "intersectLine", glm_aabb2d_intersectsLine },
  { "intersectSegment", glm_aabb2d_intersectsSegment },
  { "intersectRay", glm_aabb2d_intersectsRay },
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* }================================================================== */

/*
** {==================================================================
** Line
** ===================================================================
*/

TRAITS_DEFN(line_operator_negate, operator-, gLuaLine<>)
TRAITS_DEFN(line_operator_equals, operator==, gLuaLine<>, gLuaLine<>)
TRAITS_DEFN(line_operator_add, operator+, gLuaLine<>, gLuaLine<>::point_trait)
TRAITS_DEFN(line_operator_sub, operator-, gLuaLine<>, gLuaLine<>::point_trait)
ROTATION_MATRIX_DEFN(line_operator_mul, operator*, LAYOUT_UNARY, gLuaLine<>::as_type<gLuaQuat<>::value_type>)
TRAITS_LAYOUT_DEFN(line_equal, glm::equal, GEOM_EQUALS, gLuaLine<>)
TRAITS_LAYOUT_DEFN(line_notEqual, glm::notEqual, GEOM_EQUALS, gLuaLine<>)
TRAITS_DEFN(line_to_segment, glm::toLineSegment, gLuaLine<>, gLuaLine<>::value_trait)
TRAITS_DEFN(line_isinf, glm::isinf, gLuaLine<>)
TRAITS_DEFN(line_isnan, glm::isnan, gLuaLine<>)
TRAITS_DEFN(line_isfinite, glm::isfinite, gLuaLine<>)
TRAITS_DEFN(line_getpoint, glm::getPoint, gLuaLine<>, gLuaLine<>::value_trait)
TRAITS_LAYOUT_DEFN(line_closest, glm::closestPoint, GEOM_DISTANCE, gLuaLine<>, gLuaLine<>::point_trait)
TRAITS_LAYOUT_DEFN(line_closestRay, glm::closestPoint, GEOM_INTERSECTS, gLuaLine<>, gLuaRay<>)
TRAITS_LAYOUT_DEFN(line_closestLine, glm::closestPoint, GEOM_INTERSECTS, gLuaLine<>, gLuaLine<>)
TRAITS_LAYOUT_DEFN(line_closestSegment, glm::closestPoint, GEOM_INTERSECTS, gLuaLine<>, gLuaSegment<>)
TRAITS_LAYOUT_DEFN(line_closestTriangle, glm::closestPoint, GEOM_INTERSECTS_UV, gLuaLine<>, gLuaTriangle<>)
TRAITS_DEFN(line_contains, glm::contains, gLuaLine<>, gLuaLine<>::point_trait, gLuaLine<>::eps_trait)
TRAITS_DEFN(line_containsRay, glm::contains, gLuaLine<>, gLuaRay<>, gLuaLine<>::eps_trait)
TRAITS_DEFN(line_containsSegment, glm::contains, gLuaLine<>, gLuaSegment<>, gLuaLine<>::eps_trait)
TRAITS_LAYOUT_DEFN(line_distance, glm::distance, GEOM_DISTANCE, gLuaLine<>, gLuaLine<>::point_trait)
TRAITS_LAYOUT_DEFN(line_distanceRay, glm::distance, GEOM_INTERSECTS, gLuaLine<>, gLuaRay<>)
TRAITS_LAYOUT_DEFN(line_distanceLine, glm::distance, GEOM_INTERSECTS, gLuaLine<>, gLuaLine<>)
TRAITS_LAYOUT_DEFN(line_distanceSegment, glm::distance, GEOM_INTERSECTS, gLuaLine<>, gLuaSegment<>)
TRAITS_DEFN(line_distanceSphere, glm::distance, gLuaLine<>, gLuaSphere<>)
TRAITS_LAYOUT_DEFN(line_intersectsAABB, glm::intersects, GEOM_INTERSECTS, gLuaLine<>, gLuaAABB<>)
TRAITS_LAYOUT_DEFN(line_intersectsSphere, glm::intersects, GEOM_INTERSECTS, gLuaLine<>, gLuaSphere<>)
TRAITS_LAYOUT_DEFN(line_intersectsPlane, glm::intersects, GEOM_DISTANCE, gLuaLine<>, gLuaPlane<>)
TRAITS_LAYOUT_DEFN(line_intersectsTriangle, glm::intersects, GEOM_INTERSECTS_UV, gLuaLine<>, gLuaTriangle<>)
TRAITS_LAYOUT_DEFN(line_projectToAxis, glm::projectToAxis, GEOM_PROJECTION, gLuaLine<>, gLuaLine<>::point_trait)

static const luaL_Reg luaglm_linelib[] = {
  { "operator_negate", glm_line_operator_negate },
  { "operator_equals", glm_line_operator_equals },
  { "operator_add", glm_line_operator_add },
  { "operator_sub", glm_line_operator_sub },
  { "operator_mul", glm_line_operator_mul },
  { "equal", glm_line_equal },
  { "notEqual", glm_line_notEqual },
  //{ "tostring", glm_line_tostring },
  { "to_segment", glm_line_to_segment },
  { "isinf", glm_line_isinf },
  { "isnan", glm_line_isnan },
  { "isfinite", glm_line_isfinite },
  { "getPoint", glm_line_getpoint },
  { "closest", glm_line_closest },
  { "closestRay", glm_line_closestRay },
  { "closestLine", glm_line_closestLine },
  { "closestSegment", glm_line_closestSegment },
  { "closestTriangle", glm_line_closestTriangle },
  { "contains", glm_line_contains },
  { "containsRay", glm_line_containsRay },
  { "containsSegment", glm_line_containsSegment },
  { "distance", glm_line_distance },
  { "distanceRay", glm_line_distanceRay },
  { "distanceLine", glm_line_distanceLine },
  { "distanceSegment", glm_line_distanceSegment },
  { "distanceSphere", glm_line_distanceSphere },
  { "intersectsAABB", glm_line_intersectsAABB },
  { "intersectsSphere", glm_line_intersectsSphere },
  { "intersectsPlane", glm_line_intersectsPlane },
  { "intersectsTriangle", glm_line_intersectsTriangle },
  { "projectToAxis", glm_line_projectToAxis },
  /* @DEPRECATED: intersectsObject */
  { "intersectAABB", glm_line_intersectsAABB },
  { "intersectSphere", glm_line_intersectsSphere },
  { "intersectPlane", glm_line_intersectsPlane },
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* }================================================================== */

/*
** {==================================================================
** Ray
** ===================================================================
*/

TRAITS_DEFN(ray_operator_negate, operator-, gLuaRay<>)
TRAITS_DEFN(ray_operator_equals, operator==, gLuaRay<>, gLuaRay<>)
TRAITS_DEFN(ray_operator_add, operator+, gLuaRay<>, gLuaRay<>::point_trait)
TRAITS_DEFN(ray_operator_sub, operator-, gLuaRay<>, gLuaRay<>::point_trait)
ROTATION_MATRIX_DEFN(ray_operator_mul, operator*, LAYOUT_UNARY, gLuaRay<>::as_type<gLuaQuat<>::value_type>)
TRAITS_LAYOUT_DEFN(ray_equal, glm::equal, GEOM_EQUALS, gLuaRay<>)
TRAITS_LAYOUT_DEFN(ray_notEqual, glm::notEqual, GEOM_EQUALS, gLuaRay<>)
TRAITS_DEFN(ray_isinf, glm::isinf, gLuaRay<>)
TRAITS_DEFN(ray_isnan, glm::isnan, gLuaRay<>)
TRAITS_DEFN(ray_isfinite, glm::isfinite, gLuaRay<>)
TRAITS_DEFN(ray_getPoint, glm::getPoint, gLuaRay<>, gLuaRay<>::value_trait)
TRAITS_LAYOUT_DEFN(ray_closest, glm::closestPoint, GEOM_DISTANCE, gLuaRay<>, gLuaRay<>::point_trait)
TRAITS_LAYOUT_DEFN(ray_closestRay, glm::closestPoint, GEOM_INTERSECTS, gLuaRay<>, gLuaRay<>)
TRAITS_LAYOUT_DEFN(ray_closestLine, glm::closestPoint, GEOM_INTERSECTS, gLuaRay<>, gLuaLine<>)
TRAITS_LAYOUT_DEFN(ray_closestSegment, glm::closestPoint, GEOM_INTERSECTS, gLuaRay<>, gLuaSegment<>)
TRAITS_DEFN(ray_contains, glm::contains, gLuaRay<>, gLuaRay<>::point_trait, gLuaRay<>::eps_trait)
TRAITS_DEFN(ray_containsSegment, glm::contains, gLuaRay<>, gLuaSegment<>, gLuaRay<>::eps_trait)
TRAITS_LAYOUT_DEFN(ray_distance, glm::distance, GEOM_DISTANCE, gLuaRay<>, gLuaRay<>::point_trait)
TRAITS_LAYOUT_DEFN(ray_distanceRay, glm::distance, GEOM_INTERSECTS, gLuaRay<>, gLuaRay<>)
TRAITS_LAYOUT_DEFN(ray_distanceLine, glm::distance, GEOM_INTERSECTS, gLuaRay<>, gLuaLine<>)
TRAITS_LAYOUT_DEFN(ray_distanceSegment, glm::distance, GEOM_INTERSECTS, gLuaRay<>, gLuaSegment<>)
TRAITS_DEFN(ray_distanceSphere, glm::distance, gLuaRay<>, gLuaSphere<>)
TRAITS_LAYOUT_DEFN(ray_intersectsSphere, glm::intersects, GEOM_INTERSECTS, gLuaRay<>, gLuaSphere<>)
TRAITS_LAYOUT_DEFN(ray_intersectsAABB, glm::intersects, GEOM_INTERSECTS, gLuaRay<>, gLuaAABB<>)
TRAITS_LAYOUT_DEFN(ray_intersectsPlane, glm::intersects, GEOM_DISTANCE, gLuaRay<>, gLuaPlane<>)
TRAITS_LAYOUT_DEFN(ray_intersectsTriangle, glm::intersects, GEOM_INTERSECTS_UV, gLuaRay<>, gLuaTriangle<>)
TRAITS_LAYOUT_DEFN(ray_projectToAxis, glm::projectToAxis, GEOM_PROJECTION, gLuaRay<>, gLuaRay<>::point_trait)

static const luaL_Reg luaglm_raylib[] = {
  { "operator_negate", glm_ray_operator_negate },
  { "operator_equals", glm_ray_operator_equals },
  { "operator_add", glm_ray_operator_add },
  { "operator_sub", glm_ray_operator_sub },
  { "operator_mul", glm_ray_operator_mul },
  { "equal", glm_ray_equal },
  { "notEqual", glm_ray_notEqual },
  //{ "tostring", glm_ray_tostring },
  { "isinf", glm_ray_isinf },
  { "isnan", glm_ray_isnan },
  { "isfinite", glm_ray_isfinite },
  { "getPoint", glm_ray_getPoint },
  { "closest", glm_ray_closest },
  { "closestRay", glm_ray_closestRay },
  { "closestLine", glm_ray_closestLine },
  { "closestSegment", glm_ray_closestSegment },
  { "contains", glm_ray_contains },
  { "containsSegment", glm_ray_containsSegment },
  { "distance", glm_ray_distance },
  { "distanceRay", glm_ray_distanceRay },
  { "distanceLine", glm_ray_distanceLine },
  { "distanceSegment", glm_ray_distanceSegment },
  { "distanceSphere", glm_ray_distanceSphere },
  { "intersectsSphere", glm_ray_intersectsSphere },
  { "intersectsAABB", glm_ray_intersectsAABB },
  { "intersectsTriangle", glm_ray_intersectsTriangle },
  { "intersectPlane", glm_ray_intersectsPlane },
  { "projectToAxis", glm_ray_projectToAxis },
  /* @DEPRECATED: intersectsObject */
  { "intersectSphere", glm_ray_intersectsSphere },
  { "intersectAABB", glm_ray_intersectsAABB },
  { "intersectPlane", glm_ray_intersectsPlane },
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* }================================================================== */

/*
** {==================================================================
** LineSegment
** ===================================================================
*/

TRAITS_DEFN(segment_operator_negate, operator-, gLuaSegment<>)
TRAITS_DEFN(segment_operator_equals, operator==, gLuaSegment<>, gLuaSegment<>)
TRAITS_DEFN(segment_operator_add, operator+, gLuaSegment<>, gLuaSegment<>::point_trait)
TRAITS_DEFN(segment_operator_sub, operator-, gLuaSegment<>, gLuaSegment<>::point_trait)
ROTATION_MATRIX_DEFN(segment_operator_mul, operator*, LAYOUT_UNARY, gLuaSegment<>::as_type<gLuaQuat<>::value_type>)
TRAITS_LAYOUT_DEFN(segment_equal, glm::equal, GEOM_EQUALS, gLuaSegment<>)
TRAITS_LAYOUT_DEFN(segment_notEqual, glm::notEqual, GEOM_EQUALS, gLuaSegment<>)
TRAITS_DEFN(segment_length, glm::length, gLuaSegment<>)
TRAITS_DEFN(segment_length2, glm::length2, gLuaSegment<>)
TRAITS_DEFN(segment_isfinite, glm::isfinite, gLuaSegment<>)
TRAITS_DEFN(segment_getPoint, glm::getPoint, gLuaSegment<>, gLuaSegment<>::value_trait)
TRAITS_DEFN(segment_centerPoint, glm::centerPoint, gLuaSegment<>)
TRAITS_DEFN(segment_reverse, glm::reverse, gLuaSegment<>)
TRAITS_DEFN(segment_dir, glm::dir, gLuaSegment<>)
TRAITS_DEFN(segment_extremePoint, glm::extremePoint, gLuaSegment<>, gLuaSegment<>::point_trait)
TRAITS_LAYOUT_DEFN(segment_closestPoint, glm::closestPoint, GEOM_DISTANCE, gLuaSegment<>, gLuaSegment<>::point_trait)
TRAITS_LAYOUT_DEFN(segment_closestRay, glm::closestPoint, GEOM_INTERSECTS, gLuaSegment<>, gLuaRay<>)
TRAITS_LAYOUT_DEFN(segment_closestLine, glm::closestPoint, GEOM_INTERSECTS, gLuaSegment<>, gLuaLine<>)
TRAITS_LAYOUT_DEFN(segment_closestSegment, glm::closestPoint, GEOM_INTERSECTS, gLuaSegment<>, gLuaSegment<>)
TRAITS_LAYOUT_DEFN(segment_closestTriangle, glm::closestPoint, GEOM_INTERSECTS_UV, gLuaSegment<>, gLuaTriangle<>)
TRAITS_DEFN(segment_containsPoint, glm::contains, gLuaSegment<>, gLuaSegment<>::point_trait, gLuaSegment<>::eps_trait)
TRAITS_DEFN(segment_containsSegment, glm::contains, gLuaSegment<>, gLuaSegment<>, gLuaSegment<>::eps_trait)
TRAITS_LAYOUT_DEFN(segment_distance2, glm::distance2, GEOM_DISTANCE, gLuaSegment<>, gLuaSegment<>::point_trait)
TRAITS_LAYOUT_DEFN(segment_distanceSegment2, glm::distance2, GEOM_INTERSECTS, gLuaSegment<>, gLuaSegment<>)
TRAITS_LAYOUT_DEFN(segment_distance, glm::distance, GEOM_DISTANCE, gLuaSegment<>, gLuaSegment<>::point_trait)
TRAITS_LAYOUT_DEFN(segment_distanceRay, glm::distance, GEOM_INTERSECTS, gLuaSegment<>, gLuaRay<>)
TRAITS_LAYOUT_DEFN(segment_distanceLine, glm::distance, GEOM_INTERSECTS, gLuaSegment<>, gLuaLine<>)
TRAITS_LAYOUT_DEFN(segment_distanceSegment, glm::distance, GEOM_INTERSECTS, gLuaSegment<>, gLuaSegment<>)
TRAITS_DEFN(segment_distancePlane, glm::distance, gLuaSegment<>, gLuaPlane<>)
TRAITS_LAYOUT_DEFN(segment_intersectsSphere, glm::intersects, GEOM_INTERSECTS, gLuaSegment<>, gLuaSphere<>)
TRAITS_LAYOUT_DEFN(segment_intersectsAABB, glm::intersects, GEOM_INTERSECTS, gLuaSegment<>, gLuaAABB<>)
TRAITS_DEFN(segment_intersectsPlane, glm::intersects, gLuaSegment<>, gLuaPlane<>)
TRAITS_LAYOUT_DEFN(segment_intersectsSegment, glm::intersects, GEOM_INTERSECTS, gLuaSegment<>, gLuaSegment<>)
TRAITS_LAYOUT_DEFN(segment_intersectsTriangle, glm::intersects, GEOM_INTERSECTS_UV, gLuaSegment<>, gLuaTriangle<>)

static const luaL_Reg luaglm_segmentlib[] = {
  { "operator_negate", glm_segment_operator_negate },
  { "operator_equals", glm_segment_operator_equals },
  { "operator_add", glm_segment_operator_add },
  { "operator_sub", glm_segment_operator_sub },
  { "operator_mul", glm_segment_operator_mul },
  { "equal", glm_segment_equal },
  { "notEqual", glm_segment_notEqual },
  //{ "tostring", glm_segment_tostring },
  { "length", glm_segment_length },
  { "length2", glm_segment_length2 },
  { "isfinite", glm_segment_isfinite },
  { "getPoint", glm_segment_getPoint },
  { "centerPoint", glm_segment_centerPoint },
  { "centroid", glm_segment_centerPoint },
  { "reverse", glm_segment_reverse },
  { "dir", glm_segment_dir },
  { "extremePoint", glm_segment_extremePoint },
  { "closestPoint", glm_segment_closestPoint },
  { "closestRay", glm_segment_closestRay },
  { "closestLine", glm_segment_closestLine },
  { "closestSegment", glm_segment_closestSegment },
  { "containsPoint", glm_segment_containsPoint },
  { "containsSegment", glm_segment_containsSegment },
  { "closestTriangle", glm_segment_closestTriangle },
  { "distance2", glm_segment_distance2 },
  { "distanceSegment2", glm_segment_distanceSegment2 },
  { "distance", glm_segment_distance },
  { "distanceRay", glm_segment_distanceRay },
  { "distanceLine", glm_segment_distanceLine },
  { "distanceSegment", glm_segment_distanceSegment },
  { "distancePlane", glm_segment_distancePlane },
  { "intersectsSphere", glm_segment_intersectsSphere },
  { "intersectsAABB", glm_segment_intersectsAABB },
  { "intersectsPlane", glm_segment_intersectsPlane },
  { "intersectsSegment", glm_segment_intersectsSegment },
  { "intersectsTriangle", glm_segment_intersectsTriangle },
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* }================================================================== */

/*
** {==================================================================
** LineSegment2D
** ===================================================================
*/

TRAITS_DEFN(segment2d_operator_negate, operator-, gLuaSegment<2>)
TRAITS_DEFN(segment2d_operator_equals, operator==, gLuaSegment<2>, gLuaSegment<2>)
TRAITS_DEFN(segment2d_operator_add, operator+, gLuaSegment<2>, gLuaSegment<2>::point_trait)
TRAITS_DEFN(segment2d_operator_sub, operator-, gLuaSegment<2>, gLuaSegment<2>::point_trait)
TRAITS_LAYOUT_DEFN(segment2d_equal, glm::equal, GEOM_EQUALS, gLuaSegment<2>)
TRAITS_LAYOUT_DEFN(segment2d_notEqual, glm::notEqual, GEOM_EQUALS, gLuaSegment<2>)
TRAITS_DEFN(segment2d_length, glm::length, gLuaSegment<2>)
TRAITS_DEFN(segment2d_length2, glm::length2, gLuaSegment<2>)
TRAITS_DEFN(segment2d_isfinite, glm::isfinite, gLuaSegment<2>)
TRAITS_DEFN(segment2d_getPoint, glm::getPoint, gLuaSegment<2>, gLuaSegment<2>::value_trait)
TRAITS_DEFN(segment2d_centerPoint, glm::centerPoint, gLuaSegment<2>)
TRAITS_DEFN(segment2d_reverse, glm::reverse, gLuaSegment<2>)
TRAITS_DEFN(segment2d_dir, glm::dir, gLuaSegment<2>)
TRAITS_DEFN(segment2d_extremePoint, glm::extremePoint, gLuaSegment<2>, gLuaSegment<2>::point_trait)
TRAITS_LAYOUT_DEFN(segment2d_closestPoint, glm::closestPoint, GEOM_DISTANCE, gLuaSegment<2>, gLuaSegment<2>::point_trait)
TRAITS_LAYOUT_DEFN(segment2d_closestRay, glm::closestPoint, GEOM_INTERSECTS, gLuaSegment<2>, gLuaRay<2>)
TRAITS_LAYOUT_DEFN(segment2d_closestLine, glm::closestPoint, GEOM_INTERSECTS, gLuaSegment<2>, gLuaLine<2>)
TRAITS_LAYOUT_DEFN(segment2d_closestSegment, glm::closestPoint, GEOM_INTERSECTS, gLuaSegment<2>, gLuaSegment<2>)
TRAITS_DEFN(segment2d_containsPoint, glm::contains, gLuaSegment<2>, gLuaSegment<2>::point_trait, gLuaSegment<2>::eps_trait)
TRAITS_DEFN(segment2d_containsSegment, glm::contains, gLuaSegment<2>, gLuaSegment<2>, gLuaSegment<2>::eps_trait)
TRAITS_LAYOUT_DEFN(segment2d_distance2, glm::distance2, GEOM_DISTANCE, gLuaSegment<2>, gLuaSegment<2>::point_trait)
TRAITS_LAYOUT_DEFN(segment2d_distanceSegment2, glm::distance2, GEOM_INTERSECTS, gLuaSegment<2>, gLuaSegment<2>)
TRAITS_LAYOUT_DEFN(segment2d_distance, glm::distance, GEOM_DISTANCE, gLuaSegment<2>, gLuaSegment<2>::point_trait)
TRAITS_LAYOUT_DEFN(segment2d_distanceRay, glm::distance, GEOM_INTERSECTS, gLuaSegment<2>, gLuaRay<2>)
TRAITS_LAYOUT_DEFN(segment2d_distanceLine, glm::distance, GEOM_INTERSECTS, gLuaSegment<2>, gLuaLine<2>)
TRAITS_LAYOUT_DEFN(segment2d_distanceSegment, glm::distance, GEOM_INTERSECTS, gLuaSegment<2>, gLuaSegment<2>)
TRAITS_DEFN(segment2d_distancePlane, glm::distance, gLuaSegment<2>, gLuaPlane<2>)
TRAITS_LAYOUT_DEFN(segment2d_intersectsAABB, glm::intersects, GEOM_INTERSECTS, gLuaSegment<2>, gLuaAABB<2>)
TRAITS_LAYOUT_DEFN(segment2d_intersectsSegment, glm::intersects, GEOM_INTERSECTS, gLuaSegment<2>, gLuaSegment<2>)

static const luaL_Reg luaglm_segment2dlib[] = {
  { "operator_negate", glm_segment2d_operator_negate },
  { "operator_equals", glm_segment2d_operator_equals },
  { "operator_add", glm_segment2d_operator_add },
  { "operator_sub", glm_segment2d_operator_sub },
  { "equal", glm_segment2d_equal },
  { "notEqual", glm_segment2d_notEqual },
  //{ "tostring", glm_segment2d_tostring },
  { "length", glm_segment2d_length },
  { "length2", glm_segment2d_length2 },
  { "isfinite", glm_segment2d_isfinite },
  { "getPoint", glm_segment2d_getPoint },
  { "centerPoint", glm_segment2d_centerPoint },
  { "centroid", glm_segment2d_centerPoint },
  { "reverse", glm_segment2d_reverse },
  { "dir", glm_segment2d_dir },
  { "extremePoint", glm_segment2d_extremePoint },
  { "closestPoint", glm_segment2d_closestPoint },
  { "closestRay", glm_segment2d_closestRay },
  { "closestLine", glm_segment2d_closestLine },
  { "closestSegment", glm_segment2d_closestSegment },
  { "containsPoint", glm_segment2d_containsPoint },
  { "containsSegment", glm_segment2d_containsSegment },
  { "distance2", glm_segment2d_distance2 },
  { "distanceSegment2", glm_segment2d_distanceSegment2 },
  { "distance", glm_segment2d_distance },
  { "distanceRay", glm_segment2d_distanceRay },
  { "distanceLine", glm_segment2d_distanceLine },
  { "distanceSegment", glm_segment2d_distanceSegment },
  { "distancePlane", glm_segment2d_distancePlane },
  { "intersectsAABB", glm_segment2d_intersectsAABB },
  { "intersectsSegment", glm_segment2d_intersectsSegment },
  { GLM_NULLPTR, GLM_NULLPTR }
};

/*
** {==================================================================
** Triangle
** ===================================================================
*/

TRAITS_DEFN(triangle_operator_negate, operator-, gLuaTriangle<>)
TRAITS_DEFN(triangle_operator_equals, operator==, gLuaTriangle<>, gLuaTriangle<>)
TRAITS_DEFN(triangle_operator_add, operator+, gLuaTriangle<>, gLuaTriangle<>::point_trait)
TRAITS_DEFN(triangle_operator_sub, operator-, gLuaTriangle<>, gLuaTriangle<>::point_trait)
ROTATION_MATRIX_DEFN(triangle_operator_mul, operator*, LAYOUT_UNARY, gLuaTriangle<>::as_type<gLuaQuat<>::value_type>)
TRAITS_LAYOUT_DEFN(triangle_equal, glm::equal, GEOM_EQUALS, gLuaTriangle<>)
TRAITS_LAYOUT_DEFN(triangle_notEqual, glm::notEqual, GEOM_EQUALS, gLuaTriangle<>)
TRAITS_DEFN(triangle_isinf, glm::isinf, gLuaTriangle<>)
TRAITS_DEFN(triangle_isnan, glm::isnan, gLuaTriangle<>)
TRAITS_DEFN(triangle_isfinite, glm::isfinite, gLuaTriangle<>)
TRAITS_DEFN(triangle_isDegenerate, glm::isDegenerate, gLuaTriangle<>)
TRAITS_DEFN(triangle_centroid, glm::centroid, gLuaTriangle<>)
TRAITS_DEFN(triangle_area, glm::area, gLuaTriangle<>)
TRAITS_DEFN(triangle_signedArea, glm::signedArea, gLuaTriangle<>, gLuaTriangle<>::point_trait)
TRAITS_DEFN(triangle_perimeter, glm::perimeter, gLuaTriangle<>)
TRAITS_DEFN(triangle_edge, glm::edge, gLuaTriangle<>, gLuaTrait<int>)
TRAITS_DEFN(triangle_cornerPoint, glm::cornerPoint, gLuaTriangle<>, gLuaTrait<int>)
TRAITS_DEFN(triangle_barycentric_uvw, glm::barycentricUVW, gLuaTriangle<>, gLuaTriangle<>::point_trait)
TRAITS_DEFN(triangle_barycentric_uv, glm::barycentricUV, gLuaTriangle<>, gLuaTriangle<>::point_trait)
TRAITS_DEFN(triangle_barycentric_inside, glm::barycentricInsideTriangle, gLuaTriangle<>::value_trait, gLuaTriangle<>::value_trait, gLuaTriangle<>::value_trait)
TRAITS_DEFN(triangle_barycentric_pointuv, glm::barycentricPoint, gLuaTriangle<>, gLuaTriangle<>::value_trait, gLuaTriangle<>::value_trait)
TRAITS_DEFN(triangle_barycentric_pointuvw, glm::barycentricPoint, gLuaTriangle<>, gLuaTriangle<>::value_trait, gLuaTriangle<>::value_trait, gLuaTriangle<>::value_trait)
TRAITS_DEFN(triangle_planeCCW, glm::planeCCW, gLuaTriangle<>)
TRAITS_DEFN(triangle_unnormalizedNormalCCW, glm::unnormalizedNormalCCW, gLuaTriangle<>)
TRAITS_DEFN(triangle_normalCCW, glm::normalCCW, gLuaTriangle<>)
TRAITS_DEFN(triangle_planeCW, glm::planeCW, gLuaTriangle<>)
TRAITS_DEFN(triangle_unnormalizedNormalCW, glm::unnormalizedNormalCW, gLuaTriangle<>)
TRAITS_DEFN(triangle_normalCW, glm::normalCW, gLuaTriangle<>)
TRAITS_DEFN(triangle_extremePoint, glm::extremePoint, gLuaTriangle<>, gLuaTriangle<>::point_trait)
TRAITS_DEFN(triangle_boundingAABB, glm::boundingAABB, gLuaTriangle<>)
TRAITS_DEFN(triangle_contains, glm::contains, gLuaTriangle<>, gLuaTriangle<>::point_trait, gLuaTriangle<>::eps_trait)
TRAITS_DEFN(triangle_containsSegment, glm::contains, gLuaTriangle<>, gLuaSegment<>, gLuaTriangle<>::eps_trait)
TRAITS_DEFN(triangle_containsTriangle, glm::contains, gLuaTriangle<>, gLuaTriangle<>, gLuaTriangle<>::eps_trait)
TRAITS_DEFN(triangle_closestPoint, glm::closestPoint, gLuaTriangle<>, gLuaTriangle<>::point_trait)
TRAITS_LAYOUT_DEFN(triangle_closestSegment, glm::closestPoint, GEOM_INTERSECTS_PT, gLuaTriangle<>, gLuaSegment<>)
TRAITS_LAYOUT_DEFN(triangle_closestLine, glm::closestPoint, GEOM_INTERSECTS_PT, gLuaTriangle<>, gLuaLine<>)
TRAITS_DEFN(triangle_distance, glm::distance, gLuaTriangle<>, gLuaTriangle<>::point_trait)
TRAITS_DEFN(triangle_distanceSphere, glm::distance, gLuaTriangle<>, gLuaSphere<>)
//TRAITS_DEFN(triangle_intersectsAABB, glm::intersects, gLuaTriangle<>, gLuaAABB<>)
TRAITS_LAYOUT_DEFN(triangle_intersectsRay, glm::intersects, GEOM_INTERSECTS_UV, gLuaTriangle<>, gLuaRay<>)
TRAITS_LAYOUT_DEFN(triangle_intersectsLine, glm::intersects, GEOM_INTERSECTS_UV, gLuaTriangle<>, gLuaLine<>)
TRAITS_LAYOUT_DEFN(triangle_intersectsSegment, glm::intersects, GEOM_INTERSECTS_UV, gLuaTriangle<>, gLuaSegment<>)
TRAITS_DEFN(triangle_intersectsPlane, glm::intersects, gLuaTriangle<>, gLuaPlane<>)
TRAITS_LAYOUT_DEFN(triangle_intersectsSphere, glm::intersects, GEOM_INTERSECTS_PT, gLuaTriangle<>, gLuaSphere<>)

static const luaL_Reg luaglm_trianglelib[] = {
  { "operator_negate", glm_triangle_operator_negate },
  { "operator_equals", glm_triangle_operator_equals },
  { "operator_add", glm_triangle_operator_add },
  { "operator_sub", glm_triangle_operator_sub },
  { "operator_mul", glm_triangle_operator_mul },
  { "equal", glm_triangle_equal },
  { "notEqual", glm_triangle_notEqual },
  //{ "tostring", glm_triangle_tostring },
  { "isinf", glm_triangle_isinf },
  { "isnan", glm_triangle_isnan },
  { "isfinite", glm_triangle_isfinite },
  { "isDegenerate", glm_triangle_isDegenerate },
  { "centroid", glm_triangle_centroid },
  { "area", glm_triangle_area },
  { "signedArea", glm_triangle_signedArea },
  { "perimeter", glm_triangle_perimeter },
  { "edge", glm_triangle_edge },
  { "cornerPoint", glm_triangle_cornerPoint },
  { "extremePoint", glm_triangle_extremePoint },
  { "boundingAABB", glm_triangle_boundingAABB },
  { "uvw", glm_triangle_barycentric_uvw },
  { "uv", glm_triangle_barycentric_uv },
  { "pointuv", glm_triangle_barycentric_pointuv },
  { "pointuvw", glm_triangle_barycentric_pointuvw },
  { "inside_triangle", glm_triangle_barycentric_inside },
  { "planeCCW", glm_triangle_planeCCW },
  { "planeCW", glm_triangle_planeCW },
  { "unnormalizedNormalCCW", glm_triangle_unnormalizedNormalCCW },
  { "unnormalizedNormalCW", glm_triangle_unnormalizedNormalCW },
  { "normalCCW", glm_triangle_normalCCW },
  { "normalCW", glm_triangle_normalCW },
  { "closestPoint", glm_triangle_closestPoint },
  { "closestSegment", glm_triangle_closestSegment },
  { "closestLine", glm_triangle_closestLine },
  { "contains", glm_triangle_contains },
  { "containsSegment", glm_triangle_containsSegment },
  { "containsTriangle", glm_triangle_containsTriangle },
  { "distance", glm_triangle_distance },
  { "distanceSphere", glm_triangle_distanceSphere },
  //{ "intersectsAABB", glm_triangle_intersectsAABB },
  { "intersectsRay", glm_triangle_intersectsRay },
  { "intersectsLine", glm_triangle_intersectsLine },
  { "intersectsSegment", glm_triangle_intersectsSegment },
  { "intersectsSphere", glm_triangle_intersectsSphere },
  { "intersectsPlane", glm_triangle_intersectsPlane },
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

// @TODO(Bloat):
//GLM_BINDING_QUALIFIER(sphere_optimalEnclosingSphere) {
//  GLM_BINDING_BEGIN
//  using point_trait = gLuaSphere<>::point_trait;
//  switch (LB.top()) {
//    case 2: BIND_FUNC(LB, glm::optimalEnclosingSphere, point_trait, point_trait); break;
//    case 3: BIND_FUNC(LB, glm::optimalEnclosingSphere, point_trait, point_trait, point_trait); break;
//    case 4: BIND_FUNC(LB, glm::optimalEnclosingSphere, point_trait, point_trait, point_trait, point_trait); break;
//    default: {
//      luaL_checktype(L, LB.idx, LUA_TTABLE);
//      glmLuaArray<point_trait> lArray(LB.L, LB.idx);
//      return gLuaBase::Push(LB, glm::optimalEnclosingSphere<point_trait::value_type, glm::defaultp, glmLuaArray<point_trait>>(lArray));
//    }
//  }
//  GLM_BINDING_END
//}

TRAITS_DEFN(sphere_operator_negate, operator-, gLuaSphere<>)
TRAITS_DEFN(sphere_operator_equals, operator==, gLuaSphere<>, gLuaSphere<>)
TRAITS_DEFN(sphere_operator_add, operator+, gLuaSphere<>, gLuaSphere<>::point_trait)
TRAITS_DEFN(sphere_operator_sub, operator-, gLuaSphere<>, gLuaSphere<>::point_trait)
ROTATION_MATRIX_DEFN(sphere_operator_mul, operator*, LAYOUT_UNARY, gLuaSphere<>::as_type<gLuaQuat<>::value_type>)
TRAITS_LAYOUT_DEFN(sphere_equal, glm::equal, GEOM_EQUALS, gLuaSphere<>)
TRAITS_LAYOUT_DEFN(sphere_notEqual, glm::notEqual, GEOM_EQUALS, gLuaSphere<>)
TRAITS_DEFN(sphere_volume, glm::volume, gLuaSphere<>)
TRAITS_DEFN(sphere_surfaceArea, glm::surfaceArea, gLuaSphere<>)
TRAITS_DEFN(sphere_isinf, glm::isinf, gLuaSphere<>)
TRAITS_DEFN(sphere_isnan, glm::isnan, gLuaSphere<>)
TRAITS_DEFN(sphere_isfinite, glm::isfinite, gLuaSphere<>)
TRAITS_DEFN(sphere_isDegenerate, glm::isDegenerate, gLuaSphere<>)
TRAITS_DEFN(sphere_extremePoint, glm::extremePoint, gLuaSphere<>, gLuaSphere<>::point_trait)
TRAITS_DEFN(sphere_contains, glm::contains, gLuaSphere<>, gLuaSphere<>::point_trait, gLuaSphere<>::eps_trait)
TRAITS_DEFN(sphere_containsSegment, glm::contains, gLuaSphere<>, gLuaSegment<>)
TRAITS_DEFN(sphere_containsSphere, glm::contains, gLuaSphere<>, gLuaSphere<>, gLuaSphere<>::eps_trait)
TRAITS_DEFN(sphere_containsTriangle, glm::contains, gLuaSphere<>, gLuaTriangle<>, gLuaSphere<>::eps_trait)
TRAITS_DEFN(sphere_containsAABB, glm::contains, gLuaSphere<>, gLuaAABB<>)
TRAITS_DEFN(sphere_distance, glm::distance, gLuaSphere<>, gLuaSphere<>::point_trait)
TRAITS_DEFN(sphere_distanceSphere, glm::distance, gLuaSphere<>, gLuaSphere<>)
TRAITS_DEFN(sphere_distanceAABB, glm::distance, gLuaSphere<>, gLuaAABB<>)
TRAITS_DEFN(sphere_distanceRay, glm::distance, gLuaSphere<>, gLuaRay<>)
TRAITS_DEFN(sphere_distanceSegment, glm::distance, gLuaSphere<>, gLuaSegment<>)
TRAITS_DEFN(sphere_distanceLine, glm::distance, gLuaSphere<>, gLuaLine<>)
TRAITS_DEFN(sphere_distanceTriangle, glm::distance, gLuaSphere<>, gLuaTriangle<>)
TRAITS_DEFN(sphere_closestPoint, glm::closestPoint, gLuaSphere<>, gLuaSphere<>::point_trait)
TRAITS_DEFN(sphere_intersectsSphere, glm::intersects, gLuaSphere<>, gLuaSphere<>)
TRAITS_DEFN(sphere_intersectsAABB, glm::intersects, gLuaSphere<>, gLuaAABB<>)
TRAITS_DEFN(sphere_intersectsPlane, glm::intersects, gLuaSphere<>, gLuaPlane<>)
TRAITS_DEFN(sphere_intersectsTriangle, glm::intersects, gLuaSphere<>, gLuaTriangle<>)
TRAITS_LAYOUT_DEFN(sphere_intersectsLine, glm::intersects, GEOM_INTERSECTS_RH, gLuaSphere<>, gLuaLine<>)
TRAITS_LAYOUT_DEFN(sphere_intersectsSegment, glm::intersects, GEOM_INTERSECTS_RH, gLuaSphere<>, gLuaSegment<>)
TRAITS_LAYOUT_DEFN(sphere_intersectsRay, glm::intersects, GEOM_INTERSECTS_RH, gLuaSphere<>, gLuaRay<>)
TRAITS_DEFN(sphere_enclose, glm::enclose, gLuaSphere<>, gLuaSphere<>::point_trait)
TRAITS_DEFN(sphere_encloseSegment, glm::enclose, gLuaSphere<>, gLuaSegment<>)
TRAITS_DEFN(sphere_encloseSphere, glm::enclose, gLuaSphere<>, gLuaSphere<>)
TRAITS_DEFN(sphere_encloseAABB, glm::enclose, gLuaSphere<>, gLuaAABB<>)
TRAITS_DEFN(sphere_encloseTriangle, glm::enclose, gLuaSphere<>, gLuaTriangle<>)
TRAITS_DEFN(sphere_extendRadiusToContain, glm::extendRadiusToContain, gLuaSphere<>, gLuaSphere<>::point_trait, gLuaSphere<>::eps_trait)
TRAITS_DEFN(sphere_extendRadiusToContainSphere, glm::extendRadiusToContain, gLuaSphere<>, gLuaSphere<>, gLuaSphere<>::eps_trait)
TRAITS_DEFN(sphere_maximalContainedAABB, glm::maximalContainedAABB, gLuaSphere<>)
TRAITS_LAYOUT_DEFN(sphere_projectToAxis, glm::projectToAxis, GEOM_PROJECTION, gLuaSphere<>, gLuaSphere<>::point_trait)

static const luaL_Reg luaglm_spherelib[] = {
  { "operator_negate", glm_sphere_operator_negate },
  { "operator_equals", glm_sphere_operator_equals },
  { "operator_add", glm_sphere_operator_add },
  { "operator_sub", glm_sphere_operator_sub },
  { "operator_mul", glm_sphere_operator_mul },
  { "equal", glm_sphere_equal },
  { "notEqual", glm_sphere_notEqual },
  //{ "tostring", glm_sphere_tostring },
  { "volume", glm_sphere_volume },
  { "surfaceArea", glm_sphere_surfaceArea },
  { "isinf", glm_sphere_isinf },
  { "isnan", glm_sphere_isnan },
  { "isfinite", glm_sphere_isfinite },
  { "isDegenerate", glm_sphere_isDegenerate },
  { "extremePoint", glm_sphere_extremePoint },
  { "contains", glm_sphere_contains },
  { "containsSegment", glm_sphere_containsSegment },
  { "containsSphere", glm_sphere_containsSphere },
  { "containsTriangle", glm_sphere_containsTriangle },
  { "containsAABB", glm_sphere_containsAABB },
  { "distance", glm_sphere_distance },
  { "distanceSphere", glm_sphere_distanceSphere },
  { "distanceAABB", glm_sphere_distanceAABB },
  { "distanceRay", glm_sphere_distanceRay },
  { "distanceSegment", glm_sphere_distanceSegment },
  { "distanceLine", glm_sphere_distanceLine },
  { "distanceTriangle", glm_sphere_distanceTriangle },
  { "closestPoint", glm_sphere_closestPoint },
  { "intersectsSphere", glm_sphere_intersectsSphere },
  { "intersectsAABB", glm_sphere_intersectsAABB },
  { "intersectsLine", glm_sphere_intersectsLine },
  { "intersectsSegment", glm_sphere_intersectsSegment },
  { "intersectsRay", glm_sphere_intersectsRay },
  { "intersectsPlane", glm_sphere_intersectsPlane },
  { "intersectsTriangle", glm_sphere_intersectsTriangle },
  { "enclose", glm_sphere_enclose },
  { "encloseSegment", glm_sphere_encloseSegment },
  { "encloseSphere", glm_sphere_encloseSphere },
  { "encloseAABB", glm_sphere_encloseAABB },
  { "encloseTriangle", glm_sphere_encloseTriangle },
  { "extendRadiusToContain", glm_sphere_extendRadiusToContain },
  { "extendRadiusToContainSphere", glm_sphere_extendRadiusToContainSphere },
  { "maximalContainedAABB", glm_sphere_maximalContainedAABB },
  { "fitThroughPoints", glm_sphere_fitThroughPoints },
  //{ "optimalEnclosingSphere", glm_sphere_optimalEnclosingSphere },  // @TODO(Bloat)
  { "projectToAxis", glm_sphere_projectToAxis },
  /* @DEPRECATED: intersectsObject */
  { "intersectSphere", glm_sphere_intersectsSphere },
  { "intersectAABB", glm_sphere_intersectsAABB },
  { "intersectLine", glm_sphere_intersectsLine },
  { "intersectSegment", glm_sphere_intersectsSegment },
  { "intersectRay", glm_sphere_intersectsRay },
  { "intersectPlane", glm_sphere_intersectsPlane },
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* }================================================================== */

/*
** {==================================================================
** Circle
** ===================================================================
*/

TRAITS_DEFN(circle_operator_negate, operator-, gLuaSphere<2>)
TRAITS_DEFN(circle_operator_equals, operator==, gLuaSphere<2>, gLuaSphere<2>)
TRAITS_DEFN(circle_operator_add, operator+, gLuaSphere<2>, gLuaSphere<2>::point_trait)
TRAITS_DEFN(circle_operator_sub, operator-, gLuaSphere<2>, gLuaSphere<2>::point_trait)
TRAITS_LAYOUT_DEFN(circle_equal, glm::equal, GEOM_EQUALS, gLuaSphere<2>)
TRAITS_LAYOUT_DEFN(circle_notEqual, glm::notEqual, GEOM_EQUALS, gLuaSphere<2>)
TRAITS_DEFN(circle_area, glm::area, gLuaSphere<2>)
TRAITS_DEFN(circle_isinf, glm::isinf, gLuaSphere<2>)
TRAITS_DEFN(circle_isnan, glm::isnan, gLuaSphere<2>)
TRAITS_DEFN(circle_isfinite, glm::isfinite, gLuaSphere<2>)
TRAITS_DEFN(circle_isDegenerate, glm::isDegenerate, gLuaSphere<2>)
TRAITS_DEFN(circle_extremePoint, glm::extremePoint, gLuaSphere<2>, gLuaSphere<2>::point_trait)
TRAITS_DEFN(circle_contains, glm::contains, gLuaSphere<2>, gLuaSphere<2>::point_trait, gLuaSphere<2>::eps_trait)
TRAITS_DEFN(circle_containsSegment, glm::contains, gLuaSphere<2>, gLuaSegment<2>)
TRAITS_DEFN(circle_containsCircle, glm::contains, gLuaSphere<2>, gLuaSphere<2>, gLuaSphere<2>::eps_trait)
TRAITS_DEFN(circle_containsAABB, glm::contains, gLuaSphere<2>, gLuaAABB<2>)
TRAITS_DEFN(circle_distance, glm::distance, gLuaSphere<2>, gLuaSphere<2>::point_trait)
TRAITS_DEFN(circle_distanceSphere, glm::distance, gLuaSphere<2>, gLuaSphere<2>)
TRAITS_DEFN(circle_distanceAABB, glm::distance, gLuaSphere<2>, gLuaAABB<2>)
TRAITS_DEFN(circle_distanceRay, glm::distance, gLuaSphere<2>, gLuaRay<2>)
TRAITS_DEFN(circle_distanceSegment, glm::distance, gLuaSphere<2>, gLuaSegment<2>)
TRAITS_DEFN(circle_distanceLine, glm::distance, gLuaSphere<2>, gLuaLine<2>)
TRAITS_DEFN(circle_closestPoint, glm::closestPoint, gLuaSphere<2>, gLuaSphere<2>::point_trait)
TRAITS_DEFN(circle_intersectsCircle, glm::intersects, gLuaSphere<2>, gLuaSphere<2>)
TRAITS_DEFN(circle_intersectsAABB, glm::intersects, gLuaSphere<2>, gLuaAABB<2>)
TRAITS_DEFN(circle_intersectsPlane, glm::intersects, gLuaSphere<2>, gLuaPlane<2>)
TRAITS_LAYOUT_DEFN(circle_intersectsLine, glm::intersects, GEOM_INTERSECTS_RH, gLuaSphere<2>, gLuaLine<2>)
TRAITS_LAYOUT_DEFN(circle_intersectsSegment, glm::intersects, GEOM_INTERSECTS_RH, gLuaSphere<2>, gLuaSegment<2>)
TRAITS_LAYOUT_DEFN(circle_intersectsRay, glm::intersects, GEOM_INTERSECTS_RH, gLuaSphere<2>, gLuaRay<2>)
TRAITS_DEFN(circle_enclose, glm::enclose, gLuaSphere<2>, gLuaSphere<2>::point_trait)
TRAITS_DEFN(circle_encloseSegment, glm::enclose, gLuaSphere<2>, gLuaSegment<2>)
TRAITS_DEFN(circle_encloseSphere, glm::enclose, gLuaSphere<2>, gLuaSphere<2>)
TRAITS_DEFN(circle_encloseAABB, glm::enclose, gLuaSphere<2>, gLuaAABB<2>)
TRAITS_DEFN(circle_extendRadiusToContain, glm::extendRadiusToContain, gLuaSphere<2>, gLuaSphere<2>::point_trait, gLuaSphere<2>::eps_trait)
TRAITS_DEFN(circle_extendRadiusToContainCircle, glm::extendRadiusToContain, gLuaSphere<2>, gLuaSphere<2>, gLuaSphere<2>::eps_trait)
TRAITS_DEFN(circle_maximalContainedAABB, glm::maximalContainedAABB, gLuaSphere<2>)
TRAITS_LAYOUT_DEFN(circle_projectToAxis, glm::projectToAxis, GEOM_PROJECTION, gLuaSphere<2>, gLuaSphere<2>::point_trait)

static const luaL_Reg luaglm_circlelib[] = {
  { "operator_negate", glm_circle_operator_negate },
  { "operator_equals", glm_circle_operator_equals },
  { "operator_add", glm_circle_operator_add },
  { "operator_sub", glm_circle_operator_sub },
  { "equal", glm_circle_equal },
  { "notEqual", glm_circle_notEqual },
  //{ "tostring", glm_circle_tostring },
  { "area", glm_circle_area },
  { "isinf", glm_circle_isinf },
  { "isnan", glm_circle_isnan },
  { "isfinite", glm_circle_isfinite },
  { "isDegenerate", glm_circle_isDegenerate },
  { "extremePoint", glm_circle_extremePoint },
  { "contains", glm_circle_contains },
  { "containsSegment", glm_circle_containsSegment },
  { "containsCircle", glm_circle_containsCircle },
  { "containsAABB", glm_circle_containsAABB },
  { "distance", glm_circle_distance },
  { "distanceSphere", glm_circle_distanceSphere },
  { "distanceAABB", glm_circle_distanceAABB },
  { "distanceRay", glm_circle_distanceRay },
  { "distanceSegment", glm_circle_distanceSegment },
  { "distanceLine", glm_circle_distanceLine },
  { "closestPoint", glm_circle_closestPoint },
  { "intersectsCircle", glm_circle_intersectsCircle },
  { "intersectsAABB", glm_circle_intersectsAABB },
  { "intersectsLine", glm_circle_intersectsLine },
  { "intersectsSegment", glm_circle_intersectsSegment },
  { "intersectsRay", glm_circle_intersectsRay },
  { "intersectsPlane", glm_circle_intersectsPlane },
  { "enclose", glm_circle_enclose },
  { "encloseSegment", glm_circle_encloseSegment },
  { "encloseSphere", glm_circle_encloseSphere },
  { "encloseAABB", glm_circle_encloseAABB },
  { "extendRadiusToContain", glm_circle_extendRadiusToContain },
  { "extendRadiusToContainCircle", glm_circle_extendRadiusToContainCircle },
  { "maximalContainedAABB", glm_circle_maximalContainedAABB },
  { "projectToAxis", glm_circle_projectToAxis },
  /* @DEPRECATED: intersectsObject */
  { "intersectCircle", glm_circle_intersectsCircle },
  { "intersectAABB", glm_circle_intersectsAABB },
  { "intersectLine", glm_circle_intersectsLine },
  { "intersectSegment", glm_circle_intersectsSegment },
  { "intersectRay", glm_circle_intersectsRay },
  { "intersectPlane", glm_circle_intersectsPlane },
  { GLM_NULLPTR, GLM_NULLPTR }
};

/* }================================================================== */

/*
** {==================================================================
** Plane
** ===================================================================
*/

TRAITS_DEFN(plane_operator_negate, operator-, gLuaPlane<>)
TRAITS_DEFN(plane_operator_equals, operator==, gLuaPlane<>, gLuaPlane<>)
TRAITS_DEFN(plane_operator_add, operator+, gLuaPlane<>, gLuaPlane<>::point_trait)
TRAITS_DEFN(plane_operator_sub, operator-, gLuaPlane<>, gLuaPlane<>::point_trait)
ROTATION_MATRIX_DEFN(plane_operator_mul, operator*, LAYOUT_UNARY, gLuaPlane<>::as_type<gLuaQuat<>::value_type>)
TRAITS_LAYOUT_DEFN(plane_equal, glm::equal, GEOM_EQUALS, gLuaPlane<>)
TRAITS_LAYOUT_DEFN(plane_notEqual, glm::notEqual, GEOM_EQUALS, gLuaPlane<>)
TRAITS_DEFN(plane_fromRay, glm::planeFrom, gLuaRay<>, gLuaPlane<>::point_trait)
TRAITS_DEFN(plane_fromLine, glm::planeFrom, gLuaLine<>, gLuaPlane<>::point_trait)
TRAITS_DEFN(plane_fromLineSegment, glm::planeFrom, gLuaSegment<>, gLuaPlane<>::point_trait)
TRAITS_DEFN(plane_fromPointNormal, glm::planeFrom, gLuaPlane<>::point_trait, gLuaPlane<>::point_trait)
TRAITS_DEFN(plane_fromPoints, glm::planeFrom, gLuaPlane<>::point_trait, gLuaPlane<>::point_trait, gLuaPlane<>::point_trait)
TRAITS_DEFN(plane_isDegenerate, glm::isDegenerate, gLuaPlane<>)
TRAITS_DEFN(plane_isParallel, glm::isParallel, gLuaPlane<>, gLuaPlane<>, gLuaPlane<>::eps_trait)
TRAITS_DEFN(plane_areOnSameSide, glm::areOnSameSide, gLuaPlane<>, gLuaPlane<>::point_trait, gLuaPlane<>::point_trait)
TRAITS_DEFN(plane_examineSide, glm::examineSide, gLuaPlane<>, gLuaTriangle<>, gLuaPlane<>::eps_trait)
TRAITS_DEFN(plane_isInPositiveDirection, glm::isInPositiveDirection, gLuaPlane<>, gLuaPlane<>::point_trait)
TRAITS_DEFN(plane_isOnPositiveSide, glm::isOnPositiveSide, gLuaPlane<>, gLuaPlane<>::point_trait)
TRAITS_DEFN(plane_passesThroughOrigin, glm::passesThroughOrigin, gLuaPlane<>, gLuaPlane<>::eps_trait)
TRAITS_DEFN(plane_angle, glm::angle, gLuaPlane<>, gLuaPlane<>)
TRAITS_DEFN(plane_reverseNormal, glm::reverseNormal, gLuaPlane<>)
TRAITS_DEFN(plane_pointOnPlane, glm::pointOnPlane, gLuaPlane<>)
TRAITS_DEFN(plane_refract, glm::refract, gLuaPlane<>, gLuaPlane<>::point_trait, gLuaPlane<>::value_trait, gLuaPlane<>::value_trait)
TRAITS_DEFN(plane_project, glm::project, gLuaPlane<>, gLuaPlane<>::point_trait)
TRAITS_DEFN(plane_projectLine, glm::project, gLuaPlane<>, gLuaLine<>)
TRAITS_DEFN(plane_projectSegment, glm::project, gLuaPlane<>, gLuaSegment<>)
TRAITS_DEFN(plane_projectRay, glm::project, gLuaPlane<>, gLuaRay<>)
TRAITS_DEFN(plane_projectTriangle, glm::project, gLuaPlane<>, gLuaTriangle<>)
TRAITS_DEFN(plane_projectToNegativeHalf, glm::projectToNegativeHalf, gLuaPlane<>, gLuaPlane<>::point_trait)
TRAITS_DEFN(plane_projectToPositiveHalf, glm::projectToPositiveHalf, gLuaPlane<>, gLuaPlane<>::point_trait)
TRAITS_DEFN(plane_distance, glm::distance, gLuaPlane<>, gLuaPlane<>::point_trait)
TRAITS_DEFN(plane_distanceSegment, glm::distance, gLuaPlane<>, gLuaSegment<>)
TRAITS_DEFN(plane_distanceSphere, glm::distance, gLuaPlane<>, gLuaSphere<>)
TRAITS_DEFN(plane_signedDistance, glm::signedDistance, gLuaPlane<>, gLuaPlane<>::point_trait)
TRAITS_DEFN(plane_signedDistanceLine, glm::signedDistance, gLuaPlane<>, gLuaLine<>)
TRAITS_DEFN(plane_signedDistanceSegment, glm::signedDistance, gLuaPlane<>, gLuaSegment<>)
TRAITS_DEFN(plane_signedDistanceRay, glm::signedDistance, gLuaPlane<>, gLuaRay<>)
TRAITS_DEFN(plane_signedDistanceAABB, glm::signedDistance, gLuaPlane<>, gLuaAABB<>)
TRAITS_DEFN(plane_signedDistanceSphere, glm::signedDistance, gLuaPlane<>, gLuaSphere<>)
TRAITS_DEFN(plane_signedDistanceTriangle, glm::signedDistance, gLuaPlane<>, gLuaTriangle<>)
TRAITS_DEFN(plane_orthoProjection, glm::orthoProjection, gLuaPlane<>)
TRAITS_DEFN(plane_mirrorMatrix, glm::mirrorMatrix, gLuaPlane<>)
TRAITS_DEFN(plane_mirror, glm::mirror, gLuaPlane<>, gLuaPlane<>::point_trait)
TRAITS_DEFN(plane_closestPointRay, glm::closestPoint, gLuaPlane<>, gLuaRay<>)
TRAITS_DEFN(plane_closestPointSegment, glm::closestPoint, gLuaPlane<>, gLuaSegment<>)
TRAITS_DEFN(plane_contains, glm::contains, gLuaPlane<>, gLuaPlane<>::point_trait, gLuaPlane<>::eps_trait)
TRAITS_DEFN(plane_containsLine, glm::contains, gLuaPlane<>, gLuaLine<>, gLuaPlane<>::eps_trait)
TRAITS_DEFN(plane_containsRay, glm::contains, gLuaPlane<>, gLuaRay<>, gLuaPlane<>::eps_trait)
TRAITS_DEFN(plane_containsSegment, glm::contains, gLuaPlane<>, gLuaSegment<>, gLuaPlane<>::eps_trait)
TRAITS_DEFN(plane_containsTriangle, glm::contains, gLuaPlane<>, gLuaTriangle<>, gLuaPlane<>::eps_trait)
TRAITS_LAYOUT_DEFN(plane_intersectsRay, glm::intersects, GEOM_DISTANCE, gLuaPlane<>, gLuaRay<>)
TRAITS_LAYOUT_DEFN(plane_intersectsLine, glm::intersects, GEOM_DISTANCE, gLuaPlane<>, gLuaLine<>)
TRAITS_LAYOUT_DEFN(plane_intersectsSegment, glm::intersects, GEOM_DISTANCE, gLuaPlane<>, gLuaSegment<>)
TRAITS_DEFN(plane_intersectsTriangle, glm::intersects, gLuaPlane<>, gLuaTriangle<>)
TRAITS_DEFN(plane_intersectsSphere, glm::intersects, gLuaPlane<>, gLuaSphere<>)
TRAITS_DEFN(plane_intersectsAABB, glm::intersects, gLuaPlane<>, gLuaAABB<>)
TRAITS_DEFN(plane_clipSegment, glm::clip, gLuaPlane<>, gLuaSegment<>)

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
  const gLuaPlane<>::type plane = gLuaPlane<>::Next(LB);
  const gLuaLine<>::type line = gLuaLine<>::Next(LB);
  const int clip_type = glm::clip(plane, line, result);
  TRAITS_PUSH(LB, clip_type, result);
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(plane_intersectsPlane) {
  GLM_BINDING_BEGIN
  gLuaPlane<>::point_trait::type result;
  const gLuaPlane<>::type a = gLuaPlane<>::Next(LB);
  const gLuaPlane<>::type b = gLuaPlane<>::Next(LB);
  const gLuaPlane<>::type c = gLuaPlane<>::Next(LB);
  if (glm::intersects(a, b, c, result))
    TRAITS_PUSH(LB, true, result);
  else
    TRAITS_PUSH(LB, false);
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(plane_clipTriangle) {
  GLM_BINDING_BEGIN
  const gLuaPlane<>::type plane = gLuaPlane<>::Next(LB);
  const gLuaTriangle<>::type line = gLuaTriangle<>::Next(LB);
  gLuaTriangle<>::type t1 = gLuaTriangle<>::zero();
  gLuaTriangle<>::type t2 = gLuaTriangle<>::zero();
  switch (clip(plane, line, t1, t1)) {
    case 1: TRAITS_PUSH(LB, t1); break;
    case 2: TRAITS_PUSH(LB, t1, t2); break;
    default: {
      break;
    }
  }
  return gLuaBase::Push(LB);
  GLM_BINDING_END
}

static const luaL_Reg luaglm_planelib[] = {
  { "operator_negate", glm_plane_operator_negate },
  { "operator_equals", glm_plane_operator_equals },
  { "operator_add", glm_plane_operator_add },
  { "operator_sub", glm_plane_operator_sub },
  { "operator_mul", glm_plane_operator_mul },
  { "equal", glm_plane_equal },
  { "notEqual", glm_plane_notEqual },
  //{ "tostring", glm_plane_tostring },
  { "fromRay", glm_plane_fromRay },
  { "fromLine", glm_plane_fromLine },
  { "fromLineSegment", glm_plane_fromLineSegment },
  { "fromPointNormal", glm_plane_fromPointNormal },
  { "fromPoints", glm_plane_fromPoints },
  { "isDegenerate", glm_plane_isDegenerate },
  { "isParallel", glm_plane_isParallel },
  { "areOnSameSide", glm_plane_areOnSameSide },
  { "examineSide", glm_plane_examineSide },
  { "isInPositiveDirection", glm_plane_isInPositiveDirection },
  { "isOnPositiveSide", glm_plane_isOnPositiveSide },
  { "passesThroughOrigin", glm_plane_passesThroughOrigin },
  { "angle", glm_plane_angle },
  { "reverseNormal", glm_plane_reverseNormal },
  { "pointOnPlane", glm_plane_pointOnPlane },
  { "point", glm_plane_point },
  { "refract", glm_plane_refract },
  { "project", glm_plane_project },
  { "projectLine", glm_plane_projectLine },
  { "projectSegment", glm_plane_projectSegment },
  { "projectRay", glm_plane_projectRay },
  { "projectTriangle", glm_plane_projectTriangle },
  { "projectToNegativeHalf", glm_plane_projectToNegativeHalf },
  { "projectToPositiveHalf", glm_plane_projectToPositiveHalf },
  { "distance", glm_plane_distance },
  { "distanceSegment", glm_plane_distanceSegment },
  { "distanceSphere", glm_plane_distanceSphere },
  { "signedDistance", glm_plane_signedDistance },
  { "signedDistanceLine", glm_plane_signedDistanceLine },
  { "signedDistanceSegment", glm_plane_signedDistanceSegment },
  { "signedDistanceRay", glm_plane_signedDistanceRay },
  { "signedDistanceAABB", glm_plane_signedDistanceAABB },
  { "signedDistanceSphere", glm_plane_signedDistanceSphere },
  { "signedDistanceTriangle", glm_plane_signedDistanceTriangle },
  { "orthoProjection", glm_plane_orthoProjection },
  { "mirrorMatrix", glm_plane_mirrorMatrix },
  { "mirror", glm_plane_mirror },
  { "closestPointRay", glm_plane_closestPointRay },
  { "closestPointSegment", glm_plane_closestPointSegment },
  { "contains", glm_plane_contains },
  { "containsLine", glm_plane_containsLine },
  { "containsRay", glm_plane_containsRay },
  { "containsSegment", glm_plane_containsSegment },
  { "containsTriangle", glm_plane_containsTriangle },
  { "intersectsRay", glm_plane_intersectsRay },
  { "intersectsLine", glm_plane_intersectsLine },
  { "intersectsSegment", glm_plane_intersectsSegment },
  { "intersectsTriangle", glm_plane_intersectsTriangle },
  { "intersectsSphere", glm_plane_intersectsSphere },
  { "intersectsAABB", glm_plane_intersectsAABB },
  { "intersectsPlane", glm_plane_intersectsPlane },
  { "intersectsTriangle", glm_plane_intersectsTriangle },
  { "clipSegment", glm_plane_clipSegment },
  { "clipLine", glm_plane_clipLine },
  { "clipTriangle", glm_plane_clipTriangle },
  { GLM_NULLPTR, GLM_NULLPTR },
};

/* }================================================================== */

/*
** {==================================================================
** Polygon
** ===================================================================
*/

TRAITS_DEFN(polygon_operator_negate, operator-, gLuaPolygon<>)
TRAITS_DEFN(polygon_operator_equals, operator==, gLuaPolygon<>, gLuaPolygon<>)
TRAITS_DEFN(polygon_operator_add, operator+, gLuaPolygon<>, gLuaPolygon<>::point_trait)
TRAITS_DEFN(polygon_operator_sub, operator-, gLuaPolygon<>, gLuaPolygon<>::point_trait)
ROTATION_MATRIX_DEFN(polygon_operator_mul, operator*, LAYOUT_UNARY, gLuaPolygon<>::as_type<gLuaQuat<>::value_type>)
TRAITS_DEFN(polygon_edge, glm::edge, gLuaPolygon<>, gLuaTrait<size_t>)
TRAITS_DEFN(polygon_edge2d, glm::edge2d, gLuaPolygon<>, gLuaTrait<size_t>)
TRAITS_DEFN(polygon_diagonal, glm::diagonal, gLuaPolygon<>, gLuaTrait<size_t>, gLuaTrait<size_t>)
TRAITS_DEFN(polygon_diagonalExists, glm::diagonalExists, gLuaPolygon<>, gLuaTrait<size_t>, gLuaTrait<size_t>)
TRAITS_DEFN(polygon_basisU, glm::basisU, gLuaPolygon<>)
TRAITS_DEFN(polygon_basisV, glm::basisV, gLuaPolygon<>)
TRAITS_DEFN(polygon_mapFrom2D, glm::mapFrom2D, gLuaPolygon<>, gLuaVec2<gLuaPolygon<>::value_type>)
TRAITS_DEFN(polygon_area, glm::area, gLuaPolygon<>)
TRAITS_DEFN(polygon_perimeter, glm::perimeter, gLuaPolygon<>)
TRAITS_DEFN(polygon_centroid, glm::centroid, gLuaPolygon<>)
TRAITS_DEFN(polygon_isPlanar, glm::isPlanar, gLuaPolygon<>, gLuaPolygon<>::eps_trait)
TRAITS_DEFN(polygon_isSimple, glm::isSimple, gLuaPolygon<>)
TRAITS_DEFN(polygon_isNull, glm::isNull, gLuaPolygon<>)
TRAITS_DEFN(polygon_isfinite, glm::isfinite, gLuaPolygon<>)
TRAITS_DEFN(polygon_isDegenerate, glm::isDegenerate, gLuaPolygon<>, gLuaPolygon<>::eps_trait)
TRAITS_DEFN(polygon_isConvex, glm::isConvex, gLuaPolygon<>)
TRAITS_DEFN(polygon_planeCCW, glm::planeCCW, gLuaPolygon<>)
TRAITS_DEFN(polygon_normalCCW, glm::normalCCW, gLuaPolygon<>)
TRAITS_DEFN(polygon_planeCW, glm::planeCW, gLuaPolygon<>)
TRAITS_DEFN(polygon_normalCW, glm::normalCW, gLuaPolygon<>)
TRAITS_DEFN(polygon_pointOnEdge, glm::pointOnEdge, gLuaPolygon<>, gLuaPolygon<>::value_trait)
TRAITS_DEFN(polygon_edgeNormal, glm::edgeNormal, gLuaPolygon<>, gLuaTrait<size_t>)
TRAITS_DEFN(polygon_edgePlane, glm::edgePlane, gLuaPolygon<>, gLuaTrait<size_t>)
TRAITS_DEFN(polygon_containsSegment2D, glm::contains2D, gLuaPolygon<>, gLuaSegment<>)
TRAITS_DEFN(polygon_contains, glm::contains, gLuaPolygon<>, gLuaPolygon<>::point_trait, gLuaPolygon<>::eps_trait)
TRAITS_DEFN(polygon_containsAbove, glm::containsAbove, gLuaPolygon<>, gLuaPolygon<>::point_trait, gLuaPolygon<>::eps_trait)
TRAITS_DEFN(polygon_containsBelow, glm::containsBelow, gLuaPolygon<>, gLuaPolygon<>::point_trait, gLuaPolygon<>::eps_trait)
TRAITS_DEFN(polygon_containsPolygon, glm::contains, gLuaPolygon<>, gLuaPolygon<>, gLuaPolygon<>::eps_trait)
TRAITS_DEFN(polygon_containsSegment, glm::contains, gLuaPolygon<>, gLuaSegment<>, gLuaPolygon<>::eps_trait)
TRAITS_DEFN(polygon_containsTriangle, glm::contains, gLuaPolygon<>, gLuaTriangle<>, gLuaPolygon<>::eps_trait)
TRAITS_DEFN(polygon_minimalEnclosingAABB, glm::minimalEnclosingAABB, gLuaPolygon<>)
TRAITS_DEFN(polygon_intersectsSegment2D, glm::intersects2D, gLuaPolygon<>, gLuaSegment<>)
TRAITS_DEFN(polygon_intersectsLine, glm::intersects, gLuaPolygon<>, gLuaLine<>)
TRAITS_DEFN(polygon_intersectsRay, glm::intersects, gLuaPolygon<>, gLuaRay<>)
TRAITS_DEFN(polygon_intersectsSegment, glm::intersects, gLuaPolygon<>, gLuaSegment<>)
TRAITS_DEFN(polygon_intersectsPlane, glm::intersects, gLuaPolygon<>, gLuaPlane<>)
TRAITS_LAYOUT_DEFN(polygon_projectToAxis, glm::projectToAxis, GEOM_PROJECTION, gLuaPolygon<>, gLuaPolygon<>::point_trait)

GLM_BINDING_QUALIFIER(polygon_mapTo2D) {
  GLM_BINDING_BEGIN
  if (gLuaTrait<size_t>::Is(LB.L, LB.idx + 1))
    BIND_FUNC(LB, glm::mapTo2D, gLuaPolygon<>, gLuaTrait<size_t>);
  BIND_FUNC(LB, glm::mapTo2D, gLuaPolygon<>, gLuaPolygon<>::point_trait);
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(polygon_extremePoint) {
  GLM_BINDING_BEGIN
  gLuaPolygon<>::value_type distance(0);
  const gLuaPolygon<>::type polygon = gLuaPolygon<>::Next(LB);
  const gLuaPolygon<>::point_trait::type direction = gLuaPolygon<>::point_trait::Next(LB);
  const gLuaPolygon<>::point_trait::type point = glm::extremePoint(polygon, direction, distance);
  TRAITS_PUSH(LB, point, distance);
  GLM_BINDING_END
}

/* Polygon Metamethods */

/// <summary>
/// Create a new polygon from an array of points.
/// </summary>
GLM_BINDING_QUALIFIER(polygon_new) {
  GLM_BINDING_BEGIN
  const int top = LB.top();
  if (!lua_isnoneornil(LB.L, LB.idx) && !lua_istable(LB.L, LB.idx)) {
    return luaL_argerror(LB.L, LB.idx, lua_typename(LB.L, LUA_TTABLE));
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

    // Create a std::vector backed by the Lua allocator.
    using PolyList = glm::List<gLuaPolygon<>::point_trait::type>;
    PolyList *list = static_cast<PolyList *>(allocator.realloc(GLM_NULLPTR, 0, sizeof(PolyList)));
    if (l_unlikely(list == GLM_NULLPTR)) {
      lua_pop(L, 1);
      return luaL_error(L, "polygon allocation error");
    }

    // Populate the polygon with an array of coordinates, if one exists.
  #if GLM_GEOM_EXCEPTIONS
    try {
  #endif
      polygon->p = ::new (list) PolyList(LB.L, allocator);

      if (l_likely(top >= 1 && lua_istable(LB.L, LB.idx))) {
        glmLuaArray<gLuaPolygon<>::point_trait> lArray(LB.L, LB.idx);
        const auto e = lArray.end();
        for (auto b = lArray.begin(); b != e; ++b) {
          polygon->p->push_back(*b);
        }
      }
  #if GLM_GEOM_EXCEPTIONS
    }
    catch (...) {
      lua_pop(L, 1);
      return luaL_error(L, "unknown polygon error");
    }
  #endif
    return 1;
  }

  lua_pop(L, 2);
  return luaL_error(L, "invalid polygon metatable");
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(polygon_to_string) {
  gLuaPolygon<>::type *ud = static_cast<gLuaPolygon<>::type *>(luaL_checkudata(L, 1, gLuaPolygon<>::Metatable()));
  if (l_likely(ud->p != GLM_NULLPTR)) {
    ud->p->Validate(L);
    lua_pushfstring(L, "Polygon<%I>", ud->p->size());
    return 1;
  }

  return luaL_argerror(L, 1, "Polygon");
}

/// <summary>
/// Garbage collect an allocated polygon userdata.
/// </summary>
GLM_BINDING_QUALIFIER(polygon_gc) {
  gLuaPolygon<>::type *ud = static_cast<gLuaPolygon<>::type *>(luaL_checkudata(L, 1, gLuaPolygon<>::Metatable()));
  if (l_likely(ud->p != GLM_NULLPTR)) {
    LuaCrtAllocator<void> allocator(L);
    ud->p->Validate(L);
    ud->p->~LuaVector();  // Invoke destructor.
    allocator.realloc(ud->p, sizeof(glm::List<gLuaPolygon<>::point_trait::type>), 0);  // Free allocation
    ud->p = GLM_NULLPTR;
  }
  return 0;
}

/// <summary>
/// The number of points within a polygon.
/// </summary>
TRAITS_DEFN(polygon_len, glm::length, gLuaPolygon<>)

/// <summary>
/// Create an array of points.
/// </summary>
GLM_BINDING_QUALIFIER(polygon_call) {
  GLM_BINDING_BEGIN
  const gLuaPolygon<>::type poly = gLuaPolygon<>::Next(LB);
  lua_createtable(LB.L, static_cast<int>(poly.size()), 0);
  for (size_t i = 0; i < poly.size(); ++i) {
    if (l_unlikely(gLuaBase::Push(LB, poly[i]) != 1))
      return luaL_error(LB.L, "invalid " GLM_STRING_VECTOR " structure");
    lua_rawseti(LB.L, -2, static_cast<lua_Integer>(i) + 1);
  }
  return 1;
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(polygon_index) {
  GLM_BINDING_BEGIN
  const gLuaPolygon<>::type poly = gLuaPolygon<>::Next(LB);
  if (gLuaTrait<size_t>::Is(LB.L, LB.idx)) {
    const gLuaTrait<size_t>::type index = gLuaTrait<size_t>::Next(LB);
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
  gLuaPolygon<>::type poly = gLuaPolygon<>::Next(LB);
  if (l_likely(poly.p != GLM_NULLPTR)) {
    const size_t index = gLuaTrait<size_t>::Next(LB);
    const gLuaPolygon<>::point_trait::type value = gLuaPolygon<>::point_trait::Next(LB);

    poly.p->Validate(LB.L);
    if (index >= 1 && index <= poly.size())
      poly[index - 1] = value;
    else if (index == poly.size() + 1)
      poly.p->push_back(value);
    else {
      return luaL_error(LB.L, "Invalid polygon index");
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
    if (!gLuaPolygon<>::Is(LB.L, LB.idx))
      return luaL_argerror(LB.L, LB.idx, gLuaPolygon<>::Label());

    lua_settop(LB.L, LB.idx + 1);  // create a 2nd argument if there isn't one
    const gLuaPolygon<>::type poly = gLuaPolygon<>::Next(LB);  // Polygon
    if (gLuaTrait<size_t>::Is(LB.L, LB.idx)) {  // Index
      const gLuaTrait<size_t>::type key = gLuaTrait<size_t>::Next(LB);
      if (key >= 1 && key < poly.size())
        TRAITS_PUSH(LB, key + 1, poly[key]);
      return gLuaBase::Push(LB);
    }
    else if (lua_isnoneornil(LB.L, LB.idx) && poly.size() > 0)  // First index
      TRAITS_PUSH(LB, size_t(1), poly[0]);
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
  { "__gc", glm_polygon_gc },
  { "__index", glm_polygon_index },  // Array access
  { "__newindex", glm_polygon_newindex },  // Only allow append
  { "__len", glm_polygon_len },  // # of Points
  { "__call", glm_polygon_call },  // Generate a table.
  { "__pairs", glm_polygon_pairs },
  { "__unm", glm_polygon_operator_negate },  // Negate all points.
  { "__eq", glm_polygon_operator_equals },
  { "__add", glm_polygon_operator_add },
  { "__sub", glm_polygon_operator_sub },
  { "__mul", glm_polygon_operator_mul },
  { "__tostring", glm_polygon_to_string },
  { "new", glm_polygon_new },
  { "operator_negate", glm_polygon_operator_negate },
  { "operator_equals", glm_polygon_operator_equals },
  { "operator_add", glm_polygon_operator_add },
  { "operator_sub", glm_polygon_operator_sub },
  { "operator_mul", glm_polygon_operator_mul },
  { "edge", glm_polygon_edge },
  { "edge2d", glm_polygon_edge2d },
  { "diagonal", glm_polygon_diagonal },
  { "diagonalExists", glm_polygon_diagonalExists },
  { "basisU", glm_polygon_basisU },
  { "basisV", glm_polygon_basisV },
  { "mapTo2D", glm_polygon_mapTo2D },
  { "mapFrom2D", glm_polygon_mapFrom2D },
  { "area", glm_polygon_area },
  { "perimeter", glm_polygon_perimeter },
  { "centroid", glm_polygon_centroid },
  { "isPlanar", glm_polygon_isPlanar },
  { "isSimple", glm_polygon_isSimple },
  { "isNull", glm_polygon_isNull },
  { "isfinite", glm_polygon_isfinite },
  { "isDegenerate", glm_polygon_isDegenerate },
  { "isConvex", glm_polygon_isConvex },
  { "extremePoint", glm_polygon_extremePoint },
  { "projectToAxis", glm_polygon_projectToAxis },
  { "planeCCW", glm_polygon_planeCCW },
  { "normalCCW", glm_polygon_normalCCW },
  { "planeCW", glm_polygon_planeCW },
  { "normalCW", glm_polygon_normalCW },
  { "pointOnEdge", glm_polygon_pointOnEdge },
  { "edgeNormal", glm_polygon_edgeNormal },
  { "edgePlane", glm_polygon_edgePlane },
  { "containsSegment2D", glm_polygon_containsSegment2D },
  { "contains", glm_polygon_contains },
  { "containsAbove", glm_polygon_containsAbove },
  { "containsBelow", glm_polygon_containsBelow },
  { "containsPolygon", glm_polygon_containsPolygon },
  { "containsSegment", glm_polygon_containsSegment },
  { "containsTriangle", glm_polygon_containsTriangle },
  { "minimalEnclosingAABB", glm_polygon_minimalEnclosingAABB },
  { "intersectsSegment2D", glm_polygon_intersectsSegment2D },
  { "intersectsLine", glm_polygon_intersectsLine },
  { "intersectsRay", glm_polygon_intersectsRay },
  { "intersectsSegment", glm_polygon_intersectsSegment },
  { "intersectsPlane", glm_polygon_intersectsPlane },
  { GLM_NULLPTR, GLM_NULLPTR },
};

/* }================================================================== */

#endif
