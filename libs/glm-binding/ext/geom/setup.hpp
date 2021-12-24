/*
** Geometric setup
**
** @TODO: GLM_EXTERNAL_TEMPLATE support
** @TODO: GLM_CONFIG_DEFAULTED_FUNCTIONS support
** @TODO: Each structure should be including their required glm headers/functions
**  rather than depending on the bulk-include below.
**
** See Copyright Notice at the end of this file
*/
#ifndef EXT_GEOM_SETUP_HPP
#define EXT_GEOM_SETUP_HPP

#include <vector>

#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/vector_query.hpp>
#include <glm/ext/scalar_relational.hpp>
#include <glm/ext/vector_relational.hpp>
#if GLM_CONFIG_ALIGNED_GENTYPES == GLM_ENABLE
  #include <glm/simd/platform.h>
#endif

#include "ext/vector_extensions.hpp"
#include "ext/quat_extensions.hpp"
#include "ext/matrix_extensions.hpp"

/* @COMPAT: GLM_CONFIG_DEFAULTED_DEFAULT_CTOR introduced in 0.9.9.9 */
#if !defined(GLM_CONFIG_DEFAULTED_DEFAULT_CTOR)
  #define GLM_CONFIG_DEFAULTED_DEFAULT_CTOR GLM_CONFIG_DEFAULTED_FUNCTIONS
#endif

/* Redefinition of GLM_FUNC_QUALIFIER for "geom/" */
#define GLM_GEOM_QUALIFIER static GLM_FUNC_QUALIFIER

/* @NOTE: Requires NEVER_INLINE fix in lglm.hpp. */
#define GLM_GEOM_QUALIFIER_NOINLINE static GLM_NEVER_INLINE

/*
** Basic exception-handling check. @TODO: Allow STL dependencies iff exception
** handling is enabled.
*/
#if defined(__EXCEPTIONS) || defined(__cpp_exceptions) || defined(_CPPUNWIND)
  #define GLM_GEOM_EXCEPTIONS 1
#else
  #define GLM_GEOM_EXCEPTIONS 0
#endif

/* @COMPAT: GLM_COMPILER_HIP introduced in 0.9.9.9 */
#if !defined(GLM_COMPILER_HIP)
  #define GLM_COMPILER_HIP 0
#endif

/* gtx/string_cast.hpp is available */
#if !((GLM_COMPILER & GLM_COMPILER_CUDA) || (GLM_COMPILER & GLM_COMPILER_HIP))
  //#define GLM_GEOM_TOSTRING 1
  //#include <glm/gtx/string_cast.hpp>
  #define GLM_GEOM_TOSTRING 0
#else
  #define GLM_GEOM_TOSTRING 0
#endif

/* Check runtime preconditions on geom structures (often related to ensuring normalized vectors) */
#if defined(LUAGLM_SAFELIB)
  #define GLM_GEOM_ASSUME(x, onError) \
    do {                              \
      if (!(x))                       \
        return (onError);             \
    } while (0)
#else
  #define GLM_GEOM_ASSUME(x, onError)
#endif

/* Forward declare all structures. */
namespace glm {
  template<length_t L, typename T, qualifier Q = defaultp> struct AABB;
  template<length_t L, typename T, qualifier Q = defaultp> struct Line;
  template<length_t L, typename T, qualifier Q = defaultp> struct Ray;
  template<length_t L, typename T, qualifier Q = defaultp> struct LineSegment;
  template<length_t L, typename T, qualifier Q = defaultp> struct Triangle;
  template<length_t L, typename T, qualifier Q = defaultp> struct Sphere;
  template<length_t L, typename T, qualifier Q = defaultp> struct Plane;
  template<length_t L, typename T, qualifier Q = defaultp> struct Polygon;
}

/******************************************************************************
* GLM Binding:
* See Copyright Notice in lua.h
\******************************************************************************
* MathGeoLib:
* Copyright Jukka Jylänki
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
******************************************************************************/

#endif
