/*
** $Id: scalar_extensions.hpp $
**
** See Copyright Notice in lua.h
*/
#ifndef EXT_EXTENSION_SCALAR_HPP
#define EXT_EXTENSION_SCALAR_HPP

#include <glm/glm.hpp>
#include <glm/detail/compute_vector_relational.hpp>
#include <glm/gtc/bitfield.hpp>
#include <glm/gtc/packing.hpp>
#include <glm/gtc/color_space.hpp>
#include <glm/gtx/spline.hpp>
#if GLM_VERSION < 998  // @COMPAT: ext/scalar_common.hpp introduced in 0.9.9.8
  #include <glm/gtx/extended_min_max.hpp>
#else
  #include <glm/ext/scalar_common.hpp>
#endif

#if GLM_MESSAGES == GLM_ENABLE && !defined(GLM_EXT_INCLUDED)
  #pragma message("GLM: GLM_EXT_scalar_ext extension included")
#endif

namespace glm {

  /// <summary>
  /// Functions to suppress float-equal warnings and extension to compute_vector_relational.hpp
  /// </summary>
  namespace detail {
    template<typename genType, typename otherType = genType>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool equal_strict(const genType &x, const otherType &y) {
      return x == y;
    }

    template<typename genType, typename otherType = genType>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool not_equal_strict(const genType &x, const otherType &y) {
      return x != y;
    }

#if GLM_HAS_CXX11_STL
    template<>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool equal_strict(const float &x, const float &y) {
      return std::equal_to<float>()(x, y);
    }

    template<>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool equal_strict(const double &x, const double &y) {
      return std::equal_to<double>()(x, y);
    }

    template<>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool not_equal_strict(const float &x, const float &y) {
      return std::not_equal_to<float>()(x, y);
    }

    template<>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool not_equal_strict(const double &x, const double &y) {
      return std::not_equal_to<double>()(x, y);
    }
#endif

    template<typename genType>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool exactly_zero(const genType &x) {
      return equal_strict(x, genType(0));
    }

    template<typename genType>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool exactly_one(const genType &x) {
      return equal_strict(x, genType(1));
    }

    template<typename T>
    struct compute_equal<T, true> {
      GLM_FUNC_QUALIFIER GLM_CONSTEXPR static bool call(T a, T b) {
        return equal_strict(a, b);
      }
    };
  }

  /* Function wrappers. */

  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any(bool b) {
    return b;
  }

  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all(bool b) {
    return b;
  }

  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool not_(bool b) {
    return !b;
  }

  template<typename genIUType>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool lessThan(genIUType x, genIUType y) {
    return x < y;
  }

  template<typename genIUType>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool lessThanEqual(genIUType x, genIUType y) {
    return x <= y;
  }

  template<typename genIUType>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool greaterThan(genIUType x, genIUType y) {
    return x > y;
  }

  template<typename genIUType>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool greaterThanEqual(genIUType x, genIUType y) {
    return x >= y;
  }

  /* glm::all(glm::equal(...)) shorthand */

  template<typename genIUType>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool equal(genIUType x, genIUType y) {
    return glm::detail::equal_strict(x, y);
  }

  template<typename T>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_equal(T const &x, T const &y) {
    return all(equal(x, y));
  }

  template<typename T>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_equal(T const &x, T const &y, T eps) {
    return all(equal(x, y, eps));
  }

  template<typename T>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_equal(T const &x, T const &y, int MaxULPs) {
    return all(equal(x, y, MaxULPs));
  }

  /* glm::any(glm::notEqual(...)) shorthand */

  template<typename genIUType>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool notEqual(genIUType x, genIUType y) {
    return glm::detail::not_equal_strict(x, y);
  }

  template<typename T>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any_notequal(T const &x, T const &y) {
    return any(notEqual(x, y));
  }

  template<typename T>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any_notequal(T const &x, T const &y, T eps) {
    return any(notEqual(x, y, eps));
  }

  template<typename T>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any_notequal(T const &x, T const &y, int MaxULPs) {
    return any(notEqual(x, y, MaxULPs));
  }

  /* glm::all(glm::lessThan | glm::greaterThan | glm::lessThanEqual | glm::greaterThanEqual) */

  template<typename genIUType>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_lessThan(genIUType x, genIUType y) {
    return all(lessThan(x, y));
  }

  template<typename genIUType>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_lessThanEqual(genIUType x, genIUType y) {
    return all(lessThanEqual(x, y));
  }

