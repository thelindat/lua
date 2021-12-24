/// <summary>
/// See Copyright Notice in setup.hpp
/// </summary>
#ifndef EXT_GEOM_AABB_HPP
#define EXT_GEOM_AABB_HPP

#include <vector>
#include <iterator>

#include "setup.hpp"
#include "line.hpp"
#include "linesegment.hpp"
#include "ray.hpp"
#include "triangle.hpp"

#if GLM_MESSAGES == GLM_ENABLE && !defined(GLM_EXT_INCLUDED)
  #pragma message("GLM: GLM_EXT_GEOM_aabb extension included")
#endif

namespace glm {
  /// <summary>
  /// An axis-aligned bounding box.
  ///
  /// @NOTE: operator* creates an AABB and not an OBB.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  struct AABB {

    // -- Implementation detail --

    typedef T value_type;
    typedef AABB<L, T, Q> type;
    typedef vec<L, T, Q> point_type;

    // -- Data --

    point_type minPoint;  // minimum extent of this AABB in the world space
    point_type maxPoint;  // maximum extent of this AABB in the world space

#if GLM_CONFIG_DEFAULTED_DEFAULT_CTOR == GLM_ENABLE
    AABB() GLM_DEFAULT_CTOR;
#else
    AABB()
  #if GLM_CONFIG_CTOR_INIT != GLM_CTOR_INIT_DISABLE
      : minPoint(T(0)), maxPoint(T(0))
  #endif
    {
    }
#endif

    AABB(T scalar)
      : minPoint(scalar), maxPoint(scalar) {
    }

    AABB(const vec<L, T, Q> &min, const vec<L, T, Q> &max)
      : minPoint(min), maxPoint(max) {
    }

    AABB(const AABB<L, T, Q> &aabb)
      : minPoint(aabb.minPoint), maxPoint(aabb.maxPoint) {
    }

    AABB<L, T, Q> &operator=(const AABB<L, T, Q> &aabb) {
      minPoint = aabb.minPoint;
      maxPoint = aabb.maxPoint;
      return *this;
    }

    GLM_FUNC_QUALIFIER void setNegativeInfinity() {
      minPoint = vec<L, T, Q>(std::numeric_limits<T>::infinity());
      maxPoint = vec<L, T, Q>(-std::numeric_limits<T>::infinity());
    }

    GLM_FUNC_QUALIFIER void setFromCenterAndSize(const vec<L, T, Q> &center, const vec<L, T, Q> &size) {
      vec<L, T, Q> halfSize = T(0.5) * size;
      minPoint = center - halfSize;
      maxPoint = center + halfSize;
    }

