/// <summary>
/// See Copyright Notice in setup.hpp
/// </summary>
#ifndef EXT_GEOM_SPHERE_HPP
#define EXT_GEOM_SPHERE_HPP

#include <algorithm>

#include "setup.hpp"
#include "line.hpp"
#include "linesegment.hpp"
#include "ray.hpp"
#include "triangle.hpp"

#if GLM_MESSAGES == GLM_ENABLE && !defined(GLM_EXT_INCLUDED)
  #pragma message("GLM: GLM_EXT_GEOM_sphere extension included")
#endif

namespace glm {
  /// <summary>
  /// A 3D Sphere.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  struct Sphere {

    // -- Implementation detail --

    typedef T value_type;
    typedef Sphere<L, T, Q> type;
    typedef vec<L, T, Q> point_type;

    // -- Data --

    point_type pos;  // The center point of this sphere.
    value_type r;  // The radius of this sphere.

#if GLM_CONFIG_DEFAULTED_DEFAULT_CTOR == GLM_ENABLE
    Sphere() GLM_DEFAULT_CTOR;
#else
    Sphere()
  #if GLM_CONFIG_CTOR_INIT != GLM_CTOR_INIT_DISABLE
      : pos(T(0)), r(T(0))
  #endif
    {
    }
#endif

    Sphere(T scalar)
      : pos(scalar), r(scalar) {
    }

    Sphere(const vec<L, T, Q> &position, T radius)
      : pos(position), r(radius) {
    }

    Sphere(const Sphere<L, T, Q> &sphere)
      : pos(sphere.pos), r(sphere.r) {
    }

    Sphere<L, T, Q> &operator=(const Sphere<L, T, Q> &sphere) {
      pos = sphere.pos;
      r = sphere.r;
      return *this;
    }

    void setDegenerate() {
      pos = point_type(std::numeric_limits<T>::quiet_NaN());
      r = std::numeric_limits<T>::quiet_NaN();
    }

