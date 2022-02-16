/*
** $Id: lglm_string.hpp $
** Stack-based glm/gtx/string_cast.inl implementation
** See Copyright Notice in lua.h
*/

#ifndef lglm_string_h
#define lglm_string_h

#include <cstdio>
#include <cctype>
#include <glm/glm.hpp>

/*
** @GLMFix: GCC forbids forceinline on variadic functions. Compiling with
** glm/gtx/string_cast with GLM_FORCE_INLINE will lead to errors
*/
#if (GLM_COMPILER & GLM_COMPILER_GCC)
  #define LUAGLM_STRFUNC_QUALIFIER
#else
  #define LUAGLM_STRFUNC_QUALIFIER GLM_FUNC_QUALIFIER
#endif

/*
** {==================================================================
** glm::to_string implemented on the stack
** ===================================================================
*/

/* Avoid declaring GLM_INLINE in the string formatting functions. */
#define LUAGLM_FMT_QUALIFIER GLM_CUDA_FUNC_DEF

/* Buffer size of format string/text */
#define LUAGLM_FMT_LEN 256

/* Simplify template casting */
#define LUAGLM_STRCAST(X) static_cast<typename lglmcast<T>::value_type>((X))

/* Common lglm_compute_to_string header */
#define LUAGLM_STRHEADER                                                             \
  char const *Prefix = lglmprefix<T>::value();                                       \
  char const *Lilteral = lglmliteral<T, std::numeric_limits<T>::is_iec559>::value(); \
  char format_text[LUAGLM_FMT_LEN];

/* Forward declare functor for pushing Lua strings without intermediate std::string */
namespace glm {
namespace detail {
  static LUAGLM_STRFUNC_QUALIFIER int _vsnprintf(char *buff, size_t buff_len, const char *msg, ...) {
    int length = 0;

    va_list list;
    va_start(list, msg);
#if (GLM_COMPILER & GLM_COMPILER_VC)
    length = vsprintf_s(buff, buff_len, msg, list);
#else
    length = vsnprintf(buff, buff_len, msg, list);
#endif
    va_end(list);

    assert(length > 0);
    return length;
  }

  /// <summary>
  /// string_cast.inl: cast without the dependency
  /// </summary>
  template<typename T> struct lglmcast { typedef T value_type; };
  template<> struct lglmcast<float> { typedef double value_type; };
#if LUA_FLOAT_TYPE == LUA_FLOAT_LONGDOUBLE
  template<> struct lglmcast<long double> { typedef double value_type; };
#endif

  /// <summary>
  /// string_cast.inl: literal without the dependency
  /// </summary>
  template<typename T, bool isFloat = false>
  struct lglmliteral { GLM_FUNC_QUALIFIER static char const *value() { return "%d"; } };

  template<typename T>
  struct lglmliteral<T, true> { GLM_FUNC_QUALIFIER static char const *value() { return "%f"; } };
#if GLM_MODEL == GLM_MODEL_32 && GLM_COMPILER && GLM_COMPILER_VC
  template<> struct lglmliteral<int64_t, false> { GLM_FUNC_QUALIFIER static char const *value() { return "%lld"; } };
  template<> struct lglmliteral<uint64_t, false> { GLM_FUNC_QUALIFIER static char const *value() { return "%lld"; } };
#endif  // GLM_MODEL == GLM_MODEL_32 && GLM_COMPILER && GLM_COMPILER_VC

