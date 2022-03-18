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
#define LUAGLM_STRFUNC_QUALIFIER

/*
** {==================================================================
** glm::to_string implemented on the stack
** ===================================================================
*/

/* Buffer size of format string/text */
#define LUAGLM_FORMAT_LEN 256

/* Simplify template casting */
#define LUAGLM_SCAST(X) static_cast<typename lua_cast<T>::value_type>((X))
#define LUAGLM_LITERAL lua_literal<T, std::numeric_limits<T>::is_iec559>::value()

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
      assert(length >= 0 && static_cast<size_t>(length) < buff_len);
      return length;
    }

    /// <summary>
    /// string_cast.inl: cast without the dependency
    /// </summary>
    template<typename T> struct lua_cast { typedef T value_type; };
    template<> struct lua_cast<float> { typedef double value_type; };
  #if LUA_FLOAT_TYPE == LUA_FLOAT_LONGDOUBLE
    template<> struct lua_cast<long double> { typedef double value_type; };
  #endif

    /// <summary>
    /// string_cast.inl: literal without the dependency
    /// </summary>
    template<typename T, bool isFloat = false>
    struct lua_literal { static GLM_CONSTEXPR char const *value() { return "%d"; } };

    template<typename T>
    struct lua_literal<T, true> { static GLM_CONSTEXPR char const *value() { return "%f"; } };
  #if GLM_MODEL == GLM_MODEL_32 && GLM_COMPILER && GLM_COMPILER_VC
    template<> struct lua_literal<int64_t, false> { static GLM_CONSTEXPR char const *value() { return "%lld"; } };
    template<> struct lua_literal<uint64_t, false> { static GLM_CONSTEXPR char const *value() { return "%lld"; } };
  #endif  // GLM_MODEL == GLM_MODEL_32 && GLM_COMPILER && GLM_COMPILER_VC

    /// <summary>
    /// string_cast.inl: prefix without the dependency
    /// </summary>
    template<typename T> struct lua_prefix{};
    template<> struct lua_prefix<float> { static GLM_CONSTEXPR char const *value() { return ""; } };
    template<> struct lua_prefix<double> { static GLM_CONSTEXPR char const *value() { return ""; } };  // Changed for LuaGLM
    template<> struct lua_prefix<bool> { static GLM_CONSTEXPR char const *value() { return "b"; } };
    template<> struct lua_prefix<uint8_t> { static GLM_CONSTEXPR char const *value() { return "u8"; } };
    template<> struct lua_prefix<int8_t> { static GLM_CONSTEXPR char const *value() { return "i8"; } };
    template<> struct lua_prefix<uint16_t> { static GLM_CONSTEXPR char const *value() { return "u16"; } };
    template<> struct lua_prefix<int16_t> { static GLM_CONSTEXPR char const *value() { return "i16"; } };
    template<> struct lua_prefix<uint32_t> { static GLM_CONSTEXPR char const *value() { return "u"; } };
    template<> struct lua_prefix<int32_t> { static GLM_CONSTEXPR char const *value() { return "i"; } };
    template<> struct lua_prefix<uint64_t> { static GLM_CONSTEXPR char const *value() { return "u64"; } };
    template<> struct lua_prefix<int64_t> { static GLM_CONSTEXPR char const *value() { return "i64"; } };
  #if LUA_FLOAT_TYPE == LUA_FLOAT_LONGDOUBLE
    template<> struct lua_prefix<long double> { static GLM_CONSTEXPR char const *value() { return ""; } };
  #endif

    template<typename T>
    struct lua_compute_to_string { };

    template<typename T, qualifier Q>
    struct lua_compute_to_string<vec<1, T, Q>> {
      static LUAGLM_STRFUNC_QUALIFIER int call(char *buff, size_t buff_len, vec<1, T> const &x) {
        char f[LUAGLM_FORMAT_LEN];
        _vsnprintf(f, sizeof(f), "%svec1(%s)", lua_prefix<T>::value(), LUAGLM_LITERAL);
        return _vsnprintf(buff, buff_len, f, LUAGLM_SCAST(x[0]));
      }
    };

    template<typename T, qualifier Q>
    struct lua_compute_to_string<vec<2, T, Q>> {
      static LUAGLM_STRFUNC_QUALIFIER int call(char *buff, size_t buff_len, vec<2, T> const &x) {
        char f[LUAGLM_FORMAT_LEN];
        _vsnprintf(f, sizeof(f), "%svec2(%s, %s)", lua_prefix<T>::value(), LUAGLM_LITERAL, LUAGLM_LITERAL);
        return _vsnprintf(buff, buff_len, f, LUAGLM_SCAST(x[0]), LUAGLM_SCAST(x[1]));
      }
    };

    template<typename T, qualifier Q>
    struct lua_compute_to_string<vec<3, T, Q>> {
      static LUAGLM_STRFUNC_QUALIFIER int call(char *buff, size_t buff_len, vec<3, T> const &x) {
        char f[LUAGLM_FORMAT_LEN];
        _vsnprintf(f, sizeof(f), "%svec3(%s, %s, %s)", lua_prefix<T>::value(),
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL
        );

        return _vsnprintf(buff, buff_len, f,
          LUAGLM_SCAST(x[0]), LUAGLM_SCAST(x[1]), LUAGLM_SCAST(x[2])
        );
      }
    };

    template<typename T, qualifier Q>
    struct lua_compute_to_string<vec<4, T, Q>> {
      static LUAGLM_STRFUNC_QUALIFIER int call(char *buff, size_t buff_len, vec<4, T> const &x) {
        char f[LUAGLM_FORMAT_LEN];
        _vsnprintf(f, sizeof(f), "%svec4(%s, %s, %s, %s)", lua_prefix<T>::value(),
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL
        );

        return _vsnprintf(buff, buff_len, f,
          LUAGLM_SCAST(x[0]), LUAGLM_SCAST(x[1]), LUAGLM_SCAST(x[2]), LUAGLM_SCAST(x[3])
        );
      }
    };

    template<typename T, qualifier Q>
    struct lua_compute_to_string<qua<T, Q>> {
      static LUAGLM_STRFUNC_QUALIFIER int call(char *buff, size_t buff_len, qua<T> const &q) {
        char f[LUAGLM_FORMAT_LEN];
        _vsnprintf(f, sizeof(f), "%squat(%s, {%s, %s, %s})", lua_prefix<T>::value(),
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL
        );

        return _vsnprintf(buff, buff_len, f,
          LUAGLM_SCAST(q.w), LUAGLM_SCAST(q.x), LUAGLM_SCAST(q.y), LUAGLM_SCAST(q.z)
        );
      }
    };

    template<typename T, qualifier Q>
    struct lua_compute_to_string<mat<2, 2, T, Q>> {
      static LUAGLM_STRFUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<2, 2, T, Q> const &x) {
        char f[LUAGLM_FORMAT_LEN];
        _vsnprintf(f, sizeof(f), "%smat2x2((%s, %s), (%s, %s))", lua_prefix<T>::value(),
          LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL
        );

        return _vsnprintf(buff, buff_len, f,
          LUAGLM_SCAST(x[0][0]), LUAGLM_SCAST(x[0][1]),
          LUAGLM_SCAST(x[1][0]), LUAGLM_SCAST(x[1][1])
        );
      }
    };

    template<typename T, qualifier Q>
    struct lua_compute_to_string<mat<2, 3, T, Q>> {
      static LUAGLM_STRFUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<4, 3, T, Q> const &x) {
        char f[LUAGLM_FORMAT_LEN];
        _vsnprintf(f, sizeof(f), "%smat2x3((%s, %s, %s), (%s, %s, %s))", lua_prefix<T>::value(),
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL
        );

        return _vsnprintf(buff, buff_len, f,
          LUAGLM_SCAST(x[0][0]), LUAGLM_SCAST(x[0][1]), LUAGLM_SCAST(x[0][2]),
          LUAGLM_SCAST(x[1][0]), LUAGLM_SCAST(x[1][1]), LUAGLM_SCAST(x[1][2])
        );
      }
    };

    template<typename T, qualifier Q>
    struct lua_compute_to_string<mat<2, 4, T, Q>> {
      static LUAGLM_STRFUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<2, 4, T, Q> const &x) {
        char f[LUAGLM_FORMAT_LEN];
        _vsnprintf(f, sizeof(f), "%smat2x4((%s, %s, %s, %s), (%s, %s, %s, %s))", lua_prefix<T>::value(),
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL
        );

        return _vsnprintf(buff, buff_len, f,
          LUAGLM_SCAST(x[0][0]), LUAGLM_SCAST(x[0][1]), LUAGLM_SCAST(x[0][2]), LUAGLM_SCAST(x[0][3]),
          LUAGLM_SCAST(x[1][0]), LUAGLM_SCAST(x[1][1]), LUAGLM_SCAST(x[1][2]), LUAGLM_SCAST(x[1][3])
        );
      }
    };

    template<typename T, qualifier Q>
    struct lua_compute_to_string<mat<3, 2, T, Q>> {
      static LUAGLM_STRFUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<3, 2, T, Q> const &x) {
        char f[LUAGLM_FORMAT_LEN];
        _vsnprintf(f, sizeof(f), "%smat3x2((%s, %s), (%s, %s), (%s, %s))", lua_prefix<T>::value(),
          LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL
        );

        return _vsnprintf(buff, buff_len, f,
          LUAGLM_SCAST(x[0][0]), LUAGLM_SCAST(x[0][1]),
          LUAGLM_SCAST(x[1][0]), LUAGLM_SCAST(x[1][1]),
          LUAGLM_SCAST(x[2][0]), LUAGLM_SCAST(x[2][1])
        );
      }
    };

    template<typename T, qualifier Q>
    struct lua_compute_to_string<mat<3, 3, T, Q>> {
      static LUAGLM_STRFUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<3, 3, T, Q> const &x) {
        char f[LUAGLM_FORMAT_LEN];
        _vsnprintf(f, sizeof(f), "%smat3x3((%s, %s, %s), (%s, %s, %s), (%s, %s, %s))", lua_prefix<T>::value(),
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL
        );

        return _vsnprintf(buff, buff_len, f,
          LUAGLM_SCAST(x[0][0]), LUAGLM_SCAST(x[0][1]), LUAGLM_SCAST(x[0][2]),
          LUAGLM_SCAST(x[1][0]), LUAGLM_SCAST(x[1][1]), LUAGLM_SCAST(x[1][2]),
          LUAGLM_SCAST(x[2][0]), LUAGLM_SCAST(x[2][1]), LUAGLM_SCAST(x[2][2])
        );
      }
    };

    template<typename T, qualifier Q>
    struct lua_compute_to_string<mat<3, 4, T, Q>> {
      static LUAGLM_STRFUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<3, 4, T, Q> const &x) {
        char f[LUAGLM_FORMAT_LEN];
        _vsnprintf(f, sizeof(f), "%smat3x4((%s, %s, %s, %s), (%s, %s, %s, %s), (%s, %s, %s, %s))", lua_prefix<T>::value(),
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL
        );

        return _vsnprintf(buff, buff_len, f,
          LUAGLM_SCAST(x[0][0]), LUAGLM_SCAST(x[0][1]), LUAGLM_SCAST(x[0][2]), LUAGLM_SCAST(x[0][3]),
          LUAGLM_SCAST(x[1][0]), LUAGLM_SCAST(x[1][1]), LUAGLM_SCAST(x[1][2]), LUAGLM_SCAST(x[1][3]),
          LUAGLM_SCAST(x[2][0]), LUAGLM_SCAST(x[2][1]), LUAGLM_SCAST(x[2][2]), LUAGLM_SCAST(x[2][3])
        );
      }
    };

    template<typename T, qualifier Q>
    struct lua_compute_to_string<mat<4, 2, T, Q>> {
      static LUAGLM_STRFUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<4, 2, T, Q> const &x) {
        char f[LUAGLM_FORMAT_LEN];
        _vsnprintf(f, sizeof(f), "%smat4x2((%s, %s), (%s, %s), (%s, %s), (%s, %s))", lua_prefix<T>::value(),
          LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL
        );

        return _vsnprintf(buff, buff_len, f,
          LUAGLM_SCAST(x[0][0]), LUAGLM_SCAST(x[0][1]),
          LUAGLM_SCAST(x[1][0]), LUAGLM_SCAST(x[1][1]),
          LUAGLM_SCAST(x[2][0]), LUAGLM_SCAST(x[2][1]),
          LUAGLM_SCAST(x[3][0]), LUAGLM_SCAST(x[3][1])
        );
      }
    };

    template<typename T, qualifier Q>
    struct lua_compute_to_string<mat<4, 3, T, Q>> {
      static LUAGLM_STRFUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<4, 3, T, Q> const &x) {
        char f[LUAGLM_FORMAT_LEN];
        _vsnprintf(f, sizeof(f), "%smat4x3((%s, %s, %s), (%s, %s, %s), (%s, %s, %s), (%s, %s, %s))", lua_prefix<T>::value(),
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL
        );

        return _vsnprintf(buff, buff_len, f,
          LUAGLM_SCAST(x[0][0]), LUAGLM_SCAST(x[0][1]), LUAGLM_SCAST(x[0][2]),
          LUAGLM_SCAST(x[1][0]), LUAGLM_SCAST(x[1][1]), LUAGLM_SCAST(x[1][2]),
          LUAGLM_SCAST(x[2][0]), LUAGLM_SCAST(x[2][1]), LUAGLM_SCAST(x[2][2]),
          LUAGLM_SCAST(x[3][0]), LUAGLM_SCAST(x[3][1]), LUAGLM_SCAST(x[3][2])
        );
      }
    };

    template<typename T, qualifier Q>
    struct lua_compute_to_string<mat<4, 4, T, Q>> {
      static LUAGLM_STRFUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<4, 4, T, Q> const &x) {
        char f[LUAGLM_FORMAT_LEN];
        _vsnprintf(f, sizeof(f), "%smat4x4((%s, %s, %s, %s), (%s, %s, %s, %s), (%s, %s, %s, %s), (%s, %s, %s, %s))", lua_prefix<T>::value(),
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL,
          LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL, LUAGLM_LITERAL
        );

        return _vsnprintf(buff, buff_len, f,
          LUAGLM_SCAST(x[0][0]), LUAGLM_SCAST(x[0][1]), LUAGLM_SCAST(x[0][2]), LUAGLM_SCAST(x[0][3]),
          LUAGLM_SCAST(x[1][0]), LUAGLM_SCAST(x[1][1]), LUAGLM_SCAST(x[1][2]), LUAGLM_SCAST(x[1][3]),
          LUAGLM_SCAST(x[2][0]), LUAGLM_SCAST(x[2][1]), LUAGLM_SCAST(x[2][2]), LUAGLM_SCAST(x[2][3]),
          LUAGLM_SCAST(x[3][0]), LUAGLM_SCAST(x[3][1]), LUAGLM_SCAST(x[3][2]), LUAGLM_SCAST(x[3][3])
        );
      }
    };

    /// <summary>
    /// Return: the number of characters written if successful or negative value
    /// if an error occurred.
    /// </summary>
    template<class T>
    static GLM_FUNC_QUALIFIER int format_type(char *buff, size_t buff_len, const T &x) {
      return detail::lua_compute_to_string<T>::call(buff, buff_len, x);
    }
  }
}