  template<typename genIUType>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_greaterThan(genIUType x, genIUType y) {
    return all(greaterThan(x, y));
  }

  template<typename genIUType>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_greaterThanEqual(genIUType x, genIUType y) {
    return all(greaterThanEqual(x, y));
  }

  /* glm::any(glm::isinf(...)) shorthand */

  template<typename T>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any_isinf(const T &x) {
    return any(isinf(x));
  }

  template<typename T>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_isfinite(const T &x) {
    return all(isfinite(x));
  }

  /* glm::any(glm::isnan(...)) shorthand */

  template<typename T>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any_isnan(const T &x) {
    return any(isnan(x));
  }

  /* The other useful sign() implementation: where >= 0 returns +1 */

  template<typename genType>
  GLM_FUNC_QUALIFIER genType signP(genType v) {
    return (v >= 0) ? genType(1) : genType(-1);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType signN(genType v) {
    return (v > 0) ? genType(1) : genType(-1);
  }

  /* API Completeness */

  template<typename genType>
  GLM_FUNC_QUALIFIER genType fclamp(genType x) {
    return fclamp(x, genType(0), genType(1));
  }

  /* Numeric extensions */
  template<typename genType>
  GLM_FUNC_QUALIFIER bool isUniform(genType v, const genType eps = epsilon<genType>()) {
    ((void)v);
    ((void)eps);
    return true;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType reverse(genType v) {
    return v;
  }

  /// <summary>
  /// @COMPAT: Old versions of GLM include unnecessary precision qualifiers for
  /// glm::atan2 and glm::saturate.
  /// </summary>
  template<typename T>
  GLM_FUNC_QUALIFIER T atan2_(T x, T y) {
    return atan(x, y);
  }

  /// <summary>
  /// Calculate sin and cos simultaneously.
  /// </summary>
  template<typename genType>
  GLM_FUNC_QUALIFIER void sincos(genType v, genType &s, genType &c) {
    s = sin(v);
    c = cos(v);
  }

  /// <summary>
  /// Wraps [0, maxValue].
  /// </summary>
  template<typename genType>
  GLM_FUNC_QUALIFIER genType wrap(genType value, genType maxValue) {
    return fmod(value, maxValue) + ((value < genType(0.0)) ? maxValue : genType(0.0));
  }

  /// <summary>
  /// Wraps [minValue, maxValue].
  /// </summary>
  // template<typename genType>
  // GLM_FUNC_QUALIFIER genType wrap(genType value, genType minValue, genType maxValue) {
  //   return wrap(value - minValue, maxValue - minValue) + minValue;
  // }

  /// <summary>
  /// glm::wrapAngle defined over [-pi, pi].
  /// </summary>
  template<typename genType>
  GLM_FUNC_QUALIFIER genType wrapAngle2(genType value) {
    if (value >= genType(0))
      return fmod(value + glm::pi<genType>(), glm::two_pi<genType>()) - glm::pi<genType>();
    return fmod(value - glm::pi<genType>(), glm::two_pi<genType>()) + glm::pi<genType>();
  }

  /// <summary>
  /// Loops "t", so that it is never greater than "length" and less than zero.
  /// @TODO: Replace with wrap
  /// </summary>
  template<typename genType>
  GLM_FUNC_QUALIFIER genType loopRepeat(genType t, genType length) {
    return clamp(t - floor(t / length) * length, genType(0), length);
  }

  /// <summary>
  /// A lerp implementation that ensures values interpolate correctly when
  /// wrapped around two-pi.
  /// </summary>
  template<typename genType>
  GLM_FUNC_QUALIFIER genType lerpAngle(genType a, genType b, genType t) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'lerpAngle' only accept floating-point inputs");

    const genType dt = loopRepeat((b - a), two_pi<genType>());
    return lerp(a, a + (dt > pi<genType>() ? dt - two_pi<genType>() : dt), t);
  }

  /// <summary>
  /// Return the shortest difference between two angles (radians)
  /// </summary>
  template<typename genType>
  GLM_FUNC_QUALIFIER genType deltaAngle(genType a, genType b) {
    const genType dt = loopRepeat((b - a), two_pi<genType>());
    return min(two_pi<genType>() - dt, dt);
  }

  /// <summary>
  /// Returns a value that will increment and decrement between the value 0 and
  /// length. Note: This function is an emulation of: Unity.Mathf.PingPong
  /// </summary>
  template<typename genType>
  GLM_FUNC_QUALIFIER genType pingPong(genType t, genType length) {
    t = loopRepeat(t, length * genType(2));
    return length - abs(t - length);
  }

  /// <summary>
  /// Return a position between two points, moving no further than maxDist.
  /// </summary>
  template<typename genType>
  GLM_FUNC_QUALIFIER genType moveTowards(genType current, genType target, genType maxDist) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'moveTowards' only accept floating-point inputs");
    if (abs(target - current) <= maxDist)
      return target;

    return current + sign(target - current) * maxDist;
  }