  /// <summary>
  /// string_cast.inl: prefix without the dependency
  /// </summary>
  template<typename T> struct lglmprefix{};
  template<> struct lglmprefix<float> { GLM_FUNC_QUALIFIER static char const *value() { return ""; } };
  template<> struct lglmprefix<double> { GLM_FUNC_QUALIFIER static char const *value() { return ""; } };  // Changed for LuaGLM
#if LUA_FLOAT_TYPE == LUA_FLOAT_LONGDOUBLE
  template<> struct lglmprefix<long double> { GLM_FUNC_QUALIFIER static char const *value() { return ""; } };
#endif
  template<> struct lglmprefix<bool> { GLM_FUNC_QUALIFIER static char const *value() { return "b"; } };
  template<> struct lglmprefix<uint8_t> { GLM_FUNC_QUALIFIER static char const *value() { return "u8"; } };
  template<> struct lglmprefix<int8_t> { GLM_FUNC_QUALIFIER static char const *value() { return "i8"; } };
  template<> struct lglmprefix<uint16_t> { GLM_FUNC_QUALIFIER static char const *value() { return "u16"; } };
  template<> struct lglmprefix<int16_t> { GLM_FUNC_QUALIFIER static char const *value() { return "i16"; } };
  template<> struct lglmprefix<uint32_t> { GLM_FUNC_QUALIFIER static char const *value() { return "u"; } };
  template<> struct lglmprefix<int32_t> { GLM_FUNC_QUALIFIER static char const *value() { return "i"; } };
  template<> struct lglmprefix<uint64_t> { GLM_FUNC_QUALIFIER static char const *value() { return "u64"; } };
  template<> struct lglmprefix<int64_t> { GLM_FUNC_QUALIFIER static char const *value() { return "i64"; } };

  template<typename matType>
  struct lglm_compute_to_string { };

  template<typename T, qualifier Q>
  struct lglm_compute_to_string<vec<1, T, Q>> {
    static LUAGLM_FMT_QUALIFIER int call(char *buff, size_t buff_len, vec<1, glm_Float> const &x) {
      LUAGLM_STRHEADER
      _vsnprintf(format_text, LUAGLM_FMT_LEN, "%svec1(%s)", Prefix, Lilteral);
      return _vsnprintf(buff, buff_len, format_text, LUAGLM_STRCAST(x[0]));
    }
  };

  template<typename T, qualifier Q>
  struct lglm_compute_to_string<vec<2, T, Q>> {
    static LUAGLM_FMT_QUALIFIER int call(char *buff, size_t buff_len, vec<2, glm_Float> const &x) {
      LUAGLM_STRHEADER
      _vsnprintf(format_text, LUAGLM_FMT_LEN, "%svec2(%s, %s)", Prefix, Lilteral, Lilteral);
      return _vsnprintf(buff, buff_len, format_text, LUAGLM_STRCAST(x[0]), LUAGLM_STRCAST(x[1]));
    }
  };

  template<typename T, qualifier Q>
  struct lglm_compute_to_string<vec<3, T, Q>> {
    static LUAGLM_FMT_QUALIFIER int call(char *buff, size_t buff_len, vec<3, glm_Float> const &x) {
      LUAGLM_STRHEADER
      _vsnprintf(format_text, LUAGLM_FMT_LEN, "%svec3(%s, %s, %s)", Prefix, Lilteral, Lilteral, Lilteral);
      return _vsnprintf(buff, buff_len, format_text,
        LUAGLM_STRCAST(x[0]),
        LUAGLM_STRCAST(x[1]),
        LUAGLM_STRCAST(x[2])
      );
    }
  };

  template<typename T, qualifier Q>
  struct lglm_compute_to_string<vec<4, T, Q>> {
    static LUAGLM_FMT_QUALIFIER int call(char *buff, size_t buff_len, vec<4, glm_Float> const &x) {
      LUAGLM_STRHEADER
      _vsnprintf(format_text, LUAGLM_FMT_LEN, "%svec4(%s, %s, %s, %s)", Prefix, Lilteral, Lilteral, Lilteral, Lilteral);
      return _vsnprintf(buff, buff_len, format_text,
        LUAGLM_STRCAST(x[0]),
        LUAGLM_STRCAST(x[1]),
        LUAGLM_STRCAST(x[2]),
        LUAGLM_STRCAST(x[3])
      );
    }
  };

