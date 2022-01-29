/*
** $Id: vector_extensions.hpp $
**
** Vector-specific extensions to GLM:
**  1. API-completing functions, usually handling cases of functions without
**     genType or vec<1, genType> declarations;
**  2. Vector support for C99/C++11 <math> functions;
**  3. Functions emulated/ported from other popular vector-math libraries.
**
** See Copyright Notice in lua.h
*/
#ifndef EXT_EXTENSION_VECTOR_HPP
#define EXT_EXTENSION_VECTOR_HPP
#if !defined(GLM_ENABLE_EXPERIMENTAL)
  #define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/vector_angle.hpp>
#if GLM_VERSION < 998  // @COMPAT: ext/scalar_common.hpp introduced in 0.9.9.8
  #include <glm/gtx/extended_min_max.hpp>
#elif GLM_VERSION == 998
  #include <glm/ext/scalar_common.hpp>
#endif

#include "scalar_extensions.hpp"
#if GLM_MESSAGES == GLM_ENABLE && !defined(GLM_EXT_INCLUDED)
  #pragma message("GLM: GLM_EXT_vector_ext extension included")
#endif

namespace glm {

  /// <summary>
  /// Unit vectors
  /// </summary>
  namespace unit {
    template<typename T, qualifier Q = defaultp>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<3, T, Q> right() {
      return vec<3, T, Q>(T(1), T(0), T(0));
    }

    template<typename T, qualifier Q = defaultp>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<3, T, Q> up() {
      return vec<3, T, Q>(
#if defined(GLM_FORCE_Z_UP)
      T(0), T(0), T(1)
#else
      T(0), T(1), T(0)
#endif
      );
    }

    template<typename T, qualifier Q = defaultp>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<3, T, Q> forwardLH() {
      return vec<3, T, Q>(
#if defined(GLM_FORCE_Z_UP)
      T(0), T(-1), T(0)
#else
      T(0), T(0), T(1)
#endif
      );
    }

    template<typename T, qualifier Q = defaultp>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<3, T, Q> forwardRH() {
      return vec<3, T, Q>(
#if defined(GLM_FORCE_Z_UP)
      T(0), T(1), T(0)
#else
      T(0), T(0), T(-1)
#endif
      );
    }

    template<typename T, qualifier Q = defaultp>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<3, T, Q> forward() {
#if defined(GLM_FORCE_LEFT_HANDED)
      return forwardLH<T, Q>();
#else
      return forwardRH<T, Q>();
#endif
    }
  }

