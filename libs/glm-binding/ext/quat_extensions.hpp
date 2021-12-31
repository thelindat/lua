/*
** $Id: quat_extensions.hpp $
**
** Quaternion-specific extensions to GLM:
**  1. API-completing functions;
**  2. Functions that exist for rotation matrices but not for quaternions; see
**     PARSE_ROTATION_MATRIX.
**  3. Functions emulated/ported from other popular vector-math libraries.
**
** See Copyright Notice in lua.h
**
** @TODO: Consider SIMD support for some of the quatEulerAngle functions
*/
#ifndef EXT_EXTENSION_QUAT_HPP
#define EXT_EXTENSION_QUAT_HPP

/* @TEMP Disallow 820a2c0e625f26000c688d841836bb10483be34d */
#if defined(GLM_FORCE_QUAT_DATA_XYZW)
  #error "please compile without GLM_FORCE_QUAT_DATA_XYZW"
#endif

#if !defined(GLM_ENABLE_EXPERIMENTAL)
  #define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/glm.hpp>
#include <glm/exponential.hpp>
#if GLM_VERSION >= 991  // @COMPAT: Changed to ext/quaternion_common in 0.9.9.1
  #include <glm/ext/quaternion_common.hpp>
#else
  #include <glm/gtc/quaternion.hpp>
#endif
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/fast_square_root.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>

#include "scalar_extensions.hpp"
#include "vector_extensions.hpp"
#include "matrix_extensions.hpp"

#if GLM_MESSAGES == GLM_ENABLE && !defined(GLM_EXT_INCLUDED)
  #pragma message("GLM: GLM_EXT_quat_ext extension included")
#endif

namespace glm {

  /* EulerAngles -> Quaternion; @TODO: Optimize to avoid intermediate matrix creation */
  template<typename T, qualifier Q = defaultp> GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleX(T angleX) { return toQuat(eulerAngleX(angleX)); }
  template<typename T, qualifier Q = defaultp> GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleY(T angleY) { return toQuat(eulerAngleY(angleY)); }
  template<typename T, qualifier Q = defaultp> GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleZ(T angleZ) { return toQuat(eulerAngleZ(angleZ)); }
  template<typename T, qualifier Q = defaultp> GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleXY(T angleX, T angleY) { return toQuat(eulerAngleXY(angleX, angleY)); }
  template<typename T, qualifier Q = defaultp> GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleXZ(T angleX, T angleZ) { return toQuat(eulerAngleXZ(angleX, angleZ)); }
  template<typename T, qualifier Q = defaultp> GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleYX(T angleY, T angleX) { return toQuat(eulerAngleYX(angleY, angleX)); }
  template<typename T, qualifier Q = defaultp> GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleYZ(T angleY, T angleZ) { return toQuat(eulerAngleYZ(angleY, angleZ)); }
  template<typename T, qualifier Q = defaultp> GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleZX(T angleZ, T angleX) { return toQuat(eulerAngleZX(angleZ, angleX)); }
  template<typename T, qualifier Q = defaultp> GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleZY(T angleZ, T angleY) { return toQuat(eulerAngleZY(angleZ, angleY)); }
  template<typename T, qualifier Q = defaultp> GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleXYX(T t1, T t2, T t3) { return toQuat(eulerAngleXYX(t1, t2, t3)); }
  template<typename T, qualifier Q = defaultp> GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleXZX(T t1, T t2, T t3) { return toQuat(eulerAngleXZX(t1, t2, t3)); }
  template<typename T, qualifier Q = defaultp> GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleYXY(T t1, T t2, T t3) { return toQuat(eulerAngleYXY(t1, t2, t3)); }
  template<typename T, qualifier Q = defaultp> GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleYZY(T t1, T t2, T t3) { return toQuat(eulerAngleYZY(t1, t2, t3)); }
  template<typename T, qualifier Q = defaultp> GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleZXZ(T t1, T t2, T t3) { return toQuat(eulerAngleZXZ(t1, t2, t3)); }
  template<typename T, qualifier Q = defaultp> GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleZYZ(T t1, T t2, T t3) { return toQuat(eulerAngleZYZ(t1, t2, t3)); }