  template<typename T, qualifier Q>
  struct lglm_compute_to_string<qua<T, Q>> {
    static LUAGLM_FMT_QUALIFIER int call(char *buff, size_t buff_len, qua<glm_Float> const &q) {
      LUAGLM_STRHEADER
      _vsnprintf(format_text, LUAGLM_FMT_LEN, "%squat(%s, {%s, %s, %s})", Prefix, Lilteral, Lilteral, Lilteral, Lilteral);
      return _vsnprintf(buff, buff_len, format_text,
        LUAGLM_STRCAST(q.w),
        LUAGLM_STRCAST(q.x),
        LUAGLM_STRCAST(q.y),
        LUAGLM_STRCAST(q.z)
      );
    }
  };

  template<typename T, qualifier Q>
  struct lglm_compute_to_string<mat<2, 2, T, Q>> {
    static LUAGLM_FMT_QUALIFIER int call(char *buff, size_t buff_len, mat<2, 2, T, Q> const &x) {
      LUAGLM_STRHEADER
      _vsnprintf(format_text, LUAGLM_FMT_LEN, "%smat2x2((%s, %s), (%s, %s))",
        Prefix,
        Lilteral, Lilteral,
        Lilteral, Lilteral
      );

      return _vsnprintf(buff, buff_len, format_text,
        LUAGLM_STRCAST(x[0][0]), LUAGLM_STRCAST(x[0][1]),
        LUAGLM_STRCAST(x[1][0]), LUAGLM_STRCAST(x[1][1])
      );
    }
  };

  template<typename T, qualifier Q>
  struct lglm_compute_to_string<mat<2, 3, T, Q>> {
    static LUAGLM_FMT_QUALIFIER int call(char *buff, size_t buff_len, mat<4, 3, T, Q> const &x) {
      LUAGLM_STRHEADER
      _vsnprintf(format_text, LUAGLM_FMT_LEN, "%smat2x3((%s, %s, %s), (%s, %s, %s))",
        Prefix,
        Lilteral, Lilteral, Lilteral,
        Lilteral, Lilteral, Lilteral
      );

      return _vsnprintf(buff, buff_len, format_text,
        LUAGLM_STRCAST(x[0][0]), LUAGLM_STRCAST(x[0][1]), LUAGLM_STRCAST(x[0][2]),
        LUAGLM_STRCAST(x[1][0]), LUAGLM_STRCAST(x[1][1]), LUAGLM_STRCAST(x[1][2])
      );
    }
  };

  template<typename T, qualifier Q>
  struct lglm_compute_to_string<mat<2, 4, T, Q>> {
    static LUAGLM_FMT_QUALIFIER int call(char *buff, size_t buff_len, mat<2, 4, T, Q> const &x) {
      LUAGLM_STRHEADER
      _vsnprintf(format_text, LUAGLM_FMT_LEN, "%smat2x4((%s, %s, %s, %s), (%s, %s, %s, %s))",
        Prefix,
        Lilteral, Lilteral, Lilteral, Lilteral,
        Lilteral, Lilteral, Lilteral, Lilteral
      );

      return _vsnprintf(buff, buff_len, format_text,
        LUAGLM_STRCAST(x[0][0]), LUAGLM_STRCAST(x[0][1]), LUAGLM_STRCAST(x[0][2]), LUAGLM_STRCAST(x[0][3]),
        LUAGLM_STRCAST(x[1][0]), LUAGLM_STRCAST(x[1][1]), LUAGLM_STRCAST(x[1][2]), LUAGLM_STRCAST(x[1][3])
      );
    }
  };

  template<typename T, qualifier Q>
  struct lglm_compute_to_string<mat<3, 2, T, Q>> {
    static LUAGLM_FMT_QUALIFIER int call(char *buff, size_t buff_len, mat<3, 2, T, Q> const &x) {
      LUAGLM_STRHEADER
      _vsnprintf(format_text, LUAGLM_FMT_LEN, "%smat3x2((%s, %s), (%s, %s), (%s, %s))",
        Prefix,
        Lilteral, Lilteral,
        Lilteral, Lilteral,
        Lilteral, Lilteral
      );

      return _vsnprintf(buff, buff_len, format_text,
        LUAGLM_STRCAST(x[0][0]), LUAGLM_STRCAST(x[0][1]),
        LUAGLM_STRCAST(x[1][0]), LUAGLM_STRCAST(x[1][1]),
        LUAGLM_STRCAST(x[2][0]), LUAGLM_STRCAST(x[2][1])
      );
    }
  };