  /// <summary>
  /// </summary>
  template<typename genType>
  GLM_FUNC_QUALIFIER genType snap(const genType value, const genType step) {
    if (!glm::detail::exactly_zero(step))
      return floor((value / step) + genType(0.5)) * step;
    return value;
  }

  /// <summary>
  /// Functional inverse() wrapper; does not sanitize x == 0.
  /// </summary>
  template<typename T>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR T inverse(const T &x) {
    return T(1) / x;
  }

  /// <summary>
  /// Returns the normalized vector pointing to "y" from "x".
  /// </summary>
  template<typename genType>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR genType direction(const genType x, const genType y) {
    return normalize(vec<1, genType>(y - x)).x;
  }

  /// <summary>
  /// Returns a value 't' such that lerp(x, y, t) == value (or 0 if x == y)
  /// </summary>
  template<typename genType>
  GLM_FUNC_QUALIFIER genType lerpinverse(const genType x, const genType y, const genType value) {
    return equal(x, y, glm::epsilon<genType>()) ? genType(0) : (value - x) / (y - x);
  }

  /// <summary>
  /// Normalized lerp; API completeness.
  /// </summary>
  template<typename genType>
  GLM_FUNC_QUALIFIER genType nlerp(const genType x, const genType y, const genType t) {
    return lerp(x, y, t);
  }

  /* Functions with additional integral type support. */

  template<typename T>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_integral<T>::value, T>::type iceil(T x) {
    return x;
  }

  template<typename T>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_integral<T>::value, T>::type ifloor(T x) {
    return x;
  }

  template<typename T>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_floating_point<T>::value, T>::type iceil(T x) {
    return ceil(x);
  }

  template<typename T>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_floating_point<T>::value, T>::type ifloor(T x) {
    return floor(x);
  }

  template<typename T>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value, T>::type imod(T x, T y) {
    if (y == T(0))
      return T(0);  // attempt to perform 'n % 0'
    return ((x % y) + y) % y;
  }

  template<typename T>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_integral<T>::value && !std::is_signed<T>::value, T>::type imod(T x, T y) {
    return x - y * (x / y);
  }

  template<typename T>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_floating_point<T>::value, T>::type imod(T x, T y) {
    return mod(x, y);
  }

  template<typename T>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_integral<T>::value, T>::type pow(T x, uint y) {
    if (y == T(0))
      return x >= T(0) ? T(1) : T(-1);

    T result = x;
    for (uint i = 1; i < y; ++i)
      result *= x;
    return result;
  }

  /* Missing implicit genType support. */

  template<typename genIUType>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR genIUType compAdd(genIUType v) {
    return v;
  }

  template<typename genIUType>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR genIUType compMul(genIUType v) {
    return v;
  }

  template<typename genIUType>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR genIUType compMin(genIUType v) {
    return v;
  }