  /* Euler Conversions; @TODO: Optimize to avoid intermediate matrix creation */
  template<typename T, qualifier Q> GLM_FUNC_QUALIFIER void extractEulerAngleXYX(const qua<T, Q> &q, T &t1, T &t2, T &t3) { extractEulerAngleXYX(toMat3(q), t1, t2, t3); }
  template<typename T, qualifier Q> GLM_FUNC_QUALIFIER void extractEulerAngleXYZ(const qua<T, Q> &q, T &t1, T &t2, T &t3) { extractEulerAngleXYZ(toMat3(q), t1, t2, t3); }
  template<typename T, qualifier Q> GLM_FUNC_QUALIFIER void extractEulerAngleXZX(const qua<T, Q> &q, T &t1, T &t2, T &t3) { extractEulerAngleXZX(toMat3(q), t1, t2, t3); }
  template<typename T, qualifier Q> GLM_FUNC_QUALIFIER void extractEulerAngleXZY(const qua<T, Q> &q, T &t1, T &t2, T &t3) { extractEulerAngleXZY(toMat3(q), t1, t2, t3); }
  template<typename T, qualifier Q> GLM_FUNC_QUALIFIER void extractEulerAngleYXY(const qua<T, Q> &q, T &t1, T &t2, T &t3) { extractEulerAngleYXY(toMat3(q), t1, t2, t3); }
  template<typename T, qualifier Q> GLM_FUNC_QUALIFIER void extractEulerAngleYXZ(const qua<T, Q> &q, T &t1, T &t2, T &t3) { extractEulerAngleYXZ(toMat3(q), t1, t2, t3); }
  template<typename T, qualifier Q> GLM_FUNC_QUALIFIER void extractEulerAngleYZX(const qua<T, Q> &q, T &t1, T &t2, T &t3) { extractEulerAngleYZX(toMat3(q), t1, t2, t3); }
  template<typename T, qualifier Q> GLM_FUNC_QUALIFIER void extractEulerAngleYZY(const qua<T, Q> &q, T &t1, T &t2, T &t3) { extractEulerAngleYZY(toMat3(q), t1, t2, t3); }
  template<typename T, qualifier Q> GLM_FUNC_QUALIFIER void extractEulerAngleZXY(const qua<T, Q> &q, T &t1, T &t2, T &t3) { extractEulerAngleZXY(toMat3(q), t1, t2, t3); }
  template<typename T, qualifier Q> GLM_FUNC_QUALIFIER void extractEulerAngleZXZ(const qua<T, Q> &q, T &t1, T &t2, T &t3) { extractEulerAngleZXZ(toMat3(q), t1, t2, t3); }
  template<typename T, qualifier Q> GLM_FUNC_QUALIFIER void extractEulerAngleZYX(const qua<T, Q> &q, T &t1, T &t2, T &t3) { extractEulerAngleZYX(toMat3(q), t1, t2, t3); }
  template<typename T, qualifier Q> GLM_FUNC_QUALIFIER void extractEulerAngleZYZ(const qua<T, Q> &q, T &t1, T &t2, T &t3) { extractEulerAngleZYZ(toMat3(q), t1, t2, t3); }

  /* EulerAngles -> Quaternion (Optimized) */

  template<typename T, qualifier Q = defaultp>
  GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleXYZ(T t1, T t2, T t3) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'quatEulerAngle' only accept floating-point inputs");

