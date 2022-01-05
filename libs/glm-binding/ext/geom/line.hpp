/// <summary>
/// See Copyright Notice in setup.hpp
/// </summary>
#ifndef EXT_GEOM_LINE_HPP
#define EXT_GEOM_LINE_HPP

#include "setup.hpp"
#include <glm/gtx/vector_query.hpp>

#if GLM_MESSAGES == GLM_ENABLE && !defined(GLM_EXT_INCLUDED)
  #pragma message("GLM: GLM_EXT_GEOM_line extension included")
#endif

namespace glm {
  /// <summary>
  /// A line in defined by an origin pointer and a direction, extending to +/- infinity.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  struct Line {

    // -- Implementation detail --

    typedef T value_type;
    typedef Line<L, T, Q> type;
    typedef vec<L, T, Q> point_type;

    // -- Data --

    point_type pos;  // Specifies the origin of this line.
    point_type dir;  // The normalized direction vector of this ray.

#if GLM_CONFIG_DEFAULTED_DEFAULT_CTOR == GLM_ENABLE
    Line() GLM_DEFAULT_CTOR;
#else
    Line()
  #if GLM_CONFIG_CTOR_INIT != GLM_CTOR_INIT_DISABLE
      : pos(T(0)), dir(T(0))
  #endif
    {
    }
#endif

    Line(T scalar)
      : pos(scalar), dir(scalar) {
    }

    Line(const vec<L, T, Q> &position, const vec<L, T, Q> &direction)
      : pos(position), dir(normalize(direction)) {
      GLM_GEOM_ASSERT(glm::isNormalized(dir));
    }