#undef LUAGLM_FORMAT_LEN
#undef LUAGLM_LITERAL
#undef LUAGLM_SCAST

/* }================================================================== */

/*
** {==================================================================
** glm::hash implementation without std::hash dependency.
** ===================================================================
*/

/* Default seed for hash combinations */
#define LUAGLM_DEFAULT_SEED 0

namespace glm {
  namespace hash {
    /// <summary>
    /// Temporary solution as the previous implementation was slow. A variety of
    /// 'spatial hashing' algorithms exist for vector-type structures and should
    /// be considered.
    /// </summary>
    template<typename T>
    struct lua_hash {
      GLM_FUNC_DECL size_t operator()(const T &n) const;
    };

    template<>
    struct lua_hash<double> {
      GLM_FUNC_QUALIFIER size_t operator()(const double &n) const {
        union { double __t; size_t __a; } __scalar_hash;
        __scalar_hash.__a = 0;
        __scalar_hash.__t = n;
        return (n == 0.0) ? 0 : __scalar_hash.__a;  // -/+ 0.0 should return same hash.
      }
    };

    template<>
    struct lua_hash<float> {
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
    struct lua_hash<long double> {
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
    static GLM_INLINE void lua_combine(size_t &seed, size_t hash) {
      hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
      seed ^= hash;
    }

