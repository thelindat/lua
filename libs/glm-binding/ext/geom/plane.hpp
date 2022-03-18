/// <summary>
/// See Copyright Notice in setup.hpp
/// </summary>
#ifndef EXT_GEOM_PLANE_HPP
#define EXT_GEOM_PLANE_HPP

#include "setup.hpp"
#include "line.hpp"
#include "linesegment.hpp"
#include "ray.hpp"
#include "triangle.hpp"

#if GLM_MESSAGES == GLM_ENABLE && !defined(GLM_EXT_INCLUDED)
  #pragma message("GLM: GLM_EXT_GEOM_plane extension included")
#endif

namespace glm {
  /// <summary>
  /// An affine (N -1) dimensional subspace of a N dimensional space.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  struct Plane {

    // -- Implementation detail --

    typedef T value_type;
    typedef Plane<L, T, Q> type;
    typedef vec<L, T, Q> point_type;

    // -- Data --

    point_type normal;  // The direction this plane is facing at.
    T d;  // The offset of this plane from the origin.

#if GLM_CONFIG_DEFAULTED_DEFAULT_CTOR == GLM_ENABLE
    Plane() GLM_DEFAULT_CTOR;
#else
    Plane()
  #if GLM_CONFIG_CTOR_INIT != GLM_CTOR_INIT_DISABLE
      : normal(T(0)), d(T(0))
  #endif
    {
    }
#endif

    Plane(T scalar)
      : normal(scalar), d(scalar) {
    }

    Plane(const point_type &direction, T offset)
      : normal(direction), d(offset) {
      GLM_GEOM_ASSERT(isNormalized(normal, epsilon<T>()));
    }

    Plane(const point_type &point, const point_type &normal_)
      : normal(normal_), d(dot(point, normal)) {
      GLM_GEOM_ASSERT(isNormalized(normal, epsilon<T>()));
    }

    Plane(const Plane<L, T, Q> &plane)
      : normal(plane.normal), d(plane.d) {
      GLM_GEOM_ASSERT(isNormalized(normal, epsilon<T>()));
    }