    GLM_FUNC_QUALIFIER void enclose(const vec<L, T, Q> &point) {
      minPoint = min(minPoint, point);
      maxPoint = max(maxPoint, point);
    }
  };

  template<typename T, qualifier Q, class Matrix>
  GLM_GEOM_QUALIFIER AABB<3, T, Q> transformAsAABB(const AABB<3, T, Q> &aabb, const Matrix &m);

  template<typename T, qualifier Q, class Matrix>
  GLM_GEOM_QUALIFIER AABB<2, T, Q> transformAsAABB(const AABB<2, T, Q> &aabb, const Matrix &m);

  template<length_t L, typename T, qualifier Q>
  static AABB<L, T, Q> operator-(const AABB<L, T, Q> &aabb) {
    return AABB<L, T, Q>(-aabb.maxPoint, -aabb.minPoint);
  }

  template<length_t L, typename T, qualifier Q>
  static bool operator==(const AABB<L, T, Q> &a1, const AABB<L, T, Q> &a2) {
    return a1.minPoint == a2.minPoint && a1.maxPoint == a2.maxPoint;
  }

  template<length_t L, typename T, qualifier Q>
  static AABB<L, T, Q> operator+(const AABB<L, T, Q> &aabb, const vec<L, T, Q> &point) {
    return AABB<L, T, Q>(aabb.minPoint + point, aabb.maxPoint + point);
  }

  template<length_t L, typename T, qualifier Q>
  static AABB<L, T, Q> operator-(const AABB<L, T, Q> &aabb, const vec<L, T, Q> &point) {
    return AABB<L, T, Q>(aabb.minPoint - point, aabb.maxPoint - point);
  }

  template<length_t L, typename T, qualifier Q>
  static AABB<L, T, Q> operator*(const mat<3, 3, T, Q> &m, const AABB<L, T, Q> &aabb) {
    return transformAsAABB<T, Q, mat<3, 3, T, Q>>(aabb, m);
  }

  template<length_t L, typename T, qualifier Q>
  static AABB<L, T, Q> operator*(const mat<3, 4, T, Q> &m, const AABB<L, T, Q> &aabb) {
    return transformAsAABB<T, Q, mat<3, 4, T, Q>>(aabb, m);
  }

  template<length_t L, typename T, qualifier Q>
  static AABB<L, T, Q> operator*(const mat<4, 3, T, Q> &m, const AABB<L, T, Q> &aabb) {
    return transformAsAABB<T, Q, mat<4, 3, T, Q>>(aabb, m);
  }

  template<length_t L, typename T, qualifier Q>
  static AABB<L, T, Q> operator*(const mat<4, 4, T, Q> &m, const AABB<L, T, Q> &aabb) {
    return transformAsAABB<T, Q, mat<4, 4, T, Q>>(aabb, m);
  }

  template<length_t L, typename T, qualifier Q>
  static AABB<L, T, Q> operator*(const qua<T, Q> &q, const AABB<L, T, Q> &aabb) {
    const vec<L, T, Q> center = q * centerPoint(aabb);
    const vec<L, T, Q> newDir = abs((q * size(aabb)) * T(0.5));
    return AABB<L, T, Q>(center - newDir, center + newDir);
  }

  template<typename T, qualifier Q>
  static AABB<2, T, Q> operator*(const qua<T, Q> &q, const AABB<2, T, Q> &aabb) {
    return operator*(toMat3(q), aabb);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(AABB<L, T, Q> const &x, AABB<L, T, Q> const &y, T eps = epsilon<T>()) {
    return all_equal(x.minPoint, y.minPoint, eps) && all_equal(x.maxPoint, y.maxPoint, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(AABB<L, T, Q> const &x, AABB<L, T, Q> const &y, vec<L, T, Q> const &eps) {
    return all_equal(x.minPoint, y.minPoint, eps) && all_equal(x.maxPoint, y.maxPoint, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(AABB<L, T, Q> const &x, AABB<L, T, Q> const &y, int MaxULPs) {
    return all_equal(x.minPoint, y.minPoint, MaxULPs) && all_equal(x.maxPoint, y.maxPoint, MaxULPs);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool equal(AABB<L, T, Q> const &x, AABB<L, T, Q> const &y, vec<L, int, Q> const &MaxULPs) {
    return all_equal(x.minPoint, y.minPoint, MaxULPs) && all_equal(x.maxPoint, y.maxPoint, MaxULPs);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(AABB<L, T, Q> const &x, AABB<L, T, Q> const &y, T eps = epsilon<T>()) {
    return any_notequal(x.minPoint, y.minPoint, eps) || any_notequal(x.maxPoint, y.maxPoint, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(AABB<L, T, Q> const &x, AABB<L, T, Q> const &y, vec<L, T, Q> const &eps) {
    return any_notequal(x.minPoint, y.minPoint, eps) || any_notequal(x.maxPoint, y.maxPoint, eps);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(AABB<L, T, Q> const &x, AABB<L, T, Q> const &y, int MaxULPs) {
    return any_notequal(x.minPoint, y.minPoint, MaxULPs) || any_notequal(x.maxPoint, y.maxPoint, MaxULPs);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER GLM_CONSTEXPR bool notEqual(AABB<L, T, Q> const &x, AABB<L, T, Q> const &y, vec<L, int, Q> const &MaxULPs) {
    return any_notequal(x.minPoint, y.minPoint, MaxULPs) || any_notequal(x.maxPoint, y.maxPoint, MaxULPs);
  }

  /// <summary>
  /// Create an AABB by specifying its center and size (along each dimension).
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER AABB<L, T, Q> aabbFromCenterAndSize(const vec<L, T, Q> &center, const vec<L, T, Q> &size) {
    const vec<L, T, Q> halfSize = T(0.5) * size;
    return AABB<L, T, Q>(center - halfSize, center + halfSize);
  }

  /// <summary>
  /// Create an AABB by specifying its center and size (uniform on each dimension).
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER AABB<L, T, Q> aabbFromCenterAndSize(const vec<L, T, Q> &center, T size) {
    const vec<L, T, Q> halfSize = vec<L, T, Q>{ T(0.5) * size };
    return AABB<L, T, Q>(center - halfSize, center + halfSize);
  }

  /// <summary>
  /// Create the smallest possible AABB, in terms of volume, that contains the provided sphere.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER AABB<L, T, Q> aabbFromSphere(const Sphere<L, T, Q> &sphere) {
    const vec<L, T, Q> d(sphere.r);
    return AABB<L, T, Q>(sphere.pos - d, sphere.pos + d);
  }

  /// <summary>
  /// Tests if any component of the AABB is infinite.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isinf(const AABB<L, T, Q> &aabb) {
    return any_isinf(aabb.minPoint) || any_isinf(aabb.maxPoint);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isnan(const AABB<L, T, Q> &aabb) {
    return any_isnan(aabb.minPoint) || any_isnan(aabb.maxPoint);
  }

  /// <summary>
  /// Test if all components of the AABB are finite.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isfinite(const AABB<L, T, Q> &aabb) {
    return all(isfinite(aabb.minPoint)) && all(isfinite(aabb.maxPoint));
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T width(const AABB<2, T, Q> &aabb) {
    return aabb.maxPoint.x - aabb.minPoint.x;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T height(const AABB<2, T, Q> &aabb) {
    return aabb.maxPoint.y - aabb.minPoint.y;
  }

  /// <summary>
  /// Return true if the AABB is degenerate (i.e., does not span in a strictly
  /// positive volume).
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isDegenerate(const AABB<L, T, Q> &aabb) {
    bool result = false;
    for (length_t i = 0; i < L; ++i)
      result |= aabb.minPoint[i] >= aabb.maxPoint[i];
    return result;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isDegenerate(const AABB<3, T, Q> &aabb) {
    return aabb.minPoint.x >= aabb.maxPoint.x || aabb.minPoint.y >= aabb.maxPoint.y || aabb.minPoint.z >= aabb.maxPoint.z;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isDegenerate(const AABB<2, T, Q> &aabb) {
    return aabb.minPoint.x >= aabb.maxPoint.x || aabb.minPoint.y >= aabb.maxPoint.y;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool hasNegativeVolume(const AABB<2, T, Q> &aabb) {
    return aabb.maxPoint.x < aabb.minPoint.x || aabb.maxPoint.y < aabb.minPoint.y;
  }

  /// <summary>
  /// Return the center point of the AABB
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> centerPoint(const AABB<L, T, Q> &aabb) {
    return (aabb.minPoint + aabb.maxPoint) * T(0.5);
  }

  /// <summary>
  /// Generates a point inside the AABB. "p" is a vector of normalized values
  /// (i.e., between [0, 1]) along each axis, relative to the minpoint.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> pointInside(const AABB<L, T, Q> &aabb, const vec<L, T, Q> &p) {
    const vec<L, T, Q> d = aabb.maxPoint - aabb.minPoint;
    return aabb.minPoint + d * p;
  }

  /// <summary>
  /// Return the smallest sphere that contains the AABB.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Sphere<L, T, Q> minimalEnclosingSphere(const AABB<L, T, Q> &aabb) {
    return Sphere<L, T, Q>(centerPoint(aabb), length(aabb.maxPoint - aabb.minPoint) * T(0.5));
  }

  /// <summary>
  /// Return the largest sphere that can fit inside the AABB.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Sphere<L, T, Q> maximalContainedSphere(const AABB<L, T, Q> &aabb) {
    const vec<L, T, Q> hsize = halfSize(aabb);
    return Sphere<L, T, Q>(centerPoint(aabb), min(hsize.x, min(hsize.y, hsize.z)));
  }

  /// <summary>
  /// Return an edge (segment) of the AABB: [0, 11].
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER LineSegment<3, T, Q> edge(const AABB<3, T, Q> &aabb, int edgeIndex) {
    switch (edgeIndex) {
      case 1: return LineSegment<3, T, Q>(aabb.minPoint, vec<3, T, Q>(aabb.minPoint.x, aabb.maxPoint.y, aabb.minPoint.z));
      case 2: return LineSegment<3, T, Q>(aabb.minPoint, vec<3, T, Q>(aabb.maxPoint.x, aabb.minPoint.y, aabb.minPoint.z));
      case 3: return LineSegment<3, T, Q>(vec<3, T, Q>(aabb.minPoint.x, aabb.minPoint.y, aabb.maxPoint.z), vec<3, T, Q>(aabb.minPoint.x, aabb.maxPoint.y, aabb.maxPoint.z));
      case 4: return LineSegment<3, T, Q>(vec<3, T, Q>(aabb.minPoint.x, aabb.minPoint.y, aabb.maxPoint.z), vec<3, T, Q>(aabb.maxPoint.x, aabb.minPoint.y, aabb.maxPoint.z));
      case 5: return LineSegment<3, T, Q>(vec<3, T, Q>(aabb.minPoint.x, aabb.maxPoint.y, aabb.minPoint.z), vec<3, T, Q>(aabb.minPoint.x, aabb.maxPoint.y, aabb.maxPoint.z));
      case 6: return LineSegment<3, T, Q>(vec<3, T, Q>(aabb.minPoint.x, aabb.maxPoint.y, aabb.minPoint.z), vec<3, T, Q>(aabb.maxPoint.x, aabb.maxPoint.y, aabb.minPoint.z));
      case 7: return LineSegment<3, T, Q>(vec<3, T, Q>(aabb.minPoint.x, aabb.maxPoint.y, aabb.maxPoint.z), aabb.maxPoint);
      case 8: return LineSegment<3, T, Q>(vec<3, T, Q>(aabb.maxPoint.x, aabb.minPoint.y, aabb.minPoint.z), vec<3, T, Q>(aabb.maxPoint.x, aabb.minPoint.y, aabb.maxPoint.z));
      case 9: return LineSegment<3, T, Q>(vec<3, T, Q>(aabb.maxPoint.x, aabb.minPoint.y, aabb.minPoint.z), vec<3, T, Q>(aabb.maxPoint.x, aabb.maxPoint.y, aabb.minPoint.z));
      case 10: return LineSegment<3, T, Q>(vec<3, T, Q>(aabb.maxPoint.x, aabb.minPoint.y, aabb.maxPoint.z), aabb.maxPoint);
      case 11: return LineSegment<3, T, Q>(vec<3, T, Q>(aabb.maxPoint.x, aabb.maxPoint.y, aabb.minPoint.z), aabb.maxPoint);
      case 0:
      default: {  // First Point
        return LineSegment<3, T, Q>(aabb.minPoint, vec<3, T, Q>(aabb.minPoint.x, aabb.minPoint.y, aabb.maxPoint.z));
      }
    }
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER LineSegment<2, T, Q> edge(const AABB<2, T, Q> &aabb, int edgeIndex) {
    switch (edgeIndex) {
      case 1: return LineSegment<2, T, Q>(vec<2, T, Q>(aabb.maxPoint.x, aabb.minPoint.y), aabb.maxPoint);
      case 2: return LineSegment<2, T, Q>(aabb.maxPoint, vec<2, T, Q>(aabb.minPoint.x, aabb.maxPoint.y));
      case 3: return LineSegment<2, T, Q>(vec<2, T, Q>(aabb.minPoint.x, aabb.maxPoint.y), aabb.minPoint);
      case 0:
      default: {  // First Point
        return LineSegment<2, T, Q>(aabb.minPoint, vec<2, T, Q>(aabb.maxPoint.x, aabb.minPoint.y));
      }
    }
  }

  /// <summary>
  /// Return a corner point of the AABB: [0, 7].
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<3, T, Q> cornerPoint(const AABB<3, T, Q> &aabb, int index) {
    switch (index) {
      case 1: return vec<3, T, Q>(aabb.minPoint.x, aabb.minPoint.y, aabb.maxPoint.z);
      case 2: return vec<3, T, Q>(aabb.minPoint.x, aabb.maxPoint.y, aabb.minPoint.z);
      case 3: return vec<3, T, Q>(aabb.minPoint.x, aabb.maxPoint.y, aabb.maxPoint.z);
      case 4: return vec<3, T, Q>(aabb.maxPoint.x, aabb.minPoint.y, aabb.minPoint.z);
      case 5: return vec<3, T, Q>(aabb.maxPoint.x, aabb.minPoint.y, aabb.maxPoint.z);
      case 6: return vec<3, T, Q>(aabb.maxPoint.x, aabb.maxPoint.y, aabb.minPoint.z);
      case 7:
        return aabb.maxPoint;
      case 0:
      default: {
        return aabb.minPoint;
      }
    }
  }

  /// <summary>
  /// Return a corner point of the AABB: [0, 3].
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<2, T, Q> cornerPoint(const AABB<2, T, Q> &aabb, int index) {
    switch (index) {
      case 1: return vec<2, T, Q>(aabb.minPoint.x, aabb.maxPoint.y);
      case 2: return vec<2, T, Q>(aabb.maxPoint.x, aabb.minPoint.y);
      case 3:
        return aabb.maxPoint;
      case 0:
      default: {
        return aabb.minPoint;
      }
    }
  }

  /// <summary>
  /// Compute an extreme point along the AABB, i.e., the farthest point in a
  /// given direction.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> extremePoint(const AABB<L, T, Q> &aabb, const vec<L, T, Q> &direction) {
    vec<L, T, Q> result(T(0));
    for (length_t i = 0; i < L; ++i)
      result[i] = direction[i] >= T(0) ? aabb.maxPoint[i] : aabb.minPoint[i];
    return result;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<3, T, Q> extremePoint(const AABB<3, T, Q> &aabb, const vec<3, T, Q> &direction) {
    return vec<3, T, Q>(
      (direction.x >= T(0) ? aabb.maxPoint.x : aabb.minPoint.x),
      (direction.y >= T(0) ? aabb.maxPoint.y : aabb.minPoint.y),
      (direction.z >= T(0) ? aabb.maxPoint.z : aabb.minPoint.z)
    );
  }

  /// <summary>
  /// Computes a point along an edge of the AABB.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<3, T, Q> pointOnEdge(const AABB<3, T, Q> &aabb, int edgeIndex, T u) {
    const vec<3, T, Q> d = aabb.maxPoint - aabb.minPoint;
    switch (edgeIndex) {
      case 1: return vec<3, T, Q>(aabb.minPoint.x, aabb.maxPoint.y, aabb.minPoint.z + u * d.z);
      case 2: return vec<3, T, Q>(aabb.maxPoint.x, aabb.minPoint.y, aabb.minPoint.z + u * d.z);
      case 3: return vec<3, T, Q>(aabb.maxPoint.x, aabb.maxPoint.y, aabb.minPoint.z + u * d.z);
      case 4: return vec<3, T, Q>(aabb.minPoint.x, aabb.minPoint.y + u * d.y, aabb.minPoint.z);
      case 5: return vec<3, T, Q>(aabb.maxPoint.x, aabb.minPoint.y + u * d.y, aabb.minPoint.z);
      case 6: return vec<3, T, Q>(aabb.minPoint.x, aabb.minPoint.y + u * d.y, aabb.maxPoint.z);
      case 7: return vec<3, T, Q>(aabb.maxPoint.x, aabb.minPoint.y + u * d.y, aabb.maxPoint.z);
      case 8: return vec<3, T, Q>(aabb.minPoint.x + u * d.x, aabb.minPoint.y, aabb.minPoint.z);
      case 9: return vec<3, T, Q>(aabb.minPoint.x + u * d.x, aabb.minPoint.y, aabb.maxPoint.z);
      case 10: return vec<3, T, Q>(aabb.minPoint.x + u * d.x, aabb.maxPoint.y, aabb.minPoint.z);
      case 11: return vec<3, T, Q>(aabb.minPoint.x + u * d.x, aabb.maxPoint.y, aabb.maxPoint.z);
      case 0:  // First point
      default: {
        return vec<3, T, Q>(aabb.minPoint.x, aabb.minPoint.y, aabb.minPoint.z + u * d.z);
      }
    }
  }

  /// <summary>
  /// Return the point at the center of the given face, [0, 5], of the AABB.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<3, T, Q> faceCenterPoint(const AABB<3, T, Q> &aabb, int faceIndex) {
    const vec<3, T, Q> center = (aabb.minPoint + aabb.maxPoint) * T(0.5);
    switch (faceIndex) {
      case 1: return vec<3, T, Q>(aabb.maxPoint.x, center.y, center.z);
      case 2: return vec<3, T, Q>(center.x, aabb.minPoint.y, center.z);
      case 3: return vec<3, T, Q>(center.x, aabb.maxPoint.y, center.z);
      case 4: return vec<3, T, Q>(center.x, center.y, aabb.minPoint.z);
      case 5: return vec<3, T, Q>(center.x, center.y, aabb.maxPoint.z);
      case 0:
      default: {
        return vec<3, T, Q>(aabb.minPoint.x, center.y, center.z);
      }
    }
  }

  /// <summary>
  /// Generate a point on the surface of the given face of the AABB.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<3, T, Q> facePoint(const AABB<3, T, Q> &aabb, int faceIndex, T u, T v) {
    const vec<3, T, Q> d = aabb.maxPoint - aabb.minPoint;
    switch (faceIndex) {
      case 1: return vec<3, T, Q>(aabb.maxPoint.x, aabb.minPoint.y + u * d.y, aabb.minPoint.z + v * d.z);
      case 2: return vec<3, T, Q>(aabb.minPoint.x + u * d.x, aabb.minPoint.y, aabb.minPoint.z + v * d.z);
      case 3: return vec<3, T, Q>(aabb.minPoint.x + u * d.x, aabb.maxPoint.y, aabb.minPoint.z + v * d.z);
      case 4: return vec<3, T, Q>(aabb.minPoint.x + u * d.x, aabb.minPoint.y + v * d.y, aabb.minPoint.z);
      case 5: return vec<3, T, Q>(aabb.minPoint.x + u * d.x, aabb.minPoint.y + v * d.y, aabb.maxPoint.z);
      case 0:
      default: {
        return vec<3, T, Q>(aabb.minPoint.x, aabb.minPoint.y + u * d.y, aabb.minPoint.z + v * d.z);
      }
    }
  }

  /// <summary>
  /// Return the surface normal of the given face of the AABB.
  /// </summary>
  template<typename T, qualifier Q = glm::defaultp>
  GLM_GEOM_QUALIFIER vec<3, T, Q> faceNormalAABB(int faceIndex) {
    switch (faceIndex) {
      case 1: return vec<3, T, Q>(T(1), T(0), T(0));
      case 2: return vec<3, T, Q>(T(0), T(-1), T(0));
      case 3: return vec<3, T, Q>(T(0), T(1), T(0));
      case 4: return vec<3, T, Q>(T(0), T(0), T(-1));
      case 5: return vec<3, T, Q>(T(0), T(0), T(1));
      case 0:
      default: {
        return vec<3, T, Q>(T(-1), T(0), T(0));
      }
    }
  }

  /// <summary>
  /// Generate a plane (point and normal) for the given face of the AABB.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Plane<3, T, Q> facePlane(const AABB<3, T, Q> &aabb, int faceIndex) {
    return Plane<3, T, Q>(faceCenterPoint(aabb, faceIndex), faceNormalAABB<T, Q>(faceIndex));
  }

  /// <summary>
  /// Generates an AABB that encloses the given set of points.
  /// </summary>
  template<length_t L, typename T, qualifier Q, class Vector>
  GLM_GEOM_QUALIFIER AABB<L, T, Q> minimalEnclosingAABB(const Vector &points) {
    AABB<L, T, Q> aabb(T(0));
    aabb.setNegativeInfinity();
    for (auto it = points.begin(); it != points.end(); ++it)
      aabb.enclose(*it);
    return aabb;
  }

  /// <summary>
  /// Generates an AABB that encloses the given point iterators.
  /// </summary>
  template<class Iterator, length_t L, typename T, qualifier Q = glm::defaultp>
  GLM_GEOM_QUALIFIER AABB<L, T, Q> minimalEnclosingAABB(Iterator begin, const Iterator &end) {
    AABB<L, T, Q> result;

    result.setNegativeInfinity();
    for (; begin != end; ++begin)
      result.enclose(*begin);
    return result;
  }

  /// <summary>
  /// Return the length of the AABB along each dimension.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> size(const AABB<L, T, Q> &aabb) {
    return aabb.maxPoint - aabb.minPoint;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> halfSize(const AABB<L, T, Q> &aabb) {
    return size(aabb) * T(0.5);
  }

  /// <summary>
  /// Compute the volume of the AABB.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T volume(const AABB<L, T, Q> &aabb) {
    const vec<L, T, Q> s = size(aabb);

    T result(1);
    for (length_t i = 0; i < L; ++i)
      result *= s[i];
    return result;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T volume(const AABB<3, T, Q> &aabb) {
    const vec<3, T, Q> s = size(aabb);
    return s.x * s.y * s.z;
  }

  /// <summary>
  /// Computes the surface area of the faces of the AABB.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T surfaceArea(const AABB<3, T, Q> &aabb) {
    const vec<3, T, Q> s = size(aabb);
    return T(2) * (s.x * s.y + s.x * s.z + s.y * s.z);
  }

  /// <summary>
  /// Apply a uniform scale to the AABB
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER AABB<L, T, Q> scale(const AABB<L, T, Q> &aabb, const vec<L, T, Q> &centerPoint, T scaleFactor) {
    return AABB<L, T, Q>(
      (aabb.minPoint - centerPoint) * scaleFactor + centerPoint,
      (aabb.maxPoint - centerPoint) * scaleFactor + centerPoint
    );
  }

  /// <summary>
  /// Grow an AABB by the given size.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER AABB<L, T, Q> grow(const AABB<L, T, Q> &aabb, const T amount) {
    return AABB<L, T, Q>(aabb.minPoint - (T(0.5) * amount), aabb.maxPoint + (T(0.5) * amount));
  }

  /// <summary>
  /// Project the AABB onto the provided axis.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER void projectToAxis(const AABB<L, T, Q> &aabb, const vec<L, T, Q> &axis, T &dMin, T &dMax) {
    const vec<L, T, Q> c = (aabb.minPoint + aabb.maxPoint) * T(0.5);
    const vec<L, T, Q> e = aabb.maxPoint - c;

    const T r = abs(dot(e, abs(axis)));
    const T s = dot(axis, c);  // distance center/plane.
    dMin = s - r;
    dMax = s + r;
  }

  /// <summary>
  /// Apply a generic matrix transformation to the AABB.
  /// </summary>
  template<typename T, qualifier Q, class Matrix>
  GLM_GEOM_QUALIFIER AABB<3, T, Q> transformAsAABB(const AABB<3, T, Q> &aabb, const Matrix &m) {
    const vec<3, T, Q> centerPoint = (aabb.minPoint + aabb.maxPoint) * T(0.5);
    const vec<3, T, Q> halfSize = centerPoint - aabb.minPoint;
    const vec<3, T, Q> newCenter = transformPos(m, centerPoint);
    const vec<3, T, Q> newDir = vec<3, T, Q>(
      abs(m[0][0] * halfSize.x) + abs(m[1][0] * halfSize.y) + abs(m[2][0] * halfSize.z),
      abs(m[0][1] * halfSize.x) + abs(m[1][1] * halfSize.y) + abs(m[2][1] * halfSize.z),
      abs(m[0][2] * halfSize.x) + abs(m[1][2] * halfSize.y) + abs(m[2][2] * halfSize.z)
    );
    return AABB<3, T, Q>(newCenter - newDir, newCenter + newDir);
  }

  template<typename T, qualifier Q, class Matrix>
  GLM_GEOM_QUALIFIER AABB<2, T, Q> transformAsAABB(const AABB<2, T, Q> &aabb, const Matrix &m) {
    const T ax = m[0][0] * aabb.minPoint.x;
    const T bx = m[0][0] * aabb.maxPoint.x;
    const T ay = m[1][0] * aabb.minPoint.y;
    const T by = m[1][0] * aabb.maxPoint.y;
    const T ax2 = m[0][1] * aabb.minPoint.x;
    const T bx2 = m[0][1] * aabb.maxPoint.x;
    const T ay2 = m[1][1] * aabb.minPoint.y;
    const T by2 = m[1][1] * aabb.maxPoint.y;

    T ox = T(0), oy = T(0);
    GLM_IF_CONSTEXPR(m.length() > 3) {
      ox = m[3][0];
      oy = m[3][1];
    }

    return AABB<2, T, Q>(
    vec<2, T, Q>(min(ax, bx) + min(ay, by) + ox, min(ax2, bx2) + min(ay2, by2) + oy),
    vec<2, T, Q>(max(ax, bx) + max(ay, by) + ox, max(ax2, bx2) + max(ay2, by2) + oy));
  }

  /// <summary>
  /// Computes the closest point inside the AABB to the given point.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> closestPoint(const AABB<L, T, Q> &aabb, const vec<L, T, Q> &target) {
    return clamp(target, aabb.minPoint, aabb.maxPoint);
  }

  /// Computes the distance between the AABB and the given object(s).

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const AABB<L, T, Q> &aabb, const vec<L, T, Q> &point) {
    return distance(closestPoint(aabb, point), point);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T distance(const AABB<L, T, Q> &aabb, const Sphere<L, T, Q> &sphere) {
    return max(T(0), distance(aabb, sphere.pos) - sphere.r);
  }

  // Tests for if the given objects are fully contained inside the AABB.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const AABB<L, T, Q> &aabb, const vec<L, T, Q> &target) {
    bool result = true;
    for (length_t i = 0; i < L; ++i)
      result &= (aabb.minPoint[i] <= target[i] && target[i] <= aabb.maxPoint[i]);
    return result;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const AABB<L, T, Q> &aabb, const vec<L, T, Q> &minPoint, const vec<L, T, Q> &maxPoint) {
    bool result = true;
    for (length_t i = 0; i < L; ++i)
      result &= (aabb.minPoint[i] <= minPoint[i] && maxPoint[i] <= aabb.maxPoint[i]);
    return result;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const AABB<3, T, Q> &aabb, const vec<3, T, Q> &point) {
    return aabb.minPoint.x <= point.x && point.x <= aabb.maxPoint.x
           && aabb.minPoint.y <= point.y && point.y <= aabb.maxPoint.y
           && aabb.minPoint.z <= point.z && point.z <= aabb.maxPoint.z;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const AABB<3, T, Q> &aabb, const vec<3, T, Q> &minPoint, const vec<3, T, Q> &maxPoint) {
    return aabb.minPoint.x <= minPoint.x && maxPoint.x <= aabb.maxPoint.x
           && aabb.minPoint.y <= minPoint.y && maxPoint.y <= aabb.maxPoint.y
           && aabb.minPoint.z <= minPoint.z && maxPoint.z <= aabb.maxPoint.z;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const AABB<2, T, Q> &aabb, const vec<2, T, Q> &point) {
    return aabb.minPoint.x <= point.x && point.x <= aabb.maxPoint.x
           && aabb.minPoint.y <= point.y && point.y <= aabb.maxPoint.y;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const AABB<2, T, Q> &aabb, const vec<2, T, Q> &minPoint, const vec<2, T, Q> &maxPoint) {
    return aabb.minPoint.x <= minPoint.x && maxPoint.x <= aabb.maxPoint.x
           && aabb.minPoint.y <= minPoint.y && maxPoint.y <= aabb.maxPoint.y;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const AABB<L, T, Q> &aabb, const AABB<L, T, Q> &otheraabb) {
    return contains(aabb, otheraabb.minPoint, otheraabb.maxPoint);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const AABB<L, T, Q> &aabb, const LineSegment<L, T, Q> &lineSegment) {
    return contains(aabb, min(lineSegment.a, lineSegment.b), max(lineSegment.a, lineSegment.b));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const AABB<L, T, Q> &aabb, const Triangle<L, T, Q> &triangle) {
    return contains(aabb, boundingAABB(triangle));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const AABB<L, T, Q> &aabb, const Sphere<L, T, Q> &sphere) {
    const vec<L, T, Q> dir(sphere.r);
    return contains(aabb, sphere.pos - dir, sphere.pos + dir);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const AABB<L, T, Q> &aabb, const Polygon<L, T, Q> &polygon) {
    return contains(aabb, minimalEnclosingAABB(polygon));
  }

  // Functions to expand the AABB to enclose the given objects.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER AABB<L, T, Q> enclose(const AABB<L, T, Q> &aabb, const vec<L, T, Q> &point) {
    AABB<L, T, Q> result(aabb);
    result.enclose(point);
    return result;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER AABB<L, T, Q> enclose(const AABB<L, T, Q> &aabb, const vec<L, T, Q> &aabbMinPoint, const vec<L, T, Q> &aabbMaxPoint) {
    AABB<L, T, Q> result(aabb);
    result.enclose(aabbMinPoint);
    result.enclose(aabbMaxPoint);
    return result;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER AABB<L, T, Q> enclose(const AABB<L, T, Q> &aabb, const LineSegment<L, T, Q> &lineSegment) {
    return enclose(aabb, min(lineSegment.a, lineSegment.b), max(lineSegment.a, lineSegment.b));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER AABB<L, T, Q> enclose(const AABB<L, T, Q> &aabb, const Triangle<L, T, Q> &triangle) {
    return enclose(aabb, min(triangle.a, triangle.b, triangle.c), max(triangle.a, triangle.b, triangle.c));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER AABB<L, T, Q> enclose(const AABB<L, T, Q> &aabb, const Sphere<L, T, Q> &sphere) {
    const vec<L, T, Q> d(sphere.r);
    return enclose(aabb, sphere.pos - d, sphere.pos + d);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER AABB<L, T, Q> enclose(const AABB<L, T, Q> &aabb, const AABB<L, T, Q> &other) {
    return enclose(aabb, other.minPoint, other.maxPoint);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER AABB<L, T, Q> enclose(const AABB<L, T, Q> &aabb, const Polygon<L, T, Q> &polygon) {
    return enclose(aabb, minimalEnclosingAABB(polygon));
  }

  /// <summary>
  /// Generalized compute the intersection of a line (or ray) and the AABB.
  /// </summary>
  /// <param name="tNear">The distance to intersect (enter) the AABB</param>
  /// <param name="tFar">The distance to exit the AABB</param>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersectLineAABB(const AABB<L, T, Q> &aabb, const Line<L, T, Q> &line, T &tNear, T &tFar) {
    for (length_t i = 0; i < L; ++i) {  // test each cardinal plane
      if (!equal(line.dir[i], T(0), epsilon<T>())) {
        const T recipDir = T(1) / line.dir[i];
        const T t1 = (aabb.minPoint[i] - line.pos[i]) * recipDir;
        const T t2 = (aabb.maxPoint[i] - line.pos[i]) * recipDir;
        if (t1 < t2)
          tNear = max(t1, tNear), tFar = min(t2, tFar);
        else  // Swap t1 and t2.
          tNear = max(t2, tNear), tFar = min(t1, tFar);

        if (tNear > tFar)
          return false;  // exit before entering; AABB missed.
      }
      else if (line.pos[i] < aabb.minPoint[i] || line.pos[i] > aabb.maxPoint[i])
        return false;  // AABB completely missed.
    }
    return tNear <= tFar;
  }

  /// <summary>
  /// Compute the intersection of a line (or ray) and the AABB.
  /// </summary>
  /// <param name="tNear">The distance to intersect (enter) the AABB</param>
  /// <param name="tFar">The distance to exit the AABB</param>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersectLineAABB(const AABB<3, T, Q> &aabb, const Line<3, T, Q> &line, T &tNear, T &tFar) {
    if (!equal(line.dir.x, T(0), epsilon<T>())) {  // test each cardinal plane: X, Y and Z.
      const T recipDir = T(1) / line.dir.x;
      const T t1 = (aabb.minPoint.x - line.pos.x) * recipDir;
      const T t2 = (aabb.maxPoint.x - line.pos.x) * recipDir;
      if (t1 < t2)
        tNear = max(t1, tNear), tFar = min(t2, tFar);
      else  // Swap t1 and t2.
        tNear = max(t2, tNear), tFar = min(t1, tFar);

      if (tNear > tFar)
        return false;  // exit before entering; AABB missed.
    }
    else if (line.pos.x < aabb.minPoint.x || line.pos.x > aabb.maxPoint.x)
      return false;  // AABB completely missed.

    if (!equal(line.dir.y, T(0), epsilon<T>())) {
      const T recipDir = T(1) / line.dir.y;
      const T t1 = (aabb.minPoint.y - line.pos.y) * recipDir;
      const T t2 = (aabb.maxPoint.y - line.pos.y) * recipDir;
      if (t1 < t2)
        tNear = max(t1, tNear), tFar = min(t2, tFar);
      else
        tNear = max(t2, tNear), tFar = min(t1, tFar);

      if (tNear > tFar)
        return false;
    }
    else if (line.pos.y < aabb.minPoint.y || line.pos.y > aabb.maxPoint.y)
      return false;

    if (!equal(line.dir.z, T(0), epsilon<T>())) {
      const T recipDir = T(1) / line.dir.z;
      const T t1 = (aabb.minPoint.z - line.pos.z) * recipDir;
      const T t2 = (aabb.maxPoint.z - line.pos.z) * recipDir;
      if (t1 < t2)
        tNear = max(t1, tNear), tFar = min(t2, tFar);
      else
        tNear = max(t2, tNear), tFar = min(t1, tFar);
    }
    else if (line.pos.z < aabb.minPoint.z || line.pos.z > aabb.maxPoint.z)
      return false;

    return tNear <= tFar;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool slabs(const AABB<3, T, Q> &aabb, const Ray<3, T, Q> &ray) {
    const vec<3, T, Q> t0 = (aabb.minPoint - ray.pos) * /* INV */ ray.dir;
    const vec<3, T, Q> t1 = (aabb.maxPoint - ray.pos) * /* INV */ ray.dir;
    return compMax(min(t0, t1)) <= (compMin(max(t0, t1)) + epsilon<T>());
  }

  /// <summary>
  /// GLM Convention: intersectLineAABB
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const AABB<L, T, Q> &aabb, const Line<L, T, Q> &line, T &dNear, T &dFar) {
    return intersectLineAABB(aabb, line, dNear, dFar);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const AABB<L, T, Q> &aabb, const Line<L, T, Q> &line) {
    T dNear = -std::numeric_limits<T>::infinity();
    T dFar = std::numeric_limits<T>::infinity();
    return intersects(aabb, line, dNear, dFar);
  }

  /// <summary>
  /// GLM Convention: intersectRayAABB
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const AABB<L, T, Q> &aabb, const Ray<L, T, Q> &ray, T &dNear, T &dFar) {
    return intersectLineAABB(aabb, toLine(ray), dNear, dFar);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const AABB<L, T, Q> &aabb, const Ray<L, T, Q> &ray) {
    T dNear = T(0);
    T dFar = std::numeric_limits<T>::infinity();
    return intersects(aabb, ray, dNear, dFar);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const AABB<L, T, Q> &aabb, const LineSegment<L, T, Q> &lineSegment, T &dNear, T &dFar) {
    const vec<L, T, Q> dir = lineSegment.dir2();
    const T len = length(dir);
    if (len <= epsilon<T>()) {  // Degenerate line segment
      dNear = T(0);
      dFar = T(1);
      return contains(aabb, lineSegment.a);
    }

    const Line<L, T, Q> line(lineSegment.a, dir * (T(1) / len));
    return intersectLineAABB(aabb, line, dNear, dFar);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const AABB<L, T, Q> &aabb, const LineSegment<L, T, Q> &lineSegment) {
    T near(0), far(0);
    return intersects(aabb, lineSegment, near, far);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const AABB<L, T, Q> &aabb, const AABB<L, T, Q> &other) {
    bool result = true;
    for (length_t i = 0; i < L; ++i)
      result &= aabb.minPoint[i] < other.maxPoint[i] && other.minPoint[i] < aabb.maxPoint[i];
    return result;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const AABB<L, T, Q> &aabb, const Sphere<L, T, Q> &sphere) {
    const vec<L, T, Q> pt = closestPoint(aabb, sphere.pos);
    return distance2(sphere.pos, pt) <= sphere.r * sphere.r;  // + epsilon<T>() ?
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const AABB<L, T, Q> &aabb, const Plane<L, T, Q> &plane) {
    return intersects(plane, aabb);
  }

  /// @TODO
  // template<length_t L, typename T, qualifier Q>
  // GLM_GEOM_QUALIFIER bool intersects(const AABB<L, T, Q> &aabb, const Triangle<L, T, Q> &triangle) {
  //   return intersects(triangle, aabb);
  // }

  /// <summary>
  /// Specializations
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const AABB<3, T, Q> &aabb, const AABB<3, T, Q> &other) {
    return aabb.minPoint.x < other.maxPoint.x
           && aabb.minPoint.y < other.maxPoint.y
           && aabb.minPoint.z < other.maxPoint.z
           && other.minPoint.x < aabb.maxPoint.x
           && other.minPoint.y < aabb.maxPoint.y
           && other.minPoint.z < aabb.maxPoint.z;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const AABB<2, T, Q> &aabb, const AABB<2, T, Q> &other) {
    return aabb.minPoint.x < other.maxPoint.x
           && aabb.minPoint.y < other.maxPoint.y
           && other.minPoint.x < aabb.maxPoint.x
           && other.minPoint.y < aabb.maxPoint.y;
  }

  /// <summary>
  /// Return the intersection of two AABBs, i.e., the AABB that is contained in both.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER AABB<L, T, Q> intersection(const AABB<L, T, Q> &aabb, const AABB<L, T, Q> &other) {
    return AABB<L, T, Q>(max(aabb.minPoint, other.minPoint), min(aabb.maxPoint, other.maxPoint));
  }

  namespace detail {
#if GLM_GEOM_TOSTRING
    template<glm::length_t L, typename T, qualifier Q>
    struct compute_to_string<AABB<L, T, Q>> {
      GLM_GEOM_QUALIFIER std::string call(const AABB<L, T, Q> &aabb) {
        return detail::format("aabb(%s, %s)",
          glm::to_string(aabb.minPoint).c_str(),
          glm::to_string(aabb.maxPoint).c_str()
        );
      }
    };
#endif
  }
}

#endif