  template<typename genIUType>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR genIUType compMax(genIUType v) {
    return v;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType atan2(genType x, genType y) {
    return atan(x, y);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType normalize(genType x) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'normalize' accepts only floating-point inputs");
    return x < genType(0) ? genType(-1) : genType(1);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER bool isNormalized(genType x, const genType eps = epsilon<genType>()) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'isNormalized' only accept floating-point inputs");
    return abs(x - static_cast<genType>(1)) <= static_cast<genType>(2) * eps;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER bool isNull(genType x, const genType eps = epsilon<genType>()) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'isNull' only accept floating-point inputs");
    return x <= eps;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER bool isCompNull(genType v, const genType eps = epsilon<genType>()) {
    return abs(v) < eps;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER bool areOrthonormal(genType v0, genType v1, const genType eps = epsilon<genType>()) {
    return isNormalized(v0, eps) && isNormalized(v1, eps) && (abs(dot(v0, v1)) <= eps);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER bool areOrthogonal(genType v0, genType v1, const genType eps = epsilon<genType>()) {
    return areOrthogonal(vec<1, genType>(v0), vec<1, genType>(v1), eps);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType normalizeDot(genType x, genType y) {
    return normalizeDot(vec<1, genType>(x), vec<1, genType>(y));
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType fastNormalizeDot(genType x, genType y) {
    return fastNormalizeDot(vec<1, genType>(x), vec<1, genType>(y));
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType saturate(genType x) {
    return clamp(x, genType(0), genType(1));
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER bool openBounded(genType Value, genType Min, genType Max) {
    return openBounded(vec<1, genType>(Value), vec<1, genType>(Min), vec<1, genType>(Max)).x;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER bool closeBounded(genType Value, genType Min, genType Max) {
    return closeBounded(vec<1, genType>(Value), vec<1, genType>(Min), vec<1, genType>(Max)).x;
  }

  template<typename floatType, typename T>
  GLM_FUNC_QUALIFIER floatType compNormalize(T x) {
    return compNormalize<floatType>(vec<1, T>(x)).x;
  }

  template<typename T, typename floatType>
  GLM_FUNC_QUALIFIER T compScale(floatType x) {
    return compScale<T>(vec<1, floatType>(x)).x;
  }

  GLM_FUNC_QUALIFIER uint16 packHalf(float v) {
    return packHalf<1, defaultp>(vec<1, float>(v)).x;
  }

  GLM_FUNC_QUALIFIER float unpackHalf(uint16 v) {
    return unpackHalf<1, defaultp>(vec<1, uint16>(v)).x;
  }

  template<typename uintType, typename floatType>
  GLM_FUNC_QUALIFIER uintType packUnorm(floatType v) {
    return packUnorm<uintType, 1, floatType, defaultp>(vec<1, floatType>(v)).x;
  }

  template<typename floatType, typename uintType>
  GLM_FUNC_QUALIFIER floatType unpackUnorm(uintType v) {
    return unpackUnorm<floatType, 1, uintType, defaultp>(vec<1, uintType>(v)).x;
  }

  template<typename intType, typename floatType>
  GLM_FUNC_QUALIFIER intType packSnorm(floatType v) {
    return packSnorm<intType, 1, floatType, defaultp>(vec<1, floatType>(v)).x;
  }

  template<typename floatType, typename intType>
  GLM_FUNC_QUALIFIER floatType unpackSnorm(intType v) {
    return unpackSnorm<floatType, 1, intType, defaultp>(vec<1, intType>(v)).x;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType catmullRom(genType v1, genType v2, genType v3, genType v4, genType s) {
    return catmullRom<vec<1, genType>>(vec<1, genType>(v1), vec<1, genType>(v2), vec<1, genType>(v3), vec<1, genType>(v4), s).x;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType hermite(genType v1, genType t1, genType v2, genType t2, genType s) {
    return hermite<vec<1, genType>>(vec<1, genType>(v1), vec<1, genType>(t1), vec<1, genType>(v2), vec<1, genType>(t2), s).x;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType cubic(genType v1, genType v2, genType v3, genType v4, genType s) {
    return cubic<vec<1, genType>>(vec<1, genType>(v1), vec<1, genType>(v2), vec<1, genType>(v3), vec<1, genType>(v4), s).x;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType convertLinearToSRGB(genType ColorLinear) {
    return convertLinearToSRGB(vec<1, genType>(ColorLinear)).x;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType convertLinearToSRGB(genType ColorLinear, genType Gamma) {
    return convertLinearToSRGB(vec<1, genType>(ColorLinear), Gamma).x;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType convertSRGBToLinear(genType ColorSRGB) {
    return convertSRGBToLinear(vec<1, genType>(ColorSRGB)).x;
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType convertSRGBToLinear(genType ColorSRGB, genType Gamma) {
    return convertSRGBToLinear(vec<1, genType>(ColorSRGB), Gamma).x;
  }

  /*
  ** {======================================================
  ** C++-11/C99 wrappers
  ** =======================================================
  */

  /// <summary>
  /// Logistic function with basic overflow handling; underflow to-be-determined.
  /// </summary>
  template<typename genType>
  GLM_FUNC_QUALIFIER genType logistic(genType x) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'logistic' only accept floating-point inputs.");
    const genType e = exp(min(x, genType(44.3614196)));  // exp(-44.3614196) ~ 2^{−64}
    return e / (genType(1.0) + e);
  }

  template<>
  GLM_FUNC_QUALIFIER float logistic(float x) {
    const float e = exp(min(x, 16.6355324f));  // exp(−16.6355324) ~ 2^{−24}
    return e / (1.0f + e);
  }

#if GLM_HAS_CXX11_STL
  template<typename genType>
  GLM_FUNC_QUALIFIER genType copysign(genType x, genType y) {
    return std::copysign(x, y);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType cbrt(genType x) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'cbrt' only accept floating-point inputs.");
    return std::cbrt(x);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType expm1(genType x) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'expm1' only accept floating-point inputs.");
    return std::expm1(x);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType erf(genType x) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'erf' only accept floating-point inputs.");
    return std::erf(x);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType erfc(genType x) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'erfc' only accept floating-point inputs.");
    return std::erfc(x);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER int fpclassify(genType x) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'fpclassify' only accept floating-point inputs.");
    return std::fpclassify(x);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType fdim(genType x, genType y) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'fdim' only accept floating-point inputs.");
    return std::fdim(x, y);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType hypot(genType x, genType y) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'hypot' only accept floating-point inputs.");
    return std::hypot(x, y);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER bool isnormal(genType x) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'isnormal' only accept floating-point inputs.");
    return std::isnormal(x);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER bool isunordered(genType x, genType y) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'isunordered' only accept floating-point inputs.");
    return std::isunordered(x, y);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER int ilogb(genType x) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'ilogb' only accept floating-point inputs.");
    return std::ilogb(x);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType lgamma(genType x) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'lgamma' only accept floating-point inputs.");
    return std::lgamma(x);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType log10(genType x) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'log10' only accept floating-point inputs.");
    return std::log10(x);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType log1p(genType x) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'log1p' only accept floating-point inputs.");
    return std::log1p(x);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType logb(genType x) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'logb' only accept floating-point inputs.");
    return std::logb(x);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType nearbyint(genType x) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'nearbyint' only accept floating-point inputs.");
    return std::nearbyint(x);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType nextafter(genType x, genType y) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'nextafter' only accept floating-point inputs.");
    return std::nextafter(x, y);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType remainder(genType x, genType y) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'remainder' only accept floating-point inputs.");
    return std::remainder(x, y);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType scalbn(genType x, int y) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'scalbn' only accept floating-point inputs.");
    return std::scalbn(x, y);
  }

  template<typename genType>
  GLM_FUNC_QUALIFIER genType tgamma(genType x) {
    GLM_STATIC_ASSERT(std::numeric_limits<genType>::is_iec559, "'tgamma' only accept floating-point inputs.");
    return std::tgamma(x);
  }

#endif
  /* }====================================================== */

  /*
  ** {======================================================
  ** Monkey Patches
  ** =======================================================
  */

  /// <summary>
  /// @GLMFix: Inconsistent template arguments in associatedMin; see max:
  ///   template<typename T, typename U>
  ///   GLM_FUNC_DECL U associatedMax(T x, U a, T y, U b);
  /// </summary>
  template<typename T, typename U>
  GLM_FUNC_QUALIFIER U associatedMin(T x, U a, T y, U b) {
    return x < y ? a : b;
  }

  /// <summary>
  /// @GLMFix: -Werror when using bitfieldFillOne and bitfieldFillZero:
  ///
  /// libs/glm/glm/./gtc/bitfield.inl:229:29: warning: comparison of integer expressions of different signedness: ‘int’ and ‘long unsigned int’ [-Wsign-compare]
  ///   229 | return Bits >= sizeof(genIUType) * 8 ? ~static_cast<genIUType>(0) : (static_cast<genIUType>(1) << Bits) - static_cast<genIUType>(1);
  /// </summary>
  template<>
  GLM_FUNC_QUALIFIER int mask(int Bits) {
    return detail::mask(Bits);  // Use alternate/duplicate mask implementation
  }

  /// <summary>
  /// @GLMFix: missing long double support
  /// </summary>
  template<typename genType>
  GLM_FUNC_QUALIFIER bool epsilonEqual(genType const &x, genType const &y, genType const &epsilon) {
    return abs(x - y) < epsilon;
  }

  /// <summary>
  /// @GLMFix: missing long double support
  /// </summary>
  template<typename genType>
  GLM_FUNC_QUALIFIER bool epsilonNotEqual(genType const &x, genType const &y, genType const &epsilon) {
    return abs(x - y) >= epsilon;
  }

  /* }====================================================== */

}

#endif