    void enclose(const point_type &point, const T eps = T(0)) {
      const point_type d = point - pos;
      const T dist2 = length2(d);
      if (dist2 + eps > r * r) {
        const T dist = sqrt(dist2);
        const T halfDist = (dist - r) * T(0.5);

        pos += d * halfDist / dist;
        r += halfDist + epsilon<T>();  // deliberately use a fixed epsilon; see logic in optimalEnclosingSphere
      }
    }
  };

  template<length_t L, typename T, qualifier Q>
  static Sphere<L, T, Q> operator-(const Sphere<L, T, Q> &sphere) {
    return Sphere<L, T, Q>(sphere.pos, sphere.r);
  }

  template<length_t L, typename T, qualifier Q>
  static bool operator==(const Sphere<L, T, Q> &s1, const Sphere<L, T, Q> &s2) {
    return s1.pos == s2.pos && glm::detail::equal_strict(s1.r, s2.r);
  }

  template<length_t L, typename T, qualifier Q>
  static Sphere<L, T, Q> operator+(const Sphere<L, T, Q> &sphere, const vec<L, T, Q> &offset) {
    return Sphere<L, T, Q>(sphere.pos + offset, sphere.r);
  }

  template<length_t L, typename T, qualifier Q>
  static Sphere<L, T, Q> operator-(const Sphere<L, T, Q> &sphere, const vec<L, T, Q> &offset) {
    return Sphere<L, T, Q>(sphere.pos - offset, sphere.r);
  }

  template<typename T, qualifier Q>
  static Sphere<3, T, Q> operator*(const mat<3, 3, T, Q> &m, const Sphere<3, T, Q> &sphere) {
    return Sphere<3, T, Q>(m * sphere.pos, length(m[0]) * sphere.r);
  }

  template<typename T, qualifier Q>
  static Sphere<3, T, Q> operator*(const mat<3, 4, T, Q> &m, const Sphere<3, T, Q> &sphere) {
    return Sphere<3, T, Q>(m * sphere.pos, length(m[0]) * sphere.r);
  }

  template<typename T, qualifier Q>
  static Sphere<3, T, Q> operator*(const mat<4, 3, T, Q> &m, Sphere<3, T, Q> sphere) {
    const T scale = length(vec<3, T, Q>(m[0].x, m[0].y, m[0].z));
    return Sphere<3, T, Q>(transformPos(m, sphere.pos), scale * sphere.r);
  }

  template<typename T, qualifier Q>
  static Sphere<3, T, Q> operator*(const mat<4, 4, T, Q> &m, Sphere<3, T, Q> sphere) {
    const T scale = length(vec<3, T, Q>(m[0].x, m[0].y, m[0].z));
    return Sphere<3, T, Q>(transformPos(m, sphere.pos), scale * sphere.r);
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Sphere<3, T, Q> operator*(const qua<T, Q> &q, Sphere<3, T, Q> sphere) {
    return Sphere<3, T, Q>(q * sphere.pos, sphere.r);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(Sphere<L, T, Q> const &x, Sphere<L, T, Q> const &y, const T eps = epsilon<T>()) {
    return all_equal(x.pos, y.pos, eps) && equal(x.r, y.r, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(Sphere<L, T, Q> const &x, Sphere<L, T, Q> const &y, vec<L, T, Q> const &eps) {
    return all_equal(x.pos, y.pos, eps) && equal(x.r, y.r, eps[0]);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(Sphere<L, T, Q> const &x, Sphere<L, T, Q> const &y, int MaxULPs) {
    return all_equal(x.pos, y.pos, MaxULPs) && equal(x.r, y.r, MaxULPs);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(Sphere<L, T, Q> const &x, Sphere<L, T, Q> const &y, vec<L, int, Q> const &MaxULPs) {
    return all_equal(x.pos, y.pos, MaxULPs) && equal(x.r, y.r, MaxULPs[0]);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(Sphere<L, T, Q> const &x, Sphere<L, T, Q> const &y, const T eps = epsilon<T>()) {
    return any_notequal(x.pos, y.pos, eps) || notEqual(x.r, y.r, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(Sphere<L, T, Q> const &x, Sphere<L, T, Q> const &y, vec<L, T, Q> const &eps) {
    return any_notequal(x.pos, y.pos, eps) || notEqual(x.r, y.r, eps[0]);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(Sphere<L, T, Q> const &x, Sphere<L, T, Q> const &y, int MaxULPs) {
    return any_notequal(x.pos, y.pos, MaxULPs) || notEqual(x.r, y.r, MaxULPs);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(Sphere<L, T, Q> const &x, Sphere<L, T, Q> const &y, vec<L, int, Q> const &MaxULPs) {
    return any_notequal(x.pos, y.pos, MaxULPs) || notEqual(x.r, y.r, MaxULPs[0]);
  }

  /// <summary>
  /// Return the center of mass of the sphere.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> centroid(const Sphere<L, T, Q> &sphere) {
    return sphere.pos;
  }

  /// <summary>
  /// Return the smallest AABB that encloses the sphere.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER AABB<L, T, Q> maximalContainedAABB(const Sphere<L, T, Q> &sphere) {
    const T halfSideLength = sphere.r * sqrt(T(3));

    AABB<L, T, Q> aabb;
    aabb.setFromCenterAndSize(sphere.pos, vec<L, T, Q>(halfSideLength));
    return aabb;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isinf(const Sphere<L, T, Q> &sphere) {
    return any_isinf(sphere.pos) || isinf(sphere.r);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isnan(const Sphere<L, T, Q> &sphere) {
    return any_isnan(sphere.pos) || isnan(sphere.r);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isfinite(const Sphere<L, T, Q> &sphere) {
    return all(isfinite(sphere.pos)) && isfinite(sphere.r);
  }

  /// <summary>
  /// Test whether the Sphere is degenerate, i.e., not finite or if the radius
  /// is less-or-equal to zero.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isDegenerate(const Sphere<L, T, Q> &sphere) {
    return !(sphere.r > T(0) || !all(isfinite(sphere.pos)));
  }

  /// <summary>
  /// Compute the volume of the sphere.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T volume(const Sphere<L, T, Q> &sphere) {
    return T(4) * pi<T>() * sphere.r * sphere.r * sphere.r / T(3);
  }

  /// <summary>
  /// Compute the surface area of the sphere.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T surfaceArea(const Sphere<L, T, Q> &sphere) {
    return T(4) * pi<T>() * sphere.r * sphere.r;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T area(const Sphere<2, T, Q> &sphere) {
    return pi<T>() * sphere.r * sphere.r;
  }

  /// <summary>
  /// Compute an extreme point along the sphere, i.e., the furthest point in a
  /// given direction.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> extremePoint(const Sphere<L, T, Q> &sphere, const vec<L, T, Q> &direction) {
    const T len = length(direction);
    if (epsilonEqual<T>(len, T(0), epsilon<T>()))
      return sphere.pos;
    return sphere.pos + direction * (sphere.r / len);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> extremePoint(const Sphere<L, T, Q> &sphere, const vec<L, T, Q> &direction, T &projectionDistance) {
    const vec<L, T, Q> point = extremePoint(sphere, direction);
    projectionDistance = dot(point, direction);
    return point;
  }

  /// <summary>
  /// Project the sphere onto the given axis (direction), i.e., collapse the
  /// sphere onto an axis.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER void projectToAxis(const Sphere<L, T, Q> &sphere, const vec<L, T, Q> &direction, T &outMin, T &outMax) {
    const T d = dot(direction, sphere.pos);
    outMin = d - sphere.r;
    outMax = d + sphere.r;
  }

  /// <summary>
  /// Return point on the sphere closest to the given point.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Sphere<L, T, Q> &sphere, const vec<L, T, Q> &point) {
    const T d = distance(sphere.pos, point);
    const T t = (d >= sphere.r ? sphere.r : d);
    return sphere.pos + (point - sphere.pos) * (t / d);
  }

  // Tests if the given object is fully contained within the sphere.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Sphere<L, T, Q> &sphere, const vec<L, T, Q> &point, const T eps = epsilon<T>()) {
    return distance2(sphere.pos, point) <= sphere.r * sphere.r + eps;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Sphere<L, T, Q> &sphere, const LineSegment<L, T, Q> &line) {
    return contains(sphere, line.a) && contains(sphere, line.b);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Sphere<L, T, Q> &a, const Sphere<L, T, Q> &b) {
    return distance2(a.pos, b.pos) + b.r <= a.r;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Sphere<L, T, Q> &a, const Sphere<L, T, Q> &b, const T eps = epsilon<T>()) {
    return distance2(a.pos, b.pos) + b.r - a.r <= eps;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Sphere<L, T, Q> &sphere, const AABB<L, T, Q> &aabb) {
    for (int i = 0; i < 8; ++i) {
      if (!contains(sphere, cornerPoint(aabb, i)))
        return false;
    }
    return true;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Sphere<L, T, Q> &sphere, const Triangle<L, T, Q> &triangle, const T eps = epsilon<T>()) {
    return contains(sphere, triangle.a, eps)
           && contains(sphere, triangle.b, eps)
           && contains(sphere, triangle.c, eps);
  }

  // Computes the distance between the sphere and the given object.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Sphere<L, T, Q> &sphere, const vec<L, T, Q> &point) {
    return max(T(0), distance(sphere.pos, point) - sphere.r);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Sphere<L, T, Q> &sphere, const Sphere<L, T, Q> &other) {
    return max(T(0), distance(sphere.pos, other.pos) - sphere.r - other.r);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Sphere<L, T, Q> &sphere, const AABB<L, T, Q> &aabb) {
    return distance(aabb, sphere);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Sphere<L, T, Q> &sphere, const Ray<L, T, Q> &ray) {
    return distance(ray, sphere);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Sphere<L, T, Q> &sphere, const LineSegment<L, T, Q> &line) {
    return distance(line, sphere);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Sphere<L, T, Q> &sphere, const Line<L, T, Q> &line) {
    return distance(line, sphere);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Sphere<L, T, Q> &sphere, const Plane<L, T, Q> &plane) {
    return distance(plane, sphere);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Sphere<L, T, Q> &sphere, const Triangle<L, T, Q> &triangle) {
    return distance(triangle, sphere);
  }

  /// Tests whether the sphere and the given object intersect.

  /// <summary>
  /// Generic Line/Sphere Intersection
  /// </summary>
  /// <returns>The number of intersection points, 0, 1, or 2.</returns>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER int intersectLine(const Line<L, T, Q> &line, const Sphere<L, T, Q> &sphere, T &d1, T &d2) {
    const vec<L, T, Q> a = line.pos - sphere.pos;
    const T radSq = sphere.r * sphere.r;
    const T C = dot(a, a) - radSq;
    const T B = T(2) * dot(a, line.dir);

    T D = B * B - T(4) * C;
    if (D < T(0)) {  // No intersections.
      d1 = std::numeric_limits<T>::infinity();
      d2 = -std::numeric_limits<T>::infinity();
      return 0;
    }
    else if (D < epsilon<T>()) {  // tangent to sphere
      d1 = d2 = -B * T(0.5);
      return 1;
    }
    else {
      D = sqrt(D);
      d1 = (-B - D) * T(0.5);
      d2 = (-B + D) * T(0.5);
      return 2;
    }
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Sphere<L, T, Q> &sphere, const Sphere<L, T, Q> &other) {
    return distance2(sphere.pos, other.pos) <= ((sphere.r + other.r) * (sphere.r + other.r));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER int intersects(const Sphere<L, T, Q> &sphere, const Line<L, T, Q> &line, T &d1, T &d2) {
    return intersectLine(line, sphere, d1, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER int intersects(const Sphere<L, T, Q> &sphere, const LineSegment<L, T, Q> &line, T &d1, T &d2) {
    const int numIntersections = intersectLine(toLine(line), sphere, d1, d2);
    if (numIntersections == 0)
      return 0;

    const T lineLength = length(line);
    if (d2 < T(0) || d1 > lineLength)
      return 0;

    d1 /= lineLength;
    d2 /= lineLength;
    return numIntersections;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER int intersects(const Sphere<L, T, Q> &sphere, const Ray<L, T, Q> &ray, T &d1, T &d2) {
    int numIntersections = intersectLine(toLine(ray), sphere, d1, d2);
    if (d1 < T(0) && numIntersections == 2)  // behind the ray.
      d1 = d2;
    return (d1 >= T(0)) ? numIntersections : 0;  // otherwise, negative direction of the ray.
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Sphere<L, T, Q> &sphere, const AABB<L, T, Q> &aabb) {
    return intersects(aabb, sphere);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Sphere<L, T, Q> &sphere, const Plane<L, T, Q> &plane) {
    return intersects(plane, sphere);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Sphere<L, T, Q> &sphere, const Triangle<L, T, Q> &triangle) {
    vec<L, T, Q> intersectionPt;
    return intersects(triangle, sphere, intersectionPt);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER int intersects(const Sphere<L, T, Q> &sphere, const Line<L, T, Q> &line) {
    T d1(0), d2(0);
    return intersectLine(line, sphere, d1, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER int intersects(const Sphere<L, T, Q> &sphere, const Ray<L, T, Q> &ray) {
    T d1(0), d2(0);
    return intersects(sphere, ray, d1, d2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER int intersects(const Sphere<L, T, Q> &sphere, const LineSegment<L, T, Q> &line) {
    T d1(0), d2(0);
    return intersects(sphere, line, d1, d2);
  }

  /// Expands this sphere to enclose both the sphere and the given object.

  /// <summary>
  /// Fetch each corner point of the given object: Then enclose points by
  /// furthest distance from sphere to smallest to ensure an optimal enclosure.
  /// </summary>
  template<length_t L, typename T, qualifier Q, typename Object, int NumCorners>
  static Sphere<L, T, Q> _enclose(const Sphere<L, T, Q> &sphere, const Object &obj) {
    struct Tuple {
      int idx;
      vec<L, T, Q> point;
      T distance;
      bool operator<(const Tuple &rhs) const {
        return glm::detail::equal_strict(distance, rhs.distance) ? idx < rhs.idx : distance < rhs.distance;
      }
    };

    Tuple corners[NumCorners];
    for (int i = 0; i < NumCorners; ++i) {
      const vec<L, T, Q> point = cornerPoint(obj, i);
      corners[i] = { i, point, distance2(sphere.pos, point) };
    }

#if GLM_HAS_CXX11_STL
    std::sort(corners, corners + NumCorners);
#else
    qsort(corners, NumCorners, sizeof(Tuple), [](const void *a, const void *b) -> int {
      const Tuple &t_a = *static_cast<const Tuple *>(a);
      const Tuple &t_b = *static_cast<const Tuple *>(b);
      return (t_a < t_b) ? 1 : ((t_b < t_a) ? -1 : (t_a.idx - t_b.idx));
    });
#endif

    Sphere<L, T, Q> result(sphere);
    for (int i = NumCorners - 1; i >= 0; --i)
      result.enclose(corners[i].point);
    return result;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Sphere<L, T, Q> enclose(const Sphere<L, T, Q> &sphere, const vec<L, T, Q> &point, const T eps = epsilon<T>()) {
    Sphere<L, T, Q> result(sphere);
    result.enclose(point, eps);
    return result;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Sphere<L, T, Q> enclose(const Sphere<L, T, Q> &sphere, const LineSegment<L, T, Q> &line) {
    Sphere<L, T, Q> result(sphere);
    if (distance2(sphere.pos, line.a) > distance2(sphere.pos, line.b)) {
      result.enclose(line.a);
      result.enclose(line.b);
    }
    else {
      result.enclose(line.b);
      result.enclose(line.a);
    }
    return result;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Sphere<L, T, Q> enclose(const Sphere<L, T, Q> &sphere, const AABB<L, T, Q> &aabb) {
    return _enclose<L, T, Q, AABB<L, T, Q>, 8>(sphere, aabb);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Sphere<L, T, Q> enclose(const Sphere<L, T, Q> &sphere, const Triangle<L, T, Q> &triangle) {
    return _enclose<L, T, Q, Triangle<L, T, Q>, 3>(sphere, triangle);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Sphere<L, T, Q> enclose(const Sphere<L, T, Q> &sphere, const Sphere<L, T, Q> &other) {
    const vec<L, T, Q> furthestPoint = scaleLength((other.pos - sphere.pos), other.r);
    Sphere<L, T, Q> result(sphere);
    result.enclose(other.pos + furthestPoint);
    result.enclose(other.pos - furthestPoint);
    return result;
  }

  /// <summary>
  /// Expand the radius of the sphere until it encloses the given point.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Sphere<L, T, Q> extendRadiusToContain(const Sphere<L, T, Q> &sphere, const vec<L, T, Q> &point, const T eps = epsilon<T>()) {
    const T requiredRadius = distance(sphere.pos, point) + eps;
    return Sphere<L, T, Q>(sphere.pos, max(sphere.r, requiredRadius));
  }

  /// <summary>
  /// Expand the radius of the sphere until it encloses the given sphere.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Sphere<L, T, Q> extendRadiusToContain(const Sphere<L, T, Q> &sphere, const Sphere<L, T, Q> &other, const T eps = epsilon<T>()) {
    const T requiredRadius = distance(sphere.pos, other.pos) + other.r + eps;
    return Sphere<L, T, Q>(sphere.pos, max(sphere.r, requiredRadius));
  }

  /// <summary>
  /// Compute the coordinates of the smallest radius sphere that passes through
  /// the three points: (0,0,0), ab, and ac.
  /// </summary>
  /// <param name="s">Barycentric S-coordinate of the sphere center</param>
  /// <param name="t">Barycentric T-coordinate of the sphere center</param>
  /// <returns>
  /// True if a sphere can be fit through the three points. Otherwise, the
  /// points are collinear and false is returned.
  /// </returns>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool fitSphereThroughPoints(const vec<3, T, Q> &ab, const vec<3, T, Q> &ac, T &s, T &t) {
    const T bb = dot(ab, ab);
    const T cc = dot(ac, ac);
    const T bc = dot(ab, ac);

    T denom = bb * cc - bc * bc;
    if (!epsilonEqual(denom, T(0), epsilon<T>())) {
      denom = T(0.5) / denom;
      s = (cc * bb - bc * cc) * denom;
      t = (cc * bb - bc * bb) * denom;
      return true;
    }
    return false;
  }

  /// <summary>
  /// Compute the coordinates of the smallest radius sphere that passes through
  /// the four points: (0,0,0), ab, ac, and ad..
  /// </summary>
  /// <param name="s">Barycentric S-coordinate of the sphere center</param>
  /// <param name="t">Barycentric T-coordinate of the sphere center</param>
  /// <param name="u">Barycentric U-coordinate of the sphere center</param>
  /// <returns>
  /// True if a sphere can be fit through the three points. Otherwise, the
  /// points are collinear and false is returned.
  /// </returns>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE bool fitSphereThroughPoints(const vec<3, T, Q> &ab, const vec<3, T, Q> &ac, const vec<3, T, Q> &ad, T &s, T &t, T &u) {
    const T bb = dot(ab, ab);
    const T bc = dot(ab, ac);
    const T bd = dot(ab, ad);
    const T cc = dot(ac, ac);
    const T cd = dot(ac, ad);
    const T dd = dot(ad, ad);

    const mat<3, 3, T, Q> ms(bb, bc, bd, bc, cc, cd, bd, cd, dd);
    if (invertible(ms)) {
      const mat<3, 3, T, Q> m = inverse(ms);
      const vec<3, T, Q> v = m * vec<3, T, Q>(bb * T(0.5), cc * T(0.5), dd * T(0.5));
      s = v.x;
      t = v.y;
      u = v.z;
      return true;
    }
    return false;
  }

  /// <summary>
  /// Fit a sphere through the two given points.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Sphere<3, T, Q> fitThroughPoints(const vec<3, T, Q> &a, const vec<3, T, Q> &b) {
    return optimalEnclosingSphere(a, b);
  }

  /// <summary>
  /// Fit a sphere through the three given points. This function computes the
  /// sphere goes through the three points that has a minimized volume.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Sphere<3, T, Q> fitThroughPoints(const vec<3, T, Q> &a, const vec<3, T, Q> &b, const vec<3, T, Q> &c) {
    Sphere<3, T, Q> sphere;
    const vec<3, T, Q> ab = b - a;
    const vec<3, T, Q> ac = c - a;

    T s(0), t(0);
    if (fitSphereThroughPoints(ab, ac, s, t)) {
      const vec<3, T, Q> center = s * ab + t * ac;
      sphere.r = length(center);
      sphere.pos = a + center;
    }
    else {
      sphere.setDegenerate();
    }
    return sphere;
  }

  /// <summary>
  /// Fit a sphere through the four given points (note that four non-coplanar
  /// points can uniquely define a sphere in three dimensions)).
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Sphere<3, T, Q> fitThroughPoints(const vec<3, T, Q> &a, const vec<3, T, Q> &b, const vec<3, T, Q> &c, const vec<3, T, Q> &d) {
    Sphere<3, T, Q> sphere;

    T s(0), t(0), u(0);
    const vec<3, T, Q> ab = b - a;
    const vec<3, T, Q> ac = c - a;
    const vec<3, T, Q> ad = d - a;
    if (fitSphereThroughPoints(ab, ac, ad, s, t, u)) {
      const vec<3, T, Q> center = s * ab + t * ac + u * ad;
      sphere.r = length(center);
      sphere.pos = a + center;
    }
    else {
      sphere.setDegenerate();  // coplanar
    }

    return sphere;
  }

  /// <summary>
  /// Compute the minimal bounding sphere for two points, i.e., the smallest
  /// volume sphere that contains the provided points.
  /// <param name="eps">Expand radius by a small eps for numerical stability</param>
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE Sphere<3, T, Q> optimalEnclosingSphere(const vec<3, T, Q> &a, const vec<3, T, Q> &b, const T eps = epsilon<T>()) {
    const vec<3, T, Q> pos = (a + b) * T(0.5);
    if (all(isfinite(pos)))
      return Sphere<3, T, Q>(pos, length(b - pos) + eps);
    return Sphere<3, T, Q>(vec<3, T, Q>(T(0)), T(0));
  }

  /// <summary>
  /// Compute the minimal bounding sphere for three points, i.e., the smallest
  /// volume sphere that contains the provided points.
  /// <param name="eps">Expand radius by a small eps for numerical stability</param>
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE Sphere<3, T, Q> optimalEnclosingSphere(const vec<3, T, Q> &a, const vec<3, T, Q> &b, const vec<3, T, Q> &c, const T eps = epsilon<T>()) {
    Sphere<3, T, Q> sphere;
    const vec<3, T, Q> ab = b - a;
    const vec<3, T, Q> ac = c - a;

    T s(0), t(0);  // Ensure points are not collinear.
    const bool success = !areCollinear(ab, ac, epsilon<T>()) && fitSphereThroughPoints(ab, ac, s, t);

    // Box fitting when the points are either collinear or sufficiently far
    // enough away from the <a, b, c> triangle
    if (!success || abs(s) > T(10000) || abs(t) > T(10000)) {
      const vec<3, T, Q> minPt = min(a, min(b, c));
      const vec<3, T, Q> maxPt = max(a, max(b, c));
      sphere.pos = (minPt + maxPt) * T(0.5);
      sphere.r = distance(sphere.pos, minPt);
    }
    else if (s < T(0)) {
      sphere.pos = (a + c) * T(0.5);
      sphere.r = max(distance(a, c) * T(0.5), distance(b, sphere.pos));
    }
    else if (t < T(0)) {
      sphere.pos = (a + b) * T(0.5);
      sphere.r = max(distance(a, b) * T(0.5), distance(c, sphere.pos));
    }
    else if (s + t > T(1)) {
      sphere.pos = (b + c) * T(0.5);
      sphere.r = max(distance(b, c) * T(0.5), distance(a, sphere.pos));
    }
    else {
      sphere.pos = a + s * ab + t * ac;
      sphere.r = sqrt(max(distance2(sphere.pos, a), max(distance2(sphere.pos, b), distance2(sphere.pos, c))));
    }

    sphere.r += T(2) * eps;
    return sphere;
  }

  /// <summary>
  /// Compute the minimal bounding sphere for four points, i.e., the smallest
  /// volume sphere that contains the provided points.
  /// <param name="eps">Expand radius by a small eps for numerical stability</param>
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE Sphere<3, T, Q> optimalEnclosingSphere(const vec<3, T, Q> &a, const vec<3, T, Q> &b, const vec<3, T, Q> &c, const vec<3, T, Q> &d, const T eps = epsilon<T>()) {
    T s(0), t(0), u(0);
    Sphere<3, T, Q> sphere;

    const vec<3, T, Q> ab = b - a;
    const vec<3, T, Q> ac = c - a;
    const vec<3, T, Q> ad = d - a;

    bool success = fitSphereThroughPoints(ab, ac, ad, s, t, u);
    if (!success || s < T(0) || t < T(0) || u < T(0) || s + t + u > T(1)) {
      sphere = optimalEnclosingSphere(a, b, c, eps);
      if (!contains(sphere, d)) {
        sphere = optimalEnclosingSphere(a, b, d, eps);
        if (!contains(sphere, c)) {
          sphere = optimalEnclosingSphere(a, c, d, eps);
          if (!contains(sphere, b)) {
            sphere = optimalEnclosingSphere(b, c, d, eps);
            sphere.r = max(sphere.r, distance(a, sphere.pos) + eps);
          }
        }
      }
    }
    else {
      sphere.pos = a + s * ab + t * ac + u * ad;
      sphere.r = sqrt(max(
      distance2(sphere.pos, a),
      max(distance2(sphere.pos, b), max(distance2(sphere.pos, c), distance2(sphere.pos, d)))));
    }

    sphere.r += T(2) * eps;
    return sphere;
  }

  /// <summary>
  /// Compute the minimal bounding sphere for five points, i.e., the smallest
  /// volume sphere that contains the provided points.
  ///
  /// Note, a minimal enclosing sphere can be defined by four points (or fewer).
  /// Therefore, at least one of the points is considered redundant (or
  /// unnecessary).
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE Sphere<3, T, Q> optimalEnclosingSphere(const vec<3, T, Q> &a, const vec<3, T, Q> &b, const vec<3, T, Q> &c, const vec<3, T, Q> &d, const vec<3, T, Q> &e, int &redundantPoint, const T eps = epsilon<T>()) {
    Sphere<3, T, Q> s = optimalEnclosingSphere(b, c, d, e, eps);
    if (contains(s, a, eps)) {
      redundantPoint = 0;
      return s;
    }

    s = optimalEnclosingSphere(a, c, d, e, eps);
    if (contains(s, b, eps)) {
      redundantPoint = 1;
      return s;
    }

    s = optimalEnclosingSphere(a, b, d, e, eps);
    if (contains(s, c, eps)) {
      redundantPoint = 2;
      return s;
    }

    s = optimalEnclosingSphere(a, b, c, e, eps);
    if (contains(s, d, eps)) {
      redundantPoint = 3;
      return s;
    }

    s = optimalEnclosingSphere(a, b, c, d, eps);
    redundantPoint = 4;
    return s;
  }

  template<typename T, qualifier Q, class Vector>
  GLM_GEOM_QUALIFIER_NOINLINE Sphere<3, T, Q> optimalEnclosingSphere(const Vector &pts, const T eps = epsilon<T>()) {
    switch (pts.size()) {
      case 0: return Sphere<3, T, Q>();
      case 1: return Sphere<3, T, Q>(pts[0], T(0));
      case 2: return optimalEnclosingSphere<T, Q>(pts[0], pts[1], eps);
      case 3: return optimalEnclosingSphere<T, Q>(pts[0], pts[1], pts[2], eps);
      case 4: return optimalEnclosingSphere<T, Q>(pts[0], pts[1], pts[2], pts[3], eps);
      default: {
        break;
      }
    }

    // The set of supporting points for the minimal sphere.
    typename Vector::size_type sp[4] = { 0, 1, 2, 3 };
    bool expendable[4] = { true, true, true, true };

    Sphere<3, T, Q> s = optimalEnclosingSphere(pts[sp[0]], pts[sp[1]], pts[sp[2]], pts[sp[3]], eps);
    T rSq = s.r * s.r + eps;
    for (typename Vector::size_type i = 4; i < pts.size(); ++i) {
      if (i == sp[0] || i == sp[1] || i == sp[2] || i == sp[3])
        continue;

      // If the next point does not fit inside the currently computed minimal
      // sphere, compute a new minimal sphere that also contains pts[i].
      if (distance2(pts[i], s.pos) > rSq) {
        int redundant = 0;
        s = optimalEnclosingSphere(pts[sp[0]], pts[sp[1]], pts[sp[2]], pts[sp[3]], pts[i], redundant, eps);
        rSq = s.r * s.r + eps;

        // A sphere is uniquely defined by four points, so one of the five
        // points passed in above is now redundant, and can be removed from the
        // support set.
        if (redundant != 4 && (sp[redundant] < i || expendable[redundant])) {
          sp[redundant] = i;
          expendable[redundant] = false;
          if (sp[0] < i) expendable[0] = true;
          if (sp[1] < i) expendable[1] = true;
          if (sp[2] < i) expendable[2] = true;
          if (sp[3] < i) expendable[3] = true;

          i = 0;
        }
      }
    }

    return s;
  }

  namespace detail {
#if GLM_GEOM_TOSTRING
    template<glm::length_t L, typename T, qualifier Q>
    struct compute_to_string<Sphere<L, T, Q>> {
      GLM_GEOM_QUALIFIER std::string call(const Sphere<L, T, Q> &sphere) {
        char const *LiteralStr = literal<T, std::numeric_limits<T>::is_iec559>::value();
        std::string FormatStr(detail::format("sphere(%%s, %s)", LiteralStr));

        return detail::format(FormatStr.c_str(), glm::to_string(sphere.pos).c_str(),
          static_cast<typename cast<T>::value_type>(sphere.r)
        );
      }
    };
#endif
  }
}

#endif