  template<typename T, qualifier Q>
  struct lglm_compute_to_string<mat<3, 3, T, Q>> {
    static LUAGLM_FMT_QUALIFIER int call(char *buff, size_t buff_len, mat<3, 3, T, Q> const &x) {
      LUAGLM_STRHEADER
      _vsnprintf(format_text, LUAGLM_FMT_LEN, "%smat3x3((%s, %s, %s), (%s, %s, %s), (%s, %s, %s))",
        Prefix,
        Lilteral, Lilteral, Lilteral,
        Lilteral, Lilteral, Lilteral,
        Lilteral, Lilteral, Lilteral
      );

      return _vsnprintf(buff, buff_len, format_text,
        LUAGLM_STRCAST(x[0][0]), LUAGLM_STRCAST(x[0][1]), LUAGLM_STRCAST(x[0][2]),
        LUAGLM_STRCAST(x[1][0]), LUAGLM_STRCAST(x[1][1]), LUAGLM_STRCAST(x[1][2]),
        LUAGLM_STRCAST(x[2][0]), LUAGLM_STRCAST(x[2][1]), LUAGLM_STRCAST(x[2][2])
      );
    }
  };

  template<typename T, qualifier Q>
  struct lglm_compute_to_string<mat<3, 4, T, Q>> {
    static LUAGLM_FMT_QUALIFIER int call(char *buff, size_t buff_len, mat<3, 4, T, Q> const &x) {
      LUAGLM_STRHEADER
      _vsnprintf(format_text, LUAGLM_FMT_LEN, "%smat3x4((%s, %s, %s, %s), (%s, %s, %s, %s), (%s, %s, %s, %s))",
        Prefix,
        Lilteral, Lilteral, Lilteral, Lilteral,
        Lilteral, Lilteral, Lilteral, Lilteral,
        Lilteral, Lilteral, Lilteral, Lilteral
      );

      return _vsnprintf(buff, buff_len, format_text,
        LUAGLM_STRCAST(x[0][0]), LUAGLM_STRCAST(x[0][1]), LUAGLM_STRCAST(x[0][2]), LUAGLM_STRCAST(x[0][3]),
        LUAGLM_STRCAST(x[1][0]), LUAGLM_STRCAST(x[1][1]), LUAGLM_STRCAST(x[1][2]), LUAGLM_STRCAST(x[1][3]),
        LUAGLM_STRCAST(x[2][0]), LUAGLM_STRCAST(x[2][1]), LUAGLM_STRCAST(x[2][2]), LUAGLM_STRCAST(x[2][3])
      );
    }
  };

  template<typename T, qualifier Q>
  struct lglm_compute_to_string<mat<4, 2, T, Q>> {
    static LUAGLM_FMT_QUALIFIER int call(char *buff, size_t buff_len, mat<4, 2, T, Q> const &x) {
      LUAGLM_STRHEADER
      _vsnprintf(format_text, LUAGLM_FMT_LEN, "%smat4x2((%s, %s), (%s, %s), (%s, %s), (%s, %s))",
        Prefix,
        Lilteral, Lilteral,
        Lilteral, Lilteral,
        Lilteral, Lilteral,
        Lilteral, Lilteral
      );

      return _vsnprintf(buff, buff_len, format_text,
        LUAGLM_STRCAST(x[0][0]), LUAGLM_STRCAST(x[0][1]),
        LUAGLM_STRCAST(x[1][0]), LUAGLM_STRCAST(x[1][1]),
        LUAGLM_STRCAST(x[2][0]), LUAGLM_STRCAST(x[2][1]),
        LUAGLM_STRCAST(x[3][0]), LUAGLM_STRCAST(x[3][1])
      );
    }
  };

