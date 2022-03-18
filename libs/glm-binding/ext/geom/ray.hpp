/// <summary>
/// See Copyright Notice in setup.hpp
/// </summary>
#ifndef EXT_GEOM_RAY_HPP
#define EXT_GEOM_RAY_HPP

#include "setup.hpp"
#include "line.hpp"
#include "linesegment.hpp"
#include "triangle.hpp"

#if GLM_MESSAGES == GLM_ENABLE && !defined(GLM_EXT_INCLUDED)
  #pragma message("GLM: GLM_EXT_GEOM_ray extension included")
#endif

namespace glm {
  /// <summary>
  /// A line by an origin point and a direction: extending to infinity in one
  /// direction.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  struct Ray : Line<L, T, Q> {

    // -- Implementation detail --

    typedef T value_type;
    typedef Ray<L, T, Q> type;
    typedef vec<L, T, Q> point_type;

    // -- Data --

    using Line<L, T, Q>::pos;  // Specifies the origin of this line.
    using Line<L, T, Q>::dir;  // The normalized direction vector of this ray.

#if GLM_CONFIG_DEFAULTED_DEFAULT_CTOR == GLM_ENABLE
    Ray() GLM_DEFAULT_CTOR;
#else
    Ray()
  #if GLM_CONFIG_CTOR_INIT != GLM_CTOR_INIT_DISABLE
      : Line<L, T, Q>()
  #endif
    {
    }
#endif

    Ray(T scalar)
      : Line<L, T, Q>(scalar) {
    }

    Ray(const vec<L, T, Q> &position, const vec<L, T, Q> &direction)
      : Line<L, T, Q>(position, direction) {
      GLM_GEOM_ASSERT(isNormalized(dir, epsilon<T>()));
    }

    Ray(const Line<L, T, Q> &line)
      : Line<L, T, Q>(line) {
      GLM_GEOM_ASSERT(isNormalized(dir, epsilon<T>()));
    }

    Ray(const Ray<L, T, Q> &ray)
      : Line<L, T, Q>(ray) {
      GLM_GEOM_ASSERT(isNormalized(dir, epsilon<T>()));
    }