    Line(const Line<L, T, Q> &line)
      : pos(line.pos), dir(line.dir) {
      GLM_GEOM_ASSERT(glm::isNormalized(dir));
    }
  };

  // template<length_t L, typename T, qualifier Q>
  // GLM_GEOM_QUALIFIER Ray<L, T, Q> toRay(const Line<L, T, Q> &line) {
  //   return Ray<L, T, Q>(line.pos, line.dir;
  // }

  template<length_t L, typename T, qualifier Q>
  static Line<L, T, Q> operator-(const Line<L, T, Q> &line) {
    return Line<L, T, Q>(line.pos, -line.dir);
  }

  template<length_t L, typename T, qualifier Q>
  static bool operator==(const Line<L, T, Q> &l1, const Line<L, T, Q> &l2) {
    return l1.pos == l2.pos && l1.dir == l2.dir;
  }

  template<length_t L, typename T, qualifier Q>
  static Line<L, T, Q> operator+(const Line<L, T, Q> &ray, const vec<L, T, Q> &offset) {
    return Line<L, T, Q>(ray.pos + offset, ray.dir);
  }

  template<length_t L, typename T, qualifier Q>
  static Line<L, T, Q> operator-(const Line<L, T, Q> &ray, const vec<L, T, Q> &offset) {
    return Line<L, T, Q>(ray.pos - offset, ray.dir);
  }

  template<typename T, qualifier Q>
  static Line<3, T, Q> operator*(const mat<3, 3, T, Q> &m, const Line<3, T, Q> &line) {
    return Line<3, T, Q>(m * line.pos, m * line.dir);
  }

  template<typename T, qualifier Q>
  static Line<3, T, Q> operator*(const mat<3, 4, T, Q> &m, const Line<3, T, Q> &line) {
    return Line<3, T, Q>(m * line.pos, m * line.dir);
  }

  template<typename T, qualifier Q>
  static Line<3, T, Q> operator*(const mat<4, 3, T, Q> &m, const Line<3, T, Q> &line) {
    return Line<3, T, Q>(transformPos(m, line.pos), transformDir(m, line.dir));
  }

  template<typename T, qualifier Q>
  static Line<3, T, Q> operator*(const mat<4, 4, T, Q> &m, const Line<3, T, Q> &line) {
    return Line<3, T, Q>(transformPos(m, line.pos), transformDir(m, line.dir));
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Line<3, T, Q> operator*(const qua<T, Q> &q, const Line<3, T, Q> &line) {
    return Line<3, T, Q>(q * line.pos, q * line.dir);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(Line<L, T, Q> const &x, Line<L, T, Q> const &y, T eps = epsilon<T>()) {
    return all_equal(x.pos, y.pos, eps) && all_equal(x.dir, y.dir, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(Line<L, T, Q> const &x, Line<L, T, Q> const &y, vec<L, T, Q> const &eps) {
    return all_equal(x.pos, y.pos, eps) && all_equal(x.dir, y.dir, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(Line<L, T, Q> const &x, Line<L, T, Q> const &y, int MaxULPs) {
    return all_equal(x.pos, y.pos, MaxULPs) && all_equal(x.dir, y.dir, MaxULPs);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(Line<L, T, Q> const &x, Line<L, T, Q> const &y, vec<L, int, Q> const &MaxULPs) {
    return all_equal(x.pos, y.pos, MaxULPs) && all_equal(x.dir, y.dir, MaxULPs);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(Line<L, T, Q> const &x, Line<L, T, Q> const &y, T eps = epsilon<T>()) {
    return any_notequal(x.pos, y.pos, eps) || any_notequal(x.dir, y.dir, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(Line<L, T, Q> const &x, Line<L, T, Q> const &y, vec<L, T, Q> const &eps) {
    return any_notequal(x.pos, y.pos, eps) || any_notequal(x.dir, y.dir, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(Line<L, T, Q> const &x, Line<L, T, Q> const &y, int MaxULPs) {
    return any_notequal(x.pos, y.pos, MaxULPs) || any_notequal(x.dir, y.dir, MaxULPs);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(Line<L, T, Q> const &x, Line<L, T, Q> const &y, vec<L, int, Q> const &MaxULPs) {
    return any_notequal(x.pos, y.pos, MaxULPs) || any_notequal(x.dir, y.dir, MaxULPs);
  }

  // Forward declarations

  /// <summary>
  /// Computes the closest point pair on two lines.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool closestPointLineLine(const vec<L, T, Q> &v0, const vec<L, T, Q> &v10, const vec<L, T, Q> &v2, const vec<L, T, Q> &v32, T &d1, T &d2);

  /// <summary>
  /// Compute the closest point (along an edge) between the triangle and provided line.
  /// </summary>
  template<length_t L, typename T, qualifier Q, typename Line>
  GLM_GEOM_QUALIFIER_NOINLINE vec<L, T, Q> closestPointTriangleLine(const Triangle<L, T, Q> &t, const Line &line, T &outU, T &outV, T &outD);

  /// <summary>
  /// Tests if any component of the line is infinite.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isinf(const Line<L, T, Q> &line) {
    return any_isinf(line.pos) || any_isinf(line.dir);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isnan(const Line<L, T, Q> &line) {
    return any_isnan(line.pos) || any_isnan(line.dir);
  }

  /// <summary>
  /// Test if all components of the line are finite.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isfinite(const Line<L, T, Q> &line) {
    return all(isfinite(line.pos)) && all(isfinite(line.dir));
  }

  /// <summary>
  /// Get a point along the line at a given distance (parametric point).
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> getPoint(const Line<L, T, Q> &line, T d) {
    return line.pos + d * line.dir;
  }

  // Computes the closest point on this line to the given object.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Line<L, T, Q> &line, const vec<L, T, Q> &targetPoint, T &d) {
    d = max(T(0), dot(targetPoint - line.pos, line.dir));
    return getPoint(line, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Line<L, T, Q> &line, const Line<L, T, Q> &other, T &d1, T &d2) {
    closestPointLineLine(line.pos, line.dir, other.pos, other.dir, d1, d2);
    return getPoint(line, d1);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Line<L, T, Q> &line, const LineSegment<L, T, Q> &segment, T &d1, T &d2) {
    closestPointLineLine(line.pos, line.dir, segment.a, segment.dir2(), d1, d2);
    if (d2 < T(0)) {
      d2 = T(0);
      return closestPoint(line, segment.a, d1);
    }
    else if (d2 > T(1)) {
      d2 = T(1);
      return closestPoint(line, segment.b, d1);
    }
    return getPoint(line, d1);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Line<L, T, Q> &line, const Ray<L, T, Q> &ray, T &d1, T &d2) {
    closestPointLineLine(line.pos, line.dir, ray.pos, ray.dir, d1, d2);
    if (d2 >= T(0))
      return getPoint(line, d1);
    else {
      d2 = T(0);
      return closestPoint(line, ray.pos, d1);
    }
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Line<L, T, Q> &line, const Triangle<L, T, Q> &triangle, T &d, T &u, T &v) {
    d = intersectTriangleLine(triangle, line, u, v);  // Compute distance along line
    if (isinf(d)) {
      closestPointTriangleLine<L, T, Q, Line<L, T, Q>>(triangle, line, u, v, d);
    }

    return getPoint(line, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Line<L, T, Q> &line, const vec<L, T, Q> &targetPoint) {
    T d(0);
    return closestPoint(line, targetPoint, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Line<L, T, Q> &line, const Line<L, T, Q> &other) {
    T d1(0), d2(0);
    return closestPoint(line, other, d1, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Line<L, T, Q> &line, const LineSegment<L, T, Q> &segment) {
    T d1(0), d2(0);
    return closestPoint(line, segment, d1, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Line<L, T, Q> &line, const Ray<L, T, Q> &ray) {
    T d1(0), d2(0);
    return closestPoint(line, ray, d1, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Line<L, T, Q> &line, const Triangle<L, T, Q> &triangle) {
    T u, v, d(0);
    return closestPoint(line, triangle, d, u, v);
  }

  // Tests if the given object is fully contained on the line.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Line<L, T, Q> &line, const vec<L, T, Q> &point, T eps = epsilon<T>()) {
    T d(0);
    return distance2(closestPoint(line, point, d), point) <= eps;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Line<L, T, Q> &line, const Ray<L, T, Q> &ray, T eps = epsilon<T>()) {
    return contains(line, ray.pos, eps) && all(epsilonEqual(line.dir, ray.dir, eps));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Line<L, T, Q> &line, const LineSegment<L, T, Q> &lineSegment, T eps = epsilon<T>()) {
    return contains(line, lineSegment.a, eps) && contains(line, lineSegment.b, eps);
  }

  // Computes the distance between the line and the given object.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Line<L, T, Q> &line, const vec<L, T, Q> &point, T &d) {
    return distance(closestPoint(line, point, d), point);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Line<L, T, Q> &line, const Ray<L, T, Q> &ray, T &d1, T &d2) {
    const vec<L, T, Q> point = closestPoint(line, ray, d1, d2);
    return distance(point, getPoint(ray, d2));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Line<L, T, Q> &line, const Line<L, T, Q> &other, T &d1, T &d2) {
    const vec<L, T, Q> point = closestPoint(line, other, d1, d2);
    return distance(point, getPoint(other, d2));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Line<L, T, Q> &line, const LineSegment<L, T, Q> &other, T &d1, T &d2) {
    const vec<L, T, Q> point = closestPoint(line, other, d1, d2);
    return (d2 >= T(0) && d2 <= T(1)) ? distance(point, getPoint(other, d2)) : T(-1);  // Invalid
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Line<L, T, Q> &line, const Sphere<L, T, Q> &sphere) {
    T ignore = T(0);
    return max(T(0), distance(line, sphere.pos, ignore) - sphere.r);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Line<L, T, Q> &line, const vec<L, T, Q> &point) {
    T d(0);
    return distance(line, point, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Line<L, T, Q> &line, const Ray<L, T, Q> &ray) {
    T d1(0), d2(0);
    return distance(line, ray, d1, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Line<L, T, Q> &line, const Line<L, T, Q> &other) {
    T d1(0), d2(0);
    return distance(line, other, d1, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Line<L, T, Q> &line, const LineSegment<L, T, Q> &other) {
    T d1(0), d2(0);
    return distance(line, other, d1, d2);
  }

  /// Tests whether the line and the given object intersect.

  /// <param name="dNear">parametric distance along the line where it enters the AABB.</param>
  /// <param name="dFar">parametric distance along the line where it exits the AABB.</param>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Line<L, T, Q> &line, const AABB<L, T, Q> &aabb, T &dNear, T &dFar) {
    return intersects(aabb, line, dNear, dFar);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER int intersects(const Line<L, T, Q> &line, const Sphere<L, T, Q> &s, T &d1, T &d2) {
    return intersects(s, line, d1, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Line<L, T, Q> &line, const Plane<L, T, Q> &plane, T &d) {
    return intersects(plane, line, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Line<L, T, Q> &line, const Triangle<L, T, Q> &triangle, T &d, T &u, T &v) {
    return intersects(triangle, line, u, v, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Line<L, T, Q> &line, const AABB<L, T, Q> &aabb) {
    return intersects(aabb, line);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Line<L, T, Q> &line, const Sphere<L, T, Q> &s) {
    T d1(0), d2(0);
    return intersects(line, s, d1, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Line<L, T, Q> &line, const Plane<L, T, Q> &plane) {
    T d(0);
    return intersects(line, plane, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Line<L, T, Q> &line, const Triangle<L, T, Q> &triangle) {
    T u, v, d(0);
    return intersects(line, triangle, d, u, v);
  }

  /// <summary>
  /// Convert the Line to a LineSegment.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER LineSegment<L, T, Q> toLineSegment(const Line<L, T, Q> &line, T d) {
    return LineSegment<L, T, Q>(line.pos, getPoint(line, d));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER LineSegment<L, T, Q> toLineSegment(const Line<L, T, Q> &line, T start, T end) {
    return LineSegment<L, T, Q>(getPoint(line, start), getPoint(line, end));
  }

  /// <summary>
  /// Project the line onto the given axis (direction), i.e., collapse the line
  /// onto an axis.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER void projectToAxis(const Line<L, T, Q> &line, const vec<L, T, Q> &direction, T &outMin, T &outMax) {
    if (isPerpendicular(line.dir, direction))
      outMin = outMax = dot(direction, line.pos);
    else {
      outMin = -std::numeric_limits<T>::infinity();
      outMax = std::numeric_limits<T>::infinity();
    }
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool closestPointLineLine(const vec<L, T, Q> &v0, const vec<L, T, Q> &v1, const vec<L, T, Q> &v2, const vec<L, T, Q> &v3, T &d1, T &d2) {
    const bool isNullV1 = isNull(v1, epsilon<T>());
    const bool isNullV3 = isNull(v3, epsilon<T>());
    if (isNullV1 || isNullV3) {  // atleast one line is degenerate
      d1 = isNullV1 ? T(0) : T(0.5);
      d2 = isNullV3 ? T(0) : T(0.5);
      return false;
    }

    const T d33 = dot(v3, v3);
    if (d33 != T(0)) {  // @TODO: notEqual(d33, T(0), epsilon<T>())
      const vec<L, T, Q> v4 = v0 - v2;
      const T d43 = dot(v4, v3);
      const T d31 = dot(v3, v1);
      const T denom = dot(v1, v1) * d33 - d31 * d31;
      d1 = (denom != T(0)) ? (d43 * d31 - dot(v4, v1) * d33) / denom : T(0);  // @TODO: notEqual(denom, T(0), epsilon<T>())
      d2 = (d43 + d1 * d31) / d33;
      return true;
    }
    else {
      d1 = d2 = T(0);
      return false;  // zero direction vector.
    }
  }

  namespace detail {
#if GLM_GEOM_TOSTRING
    template<glm::length_t L, typename T, qualifier Q>
    struct compute_to_string<Line<L, T, Q>> {
      GLM_GEOM_QUALIFIER std::string call(const Line<L, T, Q> &line) {
        return detail::format("line(%s, %s)",
          glm::to_string(line.pos).c_str(),
          glm::to_string(line.dir).c_str()
        );
      }
    };
#endif
  }
}

#endif