  template<typename T, qualifier Q>
  struct lglm_compute_to_string<mat<4, 3, T, Q>> {
    static LUAGLM_FMT_QUALIFIER int call(char *buff, size_t buff_len, mat<4, 3, T, Q> const &x) {
      LUAGLM_STRHEADER
      _vsnprintf(format_text, LUAGLM_FMT_LEN, "%smat4x3((%s, %s, %s), (%s, %s, %s), (%s, %s, %s), (%s, %s, %s))",
        Prefix,
        Lilteral, Lilteral, Lilteral,
        Lilteral, Lilteral, Lilteral,
        Lilteral, Lilteral, Lilteral,
        Lilteral, Lilteral, Lilteral
      );

      return _vsnprintf(buff, buff_len, format_text,
        LUAGLM_STRCAST(x[0][0]), LUAGLM_STRCAST(x[0][1]), LUAGLM_STRCAST(x[0][2]),
        LUAGLM_STRCAST(x[1][0]), LUAGLM_STRCAST(x[1][1]), LUAGLM_STRCAST(x[1][2]),
        LUAGLM_STRCAST(x[2][0]), LUAGLM_STRCAST(x[2][1]), LUAGLM_STRCAST(x[2][2]),
        LUAGLM_STRCAST(x[3][0]), LUAGLM_STRCAST(x[3][1]), LUAGLM_STRCAST(x[3][2])
      );
    }
  };

  template<typename T, qualifier Q>
  struct lglm_compute_to_string<mat<4, 4, T, Q>> {
    static LUAGLM_FMT_QUALIFIER int call(char *buff, size_t buff_len, mat<4, 4, T, Q> const &x) {
      LUAGLM_STRHEADER
      _vsnprintf(format_text, LUAGLM_FMT_LEN, "%smat4x4((%s, %s, %s, %s), (%s, %s, %s, %s), (%s, %s, %s, %s), (%s, %s, %s, %s))",
        Prefix,
        Lilteral, Lilteral, Lilteral, Lilteral,
        Lilteral, Lilteral, Lilteral, Lilteral,
        Lilteral, Lilteral, Lilteral, Lilteral,
        Lilteral, Lilteral, Lilteral, Lilteral
      );

      return _vsnprintf(buff, buff_len, format_text,
        LUAGLM_STRCAST(x[0][0]), LUAGLM_STRCAST(x[0][1]), LUAGLM_STRCAST(x[0][2]), LUAGLM_STRCAST(x[0][3]),
        LUAGLM_STRCAST(x[1][0]), LUAGLM_STRCAST(x[1][1]), LUAGLM_STRCAST(x[1][2]), LUAGLM_STRCAST(x[1][3]),
        LUAGLM_STRCAST(x[2][0]), LUAGLM_STRCAST(x[2][1]), LUAGLM_STRCAST(x[2][2]), LUAGLM_STRCAST(x[2][3]),
        LUAGLM_STRCAST(x[3][0]), LUAGLM_STRCAST(x[3][1]), LUAGLM_STRCAST(x[3][2]), LUAGLM_STRCAST(x[3][3])
      );
    }
  };

  /// <summary>
  /// Return: the number of characters written if successful or negative value
  /// if an error occurred.
  /// </summary>
  template<class matType>
  static GLM_FUNC_QUALIFIER int format_type(char *buff, size_t buff_len, matType const &x) {
    return detail::lglm_compute_to_string<matType>::call(buff, buff_len, x);
  }
}
}

/* }================================================================== */

/*
** {==================================================================
** glm::hash implementation without std::hash dependency.
** ===================================================================
*/

namespace glm {
namespace hash {

  /// <summary>
  /// Temporary solution as the previous implementation was slow. A variety of
  /// 'spatial hashing' algorithms exist for vector-type structures and should
  /// be considered.
  /// </summary>
  template<typename T>
  struct lglm_hash {
    GLM_FUNC_DECL size_t operator()(const T &n) const;
  };