  /* glm::all(glm::equal(...)) shorthand */

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_equal(vec<L, T, Q> const &x, vec<L, T, Q> const &y) {
    return all(equal(x, y));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_equal(vec<L, T, Q> const &x, vec<L, T, Q> const &y, T eps) {
    return all(equal(x, y, eps));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_equal(vec<L, T, Q> const &x, vec<L, T, Q> const &y, int MaxULPs) {
    return all(equal(x, y, MaxULPs));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_equal(vec<L, T, Q> const &x, vec<L, T, Q> const &y, vec<L, T, Q> const &eps) {
    return all(equal(x, y, eps));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_equal(vec<L, T, Q> const &x, vec<L, T, Q> const &y, vec<L, int, Q> const &MaxULPs) {
    return all(equal(x, y, MaxULPs));
  }

  /* glm::any(glm::notEqual(...)) shorthand */

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any_notequal(vec<L, T, Q> const &x, vec<L, T, Q> const &y) {
    return any(notEqual(x, y));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any_notequal(vec<L, T, Q> const &x, vec<L, T, Q> const &y, T eps) {
    return any(notEqual(x, y, eps));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any_notequal(vec<L, T, Q> const &x, vec<L, T, Q> const &y, int MaxULPs) {
    return any(notEqual(x, y, MaxULPs));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any_notequal(vec<L, T, Q> const &x, vec<L, T, Q> const &y, vec<L, T, Q> const &eps) {
    return any(notEqual(x, y, eps));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any_notequal(vec<L, T, Q> const &x, vec<L, T, Q> const &y, vec<L, int, Q> const &MaxULPs) {
    return any(notEqual(x, y, MaxULPs));
  }

  /* glm::all(glm::lessThan | glm::greaterThan | glm::lessThanEqual | glm::greaterThanEqual) */

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_lessThan(vec<L, T, Q> const &x, vec<L, T, Q> const &y) {
    return all(lessThan(x, y));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_lessThanEqual(vec<L, T, Q> const &x, vec<L, T, Q> const &y) {
    return all(lessThanEqual(x, y));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_greaterThan(vec<L, T, Q> const &x, vec<L, T, Q> const &y) {
    return all(greaterThan(x, y));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_greaterThanEqual(vec<L, T, Q> const &x, vec<L, T, Q> const &y) {
    return all(greaterThanEqual(x, y));
  }

  /* glm::any(glm::isinf(...)) shorthand */

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any_isinf(const vec<L, T, Q> &x) {
    return any(isinf(x));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_isfinite(const vec<L, T, Q> &x) {
    return all(isfinite(x));
  }

  /* glm::any(glm::isnan(...)) shorthand */

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any_isnan(const vec<L, T, Q> &x) {
    return any(isnan(x));
  }

  /*
  ** The other useful sign() implementation: where >= 0 returns +1. This
  ** implementation avoids using detail::functor.
  */

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> signP(const vec<L, T, Q> &x) {
    return vec<L, T, Q>(lessThanEqual(vec<L, T, Q>(0), x)) - vec<L, T, Q>(lessThan(x, vec<L, T, Q>(0)));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> signN(const vec<L, T, Q> &x) {
    return vec<L, T, Q>(lessThan(vec<L, T, Q>(0), x)) - vec<L, T, Q>(lessThanEqual(x, vec<L, T, Q>(0)));
  }

  /* API Completeness */

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> fclamp(vec<L, T, Q> const &x) {
    return fclamp(x, T(0), T(1));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> ceilMultiple(vec<L, T, Q> const &Source, T const &Multiple) {
    return ceilMultiple(Source, vec<L, T, Q>(Multiple));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> floorMultiple(vec<L, T, Q> const &Source, T const &Multiple) {
    return floorMultiple(Source, vec<L, T, Q>(Multiple));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> roundMultiple(vec<L, T, Q> const &Source, T const &Multiple) {
    return roundMultiple(Source, vec<L, T, Q>(Multiple));
  }

  /* Numeric extensions */

  /// <summary>
  /// Return true if all vector elements are identical/equal.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER bool isUniform(vec<L, T, Q> const &v, const T eps = epsilon<T>()) {
    bool result = true;
    for (length_t i = 1; i < L; ++i)
      result &= equal(v[i], v[0], eps);
    return result;
  }

  /// <summary>
  /// Reverse the elements of a vector
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> reverse(vec<L, T, Q> const &v) {
    vec<L, T, Q> result;
    for (length_t i = 0; i < L; ++i)
      result[i] = v[L - i - 1];
    return result;
  }

  /// <summary>
  /// calculate sin and cos simultaneously.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER void sincos(vec<L, T, Q> const &v, vec<L, T, Q> &s, vec<L, T, Q> &c) {
    s = sin(v);
    c = cos(v);
  }

  /// <summary>
  /// Create a normalized vec2 from an angle (in radians).
  /// </summary>
  template<typename T, qualifier Q = glm::defaultp>
  GLM_FUNC_QUALIFIER vec<2, T, Q> fromAngle(T angle) {
    T sin, cos;
    sincos(angle, sin, cos);
    return vec<2, T, Q>(sin, cos);
  }

  /// <summary>
  /// Return a copy of the vector "v" with its length clamped to "maxLength"
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> clampLength(const vec<L, T, Q> &v, T maxLength) {
    return (length2(v) > (maxLength * maxLength)) ? (normalize(v) * maxLength) : v;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType clampLength(genType x, genType maxLength) {
    return clampLength(vec<1, genType>(x), maxLength).x;
  }

  /// <summary>
  /// Scales the length of vector "v" to "newLength".
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> scaleLength(const vec<L, T, Q> &v, T newLength) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'scaleLength' only accept floating-point inputs");

    const T sqlen = length2(v);
    if (sqlen < epsilon<T>()) {
      vec<L, T, Q> result(T(0));
      result[0] = newLength;
      return result;
    }
    return v * (newLength / sqrt(sqlen));
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType scaleLength(genType x, genType newLength) {
    return scaleLength(vec<1, genType>(x), newLength).x;
  }

  /// <summary>
  /// Returns the homogenized vector: divides all components by w.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<3, T, Q> homogenize(const vec<4, T, Q> &v) {
    return vec<3, T, Q>(v.x / v.w, v.y / v.w, v.z / v.w);
  }

  /// <summary>
  /// Dot product of two vectors using only the x,y,z components.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER T dot(const vec<4, T, Q> &x, const vec<3, T, Q> &y) {
    return dot(vec<3, T, Q>(x.x, x.y, x.z), y);
  }

  /* Cross product with specific axis */

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<3, T, Q> crossXAxis(const vec<3, T, Q> &v) {
    return vec<3, T, Q>(0.0f, v.z, -v.y);
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<3, T, Q> crossYAxis(const vec<3, T, Q> &v) {
    return vec<3, T, Q>(-v.z, 0.0f, v.x);
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<3, T, Q> crossZAxis(const vec<3, T, Q> &v) {
    return vec<3, T, Q>(v.y, -v.x, 0.0f);
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<3, T, Q> xAxisCross(const vec<3, T, Q> &v) {
    return vec<3, T, Q>(0.0f, -v.z, v.y);
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<3, T, Q> yAxisCross(const vec<3, T, Q> &v) {
    return vec<3, T, Q>(v.z, 0.0f, -v.x);
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<3, T, Q> zAxisCross(const vec<3, T, Q> &v) {
    return vec<3, T, Q>(-v.y, v.x, 0.0f);
  }

  /// <summary>
  /// Return true if two vectors are perpendicular to each other.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER bool isPerpendicular(const vec<L, T, Q> &v, const vec<L, T, Q> &other, T epsSq = epsilon<T>()) {
    const T d = dot(v, other);
    return d * d <= epsSq * length2(v) * length2(other);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER bool isPerpendicular(genType x, genType y, const genType eps = epsilon<genType>()) {
    return isPerpendicular(vec<1, genType>(x), vec<1, genType>(y), eps);
  }

  /// <summary>
  /// Return a normalized (direction) vector that is perpendicular to "v" and
  /// the provided "hint" vectors. If "v" points towards "hint", then "hint2" is
  /// used as a fall-back.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<3, T, Q> perpendicular(const vec<3, T, Q> &v, const vec<3, T, Q> &hint = unit::forward<T, Q>(), const vec<3, T, Q> &hint2 = unit::up<T, Q>()) {
    const vec<3, T, Q> v2 = cross(v, hint);
    return epsilonEqual(dot(v2, v2), T(0), epsilon<T>()) ? hint2 : normalize(v2);
  }

  /// <summary>
  /// Return a vector that is perpendicular to "v" and the vector returned by
  /// glm::perpendicular.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<3, T, Q> perpendicular2(const vec<3, T, Q> &v, const vec<3, T, Q> &hint = unit::forward<T, Q>(), const vec<3, T, Q> &hint2 = unit::up<T, Q>()) {
    return normalize(cross(v, perpendicular(v, hint, hint2)));
  }

  /// <summary>
  /// Computes two vectors "out" and "out2" that are orthogonal to "v" and to
  /// each other.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER void perpendicularBasis(const vec<3, T, Q> &v, vec<3, T, Q> &out, vec<3, T, Q> &out2) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'perpendicularBasis' only accept floating-point inputs");
    const T s = v.z >= T(0) ? T(1) : T(-1);
    const T a = T(-1) / (s + v.z);
    const T b = v.x * v.y * a;

    out = vec<3, T, Q>(T(1) + s * v.x * v.x * a, s * b, -s * v.x);
    out2 = vec<3, T, Q>(b, s + v.y * v.y * a, -v.y);
  }

  /// <summary>
  /// From_Z
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<3, T, Q> perpendicularFast(const vec<3, T, Q> &v) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'perpendicularFast' only accept floating-point inputs");
    if (abs(v.z) > one_over_root_two<T>()) {  // YZ Plane
      const T k = 1 / sqrt(v.y * v.y + v.z * v.z);
      return vec<3, T, Q>(T(0), -v.z * k, v.y * k);
    }
    else {  // XY Plane
      const T k = 1 / sqrt(v.x * v.x + v.y * v.y);
      return vec<3, T, Q>(-v.y * k, v.x * k, T(0));
    }
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<2, T, Q> perpendicularFast(const vec<2, T, Q> &v) {
    return vec<2, T, Q>(-v.y, v.x);
  }

  /// <summary>
  /// Make the vectors normalized and orthogonal to one another
  ///
  /// A mutable glm::orthonormalize implementation.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER void orthonormalize2(vec<3, T, Q> &x, vec<3, T, Q> &y) {
    x = normalize(x);
    y = orthonormalize(y, x);
  }

  /// <summary>
  /// Make the vectors normalized and orthogonal to one another
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER void orthonormalize3(vec<3, T, Q> &x, vec<3, T, Q> &y, vec<3, T, Q> &z) {
    x = normalize(x);
    y = orthonormalize(y, x);

    const T dot0 = dot(x, z);
    const T dot1 = dot(y, z);
    z = normalize(z - (y * dot1 + x * dot0));
  }

  /// <summary>
  /// glm::proj with the assumption "Normal" is already normalized.
  /// </summary>
  template<typename genType>
  GLM_FUNC_QUALIFIER genType projNorm(genType const &x, genType const &Normal) {
    return dot(x, Normal) * Normal;
  }

  /// <summary>
  /// Project a vector onto this plane defined by its normal orthogonal
  /// </summary>
  template<typename genType>
  GLM_FUNC_QUALIFIER genType projPlane(genType const &x, genType const &Normal) {
    return x - proj(x, Normal);
  }

  /// <summary>
  /// Breaks this vector down into parallel and perpendicular components with respect to the given direction
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER void projDecompose(const vec<L, T, Q> &v, const vec<L, T, Q> &direction, vec<L, T, Q> &outParallel, vec<L, T, Q> &outPerpendicular) {
    outParallel = proj(v, direction);
    outPerpendicular = v - outParallel;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER void projDecompose(genType v, genType direction, genType &outParallel, genType &outPerpendicular) {
    vec<1, genType> vParallel, vPerpendicular;
    projDecompose(vec<1, genType>(v), vec<1, genType>(direction), vParallel, vPerpendicular);

    outParallel = vParallel.x;
    outPerpendicular = vPerpendicular.x;
  }

  /// <summary>
  /// Return true if the three given points are collinear, i.e., lie on the same line.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER bool areCollinear(const vec<L, T, Q> &p1, const vec<L, T, Q> &p2, const vec<L, T, Q> &p3, T epsSq = epsilon<T>()) {
    return length2(cross(p2 - p1, p3 - p1)) <= epsSq;
  }

  /// <summary>
  /// </summary>
  /// <param name="negativeSideRefractionIndex">Refraction index of material exiting</param>
  /// <param name="positiveSideRefractionIndex">Refraction index of material entering</param>
  /// <returns></returns>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> refract(vec<L, T, Q> const &I, vec<L, T, Q> const &N, T negativeSideRefractionIndex, T positiveSideRefractionIndex) {
    return refract(I, N, negativeSideRefractionIndex / positiveSideRefractionIndex);
  }

  /// <summary>
  /// Return a vector containing the Cartesian coordinates of a point specified
  /// in Barycentric (relative to a N-Dimensional triangle).
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> barycentric(const vec<L, T, Q> &value1, const vec<L, T, Q> &value2, const vec<L, T, Q> &value3, T amount1, T amount2) {
    return (value1 + (amount1 * (value2 - value1))) + (amount2 * (value3 - value1));
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType barycentric(genType value1, genType value2, genType value3, genType amount1, genType amount2) {
    return barycentric(vec<1, genType>(value1), vec<1, genType>(value2), vec<1, genType>(value3), amount1, amount2).x;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> wrap(const vec<L, T, Q> &x, const vec<L, T, Q> &maxValue) {
    vec<L, T, Q> Result(T(0));
    for (length_t i = 0; i < L; ++i)
      Result[i] = wrap<T>(x[i], maxValue[i]);
    return Result;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> wrap(const vec<L, T, Q> &x, T maxValue) {
    return wrap(x, vec<L, T, Q>(maxValue));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> wrapAngle2(vec<L, T, Q> const &x) {
    return detail::functor1<vec, L, T, T, Q>::call(wrapAngle2, x);
  }

  /// <summary>
  /// Loops "t", so that it is never greater than "length" and less than zero.
  /// This function is an emulation of: Unity.Mathf.Repeat
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> loopRepeat(const vec<L, T, Q> &t, const vec<L, T, Q> &length) {
    return clamp(t - floor(t / length) * length, vec<L, T, Q>(0), length);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> loopRepeat(const vec<L, T, Q> &t, T length) {
    return loopRepeat(t, vec<L, T, Q>(length));
  }

  /// <summary>
  /// A lerp implementation that ensures values interpolate correctly when
  /// wrapped around two-pi. This function is an emulation of: Unity.Mathf.LerpAngle
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> lerpAngle(const vec<L, T, Q> &x, const vec<L, T, Q> &y, T t) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'lerpAngle' only accept floating-point inputs");

    vec<L, T, Q> Result(T(0));
    for (length_t i = 0; i < L; ++i)
      Result[i] = lerpAngle<T>(x[i], y[i], t);
    return Result;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> lerpAngle(const vec<L, T, Q> &x, const vec<L, T, Q> &y, const vec<L, T, Q> &t) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'lerpAngle' only accept floating-point inputs");

    vec<L, T, Q> Result(T(0));
    for (length_t i = 0; i < L; ++i)
      Result[i] = lerpAngle<T>(x[i], y[i], t[i]);
    return Result;
  }

  /// <summary>
  /// Return a position between two points, moving no further than maxDist. This
  /// function is an emulation of: Unity.Vector3.MoveTowards
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> moveTowards(const vec<L, T, Q> &current, const vec<L, T, Q> &target, T maxDist) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'moveTowards' only accept floating-point inputs");

    const vec<L, T, Q> delta = target - current;
    const T sqdist = dot(delta, delta);
    if (epsilonEqual(sqdist, T(0), epsilon<T>()) || (maxDist >= 0 && sqdist <= maxDist * maxDist))
      return target;

    return current + (delta / (sqrt(sqdist) * maxDist));
  }

  /// <summary>
  /// Return a rotation between two directions, rotating no further than maxRadians.
  /// This function is an emulation of: Unity.Vector3.RotateTowards
  /// </summary>
  /// <param name="current">Current direction</param>
  /// <param name="target">Desired direction</param>
  /// <param name="maxRadians">Maximum rotation (in radians) allowed for the rotation</param>
  /// <param name="maxLength">Maximum change in vector length for the rotation</param>
  /// <returns></returns>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<3, T, Q> rotateTowards(const vec<3, T, Q> &current, const vec<3, T, Q> &target, T maxRadians, T maxLength) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'rotateTowards' only accept floating-point inputs");

    const T eps = epsilon<T>();
    const T targetLen = length(target);
    const T currentLen = length(current);
    if (currentLen > eps && targetLen > eps) {
      const vec<3, T, Q> currentDir = current / currentLen;
      const vec<3, T, Q> targetDir = target / targetLen;
      const T d = dot(currentDir, targetDir);
      if (d <= (T(1.0) - eps)) {
        T delta = targetLen - currentLen;
        if (delta > T(0))
          delta = currentLen + min(delta, maxLength);
        else
          delta = currentLen - min(delta, maxLength);

        qua<T, Q> q;
        if (d < -(T(1.0) - eps))
          q = angleAxis(maxRadians, perpendicularFast(currentDir));
        else
          q = angleAxis(min(maxRadians, acos(d)), perpendicular(currentDir, targetDir));

        return q * currentDir * delta;
      }
    }

    return moveTowards(current, target, maxLength);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType rotateTowards(genType current, genType target, genType maxRadians, genType maxLength) {
    return rotateTowards(vec<1, genType>(current), vec<1, genType>(target), maxRadians, maxLength).x;
  }

  /// <summary>
  /// Changes an entities position towards a desired position over time.
  ///
  /// Note: This function is an emulation of: Unity.Vector3.SmoothDamp
  /// </summary>
  /// <param name="current">Current position</param>
  /// <param name="target">Desired position</param>
  /// <param name="currentVelocity">The current velocity of the entity</param>
  /// <param name="smoothTime">An approximation of the time it will take to reach the desired position</param>
  /// <param name="maxSpeed">Maximum entity speed.</param>
  /// <param name="deltaTime">Change in time</param>
  /// <returns></returns>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> smoothDamp(const vec<L, T, Q> &current, const vec<L, T, Q> &target, vec<L, T, Q> &currentVelocity, T smoothTime, T maxSpeed, T deltaTime) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'smoothDamp' only accept floating-point inputs");
    smoothTime = max(epsilon<T>(), smoothTime);
    vec<L, T, Q> deltaDist = (current - target);
    const T maxDist = maxSpeed * smoothTime;
    const T sqrDist = dot(deltaDist, deltaDist);

    const T o = T(2.0) / smoothTime;
    const T x = o * deltaTime;
    const T exp = T(1) / (T(1) + x + (T(0.48) * x * x) + (T(0.235) * x * x * x));
    if (sqrDist > (maxDist * maxDist))  // clamp maximum distance
      deltaDist = (deltaDist / sqrt(sqrDist)) * maxDist;

    const vec<L, T, Q> t = (currentVelocity + o * deltaDist) * deltaTime;
    vec<L, T, Q> output = (current - deltaDist) + ((deltaDist + t) * exp);

    currentVelocity = (currentVelocity - o * t) * exp;
    if (dot(target - current, output - target) > 0) {  // prevent overshoot
      currentVelocity = vec<L, T, Q>(T(0));
      output = target;
    }

    return output;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType smoothDamp(genType current, genType target, genType &currentVelocity, genType smoothTime, genType maxSpeed, genType deltaTime) {
    vec<1, genType> cv(currentVelocity);
    const vec<1, genType> result = smoothDamp(vec<1, genType>(current), vec<1, genType>(target), cv, smoothTime, maxSpeed, deltaTime);
    currentVelocity = cv.x;
    return result.x;
  }

  /// <summary>
  /// Note: Mouse coordinates must be scaled to: [-1, 1].
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<3, T, Q> rayPicking(const vec<3, T, Q> &cam_direction, const vec<3, T, Q> &cam_up, T fov, T aspectRatio, T zNear, T zFar, T mouseX, T mouseY) {
    const mat<4, 4, T, Q> proj = perspective(fov, aspectRatio, zNear, zFar);
    const mat<4, 4, T, Q> view = lookAt(vec<3, T, Q>(T(0)), cam_direction, cam_up);
    const mat<4, 4, T, Q> invVP = inverse(proj * view);
    const vec<4, T, Q> screenPos = vec<4, T, Q>(mouseX, -mouseY, T(1), T(1));
    const vec<4, T, Q> worldPos = invVP * screenPos;
    return normalize(vec<3, T, Q>(worldPos));  // Direction of the ray; originating at the camera position.
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> snap(const vec<L, T, Q> &x, const vec<L, T, Q> &y) {
    return detail::functor2<vec, L, T, Q>::call(snap, x, y);
  }

  /// <summary>
  /// Inverse of each vector component.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<L, T, Q> inverse(const vec<L, T, Q> &x) {
    return vec<L, T, Q>(T(1)) / x;
  }

  /// <summary>
  /// Returns the normalized vector pointing to "y" from "x".
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<L, T, Q> direction(const vec<L, T, Q> &x, const vec<L, T, Q> &y) {
    return normalize(y - x);
  }

  /// <summary>
  /// Returns a value 't' such that lerp(x, y, t) == value (or 0 if x == y)
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> lerpinverse(const vec<L, T, Q> &x, const vec<L, T, Q> &y, const vec<L, T, Q> &value) {
    vec<L, T, Q> result(T(0));
    for (length_t i = 0; i < L; ++i)
      result[i] = lerpinverse(x[i], y[i], value[i]);
    return result;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> lerpinverse(const vec<L, T, Q> &x, const vec<L, T, Q> &y, const T value) {
    vec<L, T, Q> result(T(0));
    for (length_t i = 0; i < L; ++i)
      result[i] = lerpinverse(x[i], y[i], value);
    return result;
  }

  /// <summary>
  /// Normalized lerp
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> nlerp(const vec<L, T, Q> &x, const vec<L, T, Q> &y, const vec<L, T, Q> &t) {
    return normalize(lerp(x, y, t));
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> nlerp(const vec<L, T, Q> &x, const vec<L, T, Q> &y, const T t) {
    return normalize(lerp(x, y, t));
  }

  /* Functions with additional integral type support. */

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_integral<T>::value, vec<L, T, Q>>::type iceil(vec<L, T, Q> const &x) {
    return x;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_integral<T>::value, vec<L, T, Q>>::type ifloor(vec<L, T, Q> const &x) {
    return x;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER typename std::enable_if<!std::is_integral<T>::value, vec<L, T, Q>>::type iceil(vec<L, T, Q> const &x) {
    return ceil(x);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER typename std::enable_if<!std::is_integral<T>::value, vec<L, T, Q>>::type ifloor(vec<L, T, Q> const &x) {
    return floor(x);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> imod(vec<L, T, Q> const &x, T y) {
    return mod(x, y);  // @TODO: Handle modulo-zero for integer vectors.
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> imod(vec<L, T, Q> const &x, vec<L, T, Q> const &y) {
    return mod(x, y);  // @TODO: Handle modulo-zero for integer vectors.
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> pow(vec<L, T, Q> const &base, T exponent) {
    return pow(base, vec<L, T, Q>(exponent));
  }

  /* Missing implicit genType support. */

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<1, bool, Q> isCompNull(vec<1, T, Q> const &v, const T eps = epsilon<T>()) {
    return vec<1, bool, Q>(abs(v.x) < eps);
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<1, T, Q> lerp(const vec<1, T, Q> &x, const vec<1, T, Q> &y, T a) {
    return mix(x, y, a);
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<1, T, Q> lerp(const vec<1, T, Q> &x, const vec<1, T, Q> &y, const vec<1, T, Q> &a) {
    return mix(x, y, a);
  }

  /*
  ** {======================================================
  ** Functions are generally not used in single-dimensional vector spaces and
  ** only exist to simplify the bindings.
  ** =======================================================
  */

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool areCollinear(vec<1, T, Q> const &v0, vec<1, T, Q> const &v1, const T eps = epsilon<T>()) {
    ((void)v0);
    ((void)v1);
    ((void)eps);
    return true;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool areCollinear(genType v0, genType v1, const genType eps = epsilon<genType>()) {
    ((void)v0);
    ((void)v1);
    ((void)eps);
    return true;
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER T simplex(vec<1, T, Q> const &v) {
    ((void)v);
    return T(0);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType simplex(genType v) {
    return simplex(vec<1, genType>(v));
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER T perlin(vec<1, T, Q> const &Position) {
    ((void)Position);
    return T(0);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType perlin(genType Position) {
    return perlin(vec<1, genType>(Position));
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER T perlin(vec<1, T, Q> const &Position, vec<1, T, Q> const &rep) {
    ((void)Position);
    ((void)rep);
    return T(0);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType perlin(genType Position, genType rep) {
    return perlin(vec<1, genType>(Position), vec<1, genType>(rep));
  }

  template<typename T>
  GLM_FUNC_QUALIFIER bool intersectLineSphere(T const &point0, T const &point1, T const &sphereCenter, T sphereRadius,
  T &intersectionPoint1, T &intersectionNormal1, T &intersectionPoint2, T &intersectionNormal2) {
    ((void)point0);
    ((void)point1);
    ((void)sphereCenter);
    ((void)sphereRadius);
    intersectionNormal1 = intersectionPoint1 = T(0);
    intersectionPoint2 = intersectionNormal2 = T(0);
    return false;
  }

  template<typename T>
  GLM_FUNC_QUALIFIER bool intersectRayPlane(T const &orig, T const &dir, T const &planeOrig, T const &planeNormal, T &intersectionDistance) {
    ((void)orig);
    ((void)dir);
    ((void)planeOrig);
    ((void)planeNormal);
    intersectionDistance = T(0);
    return false;
  }

  template<typename T>
  GLM_FUNC_QUALIFIER bool intersectRaySphere(T const &rayStarting, T const &rayNormalizedDirection, T const &sphereCenter, const T sphereRadius, T &intersectionPosition, T &intersectionNormal) {
    ((void)rayStarting);
    ((void)rayNormalizedDirection);
    ((void)sphereCenter);
    ((void)sphereRadius);
    intersectionPosition = intersectionNormal = T(0);
    return false;
  }

  /* }====================================================== */

  /*
  ** {======================================================
  ** C++-11/C99 wrappers
  ** =======================================================
  */

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> logistic(vec<L, T, Q> const &v) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'logistic' only accept floating-point inputs.");
    return detail::functor1<vec, L, T, T, Q>::call(logistic, v);
  }

#if GLM_HAS_CXX11_STL
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> copysign(vec<L, T, Q> const &v, vec<L, T, Q> const &v2) {
    return detail::functor2<vec, L, T, Q>::call(copysign, v, v2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> cbrt(vec<L, T, Q> const &v) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'cbrt' only accept floating-point inputs.");
    return detail::functor1<vec, L, T, T, Q>::call(cbrt, v);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> expm1(vec<L, T, Q> const &v) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'expm1' only accept floating-point inputs.");
    return detail::functor1<vec, L, T, T, Q>::call(expm1, v);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> erf(vec<L, T, Q> const &v) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'erf' only accept floating-point inputs.");
    return detail::functor1<vec, L, T, T, Q>::call(erf, v);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> erfc(vec<L, T, Q> const &v) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'erfc' only accept floating-point inputs.");
    return detail::functor1<vec, L, T, T, Q>::call(erfc, v);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, int, Q> fpclassify(vec<L, T, Q> const &v) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'fpclassify' only accept floating-point inputs.");
    return detail::functor1<vec, L, int, T, Q>::call(fpclassify, v);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> fdim(vec<L, T, Q> const &v, vec<L, T, Q> const &v2) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'fdim' only accept floating-point inputs.");
    return detail::functor2<vec, L, T, Q>::call(fdim, v, v2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> hypot(vec<L, T, Q> const &v, vec<L, T, Q> const &v2) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'hypot' only accept floating-point inputs.");
    return detail::functor2<vec, L, T, Q>::call(hypot, v, v2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, bool, Q> isnormal(vec<L, T, Q> const &v) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'isnormal' only accept floating-point inputs.");
    return detail::functor1<vec, L, bool, T, Q>::call(isnormal, v);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, bool, Q> isunordered(vec<L, T, Q> const &v, vec<L, T, Q> const &v2) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'isunordered' only accept floating-point inputs.");
    vec<L, bool, Q> Result(false);
    for (length_t i = 0; i < L; ++i)
      Result[i] = isunordered(v[i], v2[i]);
    return Result;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, int, Q> ilogb(vec<L, T, Q> const &v) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'ilogb' only accept floating-point inputs.");
    return detail::functor1<vec, L, int, T, Q>::call(ilogb, v);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> lgamma(vec<L, T, Q> const &v) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'lgamma' only accept floating-point inputs.");
    return detail::functor1<vec, L, T, T, Q>::call(lgamma, v);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> log10(vec<L, T, Q> const &v) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'log10' only accept floating-point inputs.");
    return detail::functor1<vec, L, T, T, Q>::call(log10, v);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> log1p(vec<L, T, Q> const &v) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'log1p' only accept floating-point inputs.");
    return detail::functor1<vec, L, T, T, Q>::call(log1p, v);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> logb(vec<L, T, Q> const &v) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'logb' only accept floating-point inputs.");
    return detail::functor1<vec, L, T, T, Q>::call(logb, v);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> nearbyint(vec<L, T, Q> const &v) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'nearbyint' only accept floating-point inputs.");
    return detail::functor1<vec, L, T, T, Q>::call(nearbyint, v);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> nextafter(vec<L, T, Q> const &v, vec<L, T, Q> const &v2) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'nextafter' only accept floating-point inputs.");
    return detail::functor2<vec, L, T, Q>::call(nextafter, v, v2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> remainder(vec<L, T, Q> const &v, vec<L, T, Q> const &v2) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'remainder' only accept floating-point inputs.");
    return detail::functor2<vec, L, T, Q>::call(remainder, v, v2);
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> scalbn(vec<L, T, Q> const &v, vec<L, int, Q> const &v2) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'scalbn' only accept floating-point inputs.");
    vec<L, T, Q> Result(T(0));
    for (length_t i = 0; i < L; ++i)
      Result[i] = scalbn(v[i], v2[i]);
    return Result;
  }

  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> tgamma(vec<L, T, Q> const &v) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'tgamma' only accept floating-point inputs.");
    return detail::functor1<vec, L, T, T, Q>::call(tgamma, v);
  }
#endif

  /* }====================================================== */

  /*
  ** {======================================================
  ** Monkey Patches
  ** =======================================================
  */

  /// <summary>
  /// @GLMFix: Incorrect 'vec<2, U, Q> associatedMin' declaration.
  /// </summary>
  template<length_t L, typename T, typename U, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, U, Q> __associatedMin(vec<L, T, Q> const &x, vec<L, U, Q> const &a, vec<L, T, Q> const &y, vec<L, U, Q> const &b) {
    vec<L, U, Q> Result;
    for (length_t i = 0, n = Result.length(); i < n; ++i)
      Result[i] = x[i] < y[i] ? a[i] : b[i];
    return Result;
  }

  /// <summary>
  /// @GLMFix: Incorrect 'vec<2, U, Q> associatedMax' declaration.
  /// </summary>
  template<length_t L, typename T, typename U, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, U, Q> __associatedMax(vec<L, T, Q> const &x, vec<L, U, Q> const &a, vec<L, T, Q> const &y, vec<L, U, Q> const &b) {
    vec<L, U, Q> Result;
    for (length_t i = 0, n = Result.length(); i < n; ++i)
      Result[i] = x[i] > y[i] ? a[i] : b[i];
    return Result;
  }

  // Specialize incorrect GLM functions to use the functions above

  template<typename T, typename U, qualifier Q>
  GLM_FUNC_QUALIFIER vec<2, U, Q> associatedMin(vec<2, T, Q> const &x, vec<2, U, Q> const &a, vec<2, T, Q> const &y, vec<2, U, Q> const &b) {
    return __associatedMin(x, a, y, b);
  }
  template<typename T, typename U, qualifier Q>
  GLM_FUNC_QUALIFIER vec<3, U, Q> associatedMin(vec<3, T, Q> const &x, vec<3, U, Q> const &a, vec<3, T, Q> const &y, vec<3, U, Q> const &b) {
    return __associatedMin(x, a, y, b);
  }
  template<typename T, typename U, qualifier Q>
  GLM_FUNC_QUALIFIER vec<4, U, Q> associatedMin(vec<4, T, Q> const &x, vec<4, U, Q> const &a, vec<4, T, Q> const &y, vec<4, U, Q> const &b) {
    return __associatedMin(x, a, y, b);
  }

  template<typename T, typename U, qualifier Q>
  GLM_FUNC_QUALIFIER vec<2, U, Q> associatedMax(vec<2, T, Q> const &x, vec<2, U, Q> const &a, vec<2, T, Q> const &y, vec<2, U, Q> const &b) {
    return __associatedMax(x, a, y, b);
  }
  template<typename T, typename U, qualifier Q>
  GLM_FUNC_QUALIFIER vec<3, U, Q> associatedMax(vec<3, T, Q> const &x, vec<3, U, Q> const &a, vec<3, T, Q> const &y, vec<3, U, Q> const &b) {
    return __associatedMax(x, a, y, b);
  }
  template<typename T, typename U, qualifier Q>
  GLM_FUNC_QUALIFIER vec<4, U, Q> associatedMax(vec<4, T, Q> const &x, vec<4, U, Q> const &a, vec<4, T, Q> const &y, vec<4, U, Q> const &b) {
    return __associatedMax(x, a, y, b);
  }

  /// <summary>
  /// @GLMFix: Generalized closestPointOnLine implementation
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> closestPointOnLine(vec<L, T, Q> const &point, vec<L, T, Q> const &a, vec<L, T, Q> const &b) {
    const T LineLength = distance(a, b);
    const vec<L, T, Q> Vector = point - a;
    const vec<L, T, Q> LineDirection = (b - a) / LineLength;

    const T Distance = dot(Vector, LineDirection);
    if (Distance <= T(0))
      return a;
    if (Distance >= LineLength)
      return b;
    return a + LineDirection * Distance;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType closestPointOnLine(genType point, genType a, genType b) {
    return closestPointOnLine(vec<1, genType>(point), vec<1, genType>(a), vec<1, genType>(b)).x;
  }

  /// <summary>
  /// @GLMFix: 'int' to 'unsigned char' [-Wimplicit-int-conversion]
  /// </summary>
  namespace detail {
    template<>
    struct compute_rand<1, uint8, glm::defaultp> {
      GLM_FUNC_QUALIFIER static vec<1, uint8, glm::defaultp> call() {
        return vec<1, uint8, glm::defaultp>(static_cast<uint8>(
        std::rand() % static_cast<int>(std::numeric_limits<uint8>::max())));
      }
    };
  }

#if (GLM_ARCH & GLM_ARCH_SSE2_BIT) && (GLM_CONFIG_SIMD == GLM_ENABLE)
  /// <summary>
  /// @GLMFix: glm/glm/detail/func_integer_simd.inl:42:32: error: could not convert ‘add0’ from ‘const __m128i’ to ‘glm::vec<4, unsigned int, glm::aligned_highp>’
  /// </summary>
  namespace detail {
    template<>
    struct compute_bitfieldBitCountStep<4, uint, glm::aligned_highp, true, true> {
      GLM_FUNC_QUALIFIER static vec<4, uint, glm::aligned_highp> call(vec<4, uint, glm::aligned_highp> const &v, uint Mask, uint Shift) {
        __m128i const set0 = v.data;

        __m128i const set1 = _mm_set1_epi32(static_cast<int>(Mask));
        __m128i const and0 = _mm_and_si128(set0, set1);
        __m128i const sft0 = _mm_slli_epi32(set0, static_cast<int>(Shift));
        __m128i const and1 = _mm_and_si128(sft0, set1);
        __m128i const add0 = _mm_add_epi32(and0, and1);

        vec<4, uint, glm::aligned_highp> Result;
        Result.data = add0;
        return Result;
      }
    };

    /// <summary>
    /// @GLMFix: Similar to above
    /// </summary>
    template<>
    struct compute_bitfieldReverseStep<4, uint, glm::aligned_highp, true, true> {
      GLM_FUNC_QUALIFIER static vec<4, uint, glm::aligned_highp> call(vec<4, uint, glm::aligned_highp> const &v, uint Mask, uint Shift) {
        __m128i const set0 = v.data;

        __m128i const set1 = _mm_set1_epi32(static_cast<int>(Mask));
        __m128i const and1 = _mm_and_si128(set0, set1);
        __m128i const sft1 = _mm_slli_epi32(and1, static_cast<int>(Shift));
        __m128i const set2 = _mm_andnot_si128(set0, _mm_set1_epi32(-1));
        __m128i const and2 = _mm_and_si128(set0, set2);
        __m128i const sft2 = _mm_srai_epi32(and2, static_cast<int>(Shift));
        __m128i const or0 = _mm_or_si128(sft1, sft2);

        vec<4, uint, glm::aligned_highp> Result;
        Result.data = or0;
        return Result;
      }
    };
  }
#endif

  /// <summary>
  /// @GLMFix: A implementation of glm::angle that's numerically stable at all angles.
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER T __angle(const vec<L, T, Q> &x, const vec<L, T, Q> &y) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'angle' only accept floating-point inputs");
    const vec<L, T, Q> xyl = x * length(y);
    const vec<L, T, Q> yxl = y * length(x);
    const T n = length(xyl - yxl);
    if (epsilonNotEqual(n, T(0), epsilon<T>()))
      return T(2) * atan2_(n, length(xyl + yxl));
    return T(0);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType __angle(const genType &x, const genType &y) {
    return glm::angle<genType>(x, y);
  }

  /// <summary>
  /// orientedAngle implementation that uses __angle
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER T __orientedAngle(vec<2, T, Q> const &x, vec<2, T, Q> const &y) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'orientedAngle' only accept floating-point inputs");
    const T Angle = __angle(x, y);
    const T partialCross = x.x * y.y - y.x * x.y;
    return (partialCross > T(0)) ? Angle : -Angle;
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER T __orientedAngle(vec<3, T, Q> const &x, vec<3, T, Q> const &y, vec<3, T, Q> const &ref) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'orientedAngle' only accept floating-point inputs");
    const T Angle = __angle(x, y);
    return mix(Angle, -Angle, dot(ref, cross(x, y)) < T(0));
  }

  /// <summary>
  /// @GLMFix: Generalized slerp implementation
  /// </summary>
  template<length_t L, typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<L, T, Q> __slerp(vec<L, T, Q> const &x, vec<L, T, Q> const &y, T const &a) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'slerp' only accept floating-point inputs");

    // Perform a linear interpolation when CosAlpha is close to 1 to avoid side
    // effect of sin(angle) becoming a zero denominator
    const T CosAlpha = dot(x, y);
    if (CosAlpha > static_cast<T>(1) - epsilon<T>())
      return mix(x, y, a);

    const T Alpha = acos(CosAlpha);  // get angle (0 -> pi)
    const T SinAlpha = sin(Alpha);  // get sine of angle between vectors (0 -> 1)
    const T t1 = sin((static_cast<T>(1) - a) * Alpha) / SinAlpha;
    const T t2 = sin(a * Alpha) / SinAlpha;
    return x * t1 + y * t2;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType __slerp(genType x, genType y, genType a) {
    return __slerp(vec<1, genType>(x), vec<1, genType>(y), a).x;
  }

  /* }====================================================== */
}

#endif