    template<typename T, glm::qualifier Q>
    GLM_FUNC_QUALIFIER size_t hash(glm::vec<1, T, Q> const &v) {
      const lua_hash<T> hasher;
      size_t seed = LUAGLM_DEFAULT_SEED;
      lua_combine(seed, hasher(v.x));
      return seed;
    }

    template<typename T, glm::qualifier Q>
    GLM_FUNC_QUALIFIER size_t hash(glm::vec<2, T, Q> const &v) {
      const lua_hash<T> hasher;
      size_t seed = LUAGLM_DEFAULT_SEED;
      lua_combine(seed, hasher(v.x));
      lua_combine(seed, hasher(v.y));
      return seed;
    }

    template<typename T, glm::qualifier Q>
    GLM_FUNC_QUALIFIER size_t hash(glm::vec<3, T, Q> const &v) {
      const lua_hash<T> hasher;
      size_t seed = LUAGLM_DEFAULT_SEED;
      lua_combine(seed, hasher(v.x));
      lua_combine(seed, hasher(v.y));
      lua_combine(seed, hasher(v.z));
      return seed;
    }

    template<typename T, glm::qualifier Q>
    GLM_FUNC_QUALIFIER size_t hash(glm::vec<4, T, Q> const &v) {
      const lua_hash<T> hasher;
      size_t seed = LUAGLM_DEFAULT_SEED;
      lua_combine(seed, hasher(v.x));
      lua_combine(seed, hasher(v.y));
      lua_combine(seed, hasher(v.z));
      lua_combine(seed, hasher(v.w));
      return seed;
    }

    template<typename T, glm::qualifier Q>
    GLM_FUNC_QUALIFIER size_t hash(glm::qua<T, Q> const &v) {
      const lua_hash<T> hasher;
      size_t seed = LUAGLM_DEFAULT_SEED;
      lua_combine(seed, hasher(v[3]));  // Avoid same sequence as glm::vec<4>
      lua_combine(seed, hasher(v[2]));
      lua_combine(seed, hasher(v[1]));
      lua_combine(seed, hasher(v[0]));
      return seed;
    }
  }
}

#undef LUAGLM_DEFAULT_SEED

/* }================================================================== */

#endif