    Ray<L, T, Q> &operator=(const Ray<L, T, Q> &ray) {
      pos = ray.pos;
      dir = ray.dir;
      return *this;
    }
  };

  template<length_t L, typename T, qualifier Q>
  static Ray<L, T, Q> operator-(const Ray<L, T, Q> &ray) {
    return Ray<L, T, Q>(ray.pos, -ray.dir);
  }

  template<length_t L, typename T, qualifier Q>
  static bool operator==(const Ray<L, T, Q> &r1, const Ray<L, T, Q> &r2) {
    return r1.pos == r2.pos && r1.dir == r2.dir;
  }

  template<length_t L, typename T, qualifier Q>
  static Ray<L, T, Q> operator+(const Ray<L, T, Q> &ray, const vec<L, T, Q> &offset) {
    return Ray<L, T, Q>(ray.pos + offset, ray.dir);
  }

  template<length_t L, typename T, qualifier Q>
  static Ray<L, T, Q> operator-(const Ray<L, T, Q> &ray, const vec<L, T, Q> &offset) {
    return Ray<L, T, Q>(ray.pos - offset, ray.dir);
  }

  template<typename T, qualifier Q>
  static Ray<3, T, Q> operator*(const mat<3, 3, T, Q> &m, const Ray<3, T, Q> &ray) {
    return Ray<3, T, Q>(m * ray.pos, m * ray.dir);
  }

  template<typename T, qualifier Q>
  static Ray<3, T, Q> operator*(const mat<3, 4, T, Q> &m, const Ray<3, T, Q> &ray) {
    return Ray<3, T, Q>(m * ray.pos, m * ray.dir);
  }

  template<typename T, qualifier Q>
  static Ray<3, T, Q> operator*(const mat<4, 3, T, Q> &m, const Ray<3, T, Q> &ray) {
    return Ray<3, T, Q>(transformPos(m, ray.pos), transformDir(m, ray.dir));
  }

  template<typename T, qualifier Q>
  static Ray<3, T, Q> operator*(const mat<4, 4, T, Q> &m, const Ray<3, T, Q> &ray) {
    return Ray<3, T, Q>(transformPos(m, ray.pos), transformDir(m, ray.dir));
  }

  template<typename T, qualifier Q>
  static Ray<3, T, Q> operator*(const qua<T, Q> &q, const Ray<3, T, Q> &ray) {
    return Ray<3, T, Q>(q * ray.pos, q * ray.dir);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(Ray<L, T, Q> const &x, Ray<L, T, Q> const &y, const T eps = epsilon<T>()) {
    return all_equal(x.pos, y.pos, eps) && all_equal(x.dir, y.dir, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(Ray<L, T, Q> const &x, Ray<L, T, Q> const &y, vec<L, T, Q> const &eps) {
    return all_equal(x.pos, y.pos, eps) && all_equal(x.dir, y.dir, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(Ray<L, T, Q> const &x, Ray<L, T, Q> const &y, int MaxULPs) {
    return all_equal(x.pos, y.pos, MaxULPs) && all_equal(x.dir, y.dir, MaxULPs);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(Ray<L, T, Q> const &x, Ray<L, T, Q> const &y, vec<L, int, Q> const &MaxULPs) {
    return all_equal(x.pos, y.pos, MaxULPs) && all_equal(x.dir, y.dir, MaxULPs);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(Ray<L, T, Q> const &x, Ray<L, T, Q> const &y, const T eps = epsilon<T>()) {
    return any_notequal(x.pos, y.pos, eps) || any_notequal(x.dir, y.dir, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(Ray<L, T, Q> const &x, Ray<L, T, Q> const &y, vec<L, T, Q> const &eps) {
    return any_notequal(x.pos, y.pos, eps) || any_notequal(x.dir, y.dir, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(Ray<L, T, Q> const &x, Ray<L, T, Q> const &y, int MaxULPs) {
    return any_notequal(x.pos, y.pos, MaxULPs) || any_notequal(x.dir, y.dir, MaxULPs);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(Ray<L, T, Q> const &x, Ray<L, T, Q> const &y, vec<L, int, Q> const &MaxULPs) {
    return any_notequal(x.pos, y.pos, MaxULPs) || any_notequal(x.dir, y.dir, MaxULPs);
  }

  /// <summary>
  /// Tests if any component of the ray is infinite.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isinf(const Ray<L, T, Q> &line) {
    return any_isinf(line.pos) || any_isinf(line.dir);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isnan(const Ray<L, T, Q> &line) {
    return any_isnan(line.pos) || any_isnan(line.dir);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isfinite(const Ray<L, T, Q> &line) {
    return all(isfinite(line.pos)) && all(isfinite(line.dir));
  }

  /// <summary>
  /// Get a point along the ray at a given distance (parametric point).
  ///
  /// Passing negative values to this function treats the ray as if it were a
  /// line.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> getPoint(const Ray<L, T, Q> &ray, T d) {
    return ray.pos + d * ray.dir;
  }

  // Computes the closest point on this ray to the given object.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Ray<L, T, Q> &ray, const vec<L, T, Q> &targetPoint, T &d) {
    d = max(T(0), dot(targetPoint - ray.pos, ray.dir));
    return getPoint(ray, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE vec<L, T, Q> closestPoint(const Ray<L, T, Q> &ray, const Line<L, T, Q> &other, T &d1, T &d2) {
    closestPointLineLine(ray.pos, ray.dir, other.pos, other.dir, d1, d2);
    if (d1 < T(0)) {
      d1 = T(0);
      closestPoint(other, ray.pos, d2);
      return ray.pos;
    }
    return getPoint(ray, d1);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE vec<L, T, Q> closestPoint(const Ray<L, T, Q> &ray, const Ray<L, T, Q> &other, T &d1, T &d2) {
    closestPointLineLine(ray.pos, ray.dir, other.pos, other.dir, d1, d2);
    if (d1 < T(0) && d2 < T(0)) {
      const vec<L, T, Q> pt = closestPoint(ray, other.pos, d1);
      const vec<L, T, Q> pt2 = closestPoint(other, ray.pos, d2);
      if (distance2(pt, other.pos) <= distance2(pt2, ray.pos)) {
        d2 = T(0);
        return pt;
      }
      else {
        d1 = T(0);
        return ray.pos;
      }
    }
    else if (d1 < T(0)) {
      closestPoint(other, ray.pos, d2);
      d1 = T(0);
      d2 = max(T(0), d2);
      return ray.pos;
    }
    else if (d2 < T(0)) {
      const vec<L, T, Q> pt = closestPoint(ray, other.pos, d1);
      d1 = max(T(0), d1);
      d2 = T(0);
      return pt;
    }
    else {
      return getPoint(ray, d1);
    }
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE vec<L, T, Q> closestPoint(const Ray<L, T, Q> &ray, const LineSegment<L, T, Q> &other, T &d1, T &d2) {
    closestPointLineLine(ray.pos, ray.dir, other.a, other.dir2(), d1, d2);
    if (d1 < T(0)) {
      d1 = T(0);
      if (d2 >= T(0) && d2 <= T(1)) {
        closestPoint(other, ray.pos, d2);
        return ray.pos;
      }
      else {
        const T t2 = (d2 < T(0)) ? T(0) : T(1);
        const vec<L, T, Q> p = (d2 < T(0)) ? other.a : other.b;
        const vec<L, T, Q> pt = closestPoint(ray, p, d1);
        const vec<L, T, Q> pt2 = closestPoint(other, ray.pos, d2);
        if (distance2(pt, p) <= distance2(pt2, ray.pos)) {
          d2 = t2;
          return pt;
        }
        else {
          d1 = T(0);
          return ray.pos;
        }
      }
    }
    else if (d2 < T(0)) {
      d2 = T(0);
      return closestPoint(ray, other.a, d1);
    }
    else if (d2 > T(1)) {
      d2 = T(1);
      return closestPoint(ray, other.b, d1);
    }
    return getPoint(ray, d1);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Ray<L, T, Q> &ray, const vec<L, T, Q> &targetPoint) {
    T d(0);
    return closestPoint(ray, targetPoint, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Ray<L, T, Q> &ray, const Line<L, T, Q> &other) {
    T d1(0), d2(0);
    return closestPoint(ray, other, d1, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Ray<L, T, Q> &ray, const LineSegment<L, T, Q> &segment) {
    T d1(0), d2(0);
    return closestPoint(ray, segment, d1, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Ray<L, T, Q> &ray, const Ray<L, T, Q> &other) {
    T d1(0), d2(0);
    return closestPoint(ray, other, d1, d2);
  }

  // Tests if the given object is fully contained on the ray.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Ray<L, T, Q> &ray, const vec<L, T, Q> &point, T distanceThreshold) {
    T ignore(0);
    return distance2(closestPoint(ray, point, ignore), point) <= distanceThreshold;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Ray<L, T, Q> &ray, const LineSegment<L, T, Q> &lineSegment, T distanceThreshold) {
    return contains(ray, lineSegment.a, distanceThreshold) && contains(ray, lineSegment.b, distanceThreshold);
  }

  // Computes the distance between the ray and the given object.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Ray<L, T, Q> &ray, const vec<L, T, Q> &point, T &d) {
    return distance(closestPoint(ray, point, d), point);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Ray<L, T, Q> &ray, const Ray<L, T, Q> &other, T &d1, T &d2) {
    const vec<L, T, Q> point = closestPoint(ray, other, d1, d2);
    return distance(point, getPoint(other, d2));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Ray<L, T, Q> &ray, const Line<L, T, Q> &line, T &d1, T &d2) {
    const vec<L, T, Q> point = closestPoint(ray, line, d1, d2);
    return distance(point, getPoint(line, d2));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Ray<L, T, Q> &ray, const LineSegment<L, T, Q> &line, T &d1, T &d2) {
    const vec<L, T, Q> point = closestPoint(ray, line, d1, d2);
    return distance(point, getPoint(line, d2));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Ray<L, T, Q> &ray, const Sphere<L, T, Q> &sphere) {
    return max(T(0), distance(ray, sphere.pos) - sphere.r);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Ray<L, T, Q> &ray, const vec<L, T, Q> &point) {
    T d(0);
    return distance(ray, point, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Ray<L, T, Q> &ray, const Ray<L, T, Q> &other) {
    T d1(0), d2(0);
    return distance(ray, other, d1, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Ray<L, T, Q> &ray, const Line<L, T, Q> &line) {
    T d1(0), d2(0);
    return distance(ray, line, d1, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Ray<L, T, Q> &ray, const LineSegment<L, T, Q> &line) {
    T d1(0), d2(0);
    return distance(ray, line, d1, d2);
  }

  /// Tests whether the ray and the given object intersect.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER int intersects(const Ray<L, T, Q> &ray, const Sphere<L, T, Q> &sphere, T &d1, T &d2) {
    return intersects(sphere, ray, d1, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Ray<L, T, Q> &ray, const AABB<L, T, Q> &aabb, T &d1, T &d2) {
    return intersects(aabb, ray, d1, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Ray<L, T, Q> &ray, const Plane<L, T, Q> &plane, T &d) {
    return intersects(plane, ray, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Ray<L, T, Q> &ray, const Triangle<L, T, Q> &triangle, T &d, T &u, T &v) {
    d = intersectTriangleLine(triangle, ray, u, v);
    return isfinite(d) && d >= T(0);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Ray<L, T, Q> &ray, const Sphere<L, T, Q> &sphere) {
    T d1(0), d2(0);
    return intersects(ray, sphere, d1, d2) > 0;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Ray<L, T, Q> &ray, const AABB<L, T, Q> &aabb) {
    T d1(0), d2(0);
    return intersects(ray, aabb, d1, d2) > 0;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Ray<L, T, Q> &ray, const Plane<L, T, Q> &plane) {
    T d(0);
    return intersects(ray, plane, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Ray<L, T, Q> &ray, const Triangle<L, T, Q> &triangle) {
    T u, v, d(0);
    return intersects(ray, triangle, d, u, v);
  }

  /// <summary>
  /// Convert the ray to a LineSegment.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER LineSegment<L, T, Q> toLineSegment(const Ray<L, T, Q> &ray, T d) {
    return LineSegment<L, T, Q>(ray.pos, getPoint(ray, d));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER LineSegment<L, T, Q> toLineSegment(const Ray<L, T, Q> &ray, T dStart, T dEnd) {
    return LineSegment<L, T, Q>(getPoint(ray, dStart), getPoint(ray, dEnd));
  }

  /// <summary>
  /// Project the ray onto the given axis (direction), i.e., collapse the ray
  /// onto an axis.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER void projectToAxis(const Ray<L, T, Q> &ray, const vec<L, T, Q> &direction, T &outMin, T &outMax) {
    T d = dot(direction, ray.dir);

    outMin = outMax = dot(direction, ray.pos);
    if (d > epsilon<T>())
      outMax = std::numeric_limits<T>::infinity();
    else if (d < -epsilon<T>())
      outMin = -std::numeric_limits<T>::infinity();
  }

  namespace detail {
#if GLM_GEOM_TOSTRING
    template<glm::length_t L, typename T, qualifier Q>
    struct compute_to_string<Ray<L, T, Q>> {
      GLM_GEOM_QUALIFIER std::string call(const Ray<L, T, Q> &ray) {
        return detail::format("ray(%s, %s)",
          glm::to_string(ray.pos).c_str(),
          glm::to_string(ray.dir).c_str()
        );
      }
    };
#endif
  }
}

#endif