    vec<3, T, Q> s, c;
    sincos(T(0.5) * vec<3, T, Q>(t1, t2, t3), s, c);
    return qua<T, Q>(
      c.x * c.y * c.z + s.y * s.z * s.x,
      s.x * c.y * c.z + s.y * s.z * c.x,
      s.y * c.x * c.z - s.x * s.z * c.y,
      s.z * c.x * c.y + s.x * s.y * c.z
    );
  }

  template<typename T, qualifier Q = defaultp>
  GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleXZY(T t1, T t2, T t3) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'quatEulerAngle' only accept floating-point inputs");

    vec<3, T, Q> s, c;
    sincos(T(0.5) * vec<3, T, Q>(t1, t3, t2), s, c);
    return qua<T, Q>(
      c.x * c.y * c.z - s.y * s.z * s.x,
      s.x * c.y * c.z - s.y * s.z * c.x,
      s.y * c.x * c.z - s.x * s.z * c.y,
      s.z * c.x * c.y + s.x * s.y * c.z
    );
  }

  template<typename T, qualifier Q = defaultp>
  GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleYXZ(T t1, T t2, T t3) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'quatEulerAngle' only accept floating-point inputs");

    vec<3, T, Q> s, c;
    sincos(T(0.5) * vec<3, T, Q>(t2, t1, t3), s, c);
    return qua<T, Q>(
      c.x * c.y * c.z - s.y * s.z * s.x,
      s.x * c.y * c.z + s.y * s.z * c.x,
      s.y * c.x * c.z - s.x * s.z * c.y,
      s.z * c.x * c.y - s.x * s.y * c.z
    );
  }

  template<typename T, qualifier Q = defaultp>
  GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleYZX(T t1, T t2, T t3) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'quatEulerAngle' only accept floating-point inputs");

    vec<3, T, Q> s, c;
    sincos(T(0.5) * vec<3, T, Q>(t3, t1, t2), s, c);
    return qua<T, Q>(
      c.x * c.y * c.z + s.y * s.z * s.x,
      s.x * c.y * c.z + s.y * s.z * c.x,
      s.y * c.x * c.z + s.x * s.z * c.y,
      s.z * c.x * c.y - s.x * s.y * c.z
    );
  }

  template<typename T, qualifier Q = defaultp>
  GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleZXY(T t1, T t2, T t3) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'quatEulerAngle' only accept floating-point inputs");

    vec<3, T, Q> s, c;
    sincos(T(0.5) * vec<3, T, Q>(t2, t3, t1), s, c);
    return qua<T, Q>(
      c.x * c.y * c.z + s.y * s.z * s.x,
      s.x * c.y * c.z - s.y * s.z * c.x,
      s.y * c.x * c.z + s.x * s.z * c.y,
      s.z * c.x * c.y + s.x * s.y * c.z
    );
  }

  template<typename T, qualifier Q = defaultp>
  GLM_FUNC_QUALIFIER qua<T, Q> quatEulerAngleZYX(T t1, T t2, T t3) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'quatEulerAngle' only accept floating-point inputs");

    vec<3, T, Q> s, c;
    sincos(T(0.5) * vec<3, T, Q>(t3, t2, t1), s, c);
    return qua<T, Q>(
      c.x * c.y * c.z - s.y * s.x * s.z,
      s.x * c.y * c.z - s.y * s.z * c.x,
      s.y * c.x * c.z + s.x * s.z * c.y,
      s.z * c.x * c.y - s.x * s.y * c.z
    );
  }

  /* API completeness for quat_cast */

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER qua<T, Q> quat_cast(qua<T, Q> const &q) {
    return q;
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER qua<T, Q> quat_cast(mat<3, 4, T, Q> const &m) {
    return quat_cast(mat<3, 3, T, Q>(m));
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER qua<T, Q> quat_cast(mat<4, 3, T, Q> const &m) {
    return quat_cast(mat<3, 3, T, Q>(m));
  }

  /* quaternion-as-vector4 operations */

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<4, bool, Q> equal(qua<T, Q> const &x, qua<T, Q> const &y, int MaxULPs) {
    return equal(x, y, vec<4, int, Q>(MaxULPs));
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<4, bool, Q> equal(qua<T, Q> const &x, qua<T, Q> const &y, vec<4, T, Q> const &eps) {
    const vec<4, T, Q> v(x.x - y.x, x.y - y.y, x.z - y.z, x.w - y.w);
    return lessThan(abs(v), eps);
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<4, bool, Q> equal(qua<T, Q> const &x, qua<T, Q> const &y, vec<4, int, Q> const &MaxULPs) {
    return equal(vec<4, T, Q>(x.x, x.y, x.z, x.w), vec<4, T, Q>(y.x, y.y, y.z, y.w), MaxULPs);
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<4, bool, Q> notEqual(qua<T, Q> const &x, qua<T, Q> const &y, int MaxULPs) {
    return notEqual(x, y, vec<4, int, Q>(MaxULPs));
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<4, bool, Q> notEqual(qua<T, Q> const &x, qua<T, Q> const &y, vec<4, int, Q> const &MaxULPs) {
    return not_(equal(x, y, MaxULPs));
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<4, bool, Q> notEqual(qua<T, Q> const &x, qua<T, Q> const &y, vec<4, T, Q> const &eps) {
    const vec<4, T, Q> v(x.x - y.x, x.y - y.y, x.z - y.z, x.w - y.w);
    return greaterThanEqual(abs(v), eps);
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_equal(qua<T, Q> const &x, qua<T, Q> const &y) {
    return all(equal(x, y));
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_equal(qua<T, Q> const &x, qua<T, Q> const &y, T eps) {
    return all(equal(x, y, eps));
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_equal(qua<T, Q> const &x, qua<T, Q> const &y, int MaxULPs) {
    return all(equal(x, y, MaxULPs));
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_equal(qua<T, Q> const &x, qua<T, Q> const &y, vec<4, T, Q> const &eps) {
    return all(equal(x, y, eps));
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool all_equal(qua<T, Q> const &x, qua<T, Q> const &y, vec<4, int, Q> const &MaxULPs) {
    return all(equal(x, y, MaxULPs));
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any_notequal(qua<T, Q> const &x, qua<T, Q> const &y) {
    return any(notEqual(x, y));
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any_notequal(qua<T, Q> const &x, qua<T, Q> const &y, T eps) {
    return any(notEqual(x, y, eps));
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any_notequal(qua<T, Q> const &x, qua<T, Q> const &y, int MaxULPs) {
    return any(notEqual(x, y, MaxULPs));
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any_notequal(qua<T, Q> const &x, qua<T, Q> const &y, vec<4, T, Q> const &eps) {
    return any(notEqual(x, y, eps));
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool any_notequal(qua<T, Q> const &x, qua<T, Q> const &y, vec<4, int, Q> const &MaxULPs) {
    return any(notEqual(x, y, MaxULPs));
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER bool isNormalized(qua<T, Q> const &q, T eps = epsilon<T>) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'isNormalized' only accept floating-point inputs");
    return abs(length(q) - static_cast<T>(1)) <= static_cast<T>(2) * eps;
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER bool isNull(qua<T, Q> const &q, T eps = epsilon<T>) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'isNull' only accept floating-point inputs");
    return length(q) <= eps;
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER qua<T, Q> fastNormalize(qua<T, Q> const &x) {
    return x * fastInverseSqrt<T>(dot(x, x));
  }

  /// <summary>
  /// Return true if the quaternion is invertible (i.e., is non-zero and finite).
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER bool invertible(const qua<T, Q> &q, T eps = epsilon<T>()) {
    return all(isfinite(q)) && length2(q) > eps;
  }

  /// <summary>
  /// Return the absolute angle between two quaternions.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER T angle(const qua<T, Q> &x, const qua<T, Q> &y) {
    return deltaAngle(T(0), angle(y * conjugate(x)));
  }

  /// <summary>
  /// Return the oriented angle between two quaternions based on a reference axis.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER T orientedAngle(const qua<T, Q> &x, const qua<T, Q> &y, const vec<3, T, Q> &ref) {
    const qua<T, Q> rot = y * conjugate(x);
    return deltaAngle(T(0), angle(rot)) * sign(dot(ref, axis(rot)));
  }

  /// <summary>
  /// Normalized lerp. Unlike glm::lerp, this function does not sanitize 't'.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER qua<T, Q> nlerp(qua<T, Q> const &x, qua<T, Q> const &y, T a) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'nlerp' only accept floating-point inputs");
    return normalize(x * (static_cast<T>(1) - a) + (y * a));
  }

  /// <summary>
  /// Create a quaternion in barycentric coordinates.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER qua<T, Q> barycentric(qua<T, Q> value1, qua<T, Q> value2, qua<T, Q> value3, T u, T v) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'barycentric' only accept floating-point inputs");

    const qua<T, Q> start = slerp(value1, value2, u + v);
    const qua<T, Q> end = slerp(value1, value3, u + v);
    return slerp(start, end, v / (u + v));
  }

  /// <summary>
  /// Rotates a vector current towards target.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER qua<T, Q> rotateTowards(qua<T, Q> const &x, qua<T, Q> const &y, T maxRadians) {
    const T q_angle = angle(x, y);
    return epsilonNotEqual(q_angle, T(0), epsilon<T>()) ? slerp(x, y, min(T(1), maxRadians / q_angle)) : y;
  }

  /// <summary>
  /// Create the (shortest arc) quaternion that rotates a source direction to
  /// coincide with the target. Note, this function is a function wrapper to
  /// quaternion constructor.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER GLM_CONSTEXPR qua<T, Q> rotateFromTo(const vec<3, T, Q> &sourceDirection, const vec<3, T, Q> &targetDirection) {
    return qua<T, Q>(normalize(sourceDirection), normalize(targetDirection));
  }

  /// <summary>
  /// quatLookAt alternative (from O3DE).
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER qua<T, Q> fromBasis(const vec<3, T, Q> &basisX, const vec<3, T, Q> &basisY, const vec<3, T, Q> &basisZ) {
    T trace(0);
    qua<T, Q> result = glm::identity<qua<T, Q>>();
    if (basisZ.z < T(0)) {
      if (basisX.x > basisY.y) {
        trace = T(1.0) + basisX.x - basisY.y - basisZ.z;
        result = qua<T, Q>(basisY.z - basisZ.y, trace, basisX.y + basisY.x, basisZ.x + basisX.z);
      }
      else {
        trace = T(1.0) - basisX.x + basisY.y - basisZ.z;
        result = qua<T, Q>(basisZ.x - basisX.z, basisX.y + basisY.x, trace, basisY.z + basisZ.y);
      }
    }
    else {
      if (basisX.x < -basisY.y) {
        trace = T(1.0) - basisX.x - basisY.y + basisZ.z;
        result = qua<T, Q>(basisX.y - basisY.x, basisZ.x + basisX.z, basisY.z + basisZ.y, trace);
      }
      else {
        trace = T(1.0) + basisX.x + basisY.y + basisZ.z;
        result = qua<T, Q>(trace, basisY.z - basisZ.y, basisZ.x - basisX.z, basisX.y - basisY.x);
      }
    }
    return result * (T(0.5) * inversesqrt(trace));
  }

  /// <summary>
  /// Creates a right-handed spherical billboard that rotates around a specified
  /// object position.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER qua<T, Q> quatbillboardRH(const vec<3, T, Q> &object, const vec<3, T, Q> &camPos, const vec<3, T, Q> &camUp, const vec<3, T, Q> &camFwd) {
    return toQuat(billboardRH<T, Q, 3, 3>(object, camPos, camUp, camFwd));
  }

  /// <summary>
  /// Creates a left-handed spherical billboard that rotates around a specified
  /// object position.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER qua<T, Q> quatbillboardLH(const vec<3, T, Q> &object, const vec<3, T, Q> &camPos, const vec<3, T, Q> &camUp, const vec<3, T, Q> &camFwd) {
    return toQuat(billboardLH<T, Q, 3, 3>(object, camPos, camUp, camFwd));
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER qua<T, Q> quatbillboard(const vec<3, T, Q> &object, const vec<3, T, Q> &pos, const vec<3, T, Q> &up, const vec<3, T, Q> &forward) {
#if (GLM_CONFIG_CLIP_CONTROL & GLM_CLIP_CONTROL_LH_BIT)
    return quatbillboardLH<T, Q>(object, pos, up, forward);
#else
    return quatbillboardRH<T, Q>(object, pos, up, forward);
#endif
  }

  /*
  ** {======================================================
  ** Fixes
  ** =======================================================
  */

  /// <summary>
  /// API completeness for matrix_extensions.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER void __axisAngle(qua<T, Q> const &q, vec<3, T, Q> &out_axis, T &out_angle) {
    out_angle = angle(q);
    out_axis = axis(q);
  }

  /// <summary>
  /// API completeness for vector_extensions.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER T __angle(qua<T, Q> const &q) {
    const T n = length(vec<3, T, Q>(q.x, q.y, q.z));
    if (epsilonNotEqual(n, T(0), epsilon<T>()))
      return T(2) * atan2(n, abs(q.w));
    return T(0);
  }

  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER T __angle(const qua<T, Q> &x, const qua<T, Q> &y) {
    return __angle(y * conjugate(x));
  }

  /// <summary>
  /// Return the oriented angle between two quaternions based on a reference axis.
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER T __orientedAngle(const qua<T, Q> &x, const qua<T, Q> &y, const vec<3, T, Q> &ref) {
    const qua<T, Q> rot = y * conjugate(x);
    return __angle(rot) * sign(dot(ref, axis(rot)));
  }

  /// <summary>
  /// @GLMFix: __slerp in vector_extensions
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER qua<T, Q> __slerp(qua<T, Q> const &x, qua<T, Q> const &y, T const &a) {
    return slerp(x, y, a);
  }

  template<typename T, typename S, qualifier Q>
  GLM_FUNC_QUALIFIER qua<T, Q> __slerp(qua<T, Q> const &x, qua<T, Q> const &y, T a, S k) {
    return slerp(x, y, a, k);
  }

#if GLM_CONFIG_ALIGNED_GENTYPES == GLM_ENABLE && defined(GLM_FORCE_DEFAULT_ALIGNED_GENTYPES)
  /// <summary>
  /// non-aligned implementation
  /// </summary>
  template<typename T, qualifier Q>
  GLM_FUNC_QUALIFIER vec<4, T, Q> __rotate(qua<T, Q> const &q, vec<4, T, Q> const &v) {
    return detail::compute_quat_mul_vec4<T, Q, false>::call(q, v);
  }
#endif

  /* }====================================================== */
}

#endif
