/// <summary>
/// See Copyright Notice in setup.hpp
/// </summary>
#ifndef EXT_GEOM_POLYGON_HPP
#define EXT_GEOM_POLYGON_HPP

#include <vector>

#include "setup.hpp"
#include "allocator.hpp"

#include "line.hpp"
#include "linesegment.hpp"
#include "aabb.hpp"
#include "plane.hpp"
#include "triangle.hpp"

#if GLM_MESSAGES == GLM_ENABLE && !defined(GLM_EXT_INCLUDED)
  #pragma message("GLM: GLM_EXT_GEOM_polygon extension included")
#endif

namespace glm {
  /// <summary>
  /// A STL vector that uses the Lua allocator
  /// </summary>
  template<typename T>
  using List = LuaVector<T>;

  /// <summary>
  /// Describes the thickness of the polygon (i.e., how the third dimension
  /// relates to the plane) for 'contains' operations.
  /// </summary>
  enum class PolyContains : uint8_t {
    Positive,  // Boundary extends in the positive direction: [0, +dist]
    Negative,  // Boundary extends in the negative direction: [-dist, 0]
    Unidirectional,  // Boundary extends in both directions: [-0.5*dist, 0.5*dist]
  };

  /// <summary>
  /// A two-dimensional closed surface in three-dimensional space.
  ///
  /// @TODO: size_t -> List<point_type>::size_type
  /// @NOTE: This polygon implementation is tailored specifically to the Lua
  ///   binding. The "glm::List" pointer is owned by the userdata bound to the
  ///   garbage collector.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  struct Polygon {

    // -- Implementation detail --

    typedef T value_type;
    typedef Polygon<L, T, Q> type;
    typedef vec<L, T, Q> point_type;

    // -- Data --

    List<point_type> *p;  // Stores the vertices of this polygon.

    // @LuaGLM Reference to the stack index the Polygon userdata belongs to;
    // note this is a Lua-specific hack.
    int stack_idx;

    Polygon(List<point_type> *points = GLM_NULLPTR)
      : p(points), stack_idx(-1) {
    }

    Polygon(const Polygon<L, T, Q> &poly)
      : p(poly.p), stack_idx(poly.stack_idx) {
    }

#if __cplusplus >= 201103L
    Polygon(Polygon<L, T, Q> &&poly)
      : p(poly.p), stack_idx(poly.stack_idx) {
      poly.p = GLM_NULLPTR;
      poly.stack_idx = -1;
    }

    Polygon<L, T, Q> &operator=(Polygon<L, T, Q> &&poly) {
      p = poly.p;
      stack_idx = poly.stack_idx;

      poly.p = GLM_NULLPTR;
      poly.stack_idx = -1;
      return *this;
    }
#endif

    Polygon<L, T, Q> &operator=(const Polygon<L, T, Q> &poly) {
      p = poly.p;
      stack_idx = poly.stack_idx;
      return *this;
    }

    GLM_FUNC_QUALIFIER size_t size() const {
      lua_assert(p != GLM_NULLPTR);
      return (p == GLM_NULLPTR) ? 0 : p->size();
    }

    GLM_FUNC_QUALIFIER const point_type &back() const {
      lua_assert(p != GLM_NULLPTR);
      return p->back();
    }

    GLM_FUNC_QUALIFIER point_type &operator[](size_t i) {
      lua_assert(p != GLM_NULLPTR);
      return p->operator[](i);
    }

    GLM_FUNC_QUALIFIER const point_type &operator[](size_t i) const {
      lua_assert(p != GLM_NULLPTR);
      return p->operator[](i);
    }

    GLM_FUNC_QUALIFIER typename List<point_type>::const_iterator begin() const {
      lua_assert(p != GLM_NULLPTR);
      return p->begin();
    }

    GLM_FUNC_QUALIFIER typename List<point_type>::const_iterator cbegin() const {
      lua_assert(p != GLM_NULLPTR);
      return p->cbegin();
    }

    GLM_FUNC_QUALIFIER typename List<point_type>::const_iterator end() const {
      lua_assert(p != GLM_NULLPTR);
      return p->end();
    }