    Plane<L, T, Q> &operator=(const Plane<L, T, Q> &plane) {
      normal = plane.normal;
      d = plane.d;
      return *this;
    }
  };

  template<length_t L, typename T, qualifier Q>
  static Plane<L, T, Q> operator-(const Plane<L, T, Q> &plane) {
    return Plane<L, T, Q>(-plane.normal, plane.d);
  }

  template<length_t L, typename T, qualifier Q>
  static bool operator==(const Plane<L, T, Q> &p1, const Plane<L, T, Q> &p2) {
    return p1.normal == p2.normal && detail::equal_strict(p1.d, p2.d);
  }

  template<length_t L, typename T, qualifier Q>
  static Plane<L, T, Q> operator+(const Plane<L, T, Q> &plane, const vec<L, T, Q> &offset) {
    return Plane<L, T, Q>(plane.normal, plane.d - dot(plane.normal, offset));
  }

  template<length_t L, typename T, qualifier Q>
  static Plane<L, T, Q> operator-(const Plane<L, T, Q> &plane, const vec<L, T, Q> &offset) {
    return Plane<L, T, Q>(plane.normal, plane.d + dot(plane.normal, offset));
  }

  template<typename T, qualifier Q>
  static Plane<3, T, Q> operator*(const mat<3, 3, T, Q> &m, const Plane<3, T, Q> &plane) {
    const mat<3, 3, T, Q> r = inverseTranspose(m);
    return Plane<3, T, Q>(plane.normal * r, plane.d);
  }

  template<typename T, qualifier Q>
  static Plane<3, T, Q> operator*(const mat<3, 4, T, Q> &m, const Plane<3, T, Q> &plane) {
    const mat<3, 3, T, Q> r(inverse(mat<3, 3, T, Q>(m)));
    return Plane<3, T, Q>(plane.normal * r, plane.d);
  }

  template<typename T, qualifier Q>
  static Plane<3, T, Q> operator*(const mat<4, 3, T, Q> &m, const Plane<3, T, Q> &plane) {
    const mat<3, 3, T, Q> r(inverse(mat<3, 3, T, Q>(m)));
    return Plane<3, T, Q>(plane.normal * r, plane.d + dot(plane.normal, r * m[3]));
  }

  template<typename T, qualifier Q>
  static Plane<3, T, Q> operator*(const mat<4, 4, T, Q> &m, const Plane<3, T, Q> &plane) {
    const mat<3, 3, T, Q> r(inverse(mat<3, 3, T, Q>(m)));
    return Plane<3, T, Q>(plane.normal * r, plane.d + dot(plane.normal, r * m[3]));
  }

  template<typename T, qualifier Q>
  static Plane<3, T, Q> operator*(const qua<T, Q> &q, const Plane<3, T, Q> &plane) {
    return operator*(toMat3(q), plane);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(Plane<L, T, Q> const &x, Plane<L, T, Q> const &y, const T eps = epsilon<T>()) {
    return all_equal(x.normal, y.normal, eps) && equal(x.d, y.d, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(Plane<L, T, Q> const &x, Plane<L, T, Q> const &y, vec<L, T, Q> const &eps) {
    return all_equal(x.normal, y.normal, eps) && equal(x.d, y.d, eps[0]);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(Plane<L, T, Q> const &x, Plane<L, T, Q> const &y, int MaxULPs) {
    return all_equal(x.normal, y.normal, MaxULPs) && equal(x.d, y.d, MaxULPs);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(Plane<L, T, Q> const &x, Plane<L, T, Q> const &y, vec<L, int, Q> const &MaxULPs) {
    return all_equal(x.normal, y.normal, MaxULPs) && equal(x.d, y.d, MaxULPs[0]);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(Plane<L, T, Q> const &x, Plane<L, T, Q> const &y, const T eps = epsilon<T>()) {
    return any_notequal(x.normal, y.normal, eps) || notEqual(x.d, y.d, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(Plane<L, T, Q> const &x, Plane<L, T, Q> const &y, vec<L, T, Q> const &eps) {
    return any_notequal(x.normal, y.normal, eps) || notEqual(x.d, y.d, eps[0]);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(Plane<L, T, Q> const &x, Plane<L, T, Q> const &y, int MaxULPs) {
    return any_notequal(x.normal, y.normal, MaxULPs) || notEqual(x.d, y.d, MaxULPs);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(Plane<L, T, Q> const &x, Plane<L, T, Q> const &y, vec<L, int, Q> const &MaxULPs) {
    return any_notequal(x.normal, y.normal, MaxULPs) || notEqual(x.d, y.d, MaxULPs[0]);
  }

  /// <summary>
  /// Construct a plane by specifying a ray that lies along the plane and its normal
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Plane<L, T, Q> planeFrom(const Ray<L, T, Q> &ray, const vec<L, T, Q> &normal) {
    const vec<L, T, Q> perpNormal = normal - proj(normal, ray.dir);
    return Plane<L, T, Q>(ray.pos, normalize(perpNormal));
  }

  /// <summary>
  /// Construct a plane by specifying a line that lies along the plane and its normal
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Plane<L, T, Q> planeFrom(const Line<L, T, Q> &line, const vec<L, T, Q> &normal) {
    const vec<L, T, Q> perpNormal = normal - proj(normal, line.dir);
    return Plane<L, T, Q>(line.pos, normalize(perpNormal));
  }

  /// <summary>
  /// Construct a plane by specifying a segment that lies along the plane and its normal
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Plane<L, T, Q> planeFrom(const LineSegment<L, T, Q> &line, const vec<L, T, Q> &normal) {
    const vec<L, T, Q> perpNormal = normal - proj(normal, line.b - line.a);
    return Plane<L, T, Q>(line.a, normalize(perpNormal));
  }

  /// <summary>
  /// Construct a plane by specifying a point on the plane and its normal.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Plane<3, T, Q> planeFrom(const vec<L, T, Q> &point, const vec<L, T, Q> &normal) {
    return Plane<L, T, Q>(point, normal);
  }

  /// <summary>
  /// Construct a plane by specifying three points on the plane.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Plane<3, T, Q> planeFrom(const vec<3, T, Q> &v1, const vec<3, T, Q> &v2, const vec<3, T, Q> &v3) {
    vec<3, T, Q> normal = cross(v2 - v1, v3 - v1);
    const T len = length(normal);
    if (len > epsilon<T>()) {
      normal /= len;
      return Plane<3, T, Q>(normal, dot(normal, v1));
    }
    else {
      return Plane<3, T, Q>(vec<3, T, Q>{ T(0), T(0), T(1) }, T(0));
    }
  }

  /// <summary>
  /// Tests if any component of the plane is infinite.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isinf(const Plane<L, T, Q> &plane) {
    return any_isinf(plane.normal) || any_isinf(plane.d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isnan(const Plane<L, T, Q> &plane) {
    return any_isnan(plane.normal) || any_isnan(plane.d);
  }

  /// <summary>
  /// Test if all components of the plane are finite.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isfinite(const Plane<L, T, Q> &plane) {
    return all(isfinite(plane.normal)) && all(isfinite(plane.d));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isDegenerate(const Plane<L, T, Q> &plane) {
    return !all(isfinite(plane.normal)) || isNull(plane.normal, epsilon<T>()) || !isfinite(plane.d);
  }

  /// <summary>
  /// Return true if two planes are parallel.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isParallel(const Plane<L, T, Q> &plane, const Plane<L, T, Q> &other, const T eps = epsilon<T>()) {
    return all(epsilonEqual(plane.normal, other.normal, eps));
  }

  /// <summary>
  /// Return true if the plane contains/passes-through the origin (i.e., T(0)).
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool passesThroughOrigin(const Plane<L, T, Q> &plane, const T eps = epsilon<T>()) {
    return abs(plane.d) <= eps;
  }

  /// <summary>
  /// Compute the angle (radians) of intersection between two planes.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T angle(const Plane<L, T, Q> &plane, const Plane<L, T, Q> &other) {
    return dot(plane.normal, other.normal);
  }

  /// <summary>
  /// Reverse the direction of the plane normal, while still representing the
  /// same set of points.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Plane<L, T, Q> reverseNormal(const Plane<L, T, Q> &plane) {
    return Plane<L, T, Q>(-plane.normal, -plane.d);
  }

  /// <summary>
  /// Returns a point on this plane.
  ///
  /// The returned point has the property that the line passing through "it" and
  /// (0,0,0) is perpendicular to this plane.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> pointOnPlane(const Plane<L, T, Q> &plane) {
    return plane.normal * plane.d;
  }

  /// <summary>
  /// Return a point on the plane at the given parameterized (u, v) coordinates.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<3, T, Q> point(const Plane<3, T, Q> &plane, T u, T v) {
    vec<3, T, Q> b1, b2;
    perpendicularBasis(plane.normal, b1, b2);
    return pointOnPlane(plane) + u * b1 + v * b2;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<3, T, Q> point(const Plane<3, T, Q> &plane, T u, T v, const vec<3, T, Q> &referenceOrigin) {
    vec<3, T, Q> b1, b2;
    perpendicularBasis(plane.normal, b1, b2);
    return project(plane, referenceOrigin) + u * b1 + v * b2;
  }

  /// <summary>
  /// Refract the given incident vector along the plane.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> refract(const Plane<L, T, Q> &plane, const vec<L, T, Q> &vec, T eta) {
    return refract(vec, plane.normal, eta);
  }

  /// <summary>
  /// Refract the given incident vector along the plane.
  /// </summary>
  /// <param name="negativeSideRefractionIndex">Refraction index of material exiting</param>
  /// <param name="positiveSideRefractionIndex">Refraction index of material entering</param>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> refract(const Plane<L, T, Q> &plane, const vec<L, T, Q> &vec, T negativeSideRefractionIndex, T positiveSideRefractionIndex) {
    return refract(vec, plane.normal, negativeSideRefractionIndex, positiveSideRefractionIndex);
  }

  /// <summary>
  /// In-place clipping operation.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool clip(const Plane<L, T, Q> &plane, vec<L, T, Q> &a, vec<L, T, Q> &b) {
    T t(0);
    bool intersects = intersectLinePlane(plane.normal, plane.d, a, b - a, t);
    if (!intersects || t <= T(0) || t >= T(1))
      return signedDistance(plane, a) > T(0);  // Within the positive/negative halfspace

    const vec<L, T, Q> pt = a + (b - a) * t;  // Point of intersection
    if (isOnPositiveSide(plane, a))
      b = pt;
    else
      a = pt;
    return true;
  }

  /// <summary>
  /// Clips a line segment against the plane, i.e., remove part of the line that
  /// lies in the negative halfspace of the plane.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER LineSegment<L, T, Q> clip(const Plane<L, T, Q> &plane, const LineSegment<L, T, Q> &line) {
    LineSegment<L, T, Q> result(line);
    return clip(plane, result.a, result.b) ? result : line;
  }

  /// <summary>
  /// Clips a line against the plane, i.e., remove part of the line that lies in
  /// the negative halfspace of the plane.
  /// </summary>
  /// <returns>
  ///   0 - If clipping removes the entire line (the line lies entirely in the negative halfspace).
  ///   1 - If clipping results in a ray (clipped at the point of intersection).
  ///   2 - If clipping keeps the entire line (the line lies entirely in the positive halfspace).
  /// </returns>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER int clip(const Plane<L, T, Q> &plane, const Line<L, T, Q> &line, Ray<L, T, Q> &outRay) {
    T t(0);
    if (!intersectLinePlane(plane.normal, plane.d, line.pos, line.dir, t)) {
      outRay.pos = line.pos;
      outRay.dir = line.dir;
      return signedDistance(plane, line.pos) <= T(0) ? 0 : 2;  // Completely within the positive/negative halfspace
    }

    outRay.pos = line.pos + line.dir * t;
    if (dot(line.dir, plane.normal) >= T(0))
      outRay.dir = line.dir;
    else
      outRay.dir = -line.dir;
    return 1;
  }

  /// <summary>
  /// @TODO: Clip a polygon against a plane, i.e., remove part(s) of the polygon
  /// that lie in the negative halfspace of the plane and returning a new
  /// polygon.
  /// </summary>
  // template<length_t L, typename T, qualifier Q>
  // GLM_GEOM_QUALIFIER Polygon<L, T, Q> clip(const Plane<L, T, Q> &plane, const Polygon<L, T, Q> &polygon) {
  // }

  /// <summary>
  /// @NOTE: "t1" or "t2" cannot be references to "triangle".
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER int clip(const Plane<L, T, Q> &plane, const Triangle<L, T, Q> &triangle, Triangle<L, T, Q> &t1, Triangle<L, T, Q> &t2) {
    const bool aSide = isOnPositiveSide(plane, triangle.a);
    const bool bSide = isOnPositiveSide(plane, triangle.b);
    const bool cSide = isOnPositiveSide(plane, triangle.c);
    switch ((aSide ? 1 : 0) + (bSide ? 1 : 0) + (cSide ? 1 : 0)) {
      case 1: {
        if (bSide) {
          t1.a = triangle.b;
          t1.b = triangle.c;
          t1.c = triangle.a;
        }
        else if (cSide) {
          t1.a = triangle.c;
          t1.c = triangle.b;
          t1.b = triangle.a;
        }
        else {
          t1 = triangle;
        }

        T t, r;
        intersects(plane, LineSegment<L, T, Q>(t1.a, t1.b), t);
        intersects(plane, LineSegment<L, T, Q>(t1.a, t1.c), r);

        t1.b = t1.a + (t1.b - t1.a) * t;
        t1.c = t1.a + (t1.c - t1.a) * r;
        return 1;
      }
      case 2: {
        if (!bSide) {
          t1.a = triangle.b;
          t1.b = triangle.c;
          t1.c = triangle.a;
        }
        else if (!cSide) {
          t1.a = triangle.c;
          t1.c = triangle.b;
          t1.b = triangle.a;
        }
        else {
          t1 = triangle;
        }

        T t, r;
        intersects(plane, LineSegment<L, T, Q>(t1.a, t1.b), t);
        intersects(plane, LineSegment<L, T, Q>(t1.a, t1.c), r);

        t2.a = t1.c;
        t2.b = t1.a + (t1.c - t1.a) * r;
        t2.c = t1.a + (t1.b - t1.a) * t;
        t1.a = t2.c;
        return 2;
      }
      case 3: {  // All vertices are on the positive side.
        t1 = triangle;
        return 1;
      }
      default: {
        break;
      }
    }
    return 0;
  }

  /// Orthographically projects the given object onto the plane.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> project(const Plane<L, T, Q> &plane, const vec<L, T, Q> &point) {
    return (point - (dot(plane.normal, point) - plane.d) * plane.normal);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER LineSegment<L, T, Q> project(const Plane<L, T, Q> &plane, const LineSegment<L, T, Q> &line) {
    return LineSegment<L, T, Q>(project(plane, line.a), project(plane, line.b));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Line<L, T, Q> project(const Plane<L, T, Q> &plane, const Line<L, T, Q> &line, bool *nonDegenerate = GLM_NULLPTR) {
    Line<L, T, Q> l;
    l.pos = project(plane, line.pos);
    l.dir = normalize(line.dir - proj(line.dir, plane.normal));
    if (nonDegenerate)
      *nonDegenerate = (length(l.dir) > T(0));
    return l;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Line<L, T, Q> project(const Plane<L, T, Q> &plane, const Ray<L, T, Q> &ray, bool *nonDegenerate = GLM_NULLPTR) {
    Line<L, T, Q> l;
    l.pos = project(plane, ray.pos);
    l.dir = normalize(ray.dir - proj(ray.dir, plane.normal));
    if (nonDegenerate)
      *nonDegenerate = (length(l.dir) > T(0));
    return l;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Triangle<L, T, Q> project(const Plane<L, T, Q> &plane, const Triangle<L, T, Q> &triangle) {
    return Triangle<L, T, Q>(project(plane, triangle.a), project(plane, triangle.b), project(plane, triangle.c));
  }

  /// <summary>
  /// Projects the given point to the negative halfspace of the plane
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> projectToNegativeHalf(const Plane<L, T, Q> &plane, const vec<L, T, Q> &point) {
    return point - max(T(0), (dot(plane.normal, point) - plane.d)) * plane.normal;
  }

  /// <summary>
  /// Projects the given point to the positive halfspace of the plane
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> projectToPositiveHalf(const Plane<L, T, Q> &plane, const vec<L, T, Q> &point) {
    return point - min(T(0), (dot(plane.normal, point) - plane.d)) * plane.normal;
  }

  // Computes the distance between the plane and the given object.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T signedDistance(const Plane<L, T, Q> &plane, const vec<L, T, Q> &point) {
    return dot(plane.normal, point) - plane.d;
  }

  template<length_t L, typename T, qualifier Q, typename Object>
  GLM_GEOM_QUALIFIER T signedDistance(const Plane<L, T, Q> &plane, const Object &object) {
    T pMin(0), pMax(0);
    projectToAxis(object, plane.normal, pMin, pMax);
    pMin -= plane.d;
    pMax -= plane.d;
    if (pMin * pMax <= T(0))
      return T(0);
    return abs(pMin) < abs(pMax) ? pMin : pMax;
  }

  /// <summary>
  /// Return true if two points are on the same side of this plane.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool areOnSameSide(const Plane<L, T, Q> &plane, const vec<L, T, Q> &p1, const vec<L, T, Q> &p2) {
    return signedDistance(plane, p1) * signedDistance(plane, p2) >= T(0);
  }

  /// <summary>
  /// Tests if the given direction vector points towards the positive side of
  /// this plane.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isInPositiveDirection(const Plane<L, T, Q> &plane, const vec<L, T, Q> &directionVector) {
    return dot(plane.normal, directionVector) >= T(0);
  }

  /// <summary>
  /// Tests if the given point lies on the positive side of this plane.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isOnPositiveSide(const Plane<L, T, Q> &plane, const vec<L, T, Q> &point) {
    return signedDistance(plane, point) >= T(0);
  }

  /// <summary>
  /// Triangle/Plane intersection test. Returning:
  ///   1 - If the triangle is completely in the positive half-space of the plane;
  ///  -1 - If the triangle is completely in the negative half-space of the plane;
  ///   0 - If the triangle intersects the plane.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER int examineSide(const Plane<L, T, Q> &plane, const Triangle<L, T, Q> &triangle, const T eps = epsilon<T>()) {
    const T a = signedDistance(plane, triangle.a);
    const T b = signedDistance(plane, triangle.b);
    const T c = signedDistance(plane, triangle.c);
    if (a >= -eps && b >= -eps && c >= -eps)
      return 1;
    if (a <= eps && b <= eps && c <= eps)
      return -1;
    return 0;
  }

  /// Computes the distance between the plane and the given object(s).

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Plane<L, T, Q> &plane, const vec<L, T, Q> &point) {
    return abs(signedDistance(plane, point));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Plane<L, T, Q> &plane, const LineSegment<L, T, Q> &line) {
    return distance(line, plane);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const Plane<L, T, Q> &plane, const Sphere<L, T, Q> &sphere) {
    return max(T(0), distance(plane, sphere.pos) - sphere.r);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T signedDistance(const Plane<L, T, Q> &plane, const AABB<L, T, Q> &aabb) {
    return signedDistance<L, T, Q, AABB<L, T, Q>>(plane, aabb);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T signedDistance(const Plane<L, T, Q> &plane, const Line<L, T, Q> &line) {
    return signedDistance<L, T, Q, Line<L, T, Q>>(plane, line);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T signedDistance(const Plane<L, T, Q> &plane, const LineSegment<L, T, Q> &lineSegment) {
    return signedDistance<L, T, Q, LineSegment<L, T, Q>>(plane, lineSegment);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T signedDistance(const Plane<L, T, Q> &plane, const Ray<L, T, Q> &ray) {
    return signedDistance<L, T, Q, Ray<L, T, Q>>(plane, ray);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T signedDistance(const Plane<L, T, Q> &plane, const Sphere<L, T, Q> &sphere) {
    return signedDistance<L, T, Q, Sphere<L, T, Q>>(plane, sphere);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T signedDistance(const Plane<L, T, Q> &plane, const Triangle<L, T, Q> &triangle) {
    return signedDistance<L, T, Q, Triangle<L, T, Q>>(plane, triangle);
  }

  /// <summary>
  /// Return an affine transformation matrix that projects orthographically onto
  /// the plane
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER mat<4, 3, T, Q> orthoProjection(const Plane<3, T, Q> &plane) {
    return orthoProjection<4, 3, T, Q>(plane.normal, plane.d);
  }

  /// <summary>
  /// Mirrors the given point with respect to the plane.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> mirror(const Plane<L, T, Q> &plane, const vec<L, T, Q> &point) {
    return point - T(2) * (dot(point, plane.normal) - plane.d) * plane.normal;
  }

  /// <summary>
  /// Returns a transformation matrix that mirrors objects along the plane.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER mat<4, 3, T, Q> mirrorMatrix(const Plane<3, T, Q> &plane) {
    return planeMirror<4, 3, T, Q>(plane.normal, plane.d);
  }

  // Computes the closest point on this plane to the given object.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Plane<L, T, Q> &plane, const vec<L, T, Q> &point) {
    return project(plane, point);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Plane<L, T, Q> &plane, const Ray<L, T, Q> &ray) {
    const T denom = dot(plane.normal, ray.dir);
    if (epsilonEqual(denom, T(0), epsilon<T>()))
      return project(plane, ray.pos);

    const T t = (plane.d - dot(plane.normal, ray.pos)) / denom;
    if (t >= T(0))
      return getPoint(ray, t);
    else
      return project(plane, ray.pos);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const Plane<L, T, Q> &plane, const LineSegment<L, T, Q> &line) {
    const T aDist = dot(plane.normal, line.a);
    const T bDist = dot(plane.normal, line.b);
    const T denom = bDist - aDist;
    if (epsilonEqual(denom, T(0), epsilon<T>()))
      return project(plane, abs(aDist) < abs(bDist) ? line.a : line.b);

    const T t = clamp((plane.d - dot(plane.normal, line.a)) / denom, T(0), T(1));
    return project(plane, getPoint(line, t));
  }

  // Tests if this plane contains the given object(s).

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Plane<L, T, Q> &plane, const vec<L, T, Q> &point, T distanceThreshold) {
    return distance(plane, point) <= distanceThreshold;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Plane<L, T, Q> &plane, const Line<L, T, Q> &line, const T eps = epsilon<T>()) {
    return contains(plane, line.pos, eps) && isPerpendicular(line.dir, plane.normal, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Plane<L, T, Q> &plane, const Ray<L, T, Q> &ray, const T eps = epsilon<T>()) {
    return contains(plane, ray.pos, eps) && isPerpendicular(ray.dir, plane.normal, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Plane<L, T, Q> &plane, const LineSegment<L, T, Q> &line, const T eps = epsilon<T>()) {
    return contains(plane, line.a, eps) && contains(plane, line.b, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Plane<L, T, Q> &plane, const Triangle<L, T, Q> &triangle, const T eps = epsilon<T>()) {
    return contains(plane, triangle.a, eps)
           && contains(plane, triangle.b, eps)
           && contains(plane, triangle.c, eps);
  }

  /// Tests whether the plane and the given object intersect.

  /// <summary>
  /// This function attempts to improve stability with lines that are almost
  /// parallel with the plane.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersectLinePlane(const vec<L, T, Q> &planeNormal, T planeD, const vec<L, T, Q> &linePos, const vec<L, T, Q> &lineDir, T &d) {
    const T denom = dot(planeNormal, lineDir);
    if (abs(denom) > epsilon<T>()) {  // line starting point to point of intersection
      d = (planeD - dot(planeNormal, linePos)) / denom;
      return true;
    }

    if (!detail::exactly_zero(denom)) {
      d = (planeD - dot(planeNormal, linePos)) / denom;
      if (abs(d) < epsilon<T>()) {
        return true;
      }
    }

    d = T(0);
    return epsilonEqual(dot(planeNormal, linePos), planeD, epsilon<T>());
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Plane<L, T, Q> &plane, const Ray<L, T, Q> &ray, T &d) {
    if (intersectLinePlane(plane.normal, plane.d, ray.pos, ray.dir, d))
      return d >= T(0);
    return false;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Plane<L, T, Q> &plane, const Line<L, T, Q> &line, T &d) {
    return intersectLinePlane(plane.normal, plane.d, line.pos, line.dir, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Plane<L, T, Q> &plane, const LineSegment<L, T, Q> &lineSegment, T &d) {
    if (intersectLinePlane(plane.normal, plane.d, lineSegment.a, lineSegment.dir(), d)) {
      d /= length(lineSegment);
      return d >= T(0) && d <= T(1);
    }
    return false;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Plane<L, T, Q> &plane, const Sphere<L, T, Q> &sphere) {
    return distance(plane, sphere.pos) <= sphere.r;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Plane<L, T, Q> &plane, const AABB<L, T, Q> &aabb) {
    const vec<L, T, Q> c = centerPoint(aabb);
    const vec<L, T, Q> e = halfSize(aabb);

    T r(0);  // Compute projection interval radius; aabb.center + t * plane.normal
    for (length_t i = 0; i < L; ++i)
      r += e[i] * abs(plane.normal[i]);

    return abs(dot(plane.normal, c) - plane.d) <= r;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Plane<L, T, Q> &plane, const Ray<L, T, Q> &ray) {
    T d(0);
    return intersects(plane, ray, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Plane<L, T, Q> &plane, const Line<L, T, Q> &line) {
    T d(0);
    return intersects(plane, line, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Plane<L, T, Q> &plane, const LineSegment<L, T, Q> &lineSegment) {
    T d(0);
    return intersects(plane, lineSegment, d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Plane<L, T, Q> &a, const Plane<L, T, Q> &b, const Plane<L, T, Q> &c, vec<L, T, Q> &result) {
    const T denom = dot(cross(a.normal, b.normal), c.normal);
    if (denom >= epsilon<T>()) {
      result = ((cross(b.normal, c.normal) * a.d) + (cross(c.normal, a.normal) * b.d) + (cross(a.normal, b.normal) * c.d)) / denom;
      return true;
    }
    return false;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Plane<L, T, Q> &plane, const Triangle<L, T, Q> &triangle) {
    const T a = signedDistance(plane, triangle.a);
    const T b = signedDistance(plane, triangle.b);
    const T c = signedDistance(plane, triangle.c);
    return (a * b <= T(0) || a * c <= T(0));
  }

  namespace detail {
#if GLM_GEOM_TOSTRING
    template<glm::length_t L, typename T, qualifier Q>
    struct compute_to_string<Plane<L, T, Q>> {
      GLM_GEOM_QUALIFIER std::string call(const Plane<L, T, Q> &plane) {
        char const *LiteralStr = literal<T, std::numeric_limits<T>::is_iec559>::value();
        std::string FormatStr(detail::format("plane(%%s, %s)", LiteralStr));

        return detail::format(FormatStr.c_str(),
          glm::to_string(plane.normal).c_str(),
          static_cast<typename cast<T>::value_type>(plane.d)
        );
      }
    };
#endif
  }
}
#endif