  template<>
  struct lglm_hash<double> {
    GLM_FUNC_QUALIFIER size_t operator()(const double &n) const {
      union { double __t; size_t __a; } __scalar_hash;
      __scalar_hash.__a = 0;
      __scalar_hash.__t = n;
      return (n == 0.0) ? 0 : __scalar_hash.__a;  // -/+ 0.0 should return same hash.
    }
  };

  template<>
  struct lglm_hash<float> {
    GLM_FUNC_QUALIFIER size_t operator()(const float &n) const {
      union { float __t; size_t __a; } __scalar_hash;
      __scalar_hash.__a = 0;
      __scalar_hash.__t = n;
      return (n == 0.f) ? 0 : __scalar_hash.__a;  // -/+ 0.0 should return same hash.
    }
  };

#if LUA_FLOAT_TYPE == LUA_FLOAT_LONGDOUBLE
  /// <summary>
  /// Not the most robust solution; see libcxx's implementation.
  /// </summary>
  template<>
  struct lglm_hash<long double> {
    GLM_FUNC_QUALIFIER size_t operator()(const long double &n) const {
      using T = long double;  // -/+ 0.0 should return same hash.
#if defined(__i386__) || (defined(__x86_64__) && defined(__ILP32__))
      union { T t; struct { size_t a, b, c, d; } s; } u;
      u.s.a = 0;
      u.s.b = 0;
      u.s.c = 0;
      u.s.d = 0;
      u.t = n;
      return (n == T(0)) ? 0 : (u.s.a ^ u.s.b ^ u.s.c ^ u.s.d);
#else
      union { T t; struct { size_t a; size_t b; } s; } u;
      u.s.a = 0;
      u.s.b = 0;
      u.t = n;
      return (n == T(0)) ? 0 : (u.s.a ^ u.s.b);
#endif
    }
  };
#endif

  /// <summary>
  /// glm::detail::hash_combine
  /// </summary>
  static GLM_INLINE void lglm_hashcombine(size_t &seed, size_t hash) {
    hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= hash;
  }

  template<typename T, glm::qualifier Q>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_floating_point<T>::value, size_t>::type hash(glm::vec<2, T, Q> const &v) {
    const lglm_hash<T> hasher;

    size_t seed = 0;
    lglm_hashcombine(seed, hasher(v.x));
    lglm_hashcombine(seed, hasher(v.y));
    return seed;
  }

  template<typename T, glm::qualifier Q>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_floating_point<T>::value, size_t>::type hash(glm::vec<3, T, Q> const &v) {
    const lglm_hash<T> hasher;

    size_t seed = 0;
    lglm_hashcombine(seed, hasher(v.x));
    lglm_hashcombine(seed, hasher(v.y));
    lglm_hashcombine(seed, hasher(v.z));
    return seed;
  }

  template<typename T, glm::qualifier Q>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_floating_point<T>::value, size_t>::type hash(glm::vec<4, T, Q> const &v) {
    const lglm_hash<T> hasher;

    size_t seed = 0;
    lglm_hashcombine(seed, hasher(v.x));
    lglm_hashcombine(seed, hasher(v.y));
    lglm_hashcombine(seed, hasher(v.z));
    lglm_hashcombine(seed, hasher(v.w));
    return seed;
  }

  template<typename T, glm::qualifier Q>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_floating_point<T>::value, size_t>::type hash(glm::qua<T, Q> const &v) {
    const lglm_hash<T> hasher;

    size_t seed = 0;
    lglm_hashcombine(seed, hasher(v.x));
    lglm_hashcombine(seed, hasher(v.y));
    lglm_hashcombine(seed, hasher(v.z));
    lglm_hashcombine(seed, hasher(v.w));
    return seed;
  }
}
}

/* }================================================================== */

#undef LUAGLM_STRHEADER

#endif