    GLM_FUNC_QUALIFIER typename List<point_type>::const_iterator cend() const {
      lua_assert(p != GLM_NULLPTR);
      return p->cend();
    }
  };

  template<length_t L, typename T, qualifier Q>
  static Polygon<L, T, Q> operator-(const Polygon<L, T, Q> &polygon) {
    Polygon<L, T, Q> p(polygon);
    for (size_t i = 0; i < p.size(); ++i)
      p[i] = operator-(p[i]);
    return p;
  }

  template<length_t L, typename T, qualifier Q>
  static bool operator==(const Polygon<L, T, Q> &p1, const Polygon<L, T, Q> &p2) {
    if (p1.size() != p2.size())
      return false;
    for (size_t i = 0; i < p1.size(); ++i) {
      if (p1[i] != p2[i])
        return false;
    }
    return true;
  }

  template<length_t L, typename T, qualifier Q>
  static Polygon<L, T, Q> operator+(const Polygon<L, T, Q> &polygon, const vec<L, T, Q> &offset) {
    Polygon<L, T, Q> p(polygon);
    for (size_t i = 0; i < p.size(); ++i)
      p[i] = p[i] + offset;
    return p;
  }

  template<length_t L, typename T, qualifier Q>
  static Polygon<L, T, Q> operator-(const Polygon<L, T, Q> &polygon, const vec<L, T, Q> &offset) {
    Polygon<L, T, Q> p(polygon);
    for (size_t i = 0; i < p.size(); ++i)
      p[i] = p[i] - offset;
    return p;
  }

  template<length_t L, typename T, qualifier Q>
  static Polygon<L, T, Q> operator*(const mat<3, 3, T, Q> &transform, const Polygon<L, T, Q> &polygon) {
    Polygon<L, T, Q> p(polygon);
    for (size_t i = 0; i < p.size(); ++i)
      p[i] = transform * p[i];
    return p;
  }

  template<length_t L, typename T, qualifier Q>
  static Polygon<L, T, Q> operator*(const mat<3, 4, T, Q> &transform, const Polygon<L, T, Q> &polygon) {
    Polygon<L, T, Q> p(polygon);
    for (size_t i = 0; i < p.size(); ++i)
      p[i] = transform * p[i];
    return p;
  }

  template<length_t L, typename T, qualifier Q>
  static Polygon<L, T, Q> operator*(const mat<4, 3, T, Q> &transform, const Polygon<L, T, Q> &polygon) {
    Polygon<L, T, Q> p(polygon);
    for (size_t i = 0; i < p.size(); ++i)
      p[i] = transformPos(transform, p[i]);
    return p;
  }

  template<length_t L, typename T, qualifier Q>
  static Polygon<L, T, Q> operator*(const mat<4, 4, T, Q> &transform, const Polygon<L, T, Q> &polygon) {
    Polygon<L, T, Q> p(polygon);
    for (size_t i = 0; i < p.size(); ++i)
      p[i] = transformPos(transform, p[i]);
    return p;
  }

  template<length_t L, typename T, qualifier Q>
  static Polygon<L, T, Q> operator*(const qua<T, Q> &transform, const Polygon<L, T, Q> &polygon) {
    Polygon<L, T, Q> p(polygon);
    for (size_t i = 0; i < p.size(); ++i)
      p[i] = transform * p[i];
    return p;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER size_t length(const Polygon<L, T, Q> &polygon) {
    return polygon.size();
  }

  /// <summary>
  /// Returns a vertex of this polygon, [0, length(polygon) - 1]
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> vertex(const Polygon<L, T, Q> &polygon, size_t i) {
    if (polygon.size() == 0 || i >= polygon.size())
      return vec<L, T, Q>(T(0));
    return polygon[i];
  }

  /// <summary>
  /// Return a line segment between two adjacent vertices of the polygon.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER LineSegment<L, T, Q> edge(const Polygon<L, T, Q> &polygon, size_t i) {
    if (polygon.size() == 0 || i >= polygon.size())
      return LineSegment<L, T, Q>();
    if (polygon.size() == 1)
      return LineSegment<L, T, Q>(polygon[0], polygon[0]);
    return LineSegment<L, T, Q>(polygon[i], polygon[(i + 1) % polygon.size()]);
  }

  /// <summary>
  /// Return a line segment between two adjacent vertices of the polygon, in the
  /// local space of the polygon.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER LineSegment<2, T, Q> edge2d(const Polygon<3, T, Q> &polygon, size_t i) {
    if (polygon.size() == 0 || i >= polygon.size())
      return LineSegment<2, T, Q>();
    if (polygon.size() == 1)
      return LineSegment<2, T, Q>(vec<2, T, Q>(T(0)), vec<2, T, Q>(T(0)));
    return LineSegment<2, T, Q>(mapTo2D(polygon, i), mapTo2D(polygon, (i + 1) % polygon.size()));
  }

  /// <summary>
  /// Return the normal vector of the given edge, i.e., the vector perpendicular to
  /// the plane the polygon lies in.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> edgeNormal(const Polygon<L, T, Q> &polygon, size_t idx) {
    return normalize(cross(edge(polygon, idx).dir(), normalCCW(polygon)));
  }

  /// <summary>
  /// Return the normal plane of the given edge.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Plane<L, T, Q> edgePlane(const Polygon<L, T, Q> &polygon, size_t idx) {
    return Plane<L, T, Q>(edge(polygon, idx).a, edgeNormal(polygon, idx));
  }

  /// <summary>
  /// Compute an extreme point along the polygon, i.e., the furthest point in a
  /// given direction.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> extremePoint(const Polygon<L, T, Q> &polygon, const vec<L, T, Q> &direction, T &projectionDistance) {
    projectionDistance = -std::numeric_limits<T>::infinity();

    vec<L, T, Q> mostExtreme(T(0));
    for (size_t i = 0; i < polygon.size(); ++i) {
      const T d = dot(direction, polygon[i]);
      if (d > projectionDistance) {
        projectionDistance = d;
        mostExtreme = polygon[i];
      }
    }
    return mostExtreme;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> extremePoint(const Polygon<L, T, Q> &polygon, const vec<L, T, Q> &direction) {
    T projectionDistance(0);
    return extremePoint(polygon, direction, projectionDistance);
  }

  /// <summary>
  /// Project the polygon onto the provided axis.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER void projectToAxis(const Polygon<L, T, Q> &polygon, const vec<L, T, Q> &direction, T &outMin, T &outMax) {
    outMin = dot(extremePoint(polygon, -direction), direction);
    outMax = dot(extremePoint(polygon, direction), direction);
  }

  /// <summary>
  /// Tests whether the diagonal that joins the two given vertices lie inside
  /// the polygon and is not intersected by edges of the polygon.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE bool diagonalExists(const Polygon<L, T, Q> &polygon, size_t i, size_t j) {
    if (i > j) {  // Ensure "i" is the minimal index.
      const size_t tmp = i;
      i = j;
      j = tmp;
    }

    if (polygon.size() < 3 || i == j)  // Degenerate if i == j.
      return false;
    else if (i >= polygon.size() || j >= polygon.size())
      return false;
    else if (i + 1 == j)  // Is this LineSegment an edge of this polygon?
      return false;

    GLM_GEOM_ASSUME(isPlanar(polygon), false);
    const Plane<L, T, Q> polygonPlane = planeCCW(polygon);
    const LineSegment<L, T, Q> diag = project(polygonPlane, LineSegment<L, T, Q>(polygon[i], polygon[j]));

    // First check that this diagonal line is not intersected by an edge of this polygon.
    for (size_t k = 0; k < polygon.size(); ++k) {
      if (!(k == i || k + 1 == i || k == j)) {
        const LineSegment<L, T, Q> d = project(polygonPlane, LineSegment<L, T, Q>(polygon[k], polygon[k + 1]));
        if (intersects(d, diag))
          return false;
      }
    }

    return isConvex(polygon);
  }

  /// <summary>
  /// Returns the diagonal (segment) that joins the two given vertices of the
  /// polygon. If |i - j| == 1, then an edge of the polygon is returned.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER LineSegment<L, T, Q> diagonal(const Polygon<L, T, Q> &polygon, size_t i, size_t j) {
    const vec<L, T, Q> a = i < polygon.size() ? polygon[i] : vec<L, T, Q>(T(0));
    const vec<L, T, Q> b = j < polygon.size() ? polygon[j] : vec<L, T, Q>(T(0));
    return LineSegment<L, T, Q>(a, b);
  }

  /// <summary>
  /// Generates the U-vector (i.e., local space "x" axis) of the polygon.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<3, T, Q> basisU(const Polygon<3, T, Q> &polygon) {
    if (polygon.size() < 2)
      return glm::unit::right<T, Q>();
    return normalize(polygon[1] - polygon[0]);
  }

  /// <summary>
  /// Generates the V-vector (i.e., local-space "y" axis) of the polygon.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<3, T, Q> basisV(const Polygon<3, T, Q> &polygon) {
    if (polygon.size() < 2)
      return glm::unit::up<T, Q>();
    return normalize(cross(normalCCW(polygon), basisU(polygon)));
  }

  /// <summary>
  /// Maps the given (world) space point to the local 2D space of the polygon.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<2, T, Q> mapTo2D(const Polygon<3, T, Q> &polygon, const vec<3, T, Q> &point) {
    const vec<3, T, Q> bu = basisU(polygon);
    const vec<3, T, Q> bv = basisV(polygon);
    const vec<3, T, Q> pt = point - ((polygon.size() == 0) ? vec<3, T, Q>(T(0)) : polygon[0]);
    return vec<2, T, Q>(dot(pt, bu), dot(pt, bv));
  }

  /// <summary>
  /// Map the given vertex to the local 2D space of the polygon.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<2, T, Q> mapTo2D(const Polygon<3, T, Q> &polygon, size_t i) {
    return (i < polygon.size()) ? mapTo2D(polygon, polygon[i]) : vec<2, T, Q>(T(0));
  }

  /// <summary>
  /// Map the given local 2D space coordinate to a 3D point world space coordinate.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<3, T, Q> mapFrom2D(const Polygon<3, T, Q> &polygon, const vec<2, T, Q> &point) {
    if (polygon.size() == 0)
      return vec<3, T, Q>(T(0));
    return polygon[0] + point.x * basisU(polygon) + point.y * basisV(polygon);
  }

  /// <summary>
  /// Return the surface area of the polygon.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T area(const Polygon<L, T, Q> &polygon) {
    vec<L, T, Q> area(0);
    if (polygon.size() <= 2)
      return T(0);

    GLM_GEOM_ASSUME(isPlanar(polygon), T(0));
    size_t i = polygon.size() - 1;
    for (size_t j = 0; j < polygon.size(); ++j) {
      area += cross(polygon[i], polygon[j]);
      i = j;
    }
    return abs(dot(normalCCW(polygon), area)) * T(0.5);
  }

  /// <summary>
  /// Return the total edge length of the polygon.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER T perimeter(const Polygon<L, T, Q> &polygon) {
    T perimeter = T(0);
    for (size_t i = 0; i < polygon.size(); ++i)
      perimeter += length(edge(polygon, i));
    return perimeter;
  }

  /// <summary>
  /// Return the center of mass of the polygon.
  ///
  /// Per MathGeoLib: This function does not properly compute the centroid.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> centroid(const Polygon<L, T, Q> &polygon) {
    if (polygon.size() == 0)
      return vec<L, T, Q>(T(0));

    vec<L, T, Q> centroid(T(0));
    for (const typename Polygon<L, T, Q>::point_type &p : polygon)
      centroid += p;
    return centroid / static_cast<T>(polygon.size());
  }

  /// <summary>
  /// Tests if the polygon is planar, i.e., all of its vertices lie on the same
  /// plane.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE bool isPlanar(const Polygon<L, T, Q> &polygon, T epsSq = epsilon<T>()) {
    if (polygon.size() == 0)
      return false;
    else if (polygon.size() <= 3)
      return true;

    const vec<L, T, Q> normal = cross(polygon[1] - polygon[0], polygon[2] - polygon[0]);
    const T lenSq = length2(normal);
    for (size_t i = 3; i < polygon.size(); ++i) {
      const T d = dot(normal, polygon[i] - polygon[0]);
      if (d * d > epsSq * lenSq) {
        return false;
      }
    }
    return true;
  }

  /// <summary>
  /// Tests if the polygon is simple, i.e., no two non-consecutive edges have a
  /// point in common.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE bool isSimple(const Polygon<L, T, Q> &polygon) {
    GLM_GEOM_ASSUME(isPlanar(polygon), false);

    const size_t p_size = polygon.size();
    const Plane<L, T, Q> plane = planeCCW(polygon);
    for (size_t i = 0; i < p_size; ++i) {
      const LineSegment<L, T, Q> si = project(plane, edge(polygon, i));

      for (size_t j = i + 2; j < p_size; ++j) {
        if (i == 0 && j == p_size - 1)
          continue;

        const LineSegment<L, T, Q> sj = project(plane, edge(polygon, j));
        if (intersects(si, sj))
          return false;
      }
    }
    return true;
  }

  /// <summary>
  /// Tests if the polygon is null, i.e., has no vertices.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isNull(const Polygon<L, T, Q> &polygon) {
    return polygon.size() == 0;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isfinite(const Polygon<L, T, Q> &polygon) {
    if (polygon.size() == 0)
      return true;

    for (const typename Polygon<L, T, Q>::point_type &p : polygon) {
      if (!all(isfinite(p)))
        return false;
    }
    return true;
  }

  /// <summary>
  /// Return true if the polygon is degenerate:
  ///   1. It has two-or-less vertices;
  ///   2. its surface area is less or equal than a given epsilon
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool isDegenerate(const Polygon<L, T, Q> &polygon, T eps = epsilon<T>()) {
    return polygon.size() < 3 || area(polygon) <= eps;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool orientedCCW(const vec<2, T, Q> &a, const vec<2, T, Q> &b, const vec<2, T, Q> &c) {
    return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x) >= T(0);
  }

  /// <summary>
  /// Tests whether the polygon is convex, i.e., for each pair of points inside
  /// the polygon, the segment joining those points is also completely inside
  /// the polygon.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE bool isConvex(const Polygon<3, T, Q> &polygon) {
    const size_t size = polygon.size();
    if (size == 0)
      return false;
    else if (size <= 3)
      return true;

    GLM_GEOM_ASSUME(isPlanar(polygon), false);
    size_t i = size - 2;
    size_t j = size - 1;
    size_t k = 0;
    while (k < size) {
      const vec<2, T, Q> a = mapTo2D(polygon, i);
      const vec<2, T, Q> b = mapTo2D(polygon, j);
      const vec<2, T, Q> c = mapTo2D(polygon, k);
      if (!orientedCCW(a, b, c))
        return false;

      i = j;
      j = k;
      ++k;
    }
    return true;
  }

  /// <summary>
  /// Computes a point on the perimeter of this polygon.
  /// </summary>
  /// <param name="dist">A value between [0, 1] corresponding to a relative location along the polygons perimeter.</param>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE vec<L, T, Q> pointOnEdge(const Polygon<L, T, Q> &polygon, const T dist) {
    if (polygon.size() == 0)
      return vec<L, T, Q>();
    else if (polygon.size() < 2)
      return polygon[0];

    T d = perimeter(polygon) * (dist - floor(dist));
    for (size_t i = 0; i < polygon.size(); ++i) {
      const LineSegment<L, T, Q> e = edge(polygon, i);
      const T len = length(e);
      if (epsilonEqual(len, T(0), epsilon<T>()))
        return vec<L, T, Q>();  // degenerate polygon

      if (d <= len)
        return getPoint(e, d / len);
      d -= len;
    }
    return polygon[0];  // LOG: Should not reach this.
  }

  /// <summary>
  /// Computes the plane the polygon is contained in.
  ///
  /// The normal vector of the plane points to the direction from which the
  /// vertices wind in counter-clockwise order.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE Plane<3, T, Q> planeCCW(const Polygon<3, T, Q> &polygon) {
    const size_t p_size = polygon.size();
    const vec<3, T, Q> hint = unit::forward<T, Q>();
    const vec<3, T, Q> hint2 = unit::up<T, Q>();
    if (p_size > 3) {
      Plane<3, T, Q> plane;
      for (size_t i = 0; i < p_size - 2; ++i) {
        for (size_t j = i + 1; j < p_size - 1; ++j) {
          const vec<3, T, Q> pij = polygon[j] - polygon[i];

          for (size_t k = j + 1; k < p_size; ++k) {
            plane.normal = cross(pij, polygon[k] - polygon[i]);

            const T lenSq = length2(plane.normal);
            if (lenSq > epsilon<T>()) {
              plane.normal /= sqrt(lenSq);
              plane.d = dot(plane.normal, polygon[i]);
              return plane;
            }
          }
        }
      }

      const vec<3, T, Q> dir = normalize(polygon[1] - polygon[0]);  // Collinear points cannot form a plane.
      return planeFrom(Line<3, T, Q>(polygon[0], dir), perpendicular(dir, hint, hint2));
    }

    if (p_size == 3)
      return planeFrom(polygon[0], polygon[1], polygon[2]);
    else if (p_size == 2) {
      const vec<3, T, Q> dir = normalize(polygon[1] - polygon[0]);
      return planeFrom(Line<3, T, Q>(polygon[0], dir), perpendicular(dir, hint, hint2));
    }
    else if (p_size == 1)
      return planeFrom(polygon[0], unit::up<T, Q>());
    else
      return Plane<3, T, Q>();
  }

  /// <summary>
  /// Compute the normal of the polygon in the counter-clockwise direction.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> normalCCW(const Polygon<L, T, Q> &polygon) {
    return planeCCW(polygon).normal;
  }

  /// <summary>
  /// Computes the (clockwise, i.e., normal vector points in the clockwise
  /// direction) plane this polygon is contained in.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER Plane<3, T, Q> planeCW(const Polygon<L, T, Q> &polygon) {
    return reverseNormal(planeCCW(polygon));
  }

  /// <summary>
  /// Compute the normal of the polygon in the clockwise direction.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER vec<L, T, Q> normalCW(const Polygon<L, T, Q> &polygon) {
    return planeCW(polygon).normal;
  }

  /// <summary>
  /// Return the smallest AABB that encloses the polygon.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER AABB<L, T, Q> minimalEnclosingAABB(const Polygon<L, T, Q> &polygon) {
    if (polygon.size() == 0)
      return AABB<L, T, Q>();

    AABB<L, T, Q> aabb;
    aabb.setNegativeInfinity();
    for (const typename Polygon<L, T, Q>::point_type &p : polygon)
      aabb.enclose(p);
    return aabb;
  }

  // Tests if the given object (worldspace) is fully contained inside the polygon.

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE bool contains(const Polygon<3, T, Q> &polygon, const vec<3, T, Q> &worldSpacePoint, PolyContains type, T thickness = epsilon<T>()) {
    if (polygon.size() < 3)
      return false;

    const vec<3, T, Q> bu = basisU(polygon);
    const vec<3, T, Q> bv = basisV(polygon);
    if (!isNormalized(bu, epsilon<T>()) || !isNormalized(bv, epsilon<T>()) || !isPerpendicular(bu, bv))
      return false;

    const T pdelt = dot(cross(bu, bv), polygon[0] - worldSpacePoint);
    bool contains = true;
    switch (type) {  // Check if the point is within the plane of the polygon
      case PolyContains::Positive:
        contains = pdelt >= 0 && (pdelt * pdelt) <= (thickness * thickness);
        break;
      case PolyContains::Negative:
        contains = pdelt <= 0 && (pdelt * pdelt) <= (thickness * thickness);
        break;
      case PolyContains::Unidirectional:
      default: {
        contains = (T(0.25) * (pdelt * pdelt)) <= (thickness * thickness);
        break;
      }
    }

    if (!contains)
      return false;

    // Crossings Test
    const T eps = epsilon<T>();
    vec<3, T, Q> vt = polygon.back() - worldSpacePoint;
    vec<2, T, Q> p0 = vec<2, T, Q>(dot(vt, bu), dot(vt, bv));
    if (abs(p0.y) < eps)
      p0.y = -eps;

    size_t numIntersections = 0;
    for (size_t i = 0; i < polygon.size(); ++i) {
      vt = polygon[i] - worldSpacePoint;

      vec<2, T, Q> p1 = vec<2, T, Q>(dot(vt, bu), dot(vt, bv));
      if (abs(p1.y) < eps)
        p1.y = -eps;

      if (p0.y * p1.y < T(0)) {
        if (min(p0.x, p1.x) > T(0))
          ++numIntersections;
        else if (max(p0.x, p1.x) > T(0)) {
          const vec<2, T, Q> delta = p1 - p0;
          if (delta.y != T(0)) {  // @TODO: notEqual(delta.y, T(0), epsilon<T>());
            const T t = -p0.y / delta.y;
            const T x = p0.x + t * delta.x;
            if (t >= T(0) && t <= T(1) && x > T(0)) {
              ++numIntersections;
            }
          }
        }
      }
      p0 = p1;
    }

    return (numIntersections % 2) == 1;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE bool contains(const Polygon<3, T, Q> &polygon, const vec<3, T, Q> &worldSpace, T polygonThickness = epsilon<T>()) {
    return contains(polygon, worldSpace, PolyContains::Unidirectional, polygonThickness);
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE bool containsAbove(const Polygon<3, T, Q> &polygon, const vec<3, T, Q> &worldSpace, T polygonThickness = epsilon<T>()) {
    return contains(polygon, worldSpace, PolyContains::Positive, polygonThickness);
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE bool containsBelow(const Polygon<3, T, Q> &polygon, const vec<3, T, Q> &worldSpace, T polygonThickness = epsilon<T>()) {
    return contains(polygon, worldSpace, PolyContains::Negative, polygonThickness);
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE bool contains2D(const Polygon<3, T, Q> &polygon, const LineSegment<3, T, Q> &localLineSegment) {
    if (polygon.size() < 3)
      return false;

    const vec<3, T, Q> bu = basisU(polygon);
    const vec<3, T, Q> bv = basisV(polygon);

    LineSegment<3, T, Q> edge;
    edge.a = vec<3, T, Q>(dot(polygon.back(), bu), dot(polygon.back(), bv), T(0));
    for (const typename Polygon<3, T, Q>::point_type &p : polygon) {
      edge.b = vec<3, T, Q>(dot(p, bu), dot(p, bv), T(0));
      if (intersects(edge, localLineSegment))
        return false;
      edge.a = edge.b;
    }

    // Entire segment is fully inside or outside the polygon, determine which.
    return contains(polygon, mapFrom2D(polygon, vec<2, T, Q>(localLineSegment.a.x, localLineSegment.a.y)));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Polygon<L, T, Q> &polygon, const Polygon<L, T, Q> &worldSpacePolygon, T polygonThickness = epsilon<T>()) {
    if (polygon.size() == 0)
      return false;

    for (const typename Polygon<L, T, Q>::point_type &p : worldSpacePolygon) {
      if (!contains(polygon, p, polygonThickness))
        return false;
    }
    return true;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Polygon<3, T, Q> &polygon, const LineSegment<3, T, Q> &worldSpaceLineSegment, T polygonThickness = epsilon<T>()) {
    if (polygon.size() < 3)
      return false;

    const Plane<3, T, Q> plane = planeCCW(polygon);
    if (distance(plane, worldSpaceLineSegment.a) > polygonThickness
        || distance(plane, worldSpaceLineSegment.b) > polygonThickness) {
      return false;
    }

    // For robustness, project onto the polygon plane.
    const LineSegment<3, T, Q> l = project(plane, worldSpaceLineSegment);
    if (!contains(polygon, l.a) || !contains(polygon, l.b))
      return false;

    for (size_t i = 0; i < polygon.size(); ++i) {
      if (intersects(project(plane, edge(polygon, i)), l))
        return false;
    }
    return true;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool contains(const Polygon<3, T, Q> &polygon, const Triangle<3, T, Q> &worldSpaceTriangle, T polygonThickness = epsilon<T>()) {
    return contains(polygon, edge(worldSpaceTriangle, 0), polygonThickness)
           && contains(polygon, edge(worldSpaceTriangle, 1), polygonThickness)
           && contains(polygon, edge(worldSpaceTriangle, 2), polygonThickness);
  }

  /// Tests whether the polygon and the given object intersect.

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Polygon<L, T, Q> &polygon, const Line<L, T, Q> &line) {
    T d(0);
    const Plane<L, T, Q> plane = planeCCW(polygon);
    return intersects(plane, line, d) ? contains(polygon, getPoint(line, d)) : false;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Polygon<L, T, Q> &polygon, const Ray<L, T, Q> &ray) {
    T d(0);
    const Plane<L, T, Q> plane = planeCCW(polygon);
    return intersects(plane, ray, d) ? contains(polygon, getPoint(ray, d)) : false;
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE bool intersects2D(const Polygon<3, T, Q> &polygon, const LineSegment<3, T, Q> &localSpaceLineSegment) {
    if (polygon.size() < 3)
      return false;

    const vec<3, T, Q> bu = basisU(polygon);
    const vec<3, T, Q> bv = basisV(polygon);

    LineSegment<3, T, Q> edge;
    edge.a = vec<3, T, Q>(dot(polygon.back(), bu), dot(polygon.back(), bv), T(0));
    for (size_t i = 0; i < polygon.size(); ++i) {
      edge.b = vec<3, T, Q>(dot(polygon[i], bu), dot(polygon[i], bv), 0);
      if (intersects(edge, localSpaceLineSegment))
        return true;
      edge.a = edge.b;
    }

    // Entire segment is fully inside or outside the polygon, determine which.
    return contains(polygon, mapFrom2D(polygon, vec<2, T, Q>(localSpaceLineSegment.a.x, localSpaceLineSegment.a.y)));
  }

  template<typename T, qualifier Q>
  GLM_GEOM_QUALIFIER_NOINLINE bool intersects(const Polygon<3, T, Q> &polygon, const LineSegment<3, T, Q> &line) {
    const Plane<3, T, Q> plane = planeCCW(polygon);
    const T denom = dot(plane.normal, line.b - line.a);  // Compute line-plane intersection
    if (abs(denom) < epsilon<T>()) {  // plane & segment are planar
      const vec<2, T, Q> a = mapTo2D(polygon, line.a);
      const vec<2, T, Q> b = mapTo2D(polygon, line.b);
      const LineSegment<3, T, Q> segment(vec<3, T, Q>(a.x, a.y, T(0)), vec<3, T, Q>(b.x, b.y, T(0)));
      return intersects2D(polygon, segment);
    }

    // The line segment properly intersects the plane of the polygon
    const T t = (plane.d - dot(plane.normal, line.a)) / denom;
    return (t >= T(0) && t <= T(1)) ? contains(polygon, getPoint(line, t)) : false;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_GEOM_QUALIFIER bool intersects(const Polygon<L, T, Q> &polygon, const Plane<L, T, Q> &plane) {
    if (polygon.size() == 0)
      return false;

    // Project the points of this polygon onto the plane normal. If there are
    // points on both sides of the plane, then the polygon intersects the plane.
    T minD = std::numeric_limits<T>::infinity();
    T maxD = -std::numeric_limits<T>::infinity();
    for (const typename Polygon<L, T, Q>::point_type &p : polygon) {
      const T d = signedDistance(plane, p);
      minD = min(minD, d);
      maxD = max(maxD, d);
    }

    // Allow a very small (epsilon) tolerance.
    return minD <= epsilon<T>() && maxD >= -epsilon<T>();
  }

  namespace detail {
#if GLM_GEOM_TOSTRING
    template<length_t L, typename T, qualifier Q>
    struct compute_to_string<Polygon<L, T, Q>> {
      GLM_GEOM_QUALIFIER std::string call(const Polygon<L, T, Q> &polygon) {
        ((void)polygon);
        return std::string("Polygon");
      }
    };
#endif
  }
}
#endif
