**Automatically Generated**

Extensions to [GLM](https://github.com/g-truc/glm). Functions reside in the
base `glm` library table unless stated otherwise.

# Extended Functions

## Constructors

Functions declared in the global table (`_G`).

### vec

```lua
-- Generic vector constructor; infers dimensions based on the number of numeric
-- arguments.
--
-- Rules:
--   1. A primitive type (float, int, bool) will be stored at v[X];
--   2. A vector/quaternions (of N dimensions) will have its contents stored
--      at v[X], v[X + 1], ..., v[X + N] following x, y, z, w ordering;
--   3. Otherwise, a lua_error is thrown.
fvec = vec(...)

-- Applies integer-casting rules to the input values
ivec = ivec(...)

-- Applies bool-casting to the input values
bvec = bvec(...)

--[[
    vec#(value) - Create a vector of specific dimension with the value
      for each dimension, e.g., vec3(0).

    vec#(...) that ensures two, three, four numeric arguments respectively:
      - vec2(...) / ivec2(...) / bvec2(...)
      - vec3(...) / ivec3(...) / bvec3(...)
      - vec4(...) / ivec4(...) / bvec4(...)
--]]
```

#### Examples

```lua
> vec(math.pi, math.pi, math.pi)
vec3(3.141593, 3.141593, 3.141593)

> vec3(math.pi)
vec(3.141593, 3.141593, 3.141593)

-- Creates an integer vector (note: vectors are floating point internally)
> ivec3(1.1,2.2,3.8)
vec3(1.000000, 2.000000, 3.000000)
```

### mat

```lua
-- Generic matrix population/construction function. Iterate over the current Lua
-- stack and produce a matrix type according to the rules:
--   1. If the first and only object is a number or vector: populate the diagonal of a matrix.
--   2. If the first and only object is a quaternion: cast it to the arbitrarily
--      sized matrix. This logic follows glm::toMat3 and glm::toMat4 and uses
--      constructors to down/up-cast the matrix.
--   3. If the first object (second if recycling matrices) is a matrix: down/up-cast it.
--   4. Otherwise, expect a column vector for each dimension of the matrix.
matrix = mat(...)

-- Matrix constructor that recycles r_mat.
matrix = mat(r_mat, ...)

--[[
    Ensures dimensions based on the "NxM" suffix (each accepting an optional r_mat):
    mat2x2(...), mat2x3(...), mat2x4(...),
    mat3x2(...), mat3x3(...), mat3x4(...),
    mat4x2(...), mat4x3(...), mat4x4(...)

    Symmetric dimensions:
    mat2(...), mat3(...), mat4(...)
--]]
```

#### Examples

```lua
> mat3x3(vec3(glm.e, glm.e, glm.e), vec3(math.pi, math.pi, math.pi), vec3(1,1,1))
mat3x3((2.718282, 2.718282, 2.718282), (3.141593, 3.141593, 3.141593), (1.000000, 1.000000, 1.000000))

-- Infer matrix dimensions based on arguments
> mat(vec3(glm.e, glm.e, glm.e), vec3(math.pi, math.pi, math.pi), vec3(1,1,1))
mat3x3((2.718282, 2.718282, 2.718282), (3.141593, 3.141593, 3.141593), (1.000000, 1.000000, 1.000000))

> mat4x2(math.pi)
mat4x2((3.141593, 0.000000), (0.000000, 3.141593), (0.000000, 0.000000), (0.000000, 0.000000))

> mat3x3(quat(35.0, vec3(0,0,1)))
mat3x3((0.819152, 0.573576, 0.000000), (-0.573576, 0.819152, 0.000000), (0.000000, 0.000000, 1.000000))
```

### qua

```lua
-- Return the identity quaternion.
q = quat()

-- Generic quaternion constructor. Note, "qua" also aliased to quat.
q = quat(w --[[ number ]], x --[[ number ]], y --[[ number ]], z --[[ number ]])
q = quat(xyz --[[ vec3 ]], w --[[ number ]])

-- The shortest arc quaternion that rotates a source direction to coincide with
-- the target.
q = quat(source --[[ vec3 ]], target --[[ vec3 ]])

-- Build a quaternion from an angle (in degrees for grit-lua compatibility) and
-- a normalized axis.
q = quat(angle --[[ number ]], axis --[[ vec3 ]])

-- matrix to quaternion.
q = quat(m --[[ mat3x3 ]])
q = quat(m --[[ mat4x4 ]])
```

## grit-lua compatibility

Compatibility functions declared in the global table (`_G`):

### dot

```lua
-- Returns the dot product of x and y.
integer = dot(x --[[ integer ]], y --[[ integer ]])
number = dot(x --[[ number ]], y --[[ number ]])
number = dot(x --[[ vecN ]], y --[[ vecN ]])
number = dot(x --[[ quat ]], y --[[ quat ]])
```

### cross

```lua
-- Returns the cross product of x and y.
number = cross(x --[[ vec2 ]], y --[[ vec2 ]])
vec3 = cross(x --[[ vec3 ]], y --[[ vec3 ]])
vec3 = cross(x --[[ vec3 ]], y --[[ quat ]])
vec3 = cross(x --[[ quat ]], y --[[ vec3 ]])
quat = cross(x --[[ quat ]], y --[[ quat ]])
```

### inverse

```lua
-- Returns the quaternion inverse.
quat = inverse(q --[[ quat ]])

-- Return the inverse of a squared matrix.
mat2x2 = inverse(m --[[ mat2x2 ]])
mat3x3 = inverse(m --[[ mat3x3 ]])
mat4x4 = inverse(m --[[ mat4x4 ]])
```

### norm

```lua
-- Returns a vector in the same direction as x but with length of 1.
vecN = norm(v --[[ vecN ]])
quat = norm(q --[[ quat ]])
```

### slerp

```lua
-- Returns spherical interpolation between two vectors.
vecN = slerp(x --[[ vecN ]], y --[[ vecN ]], t --[[ number ]])
quat = slerp(x --[[ quat ]], y --[[ quat ]], t --[[ number ]])
```

## Constants

See [glm/gtc/constants.hpp](http://glm.g-truc.net/0.9.9/api/a00708.html):

- `feps`: vector-float epsilon;
- `huge`: lmathlib compatibility;
- `maxinteger`: lmathlib compatibility;
- `mininteger`: lmathlib compatibility;
- `FP_NORMAL, FP_SUBNORMAL, FP_ZERO, FP_INFINITE, FP_NAN`: [Floating Point Categories](https://en.cppreference.com/w/cpp/numeric/math/FP_categories);

## Base

### hash

```lua
-- std::hash wrapper
integer = hash(v --[[ vecN ]])
integer = hash(q --[[ quat ]])
integer = hash(m --[[ matNxM ]])
```

### unpack

```lua
-- Unpack the components of the provided type.
--
-- Vector: Push the available x, y, z, w fields.
-- Matrix: Push each column (or row) vector.
... = unpack(v --[[ vecN ]])
... = unpack(q --[[ quat ]])
... = unpack(m --[[ matNxM ]])
```

### to\_string

```lua
-- glm::to_string wrapper
string,...,string = to_string(...)
```

### all\_equal

```lua
-- glm.all(glm.equal(...)) shorthand

bool = all_equal(v1 --[[ vec ]], v2 --[[ vec ]])
bool = all_equal(v1 --[[ vec ]], v2 --[[ vec ]], eps --[[ number ]])
bool = all_equal(v1 --[[ vec ]], v2 --[[ vec ]], eps --[[ vec ]])
bool = all_equal(v1 --[[ vec ]], v2 --[[ vec ]], ULPs --[[ integer ]])

bool = all_equal(q1 --[[ quat ]], q2 --[[ quat ]])
bool = all_equal(q1 --[[ quat ]], q2 --[[ quat ]], eps --[[ number ]])
bool = all_equal(q1 --[[ quat ]], q2 --[[ quat ]], eps --[[ vec4 ]])
bool = all_equal(q1 --[[ quat ]], q2 --[[ quat ]], ULPs --[[ integer ]])

bool = all_equal(m1 --[[ matrix ]], m2 --[[ matrix ]])
bool = all_equal(m1 --[[ matrix ]], m2 --[[ matrix ]], eps --[[ number ]])
bool = all_equal(m1 --[[ matrix ]], m2 --[[ matrix ]], eps --[[ vec4 ]])
bool = all_equal(m1 --[[ matrix ]], m2 --[[ matrix ]], ULPs --[[ integer ]])
```

### any\_notequal

```lua
-- glm.any(glm.notEqual(...)) shorthand

bool = any_notequal(v1 --[[ vec ]], v2 --[[ vec ]])
bool = any_notequal(v1 --[[ vec ]], v2 --[[ vec ]], eps --[[ number ]])
bool = any_notequal(v1 --[[ vec ]], v2 --[[ vec ]], eps --[[ vec ]])
bool = any_notequal(v1 --[[ vec ]], v2 --[[ vec ]], ULPs --[[ integer ]])

bool = any_notequal(q1 --[[ quat ]], q2 --[[ quat ]])
bool = any_notequal(q1 --[[ quat ]], q2 --[[ quat ]], eps --[[ number ]])
bool = any_notequal(q1 --[[ quat ]], q2 --[[ quat ]], eps --[[ vec4 ]])
bool = any_notequal(q1 --[[ quat ]], q2 --[[ quat ]], ULPs --[[ integer ]])

bool = any_notequal(m1 --[[ matrix ]], m2 --[[ matrix ]])
bool = any_notequal(m1 --[[ matrix ]], m2 --[[ matrix ]], eps --[[ number ]])
bool = any_notequal(m1 --[[ matrix ]], m2 --[[ matrix ]], eps --[[ vec4 ]])
bool = any_notequal(m1 --[[ matrix ]], m2 --[[ matrix ]], ULPs --[[ integer ]])
```

### right

```lua
-- A unit vector designating right
vec3 = right()
```

### up

```lua
-- A unit vector designating up, see GLM_FORCE_Z_UP
vec3 = up()
```

### forward

```lua
-- A unit vector designating forward, see GLM_FORCE_Z_UP and GLM_FORCE_LEFT_HANDED
vec3 = forward()

-- A unit vector designating forward for left-handed coordinate systems,
-- however, still dependent on the state of GLM_FORCE_Z_UP
vec3 = forwardLH()

-- A unit vector designating forward for right-handed coordinate systems
vec3 = forwardRH()
```

## glm/common.hpp

### signP

```lua
-- Returns the non-negative sign (1.0 if >= 0) for each component
s --[[ number ]] = signP(x --[[ number ]])
s --[[ vecN ]] = signP(x --[[ vecN ]])
```

### signN

```lua
-- Returns the non-positive sign (1.0 if > 0) for each component
s --[[ number ]] = signN(x --[[ number ]])
s --[[ vecN ]] = signN(x --[[ vecN ]])
```

### fdim

```lua
-- Returns the positive difference between x and y (C99/C++11)
vecN = fdim(x --[[ vecN ]], y --[[ vecN ]])
```

### hypot

```lua
-- Returns the hypotenuse of a right-angled triangle whose legs are x and y
-- (C99/C++11)
vecN = hypot(x --[[ vecN ]], y --[[ vecN ]])
```

### isnormal

```lua
-- Returns whether x is a normal value: i.e., whether it is neither
-- infinity, NaN, zero, or subnormal (C99/C++11)
bvecN = isnormal(x --[[ vecN ]])
```

### isunordered

```lua
-- Returns whether x or y are unordered values (C99/C++11)
bvecN = isunordered(x --[[ vecN ]], y --[[ vecN ]])
```

### nearbyint

```lua
-- Rounds x to an integral value (C99/C++11)
vecN = nearbyint(x --[[ vecN ]])
```

### nextafter

```lua
-- Returns the next representable value after x in the direction of y
-- (C99/C++11)
vecN = nextafter(x --[[ vecN ]], y --[[ vecN ]])
```

### remainder

```lua
-- Returns the floating-point remainder of numer/denom (C99/C++11)
vecN = remainder(numer --[[ vecN ]], denom --[[ vecN ]])
```

### scalbn

```lua
-- Scales x by FLT_RADIX raised to the power of n (C99/C++11)
vecN = scalbn(x --[[ vecN ]], n --[[ ivecN ]])
```

### copysign

```lua
-- Returns a value with the magnitude of x and the sign of y (C99/C++11)
vecN = copysign(x --[[ vecN ]], y --[[ vecN ]])
```

### fpclassify

```lua
-- Returns a value of type int that matches one of the classification macro
-- constants, depending on the value of x (C99/C++11)
ivecN = fpclassify(x --[[ vecN ]])
```

### Aliases

- `fabs = abs`
- `tointeger = toint`
- `signbit = sign`

## glm/exponential.hpp

### logistic

```lua
-- Returns exp(v) / (exp(v) + 1)
vecN = logistic(v --[[ vecN ]])
```

### expm1

```lua
-- Returns e raised to the power x minus one: ex-1 (C99/C++11)
vecN = expm1(v --[[ vecN ]])
```

### cbrt

```lua
-- Returns the cubic root of x (C99/C++11)
vecN = cbrt(v --[[ vecN ]])
```

### log10

```lua
-- Returns the common (base-10) logarithm of x (C99/C++11)
vecN = log10(v --[[ vecN ]])
```

### log1p

```lua
-- Returns the natural logarithm of one plus x (C99/C++11)
vecN = log1p(v --[[ vecN ]])
```

### logb

```lua
-- Returns the logarithm of |x|, using FLT_RADIX as base for the logarithm
-- (C99/C++11)
vecN = logb(v --[[ vecN ]])
```

### ilogb

```lua
-- Returns the integral part of the logarithm of |x|, using FLT_RADIX as base
-- for the logarithm (C99/C++11)
ivecN = ilogb(v --[[ vecN ]])
```

## glm/trigonometric.hpp

### sincos

```lua
-- Calculate sin and cos simultaneously
n_sin --[[ number ]],n_cos --[[ number ]] = sincos(n --[[ number ]])
v_sin --[[ vecN ]],v_cos --[[ vecN ]] = sincos(v --[[ vecN ]])
```

### fromAngle

```lua
-- CreateFromAngle: calculate sin and cos simultaneously
v_sincos --[[ vec2 ]] = fromAngle(n --[[ number ]])
```

### Aliases

- `deg = degrees`
- `rad = radians`

## glm/matrix.hpp

### invertible

```lua
-- Return true if the matrix has an inverse (up to a given epsilon)
bool = invertible(m --[[ matNxM ]])
```

## glm/vector\_relational.hpp

### ult

```lua
-- Unsigned x < y (lmathlib)
bvecN = ult(x --[[ vecN ]], y --[[ vecN ]])
```

### ulte

```lua
-- Unsigned x <= y (lmathlib)
bvecN = ulte(x --[[ vecN ]], y --[[ vecN ]])
```

## glm/geometric.hpp

### clampLength

```lua
-- Return a copy of the vector "v" with its length clamped to "maxLength"
vecN = clampLength(v --[[ vecN ]], maxLength --[[ number ]])
```

### scaleLength

```lua
-- Scale the length of vector "v" to a newLength
vecN = scaleLength(v --[[ vecN ]], newLength --[[ number ]])
```

### homogenize

```lua
-- Homogenizes the vector, i.e., divides all components by w.
vec4 = homogenize(v --[[ vec4 ]],
```

### dot3

```lua
-- Return the product of two vectors using only the x,y,z components.
number = dot3(x --[[ vec4 ]], y --[[ vec4 ]])
```

### Aliases

- `norm = normalize`
- `magnitude = length`
- `clampMagnitude = clampLength`
- `scaleMagnitude = scaleLength`

## glm/functions.hpp

### moveTowards

```lua
-- Return a position between two points, moving no further from "from" than a
-- specified distance
vecN = moveTowards(
    from --[[ vecN ]],
    to --[[ vecN ]],
    maxDistance --[[ number ]]
)
```

### rotateTowards

```lua
-- Return a direction between two vectors; rotating no further from "from" than
-- a specified angle
vecN = rotateTowards(
    from --[[ vecN ]],
    to --[[ vecN ]],
    maxRadians --[[ number ]],
    maxLength --[[ number ]]
)
```

### smoothDamp

```lua
-- Gradually changes a vector position to reach the target position over time.
-- This calculation is derived from a smoothing time (an approximate time to
-- reach the target position) and an optional maximum speed
newPos --[[ vecN ]], newVelocity --[[ vecN ]] = smoothDamp(
    currentPos --[[ vecN ]],
    targetPos --[[ vecN ]],
    currentVelocity --[[ vecN ]],
    smooth_time --[[ number ]],
    maxSpeed --[[ number ]],
    deltaTime --[[ number ]]
)
```

### erf

```lua
-- Returns the error function value (C99/C++11)
vecN = erf(v --[[ vecN ]])
```

### erfc

```lua
-- Returns the complementary error function value (C99/C++11)
vecN = erfc(v --[[ vecN ]])
```

### lgamma

```lua
-- Returns the natural logarithm of the absolute value of the gamma function
-- (C99/C++11)
vecN = lgamma(v --[[ vecN ]])
```

### tgamma

```lua
-- Returns the gamma function (C99/C++11)
vecN = tgamma(v --[[ vecN ]])
```

## glm/ext/matrix\_projection.hpp

### rayPicking

```lua
-- Note, mouse coordinates are on a [-1, 1] scale
vec3 = rayPicking(
    cam_direction --[[ vec3 ]],
    cam_up --[[ vec3 ]],
    fov --[[ number ]],
    aspectRatio --[[ number ]],
    nearDof --[[ number ]],
    farDof --[[ number ]],
    mouseX --[[ number ]],
    mouseY --[[ number ]]
)
```

### containsProjection

```lua
-- Returns true if the matrix contains a projection, i.e., the last row differs
-- (up to an epsilon) of [0, 0, 0, 1]
bool = containsProjection(m --[[ mat4x4 ]], epsilon --[[ number ]])
```

## glm/ext/matrix\_transform.hpp

### transformPos

```lua
-- Transforms the given point by the the matrix, i.e.,
-- M * (p.x, p.y, p.z, 1). Note that this function cannot have a projection
vec3 = transformPos(m --[[ matrix ]], p --[[ vec3 ]])
```

### transformPosPerspective

```lua
-- Transforms the given point by the matrix4x4 (with a perspective divide)
vec3 = transformPosPerspective(m --[[ mat4x4 ]], p --[[ vec3 ]])
```

### transformDir

```lua
-- Transforms the given direction by the matrix, i.e.,
-- M * (dir.x, dir.y, dir.z, 0)
vec3 = transformDir(m --[[ matrix ]], dir --[[ vec3 ]])
```

### lookRotationRH

```lua
-- Create a right-handed rotation matrix for a given forward and up-vector
mat3x3 = lookRotationRH(fwd --[[ vec3 ]], up --[[ vec3 ]])
```

### lookRotationLH

```lua
--  Create a left-handed rotation matrix for a given forward and up-vector
mat3x3 = lookRotationLH(fwd --[[ vec3 ]], up --[[ vec3 ]])
```

### lookRotation

```lua
-- Create a rotation matrix for a given forward and up-vector, see
-- GLM_FORCE_LEFT_HANDED
mat3x3 = lookRotation(fwd --[[ vec3 ]], up --[[ vec3 ]])
```

### billboardRH

```lua
-- Creates a right-handed spherical billboard that rotates around a specified
-- object position
mat3x3 = billboardRH(
    objectPos --[[ vec3 ]],
    camPos --[[ vec3 ]],
    camUp --[[ vec3 ]],
    camFwd --[[ vec3 ]]
)
```

### billboardLH

```lua
-- Creates a left-handed spherical billboard that rotates around a specified
-- object position
mat3x3 = billboardLH(
    objectPos --[[ vec3 ]],
    camPos --[[ vec3 ]],
    camUp --[[ vec3 ]],
    camFwd --[[ vec3 ]]
)
```

### billboard

```lua
-- Creates a spherical billboard that rotates around a specified object
-- position, see GLM_FORCE_LEFT_HANDED
mat3x3 = billboard(
    objectPos --[[ vec3 ]],
    camPos --[[ vec3 ]],
    camUp --[[ vec3 ]],
    camFwd --[[ vec3 ]]
)
```

## glm/ext/quaternion\_common.hpp

### invertible

```lua
-- Return true if the quaternion is invertible (i.e., is non-zero and finite)
bool = invertible(q --[[ quat ]])
```

### barycentric

```lua
-- Create a quaternion in barycentric coordinates
quat = barycentric(
    a --[[ quat ]],
    b --[[ quat ]],
    c --[[ quat ]],
    u --[[ number ]],
    v --[[ number ]]
)
```

## glm/ext/quaternion\_extensions.hpp

### angle

```lua
-- Returns the angle, in radians, between two quaternions.
rads = angle(x --[[ quat ]], y --[[ quat ]])

-- Returns the smallest absolute angle, in radians, between two quaternions.
-- Note, this function is implemented with atan2
rads = angle2(x --[[ quat ]], y --[[ quat ]])
```

## glm/ext/scalar\_common.hpp

### wrap

```lua
-- Wraps 'v' to [0, maxValue].
vecN = loopRepeat(v --[[ vecN ]], maxValue --[[ vecN ]])
```

### deltaAngle

```lua
-- Return the shortest difference between two angles (radians)
angle = deltaAngle(a --[[ number ]], b --[[ number ]])
```

### loopRepeat

```lua
-- Loops "t" so that it is never greater than "length" and less than zero
vecN = loopRepeat(t --[[ vecN ]], length --[[ number ]])
vecN = loopRepeat(t --[[ vecN ]], length --[[ vecN ]])
```

### pingPong

```lua
-- Return a value that will increment and decrement between the value 0 and
-- length
vecN = pingPong(v --[[ vecN ]], length --[[ vecN ]])
```

### lerpAngle

```lua
-- A lerp implementation that ensures values interpolate correctly when they
-- wrap around two-pi (radians)
vecN = lerpAngle(from --[[ vecN ]], to --[[ vecN ]], t --[[ number ]])
vecN = lerpAngle(from --[[ vecN ]], to --[[ vecN ]], t --[[ vecN ]])
```

### slerp

```lua
-- Returns a spherical interpolation between two vectors
vecN = slerp(from --[[ vecN ]], to --[[ vecN ]], t --[[ number ]])
vecN = slerp(from --[[ number ]], to --[[ number ]], t --[[ number ]])

quat = slerp(from --[[ quat ]], to --[[ quat ]], t --[[ number ]])
```

## glm/ext/transform.hpp

### trs

```lua
-- Creates a translation, rotation and scaling matrix
mat4x4 = trs(
    translation --[[ vec3 ]],
    rotation --[[ quat ]],
    scale --[[ vec3 ]]
)
```

### inverse\_world\_tensor

```lua
-- Return the inertia tensor in global coordinates
mat3x3 = inverse_world_tensor(inverseTensor --[[ vec3 --]], localToWorld --[[ mat3x3 --]])
```

## glm/gtc/epsilon.hpp

### Aliases

- `approximately = epsilonEqual`

## glm/gtc/quaternion.hpp

### quatbillboardRH

```lua
-- See billboardRH
quat = quatbillboardRH(
    objectPos --[[ vec3 ]],
    camPos --[[ vec3 ]],
    camUp --[[ vec3 ]],
    camFwd --[[ vec3 ]]
)
```

### quatbillboardLH

```lua
-- See billboardLH
quat = quatbillboardLH(
    objectPos --[[ vec3 ]],
    camPos --[[ vec3 ]],
    camUp --[[ vec3 ]],
    camFwd --[[ vec3 ]]
)
```

### quatbillboard

```lua
-- See billboard
quat = quatbillboard(
    objectPos --[[ vec3 ]],
    camPos --[[ vec3 ]],
    camUp --[[ vec3 ]],
    camFwd --[[ vec3 ]]
)
```

### Aliases

- `quatlookRotation = glm_quatLookAtLH`
- `quatlookRotationRH = glm_quatLookAtLH`
- `quatlookRotationLH = glm_quatLookAtLH`

## glm/gtx/component\_wise.hpp

Specializations for compScale and compNormalize;

### compScale

```lua
-- Convert a normalized float vector to an integer vector.
vecN = compScale_i8(v --[[ vecN ]])
vecN = compScale_u8(v --[[ vecN ]])
vecN = compScale_i16(v --[[ vecN ]])
vecN = compScale_u16(v --[[ vecN ]])
```

### compNormalize

```lua
-- Convert an integer vector to a normalized float vector.
vecN = compNormalize_i8(v --[[ vecN ]])
vecN = compNormalize_u8(v --[[ vecN ]])
vecN = compNormalize_i16(v --[[ vecN ]])
vecN = compNormalize_u16(v --[[ vecN ]])
```

## glm/gtx/fast\_trigonometry.hpp

```lua
-- Wrap an angle to [-pi, pi]
vecN = wrapAngle2(v --[[ vecN ]])
```

## glm/gtx/matrix\_query.hpp

### extractScale

```lua
-- Return the scaling components of the matrix
vecN = extractScale(m --[[ matNxM ]])
```

### hasUniformScale

```lua
-- Returns true if the matrix contains only uniform scaling (up to a given eps)
bool = hasUniformScale(m --[[ matNxM ]], epsilon --[[ number ]])
```

## glm/gtx/matrix\_storage.hpp

### colMajor

```lua
-- Generalized glm::colMajor implementation.
m --[[ matNxM --] = colMajor(mat --[[ matNxM --]])
m --[[ mat2x2 --] = colMajor(v1 --[[ vec2 ]], v2 --[[ vec2 ]])
m --[[ mat3x3 --] = colMajor(v1 --[[ vec3 ]], v2 --[[ vec3 ]], v3 --[[ vec3 ]])
m --[[ mat4x4 --] = colMajor(v1 --[[ vec4 ]], v2 --[[ vec4 ]], v3 --[[ vec4 ]], v4 --[[ vec4 ]])
```

### rowMajor

```lua
-- Generalized glm::rowMajor implementation.
m --[[ matNxM --] = rowMajor(mat --[[ matNxM --]])
m --[[ mat2x2 --] = rowMajor(v1 --[[ vec2 ]], v2 --[[ vec2 ]])
m --[[ mat3x3 --] = rowMajor(v1 --[[ vec3 ]], v2 --[[ vec3 ]], v3 --[[ vec3 ]])
m --[[ mat4x4 --] = rowMajor(v1 --[[ vec4 ]], v2 --[[ vec4 ]], v3 --[[ vec4 ]], v4 --[[ vec4 ]])
```

## glm/gtx/norm.hpp

### Aliases

- `sqrMagnitude = length2`
- `lengthSquared = length2`
- `distanceSquared = distance2`

## glm/gtx/orthonormalize\_hpp

### orthonormalize3

```lua
-- Make the vectors normalized and orthogonal to one another
normal,tangent = orthonormalize3(normal --[[ vec3 ]], tangent --[[ vec3 ]])
normal,tangent,binormal = orthonormalize3(normal --[[ vec3 ]], tangent --[[ vec3 ]], binormal --[[ vec3 ]])
```

## glm/gtx/projection.hpp

### projNorm

```lua
-- Project this vector 'v' onto a normalized direction vector
vproj --[[ vecN ]] = projNorm(v --[[ vecN ]], normal --[[ vecN ]])
```

### projPlane

```lua
-- Project a vector onto this plane defined by its orthogonal normal
vproj --[[ vecN ]] = projPlane(v --[[ vecN ]], normal --[[ vecN ]])
```

### projDecompose

```lua
-- Decompose the vector into parallel and perpendicular components with respect
-- to a given direction
para --[[ vecN ]], perp --[[ vecN ]] = projDecompose(v --[[ vecN ]], dir --[[ vecN ]])
```

## glm/gtx/perpendicular.hpp

### isPerpendicular

```lua
-- Return true if two vectors are perpendicular to one other
bool = isPerpendicular(v1 --[[ vecN ]], v2 --[[ vecN ]], epsilon --[[ number ]])
```

### perpendicular

```lua
-- Return a normalized direction vector that is perpendicular to "v" and the
-- specified hint vectors. If "v" points towards first hint vector, then the
-- second is used as a fallback
vec3 = perpendicular(v --[[ vec3 ]], hint --[[ vec3 ]], hint2 --[[ vec3 ]])
vec3 = perpendicular(v --[[ vec3 ]])
```

### perpendicular2

```lua
-- Return a normalized direction vector that is perpendicular to "v" and the
-- vector returned by glm::perpendicular
vec3 = perpendicular2(v --[[ vec3 ]], hint --[[ vec3 ]], hint2 --[[ vec3 ]])
vec3 = perpendicular2(v --[[ vec3 ]])
```

### perpendicularBasis

```lua
-- Returns two vectors that are orthogonal the vector and to each other
vec3,vec3 = perpendicularBasis(v --[[ vec3 ]])
```

### perpendicularFast

```lua
-- (Fast) Compute an orthonormal of the vector, i.e., compute a vector that is
-- mutually perpendicular to this axis
vec3 = perpendicularFast(v --[[ vec3 ]])
```

### Aliases

- `basis = perpendicularBasis`

## glm/gtx/quaternion\_transform.hpp

### rotateFromTo

```lua
-- Create a shortest arc quaternion that rotates a source direction to coincide
-- with the target
quat = rotateFromTo(source --[[ vec3 ]], target --[[ vec3 ]])
```

### rotateTowards

```lua
-- Return a rotation between two quaternions; rotating no further than
-- maxRadians
quat = rotateTowards(
    source --[[ quat ]],
    target --[[ quat ]],
    maxRadians --[[ number ]]
)
```

### Aliases

- `transformDir = operator*(quat, vec3)`

## glm/gtx/rotate\_vector.hpp

### barycentric

```lua
-- Return a vector containing the Cartesian coordinates of a point specified
-- in barycentric coordinates (i.e., relative to a N-dimensional triangle)
vecN = barycentric(
    a --[[ vecN ]],
    b --[[ vecN ]],
    c --[[ vecN ]],
    u --[[ number ]],
    v --[[ number ]]
)
```

## glm/gtx/vector\_angle.hpp

###

```lua
-- Returns the smallest absolute angle, in radians, between two vectors.
-- Note, this function is implemented with atan22
rads = angle2(x --[[ vecN ]], y --[[ vecN ]])
```

### Aliases

- `signedAngle = orientedAngle`

## glm/gtx/vector\_query.hpp

### isUniform

```lua
-- Check whether all components are nearly equal.
bool = isUniform(v --[[ vecN ]], epsilon --[[ number ]])
```

### Aliases

- `isZero = isNull`

## glm/gtx/euler\_angles.hpp

### quatEulerAngleX

```lua
-- A quaternion from an euler angle X
quat = quatEulerAngleX(X --[[ number ]])
```

### quatEulerAngleY

```lua
-- A quaternion from an euler angle Y
quat = quatEulerAngleY(Y --[[ number ]])
```

### quatEulerAngleZ

```lua
-- A quaternion from an euler angle Z
quat = quatEulerAngleZ(Z --[[ number ]])
```

### quatEulerAngleXY

```lua
-- A quaternion from euler angles (X*Y)
quat = quatEulerAngleXY(X --[[ number ]], Y --[[ number ]])
```

### quatEulerAngleXZ

```lua
-- A quaternion from euler angles (X*Z)
quat = quatEulerAngleXZ(X --[[ number ]], Z --[[ number ]])
```

### quatEulerAngleYX

```lua
-- A quaternion from euler angles (Y*X)
quat = quatEulerAngleYX(Y --[[ number ]], X --[[ number ]])
```

### quatEulerAngleYZ

```lua
-- A quaternion from euler angles (Y*Z)
quat = quatEulerAngleYZ(Y --[[ number ]], Z --[[ number ]])
```

### quatEulerAngleZX

```lua
-- A quaternion from euler angles (Z*X)
quat = quatEulerAngleZX(Z --[[ number ]], X --[[ number ]])
```

### quatEulerAngleZY

```lua
-- A quaternion from euler angles (Z*Y)
quat = quatEulerAngleZY(Z --[[ number ]], Y --[[ number ]])
```

### quatEulerAngleXYX

```lua
-- A quaternion from euler angles (X*Y*X)
quat = quatEulerAngleXYX(X --[[ number ]], Y --[[ number ]], X --[[ number ]])
```

### quatEulerAngleXYZ

```lua
-- A quaternion from euler angles (X*Y*Z)
quat = quatEulerAngleXYZ(X --[[ number ]], Y --[[ number ]], Z --[[ number ]])
```

### quatEulerAngleXZX

```lua
-- A quaternion from euler angles (X*Z*X)
quat = quatEulerAngleXZX(X --[[ number ]], Z --[[ number ]], X --[[ number ]])
```

### quatEulerAngleXZY

```lua
-- A quaternion from euler angles (X*Z*Y)
quat = quatEulerAngleXZY(X --[[ number ]], Z --[[ number ]], Y --[[ number ]])
```

### quatEulerAngleYXY

```lua
-- A quaternion from euler angles (Y*X*Y)
quat = quatEulerAngleYXY(Y --[[ number ]], X --[[ number ]], Y --[[ number ]])
```

### quatEulerAngleYXZ

```lua
-- A quaternion from euler angles (Y*X*Z)
quat = quatEulerAngleYXZ(Y --[[ number ]], X --[[ number ]], Z --[[ number ]])
```

### quatEulerAngleYZX

```lua
-- A quaternion from euler angles (Y*Z*X)
quat = quatEulerAngleYZX(Y --[[ number ]], Z --[[ number ]], X --[[ number ]])
```

### quatEulerAngleYZY

```lua
-- A quaternion from euler angles (Y*Z*Y)
quat = quatEulerAngleYZY(Y --[[ number ]], Z --[[ number ]], Y --[[ number ]])
```

### quatEulerAngleZXY

```lua
-- A quaternion from euler angles (Z*X*Y)
quat = quatEulerAngleZXY(Z --[[ number ]], X --[[ number ]], Y --[[ number ]])
```

### quatEulerAngleZXZ

```lua
-- A quaternion from euler angles (Z*X*Z)
quat = quatEulerAngleZXZ(Z --[[ number ]], X --[[ number ]], Z --[[ number ]])
```

### quatEulerAngleZYX

```lua
-- A quaternion from euler angles (Z*Y*X)
quat = quatEulerAngleZYX(Z --[[ number ]], Y --[[ number ]], X --[[ number ]])
```

### quatEulerAngleZYZ

```lua
-- A quaternion from euler angles (Z*Y*Z)
quat = quatEulerAngleZYZ(Z --[[ number ]], Y --[[ number ]], Z --[[ number ]])
```

### Aliases

- `eulerX = eulerAngleX`
- `eulerY = eulerAngleY`
- `eulerZ = eulerAngleZ`
- `eulerXZ = eulerAngleXZ`
- `eulerXY = eulerAngleXY`
- `eulerYX = eulerAngleYX`
- `eulerYZ = eulerAngleYZ`
- `eulerZX = eulerAngleZX`
- `eulerZY = eulerAngleZY`
- `eulerXYX = eulerAngleXYX`
- `eulerXYZ = eulerAngleXYZ`
- `eulerXZX = eulerAngleXZX`
- `eulerXZY = eulerAngleXZY`
- `eulerYXY = eulerAngleYXY`
- `eulerYXZ = eulerAngleYXZ`
- `eulerYZX = eulerAngleYZX`
- `eulerYZY = eulerAngleYZY`
- `eulerZXY = eulerAngleZXY`
- `eulerZXZ = eulerAngleZXZ`
- `eulerZYX = eulerAngleZYX`
- `eulerZYZ = eulerAngleZYZ`

# Random Number Generation

Bindings to [Pseudo-random number generation](https://en.cppreference.com/w/cpp/numeric/random)
where [UniformRandomBitGenerator](https://en.cppreference.com/w/cpp/named_req/UniformRandomBitGenerator)
is implemented using `math.random` (cached as a function upval on library loading).

Accessed by `glm.distribution[name]`.

## uniform\_int

```lua
-- a: distribution minimum value (integer); a >= 0
-- b: distribution maximum value (integer)
value = uniform_int([a, b])
```

## uniform\_real

```lua
-- a: distribution minimum value (number); a >= 0
-- b: distribution maximum value (number)
value = uniform_real([a, b])
```

## bernoulli

```lua
-- p: probability of generating true (number); 0.0 <= p && p <= 1.0
value = bernoulli([p])
```

## binomial

```lua
-- t: number of trials (integer); t > 0
-- p: probability of a trial generating true (number); 0.0 <= p && p <= 1.0
value = binomial([t, p])
```

## negative\_binomial

```lua
-- k: number of trial successes (integer); k > 0
-- p: probability of a trial generating true (number); 0.0 < p && p <= 1.0
value = negative_binomial([k, p])
```

## geometric

```lua
-- p: probability of a trial generating true (number); 0.0 < p && p < 1.0
value = geometric([p])
```

## poisson

```lua
-- μ: the mean of the distribution (number); n > 0
value = poisson([μ])
```

## exponential

```lua
-- λ: the rate parameter (number); n > 0
value = exponential([λ])
```

## gamma

```lua
-- α: shape (number); n > 0
-- β: scale (number); n > 0
value = gamma([α, β])
```

## weibull

```lua
-- a: shape (number); n > 0
-- b: scale (number); n > 0
value = weibull([a, b])
```

## extreme\_value

```lua
-- a: location (number)
-- b: scale (number); n > 0
value = extreme_value([a, b])
```

## normal

```lua
-- μ: mean (number)
-- σ: standard deviation (number); n > 0
value = normal([μ, σ])
```

## lognormal

```lua
-- m: log-scale (number)
-- s: shape (number); n > 0
value = lognormal([m, s])
```

## chi\_squared

```lua
-- n: degrees of freedom (number); n > 0
value = chi_squared([n])
```

## cauchy

```lua
-- a: location (number)
-- b: scale (number); b > 0
value = cauchy([a, b])
```

## fisher\_f

```lua
-- m: degrees of freedom (number); m > 0
-- n: degrees of freedom (number); n > 0
value = fisher_f([m, n])
```

## student\_t

```lua
-- n: degrees of freedom (number); n > 0
value = student_t([n])
```

# Geometry API

## AABB

An Axis-Aligned Bounding Box. All operators (non-constructors) are of the form:

```lua
-- aabbMin --[[ vec3 ]]: The minimum extent of the AABB in world space,
-- aabbMax --[[ vec3 ]]: The maximum extent of the AABB in world space,
-- ...: variadic function parameters
result = aabb.function(aabbMin, aabbMax, ...)

-- aabbMin --[[ vec2 ]]: The minimum extent of the AABB in world space,
-- aabbMax --[[ vec2 ]]: The maximum extent of the AABB in world space,
-- ...: variadic function parameters
result = aabb2d.function(aabbMin, aabbMax, ...)
```

### aabb.new

```lua
-- Create a new AABB that encloses all coordinates on the Lua stack (or within a
-- table if it is the first argument)
aabbMin,aabbMax = aabb.new(...)
aabbMin,aabbMax = aabb2d.new(...)
```

### aabb.fromCenterAndSize

```lua
-- Create an AABB by specifying its center and size (uniform on each dimension)
aabbMin,aabbMax = aabb.fromCenterAndSize(center --[[ vec3 ]], size --[[ number ]])
```

### aabb.aabbFromSphere

```lua
-- Create the smallest possible AABB, in terms of volume, that contains the
-- provided sphere
aabbMin,aabbMax = aabb.aabbFromSphere(spherePos --[[ vec3 ]], sphereRad --[[ number ]])
aabbMin,aabbMax = aabb2d.fromCenterAndSize(center --[[ vec2 ]], size --[[ number ]])
```

### aabb.operator\_negate

```lua
aabbMin,aabbMax = aabb.operator_negate(...)
aabbMin,aabbMax = aabb2d.operator_negate(...)
```

### aabb.operator\_add

```lua
aabbMin,aabbMax = aabb.operator_add(..., point --[[ vec3 ]])
aabbMin,aabbMax = aabb2d.operator_add(..., point --[[ vec2 ]])
```

### aabb.operator\_sub

```lua
aabbMin,aabbMax = aabb.operator_sub(..., point --[[ vec3 ]])
aabbMin,aabbMax = aabb2d.operator_sub(..., point --[[ vec2 ]])
```

### aabb.operator\_mul

```lua
aabbMin,aabbMax = aabb.operator_mul(matrix3x3, ...)
aabbMin,aabbMax = aabb.operator_mul(matrix4x3, ...)
aabbMin,aabbMax = aabb.operator_mul(matrix4x4, ...)
aabbMin,aabbMax = aabb.operator_mul(quat, ...)

aabbMin,aabbMax = aabb2d.operator_mul(matrix3x3, ...)
aabbMin,aabbMax = aabb2d.operator_mul(matrix4x3, ...)
aabbMin,aabbMax = aabb2d.operator_mul(matrix4x4, ...)
aabbMin,aabbMax = aabb2d.operator_mul(quat, ...)
```

### aabb.equal

```lua
bool = aabb.equal(..., otherMin --[[ vec3 ]], otherMax --[[ vec3 ]])
bool = aabb.equal(..., otherMin --[[ vec3 ]], otherMax --[[ vec3 ]], eps --[[ number ]])
bool = aabb.equal(..., otherMin --[[ vec3 ]], otherMax --[[ vec3 ]], eps --[[ vec3 ]])
bool = aabb.equal(..., otherMin --[[ vec3 ]], otherMax --[[ vec3 ]], ULPs --[[ integer ]])

bool = aabb2d.equal(..., otherMin --[[ vec2 ]], otherMax --[[ vec2 ]])
bool = aabb2d.equal(..., otherMin --[[ vec2 ]], otherMax --[[ vec2 ]], eps --[[ number ]])
bool = aabb2d.equal(..., otherMin --[[ vec2 ]], otherMax --[[ vec2 ]], eps --[[ vec2 ]])
bool = aabb2d.equal(..., otherMin --[[ vec2 ]], otherMax --[[ vec2 ]], ULPs --[[ integer ]])

-- explicit operator==(AABB, AABB)
bool = aabb.operator_equals(..., otherMin --[[ vec3 ]], otherMax --[[ vec3 ]])
bool = aabb2d.operator_equals(..., otherMin --[[ vec2 ]], otherMax --[[ vec2 ]])
```

### aabb.notEqual

```lua
bool = aabb.notEqual(..., otherMin --[[ vec3 ]], otherMax --[[ vec3 ]])
bool = aabb.notEqual(..., otherMin --[[ vec3 ]], otherMax --[[ vec3 ]], eps --[[ number ]])
bool = aabb.notEqual(..., otherMin --[[ vec3 ]], otherMax --[[ vec3 ]], eps --[[ vec3 ]])
bool = aabb.notEqual(..., otherMin --[[ vec3 ]], otherMax --[[ vec3 ]], ULPs --[[ integer ]])

bool = aabb2d.notEqual(..., otherMin --[[ vec2 ]], otherMax --[[ vec2 ]])
bool = aabb2d.notEqual(..., otherMin --[[ vec2 ]], otherMax --[[ vec2 ]], eps --[[ number ]])
bool = aabb2d.notEqual(..., otherMin --[[ vec2 ]], otherMax --[[ vec2 ]], eps --[[ vec2 ]])
bool = aabb2d.notEqual(..., otherMin --[[ vec2 ]], otherMax --[[ vec2 ]], ULPs --[[ integer ]])
```

### aabb.isinf

```lua
-- Tests if any component of the AABB is infinite
bool = aabb.isinf(...)
bool = aabb2d.isinf(...)
```

### aabb.isnan

```lua
-- Tests if any component of the AABB is NaN
bool = aabb.isnan(...)
bool = aabb2d.isnan(...)
```

### aabb.isfinite

```lua
-- Test if all components of the AABB are finite
bool = aabb.isfinite(...)
bool = aabb2d.isfinite(...)
```

### aabb.isDegenerate

```lua
-- Return true if the AABB is degenerate, i.e., does not span in a strictly
-- positive volume
bool = aabb.isDegenerate(...)
bool = aabb2d.isDegenerate(...)
```

### aabb.centerPoint

```lua
-- @ALIAS: aabb.centroid
-- Return the center point of the AABB
vec3 = aabb.centerPoint(...)
vec2 = aabb2d.centerPoint(...)
```

### aabb.pointInside

```lua
-- Generates a point inside the AABB where "p" is a vector of normalized values
-- (i.e., between [0, 1]) along each axis relative to the AABB minpoint
vec3 = aabb.pointInside(..., p --[[ vec3 ]])
vec2 = aabb2d.pointInside(..., p --[[ vec2 ]])
```

### aabb.minimalEnclosingSphere

```lua
-- Return the smallest sphere that contains the AABB
spherePos --[[ vec3 ]], sphereRad --[[ number ]] = aabb.minimalEnclosingSphere(...)
```

### aabb.maximalContainedSphere

```lua
-- Return the largest sphere that can fit inside the AABB
spherePos --[[ vec3 ]], sphereRad --[[ number ]] = aabb.maximalContainedSphere(...)
```

### aabb.edge

```lua
-- Return an edge (segment) of the AABB: [0, 11]
segStart --[[ vec3 ]], segEnd --[[ vec3 ]] = aabb.edge(..., index --[[ integer ]])
segStart --[[ vec2 ]], segEnd --[[ vec2 ]] = aabb2d.edge(..., index --[[ integer ]])
```

### aabb.cornerPoint

```lua
-- Return a corner point of the AABB: [0, 7]
vec3 = aabb.cornerPoint(..., index --[[ integer ]])

-- Return a corner point of the AABB: [0, 4]
vec2 = aabb2d.cornerPoint(..., index --[[ integer ]])
```

### aabb.extremePoint

```lua
-- Compute an extreme point along the AABB, i.e., the furthest point in a given
-- direction
vec3 = aabb.extremePoint(..., direction --[[ vec3 ]])
vec2 = aabb2d.extremePoint(..., direction --[[ vec2 ]])
```

### aabb.pointOnEdge

```lua
-- Computes a point along an edge of the AABB: [0, 11]
vec3 = aabb.pointOnEdge(..., edgeIndex --[[ integer ]], u --[[ number ]])
```

### aabb.faceCenterPoint

```lua
-- Return the point at the center of the given face, [0, 5], of the AABB
vec3 = aabb.faceCenterPoint(..., faceIndex --[[ integer ]])
```

### aabb.facePoint

```lua
-- Generate a point on the surface of the given face of the AABB
vec3 = aabb.facePoint(...,
    faceIndex --[[ integer ]],
    u --[[ number ]],
    v --[[ number ]]
)
```

### aabb.faceNormal

```lua
-- Return the surface normal of the given face of the AABB
vec3 = aabb.faceNormal(..., faceIndex --[[ integer ]])
```

### aabb.facePlane

```lua
-- Generate a plane (point and normal) for the given face of the AABB
planeNormal --[[ vec3 ]], planeOffset --[[ number ]] = aabb.facePlane(...,
    faceIndex --[[ integer ]]
)
```

### aabb.size

```lua
-- Return the length of the AABB along each dimension
vec3 = aabb.size(...)
vec2 = aabb2d.size(...)
```

### aabb.halfSize

```lua
-- Return the radius of the AABB along each dimension
vec3 = aabb.halfSize(...)
vec2 = aabb2d.halfSize(...)
```

### aabb.volume

```lua
-- Compute the volume of the AABB
number = aabb.volume(...)
number = aabb2d.volume(...)
```

### aabb.surfaceArea

```lua
-- Computes the surface area of the faces of the AABB
number = aabb.surfaceArea(...)
```

### aabb.scale

```lua
-- Apply a uniform scale to the AABB
aabbMin,aabbMax = aabb.scale(..., centerPoint --[[ vec3 --]], factor --[[ number ]])
aabbMin,aabbMax = aabb2d.scale(..., centerPoint --[[ vec2 --]], factor --[[ number ]])
```

### aabb.projectToAxis

```lua
-- Project the AABB onto the provided axis
parametricMin --[[ number ]], parametricMax --[[ number ]] = aabb.projectToAxis(, axis --[[ vec3 ]])
parametricMin --[[ number ]], parametricMax --[[ number ]] = aabb2d.projectToAxis(, axis --[[ vec2 ]])
```

### aabb.closestPoint

```lua
-- Computes the closest point inside the AABB to the given point
vec3 = aabb.closestPoint(..., point --[[ vec3 ]])
vec2 = aabb2d.closestPoint(..., point --[[ vec2 ]])
```

### aabb.distance

```lua
-- Computes the distance between the AABB and the given objects
number = aabb.distance(..., point --[[ vec3 ]])
number = aabb.distanceSphere(..., spherePos --[[ vec3 ]], sphereRad --[[ number ]])

number = aabb2d.distance(..., point --[[ vec2 ]])
number = aabb2d.distanceSphere(..., spherePos --[[ vec2 ]], sphereRad --[[ number ]])
```

### aabb.contains

```lua
-- Tests for if the given objects are fully contained inside the AABB
bool = aabb.contains(..., point --[[ vec3 ]])
bool = aabb2d.contains(..., point --[[ vec2 ]])

bool = aabb.containsAABB(..., aabbMin --[[ vec3 ]], aabbMax --[[ vec3 ]])
bool = aabb2d.containsAABB(..., aabbMin --[[ vec2 ]], aabbMax --[[ vec2 ]])

bool = aabb.containsSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
bool = aabb2d.containsSegment(..., segStart --[[ vec2 ]], segEnd --[[ vec2 ]])

bool = aabb.containsSphere(..., spherePos --[[ vec3 ]], sphereRad --[[ number ]])
bool = aabb2d.containsSphere(..., spherePos --[[ vec2 ]], sphereRad --[[ number ]])

bool = aabb.containsTriangle(..., ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]])
bool = aabb.containsPolygon(..., polygon --[[ polygon ]])
```

### aabb.enclose

```lua
-- Expand the AABB to enclose the given objects
aabbMin,aabbMax = aabb.enclose(..., point --[[ vec3 ]])
aabbMin,aabbMax = aabb.encloseSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
aabbMin,aabbMax = aabb.encloseSphere(..., spherePos --[[ vec3 ]], sphereRad --[[ number ]])
aabbMin,aabbMax = aabb.encloseAABB(..., otherMin --[[ vec3 ]], otherMax --[[ vec3 ]])
aabbMin,aabbMax = aabb.encloseTriangle(..., ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]])
aabbMin,aabbMax = aabb.enclosePolygon(..., polygon --[[ polygon ]])

aabbMin,aabbMax = aabb2d.enclose(..., point --[[ vec2 ]])
aabbMin,aabbMax = aabb2d.encloseSegment(..., segStart --[[ vec2 ]], segEnd --[[ vec2 ]])
aabbMin,aabbMax = aabb2d.encloseSphere(..., spherePos --[[ vec2 ]], sphereRad --[[ number ]])
aabbMin,aabbMax = aabb2d.encloseAABB(..., otherMin --[[ vec2 ]], otherMax --[[ vec2 ]])
```

### aabb.enclose

```lua
-- Expand all dimensions by delta
aabbMin,aabbMax = aabb.enclose(..., delta --[[ vec3 ]])
```

### aabb.clamp

```lua
-- Clamp the AABB to be contained within the specified (other) AABB.
aabbMin,aabbMax = aabb.clamp(..., otherMin --[[ vec3 ]], otherMax --[[ vec3 ]])
```

### aabb.intersects

```lua
-- Functions to determine if the AABB to intersect the given objects.
-- result --[[ bool ]]: True on intersection
-- dNear/dFar: Distances along the object where it enters/exits the AABB. When
--           : passed as an optional parameter: a limit to the intersection
--           : test, e.g., absolute distances (-100, 100) for lines/rays and
--           : relative distances (0.25, 0.75) for segments.
result = aabb.intersectsAABB(..., otherMin --[[ vec3 ]], otherMax --[[ vec3 ]])
result = aabb2d.intersectsAABB(..., otherMin --[[ vec2 ]], otherMax --[[ vec2 ]])

result = aabb.intersectsSphere(..., spherePos --[[ vec3 ]], sphereRad --[[ number ]])
result = aabb2d.intersectsSphere(..., spherePos --[[ vec2 ]], sphereRad --[[ number ]])

result = aabb.intersectsPlane(..., planeNormal --[[ vec3 ]], planeOffset --[[ number]])

result,dNear,dFar = aabb.intersectsLine(..., linePos --[[ vec3 ]], lineDir --[[ vec3 ]], [dNear[, dFar]])
result,dNear,dFar = aabb2d.intersectsLine(..., linePos --[[ vec2 ]], lineDir --[[ vec2 ]], [dNear[, dFar]])

result,dNear,dFar = aabb.intersectsRay(..., rayPos --[[ vec3 ]], rayDir --[[ vec3 ]], [dNear[, dFar]])
result,dNear,dFar = aabb2d.intersectsRay(..., rayPos --[[ vec2 ]], rayDir --[[ vec2 ]], [dNear[, dFar]])

result,dNear,dFar = aabb.intersectsSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]], [dNear[, dFar]])
result,dNear,dFar = aabb2d.intersectsSegment(..., segStart --[[ vec2 ]], segEnd --[[ vec2 ]], [dNear[, dFar]])
```

### aabb.intersection

```lua
-- Return the intersection of two AABBs, i.e., the AABB that is contained in both.
aabbMin,aabbMax = aabb.intersection(..., otherMin --[[ vec3 ]], otherMax --[[ vec3 ]])
aabbMin,aabbMax = aabb2d.intersection(..., otherMin --[[ vec2 ]], otherMax --[[ vec2 ]])
```

## Line

A line that extends to plus/minus infinity. All operators (non-constructors)
are of the form:

```lua
-- linePos --[[ vec3 ]]: The origin of the line in world space,
-- lineDir --[[ vec3 ]]: The direction of the line,
-- ...: variadic function parameters.
result = line.function(linePos, lineDir, ...)
```

### line.operator\_negate

```lua
linePos,lineDir = line.operator_negate(...)
```

### line.operator\_add

```lua
linePos,lineDir = line.operator_add(..., point --[[ vec3 ]])
```

### line.operator\_sub

```lua
linePos,lineDir = line.operator_sub(..., point --[[ vec3 ]])
```

### line.operator\_mul

```lua
linePos,lineDir = line.operator_mul(matrix3x3, ...)
linePos,lineDir = line.operator_mul(matrix4x3, ...)
linePos,lineDir = line.operator_mul(matrix4x4, ...)
linePos,lineDir = line.operator_mul(quat, ...)
```

### line.equal

```lua
bool = line.equal(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]])
bool = line.equal(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]], eps --[[ number ]])
bool = line.equal(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]], eps --[[ vec3 ]])
bool = line.equal(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]], ULPs --[[ integer ]])

-- explicit operator==(Line, Line)
bool = line.operator_equals(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]])
```

### line.notEqual

```lua
bool = line.notEqual(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]])
bool = line.notEqual(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]], eps --[[ number ]])
bool = line.notEqual(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]], eps --[[ vec3 ]])
bool = line.notEqual(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]], ULPs --[[ integer ]])
```

### line.to\_segment

```lua
-- Convert the Line to a LineSegment by specifying the (possibly negative)
-- distance to the endpoint along this Line
segStart --[[ vec3 ]], segEnd --[[ vec3 ]] = line.to_segment(..., distance --[[ number ]])
```

### line.isinf

```lua
-- Tests if any component of the line is infinite
bool = line.isinf(...)
```

### line.isnan

```lua
-- Tests if any component of the line is NaN
bool = line.isnan(...)
```

### line.isfinite

```lua
-- Test if all components of the line are finite
bool = line.isfinite(...)
```

### line.getPoint

```lua
-- Get a point along the line at a given distance (parametric point)
vec3 = line.getPoint(..., distance --[[ number ]])
```

### line.projectToAxis

```lua
-- Project the line onto the given axis (direction), i.e., collapse the line
parametricMin --[[ number ]], parametricMax --[[ number ]] = line.projectToAxis(..., axis --[[ vec3 ]])
```

### line.closest

```lua
-- Computes the closest point on this line to the given object
-- d --[[ number ]]: Parametric distance along along the line,
-- d2 --[[ number ]]: Parametric distance along the other object,
-- u,v --[[ number ]]: Barycentric triangle coordinates.
-- p --[[ vec3 ]]: Closest point,
p,d = line.closest(..., point --[[ vec3 ]])
p,d,d2 = line.closestRay(..., rayPos --[[ vec3 ]], rayDir --[[ vec3 ]])
p,d,d2 = line.closestLine(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]])
p,d,d2 = line.closestSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
p,d,u,v = line.closestTriangle(..., ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]])
```

### line.contains

```lua
-- Tests if the given object is fully contained on the line
bool = line.contains(..., point --[[ vec3 ]], epsilon --[[ number ]])
bool = line.containsRay(..., rayPos --[[ vec3 ]], rayDir --[[ vec3 ]], epsilon --[[ number ]])
bool = line.containsSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]], epsilon --[[ number ]])
```

### line.distance

```lua
-- Computes the distance between the line and the given object
-- d --[[ number ]]: Parametric distance along along the line,
-- d2 --[[ number ]]: Parametric distance along the other object,
-- dist --[[ number ]]: Distance between the two objects.
dist,d = line.distance(..., point --[[ vec3 ]])
dist,d,d2 = line.distanceRay(..., rayPos --[[ vec3 ]], rayDir --[[ vec3 ]])
dist,d,d2 = line.distanceLine(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]])
dist,d,d2 = line.distanceSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
dist = line.distanceSphere(..., spherePos --[[ vec3 ]], sphereRad --[[ number ]])
```

### line.intersects

```lua
-- Tests whether the line and the given object intersect
bool = line.intersectsPlane(..., planeNormal --[[ vec3 ]], planeOffset --[[ number ]])

-- count --[[ integer ]]: Number of intersection points between the object and
--                      : sphere: 0, 1 (dNear), or 2 (dNear & dFar)
-- u,v --[[ number ]]: Barycentric triangle coordinates.
-- dNear/dFar: Distances along the object where it enters/exits the line. When
--           : passed as an optional parameter: a limit to the intersection
--           : test, e.g., absolute distances (-100, 100) for lines/rays and
--           : relative distances (0.25, 0.75) for segments.
bool,dNear,dFar = line.intersectsAABB(..., aabbMin --[[ vec3 ]], aabbMax --[[ vec3 ]], [dNear[, dFar]])
bool,dNear,u,v = line.intersectsTriangle(..., ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]])
count,dNear,dFar = line.intersectsSphere(..., spherePos --[[ vec3 ]], sphereRad --[[ number ]])
```

## Ray

A line that extends to infinity is only **one** direction. All operators
(non-constructors) are of the form:

```lua
-- rayPos --[[ vec3 ]]: The origin of the line in world space,
-- rayDir --[[ vec3 ]]: The direction of the line,
-- ...: variadic function parameters
result = line.function(rayPos, rayDir, ...)
```

### ray.operator\_negate

```lua
rayPos,rayDir = ray.operator_negate(...)
```

### ray.operator\_add

```lua
rayPos,rayDir = ray.operator_add(..., offset --[[ vec3 ]])
```

### ray.operator\_sub

```lua
rayPos,rayDir = ray.operator_sub(..., offset --[[ vec3 ]])
```

### ray.operator\_mul

```lua
rayPos,rayDir = ray.operator_mul(matrix3x3, ...)
rayPos,rayDir = ray.operator_mul(matrix4x3, ...)
rayPos,rayDir = ray.operator_mul(matrix4x4, ...)
rayPos,rayDir = ray.operator_mul(quat, ...)
```

### ray.equal

```lua
bool = ray.equal(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]])
bool = ray.equal(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]], eps --[[ number ]])
bool = ray.equal(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]], eps --[[ vec3 ]])
bool = ray.equal(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]], ULPs --[[ integer ]])

-- explicit operator==(Ray, Ray)
bool = ray.operator_equals(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]])
```

### ray.notEqual

```lua
bool = ray.notEqual(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]])
bool = ray.notEqual(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]], eps --[[ number ]])
bool = ray.notEqual(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]], eps --[[ vec3 ]])
bool = ray.notEqual(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]], ULPs --[[ integer ]])
```

### ray.isinf

```lua
-- Tests if any component of the ray is infinite
bool = ray.isinf(...)
```

### ray.isnan

```lua
-- Tests if any component of the ray is NaN
bool = ray.isnan(...)
```

### ray.isfinite

```lua
-- Test if all components of the AABB are finite
bool = ray.isfinite(...)
```

### ray.getPoint

```lua
-- Get a point along the ray at a given distance (parametric point). Passing
-- negative values to this function treats the ray as if it were a line
vec3 = ray.getPoint(..., distance --[[ number ]])
```

### ray.projectToAxis

```lua
-- Project the ray onto the given axis (direction), i.e., collapse the ray onto
-- an axis
--
-- d --[[ number ]]: Parametric distance along along the ray,
-- d2 --[[ number ]]: Parametric distance along the other object.
d,d2 = ray.projectToAxis(..., direction --[[ vec3 ]])
```

### ray.closest

```lua
-- Computes the closest point on this ray to the given object
-- d --[[ number ]]: Parametric distance along along the ray,
-- d2 --[[ number ]]: Parametric distance along the other object,
-- p --[[ vec3 ]]: Closest point
p,d = ray.closest(..., point --[[ vec3 ]])
p,d,d2 = ray.closestRay(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]])
p,d,d2 = ray.closestLine(..., linePos --[[ vec3 ]], lineDir --[[ vec3 ]]))
p,d,d2 = ray.closestSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
```

### ray.contains

```lua
-- Tests if the given object is fully contained on the ray.
bool = ray.contains(..., point --[[ vec3 ]])
bool = ray.containsSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
```

### ray.distance

```lua
-- Computes the distance between the ray and the given object
-- d --[[ number ]]: Parametric distance along along the ray,
-- d2 --[[ number ]]: Parametric distance along the other object,
-- dist --[[ number ]]: Distance between the ray and other object
dist,d = ray.distance(..., point --[[ vec3 ]])
dist,d,d2 = ray.distanceLine(..., linePos --[[ vec3 ]], lineDir --[[ vec3 ]])
dist,d,d2 = ray.distanceRay(..., otherPos --[[ vec3 ]], otherDir --[[ vec3 ]])
dist,d,d2 = ray.distanceSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
dist = ray.distanceSphere(..., spherePos --[[ vec3 ]], sphereRad --[[ number ]])
```

### ray.intersects

```lua
-- Tests whether the ray and the given object intersect
-- count --[[ integer ]]: Number of intersection points between the object and
--                      : sphere: 0, 1 (dNear), or 2 (dNear & dFar)
-- u,v --[[ number ]]: Barycentric triangle coordinates.
-- dNear/dFar: Distances along the object where it enters/exits the ray. When
--           : passed as an optional parameter: a limit to the intersection
--           : test, e.g., absolute distances (-100, 100) for lines/rays and
--           : relative distances (0.25, 0.75) for segments.
bool,dNear,dFar = ray.intersectsAABB(..., aabbMin --[[ vec3 ]], aabbMax --[[ vec3 ]], [dNear[, dFar]])
bool,dNear = ray.intersectsPlane(..., planeNormal --[[ vec3 ]], planeOffset --[[ number ]])
bool,dNear,u,v = line.intersectsTriangle(..., ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]])
count,dNear,dFar = ray.intersectsSphere(..., spherePos --[[ vec3 ]], sphereRad --[[ number ]])
```

## Segment

A line in world-space with a finite/definite start and end point. All operators
(non-constructors) are of the form:

```lua
-- segStart --[[ vec3 ]]: The starting point of the line segment,
-- segEnd --[[ vec3 ]]: The ending point to the line segment,
-- ...: variadic function parameters
result = segment.function(segStart, segEnd, ...)
```

### segment.operator\_negate

```lua
segStart,segEnd = segment.operator_negate(...)
segStart,segEnd = segment2d.operator_negate(...)
```

### segment.operator\_add

```lua
segStart,segEnd = segment.operator_add(..., point --[[ vec3 ]])
segStart,segEnd = segment2d.operator_add(..., point --[[ vec2 ]])
```

### segment.operator\_sub

```lua
segStart,segEnd = segment.operator_sub(..., point --[[ vec3 ]])
segStart,segEnd = segment2d.operator_sub(..., point --[[ vec2 ]])
```

### segment.operator\_mul

```lua
segStart,segEnd = segment.operator_mul(matrix3x3, ...)
segStart,segEnd = segment.operator_mul(matrix4x3, ...)
segStart,segEnd = segment.operator_mul(matrix4x4, ...)
segStart,segEnd = segment.operator_mul(quat, ...)
```

### segment.equal

```lua
bool = segment.equal(..., otherStart --[[ vec3 ]], otherEnd --[[ vec3 ]])
bool = segment.equal(..., otherStart --[[ vec3 ]], otherEnd --[[ vec3 ]], eps --[[ number ]])
bool = segment.equal(..., otherStart --[[ vec3 ]], otherEnd --[[ vec3 ]], eps --[[ vec3 ]])
bool = segment.equal(..., otherStart --[[ vec3 ]], otherEnd --[[ vec3 ]], ULPs --[[ integer ]])
bool = segment2d.equal(..., otherStart --[[ vec2 ]], otherEnd --[[ vec2 ]])
bool = segment2d.equal(..., otherStart --[[ vec2 ]], otherEnd --[[ vec2 ]], eps --[[ number ]])
bool = segment2d.equal(..., otherStart --[[ vec2 ]], otherEnd --[[ vec2 ]], eps --[[ vec2 ]])
bool = segment2d.equal(..., otherStart --[[ vec2 ]], otherEnd --[[ vec2 ]], ULPs --[[ integer ]])

-- explicit operator==(Segment, Segment)
bool = segment.operator_equals(..., otherStart --[[ vec3 ]], otherEnd --[[ vec3 ]])
bool = segment2d.operator_equals(..., otherStart --[[ vec2 ]], otherEnd --[[ vec2 ]])
```

### segment.notEqual

```lua
bool = segment.notEqual(..., otherStart --[[ vec3 ]], otherEnd --[[ vec3 ]])
bool = segment.notEqual(..., otherStart --[[ vec3 ]], otherEnd --[[ vec3 ]], eps --[[ number ]])
bool = segment.notEqual(..., otherStart --[[ vec3 ]], otherEnd --[[ vec3 ]], eps --[[ vec3 ]])
bool = segment.notEqual(..., otherStart --[[ vec3 ]], otherEnd --[[ vec3 ]], ULPs --[[ integer ]])

bool = segment2d.notEqual(..., otherStart --[[ vec2 ]], otherEnd --[[ vec2 ]])
bool = segment2d.notEqual(..., otherStart --[[ vec2 ]], otherEnd --[[ vec2 ]], eps --[[ number ]])
bool = segment2d.notEqual(..., otherStart --[[ vec2 ]], otherEnd --[[ vec2 ]], eps --[[ vec2 ]])
bool = segment2d.notEqual(..., otherStart --[[ vec2 ]], otherEnd --[[ vec2 ]], ULPs --[[ integer ]])
```

### segment.length

```lua
-- Return the length of this line segment
number = segment.length(...)
number = segment2d.length(...)

-- Return the squared length of this line segment
number = segment.length2(...)
number = segment2d.length2(...)
```

### segment.isfinite

```lua
-- Tests if any component of the segment is infinite
number = segment.length2(...)
number = segment2d.length2(...)
```

### segment.getPoint

```lua
-- Get a point along the line at a given distance (parametric point)
vec3 = segment.getPoint(..., distance --[[ number ]])
vec2 = segment2d.getPoint(..., distance --[[ number ]])
```

### segment.centerPoint

```lua
-- @ALIAS: segment.centroid
-- Returns the center point of the line segment: getPoint(line, T(0.5))
vec3 = segment.centerPoint(...)
vec2 = segment2d.centerPoint(...)
```

### segment.reverse

```lua
-- Reverses the direction of the line segment
segEnd,segStart = segment.reverse(...)
segEnd,segStart = segment2d.reverse(...)
```

### segment.dir

```lua
-- Returns the (normalized) direction vector that points from segStart to segEnd
vec3 = segment.dir(...)
vec2 = segment2d.dir(...)
```

### segment.extremePoint

```lua
-- Compute an extreme point along the segment, i.e., the furthest point in a
-- given direction
vec3 = segment.extremePoint(..., direction --[[ vec3 ]])
vec2 = segment2d.extremePoint(..., direction --[[ vec2 ]])
```

### segment.closest

```lua
-- Computes the closest point on this segment to the given object.
-- d --[[ number ]]: Parametric distance along along the line,
-- d2 --[[ number ]]: Parametric distance along the other object,
-- u,v --[[ number ]]: Barycentric triangle coordinates.
-- p --[[ vec3 ]]: Closest point
p,d = segment.closestPoint(..., point --[[ vec3 ]])
p,d,d2 = segment.closestRay(..., rayPos --[[ vec3 ]], rayDir --[[ vec3 ]])
p,d,d2 = segment.closestLine(..., linePos --[[ vec3 ]], lineDir --[[ vec3 ]])
p,d,d2 = segment.closestSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
p,d,u,v = segment.closestTriangle(..., ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]])

p,d = segment2d.closestPoint(..., point --[[ vec2 ]])
p,d,d2 = segment2d.closestRay(..., rayPos --[[ vec2 ]], rayDir --[[ vec2 ]])
p,d,d2 = segment2d.closestLine(..., linePos --[[ vec2 ]], lineDir --[[ vec2 ]])
p,d,d2 = segment2d.closestSegment(..., segStart --[[ vec2 ]], segEnd --[[ vec2 ]])
```

### segment.contains

```lua
-- Tests if the given object is fully contained on the segment
bool = segment.containsPoint(..., point --[[ vec3 ]], epsilon --[[ number ]])
bool = segment2d.containsPoint(..., point --[[ vec2 ]], epsilon --[[ number ]])

bool = segment.containsSegment(... segStart --[[ vec3 ]], segEnd --[[ vec3 ]], epsilon --[[ number ]])
bool = segment2d.containsSegment(... segStart --[[ vec2 ]], segEnd --[[ vec2 ]], epsilon --[[ number ]])
```

### segment.distance

```lua
-- Computes the distance between the segment and the given object
-- d --[[ number ]]: Parametric distance along along the segment,
-- d2 --[[ number ]]: Parametric distance along the other object,
-- dist --[[ number ]]: Distance between the two objects
dist,d = segment.distance(..., point --[[ vec3 ]])
dist,d = segment2d.distance(..., point --[[ vec2 ]])

dist,d,d2 = segment.distanceRay(..., rayPos --[[ vec3 ]], rayDir --[[ vec3 ]])
dist,d,d2 = segment2d.distanceRay(..., rayPos --[[ vec2 ]], rayDir --[[ vec2 ]])

dist,d,d2 = segment.distanceLine(..., linePos --[[ vec3 ]], lineDir --[[ vec3 ]])
dist,d,d2 = segment2d.distanceLine(..., linePos --[[ vec2 ]], lineDir --[[ vec2 ]])

dist,d,d2 = segment.distanceSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
dist,d,d2 = segment2d.distanceSegment(..., segStart --[[ vec2 ]], segEnd --[[ vec2 ]])

dist = segment.distancePlane(..., planeNormal --[[ vec3 ]], planeOffset --[[ number ]])
dist = segment2d.distancePlane(..., planeNormal --[[ vec2 ]], planeOffset --[[ number ]])

-- Squared distanced
dist,d = segment.distance2(..., point --[[ vec3 ]])
dist,d = segment2d.distance2(..., point --[[ vec2 ]])

dist,d,d2 = segment.distanceSegment2(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
dist,d,d2 = segment2d.distanceSegment2(..., segStart --[[ vec2 ]], segEnd --[[ vec2 ]])
```

### segment.intersects

```lua
-- Tests whether the segment and the given object intersect
-- d --[[ number ]]: Parametric distance along along the segment,
-- d2 --[[ number ]]: Parametric distance along the other object,
-- u,v --[[ number ]]: Barycentric triangle coordinates.
-- count --[[ integer ]]: Number of intersection points between the line and
--                      : sphere: 0, 1 (dNear), or 2 (dNear & dFar)
bool,d,d2 = segment.intersectsAABB(..., aabbMin --[[ vec3 ]], aabbMax --[[ vec3 ]])
bool,d,d2 = segment2d.intersectsAABB(..., aabbMin --[[ vec2 ]], aabbMax --[[ vec2 ]])

bool,d,d2 = segment.intersectsSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]], epsilon --[[ number ]])
bool,d,d2 = segment2d.intersectsSegment(..., segStart --[[ vec2 ]], segEnd --[[ vec2 ]], epsilon --[[ number ]])

bool,d = segment.intersectsPlane(..., planeNormal --[[ vec3 ]], planeOffset --[[ number ]])
bool,d,u,v = segment.intersectsTriangle(..., ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]])
count,dNear,dFar = segment.intersectsSphere(..., spherePos --[[ vec3 ]], sphereRad --[[ number ]])
```

## Sphere

A Sphere. All operators (non-constructors) are of the form:

```lua
-- spherePos: The centroid of the sphere (or circle),
-- sphereRad: The radius of the sphere (or circle),
-- ...: variadic function parameters
result = sphere.function(spherePos --[[ vec3 ]], sphereRad --[[ number ]], ...)
result = circle.function(circlePos --[[ vec2 ]], circleRad --[[ number ]], ...)
```

### sphere.operator\_negate

```lua
spherePos,sphereRad = sphere.operator_negate(...)
circlePos,circleRad = circle.operator_negate(...)
```

### sphere.operator\_add

```lua
spherePos,sphereRad = sphere.operator_add(..., offset --[[ vec3 ]])
circlePos,circleRad = circle.operator_add(..., offset --[[ vec2 ]])
```

### sphere.operator\_sub

```lua
spherePos,sphereRad = sphere.operator_sub(..., offset --[[ vec3 ]])
circlePos,circleRad = circle.operator_sub(..., offset --[[ vec2 ]])
```

### sphere.operator\_mul

```lua
spherePos,sphereRad = sphere.operator_mul(matrix3x3, ...)
spherePos,sphereRad = sphere.operator_mul(matrix4x3, ...)
spherePos,sphereRad = sphere.operator_mul(matrix4x4, ...)
spherePos,sphereRad = sphere.operator_mul(quat, ...)
```

### sphere.equal

```lua
bool = sphere.equal(..., otherPos --[[ vec3 ]], otherRad --[[ number ]])
bool = sphere.equal(..., otherPos --[[ vec3 ]], otherRad --[[ number ]], eps --[[ number ]])
bool = sphere.equal(..., otherPos --[[ vec3 ]], otherRad --[[ number ]], eps --[[ vec3 ]])
bool = sphere.equal(..., otherPos --[[ vec3 ]], otherRad --[[ number ]], ULPs --[[ integer ]])

bool = circle.equal(..., otherPos --[[ vec2 ]], otherRad --[[ number ]])
bool = circle.equal(..., otherPos --[[ vec2 ]], otherRad --[[ number ]], eps --[[ number ]])
bool = circle.equal(..., otherPos --[[ vec2 ]], otherRad --[[ number ]], eps --[[ vec2 ]])
bool = circle.equal(..., otherPos --[[ vec2 ]], otherRad --[[ number ]], ULPs --[[ integer ]])

-- explicit operator==(Sphere, Sphere)
bool = sphere.operator_equals(..., otherPos --[[ vec3 ]], otherRad --[[ number ]])
bool = circle.operator_equals(..., otherPos --[[ vec2 ]], otherRad --[[ number ]])
```

### sphere.notEqual

```lua
bool = sphere.notEqual(..., otherPos --[[ vec3 ]], otherRad --[[ number ]])
bool = sphere.notEqual(..., otherPos --[[ vec3 ]], otherRad --[[ number ]], eps --[[ number ]])
bool = sphere.notEqual(..., otherPos --[[ vec3 ]], otherRad --[[ number ]], eps --[[ vec3 ]])
bool = sphere.notEqual(..., otherPos --[[ vec3 ]], otherRad --[[ number ]], ULPs --[[ integer ]])

bool = circle.notEqual(..., otherPos --[[ vec2 ]], otherRad --[[ number ]])
bool = circle.notEqual(..., otherPos --[[ vec2 ]], otherRad --[[ number ]], eps --[[ number ]])
bool = circle.notEqual(..., otherPos --[[ vec2 ]], otherRad --[[ number ]], eps --[[ vec2 ]])
bool = circle.notEqual(..., otherPos --[[ vec2 ]], otherRad --[[ number ]], ULPs --[[ integer ]])
```

### sphere.isinf

```lua
-- Tests if any component of the sphere/circle is infinite
bool = sphere.isinf(...)
bool = circle.isinf(...)
```

### sphere.isnan

```lua
-- Tests if any component of the sphere/circle is NaN
bool = sphere.isnan(...)
bool = circle.isnan(...)
```

### sphere.isfinite

```lua
-- Test if all components of the sphere/circle are finite
bool = sphere.isfinite(...)
```

### sphere.isDegenerate

```lua
-- Test whether the sphere/circle is degenerate, i.e., not finite or if the
-- radius is less-or-equal to zero
bool = sphere.isDegenerate(...)
bool = circle.isDegenerate(...)
```

### sphere.volume

```lua
-- Return the volume of the sphere
number = sphere.volume(...)
```

### sphere.surfaceArea

```lua
-- Return the surface area of the sphere
number = sphere.surfaceArea(...)
number = circle.area(...)
```

### sphere.extremePoint

```lua
-- Return an extreme point along the sphere, i.e., the furthest point in a given
-- direction
vec3 = sphere.extremePoint(..., direction --[[ vec3 ]])
vec2 = circle.extremePoint(..., direction --[[ vec2 ]])
```

### sphere.enclose

```lua
-- Expands the sphere to enclose both the sphere and the given object(s)
spherePos,sphereRad = sphere.enclose(..., point --[[ vec3 ]])
circlePos,circleRad = circle.enclose(..., point --[[ vec2 ]])

spherePos,sphereRad = sphere.encloseSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
circlePos,circleRad = circle.encloseSegment(..., segStart --[[ vec2 ]], segEnd --[[ vec2 ]])

spherePos,sphereRad = sphere.encloseSphere(..., otherPos --[[ vec3 ]], otherRad --[[ number ]]))
circlePos,circleRad = circle.encloseCircle(..., otherPos --[[ vec2 ]], otherRad --[[ number ]]))

spherePos,sphereRad = sphere.encloseAABB(..., aabbMin --[[ vec3 ]], aabbMax --[[ vec3 ]])
circlePos,circleRad = circle.encloseAABB(..., aabbMin --[[ vec2 ]], aabbMax --[[ vec2 ]])

spherePos,sphereRad = sphere.encloseTriangle(..., ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]])
```

### sphere.extendRadiusToContain

```lua
-- Expands the radius of the Sphere until it encloses the given object(s)
spherePos,sphereRad = sphere.extendRadiusToContain(..., point --[[ vec3 ]])
circlePos,circleRad = circle.extendRadiusToContain(..., point --[[ vec2 ]])

spherePos,sphereRad = sphere.extendRadiusToContainSphere(..., otherPos --[[ vec3 ]], otherRad --[[ number ]])
circlePos,circleRad = circle.extendRadiusToContainCircle(..., otherPos --[[ vec2 ]], otherRad --[[ number ]])
```

### sphere.maximalContainedAABB

```lua
-- Return the smallest AABB that encloses the sphere/circle
aabbMin --[[ vec3 ]], aabbMax --[[ vec3 ]] = sphere.maximalContainedAABB(...)
aabbMin --[[ vec2 ]], aabbMax --[[ vec2 ]] = circle.maximalContainedAABB(...)
```

### sphere.optimalEnclosingSphere

```lua
-- Compute the minimum bounding sphere for some number of points, i.e., the
-- smallest volume sphere that contains the provided points. Note, a minimal
-- enclosing sphere can be defined by four points (or fewer)

-- Compute the minimal bounding sphere for a fixed number of points
spherePos,sphereRad = sphere.optimalEnclosingSphere(a --[[ vec3 ]], b --[[ vec3 ]])
spherePos,sphereRad = sphere.optimalEnclosingSphere(a --[[ vec3 ]], b --[[ vec3 ]], c --[[ vec3 ]])
spherePos,sphereRad = sphere.optimalEnclosingSphere(a --[[ vec3 ]], b --[[ vec3 ]], c --[[ vec3 ]]), d --[[ vec3 ]])

-- Compute the minimal bounding sphere for a list of points
spherePos,sphereRad = sphere.optimalEnclosingSphere(array --[[ table ]])
```

### sphere.closestPoint

```lua
-- Return point on the sphere closest to the given point
vec3 = sphere.closestPoint(..., point --[[ vec3 ]])
vec2 = circle.closestPoint(..., point --[[ vec2 ]])
```

### sphere.projectToAxis

```lua
-- Project the Sphere onto the provided axis
parametricMin --[[ number ]], parametricMax --[[ number ]] = sphere.projectToAxis(, axis --[[ vec3 ]])
```

### sphere.contains

```lua
// Tests if the given object is fully contained within the sphere.
bool = sphere.contains(..., point --[[ vec3 ]])
bool = circle.contains(..., point --[[ vec2 ]])

bool = sphere.containsSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
bool = circle.containsSegment(..., segStart --[[ vec2 ]], segEnd --[[ vec2 ]])

bool = sphere.containsSphere(..., otherPos --[[ vec3 ]], otherRad --[[ number ]])
bool = circle.containsCircle(..., otherPos --[[ vec2 ]], otherRad --[[ number ]])

bool = sphere.containsAABB(..., aabbMin --[[ vec3 ]], aabbMax --[[ vec3 ]])
bool = circle.containsAABB(..., aabbMin --[[ vec2 ]], aabbMax --[[ vec2 ]])

bool = sphere.containsTriangle(..., ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]])
```

### sphere.distance

```lua
-- Computes the distance between the sphere and the given object
number = sphere.distance(..., point --[[ vec3 ]])
number = circle.distance(..., point --[[ vec2 ]])

number = sphere.distanceSphere(..., otherPos --[[ vec3 ]], otherRad --[[ number ]])
number = circle.distanceCircle(..., otherPos --[[ vec2 ]], otherRad --[[ number ]])

number = sphere.distanceAABB(..., aabbMin --[[ vec3 ]], aabbMax --[[ vec3 ]])
number = circle.distanceAABB(..., aabbMin --[[ vec2 ]], aabbMax --[[ vec2 ]])

number = sphere.distanceRay(..., rayPos --[[ vec3 ]], rayDir --[[ vec3 ]])
number = circle.distanceRay(..., rayPos --[[ vec2 ]], rayDir --[[ vec2 ]])

number = sphere.distanceSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
number = circle.distanceSegment(..., segStart --[[ vec2 ]], segEnd --[[ vec2 ]])

number = sphere.distanceLine(..., linePos --[[ vec3 ]], lineDir --[[ vec3 ]])
number = circle.distanceLine(..., linePos --[[ vec2 ]], lineDir --[[ vec2 ]])

number = sphere.distanceTriangle(..., ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]])
```

### sphere.intersects

```lua
-- Tests whether the sphere and the given object intersect
bool = sphere.intersectsSphere(..., otherPos --[[ vec3 ]], otherRad --[[ number ]])
bool = circle.intersectsCircle(..., otherPos --[[ vec2 ]], otherRad --[[ number ]])

bool = sphere.intersectsAABB(..., aabbMin --[[ vec3 ]], aabbMax --[[ vec3 ]])
bool = circle.intersectsAABB(..., aabbMin --[[ vec2 ]], aabbMax --[[ vec2 ]])

bool = sphere.intersectsPlane(..., planeNormal --[[ vec3 ]], planeOffset --[[ number ]])
bool = circle.intersectsPlane(..., planeNormal --[[ vec2 ]], planeOffset --[[ number ]])

bool = sphere.intersectsTriangle(..., ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]])

-- count --[[ integer ]]: Number of intersection points between the ray and
--                      : sphere: 0, 1 (dNear), or 2 (dFar).
-- dNear/dFar: Distances along the object where it enters/exits the sphere. When
--           : passed as an optional parameter: a limit to the intersection
--           : test, e.g., absolute distances (-100, 100) for lines/rays and
--           : relative distances (0.25, 0.75) for segments.
count,dNear,dFar = sphere.intersectsLine(..., linePos --[[ vec3 ]], lineDir --[[ vec3 ]])
count,dNear,dFar = circle.intersectsLine(..., linePos --[[ vec2 ]], lineDir --[[ vec2 ]])

count,dNear,dFar = sphere.intersectsSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
count,dNear,dFar = circle.intersectsSegment(..., segStart --[[ vec2 ]], segEnd --[[ vec2 ]])

count,dNear,dFar = sphere.intersectsRay(..., rayPos --[[ vec3 ]], rayDir --[[ vec3 ]])
count,dNear,dFar = circle.intersectsRay(..., rayPos --[[ vec2 ]], rayDir --[[ vec2 ]])
```

## Plane

An affine 2D subspace of a 3D dimensional space. All operators
(non-constructors) are of the form:

```lua
-- planeNormal --[[ vec3 ]]: The direction the plane is facing,
-- planeOffset --[[ vec3 ]]: The offset of this plane from the origin (0,0,0),
-- ...: variadic function parameters
result = plane.function(planeNormal, planeOffset, ...)
```

### plane.operator\_negate

```lua
planeNormal,planeOffset = plane.operator_negate(...)
```

### plane.operator\_add

```lua
planeNormal,planeOffset = plane.operator_add(..., offset --[[ vec3 ]])
```

### plane.operator\_sub

```lua
planeNormal,planeOffset = plane.operator_sub(..., offset --[[ vec3 ]])
```

### plane.operator\_mul

```lua
planeNormal,planeOffset = plane.operator_mul(matrix3x3, ...)
planeNormal,planeOffset = plane.operator_mul(matrix4x3, ...)
planeNormal,planeOffset = plane.operator_mul(matrix4x4, ...)
planeNormal,planeOffset = plane.operator_mul(quat, ...)
```

### plane.equal

```lua
bool = plane.equal(..., otherNormal --[[ vec3 ]], otherOffset --[[ number ]])
bool = plane.equal(..., otherNormal --[[ vec3 ]], otherOffset --[[ number ]], eps --[[ number ]])
bool = plane.equal(..., otherNormal --[[ vec3 ]], otherOffset --[[ number ]], eps --[[ vec3 ]])
bool = plane.equal(..., otherNormal --[[ vec3 ]], otherOffset --[[ number ]], ULPs --[[ integer ]])

-- explicit operator==(Plane, Plane)
bool = plane.operator_equals(..., otherNormal --[[ vec3 ]], otherOffset --[[ number ]])
```

### plane.notEqual

```lua
bool = plane.notEqual(..., otherNormal --[[ vec3 ]], otherOffset --[[ number ]])
bool = plane.notEqual(..., otherNormal --[[ vec3 ]], otherOffset --[[ number ]], eps --[[ number ]])
bool = plane.notEqual(..., otherNormal --[[ vec3 ]], otherOffset --[[ number ]], eps --[[ vec3 ]])
bool = plane.notEqual(..., otherNormal --[[ vec3 ]], otherOffset --[[ number ]], ULPs --[[ integer ]])
```

### plane.from

```lua
-- Construct a plane by specifying a ray that lies along the plane and its normal
planeNormal,planeOffset = plane.fromRay(rayPos --[[ vec3 ]], rayDir --[[ vec3 ]], normal --[[ vec3 ]])

-- Construct a plane by specifying a line that lies along the plane and its normal
planeNormal,planeOffset = plane.fromLine(linePos --[[ vec3 ]], lineDir --[[ vec3 ]], normal --[[ vec3 ]])

-- Construct a plane by specifying a segment that lies along the plane and its normal
planeNormal,planeOffset = plane.fromLineSegment(segStart --[[ vec3 ]], segEnd --[[ vec3 ]], normal --[[ vec3 ]])

-- Construct a plane by specifying a point on the plane and its normal
planeNormal,planeOffset = plane.fromPointNormal(point --[[ vec3 ]], normal --[[ vec3 ]])

-- Construct a plane by specifying three points on the plane
planeNormal,planeOffset = plane.fromPoints(a --[[ vec3 ]], b --[[ vec3 ]], c --[[ vec3 ]])
```

### plane.isDegenerate

```lua
-- Test whether the plane is degenerate, i.e., not finite or if the normal
-- vector is null (length is zero, see glm::isNull)
bool = plane.isDegenerate(...)
```

### plane.isParallel

```lua
-- Return true if two planes are parallel (up to a given epsilon)
bool = plane.isParallel(..., otherNormal --[[ vec3 ]], otherOffset --[[ number ]], epsilon --[[ number ]])
```

### plane.areOnSameSide

```lua
-- Return true if two points are on the same side of the plane
bool = plane.areOnSameSide(..., point --[[ vec3 ]], other --[[ vec3 ]])
```

### plane.examineSide

```lua
-- Triangle/Plane intersection test. Returning
--   1 - If the triangle is completely in the positive half-space of the plane;
--  -1 - If the triangle is completely in the negative half-space of the plane;
--   0 - If the triangle intersects the plane.
result --[[ int ]] = plane.examineSide(..., ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]], epsilon --[[ number ]])
```

### plane.isInPositiveDirection

```lua
-- Tests if the given direction vector points towards the positive side of the
-- plane
bool = plane.isInPositiveDirection(..., direction --[[ vec3 ]])
```

### plane.isOnPositiveSide

```lua
-- Tests if the given point lies on the positive side of this plane
plane.isOnPositiveSide(..., point --[[ vec3 ]])
```

### plane.passesThroughOrigin

```lua
-- Return true if the plane contains/passes-through the origin (i.e., 0, 0, 0)
bool = plane.passesThroughOrigin(...)
```

### plane.angle

```lua
-- Compute the angle (radians) of intersection between two planes
number = plane.angle(..., otherNormal --[[ vec3 ]], otherOffset --[[ number ]])
```

### plane.reverseNormal

```lua
-- Reverse the direction of the plane normal, while still representing the same
-- set of points
planeNormal --[[ vec3 ]], planeOffset --[[ number ]] = plane.reverseNormal(...)
```

### plane.pointOnPlane

```lua
-- Returns a point on this plane.
--
-- The returned point has the property that the line passing through "it" and
-- (0,0,0) is perpendicular to the plane.
vec3 = plane.pointOnPlane(...)
```

### plane.point

```lua
-- Return a point on the plane at the given parameterized (u, v) coordinates
vec3 = plane.point(... u --[[ number ]], v --[[ number ]])

-- Reference: A point that defines an origin for the returned points.
vec3 = plane.point(... u --[[ number ]], v --[[ number ]], reference --[[ vec3 ]])
```

### plane.refract

```lua
-- Refract the given incident vector along the plane
-- negativeSideRefraction: Refraction index of material exiting,
-- positiveSideRefraction: Refraction index of material entering.
vec3 = plane.refract(..., incident --[[[ vec3 ]], negativeSideRefraction --[[ number ]], positiveSideRefraction --[[ number ]])
```

### plane.project

```lua
-- Orthographically projects the given object onto the plane
vec3 = plane.project(..., point --[[ vec3 ]])
linePos --[[ vec3 ]], lineDir --[[ vec3 ]] = plane.projectLine(..., linePos --[[ vec3 ]], lineDir --[[ vec3 ]])
segStart --[[ vec3 ]], segEnd --[[ vec3 ]] = plane.projectSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
rayPos --[[ vec3 ]], rayDir --[[ vec3 ]] = plane.projectRay(..., rayPos --[[ vec3 ]], rayDir --[[ vec3 ]])
ta,tb,tc --[[ vec3 ]] = plane.projectTriangle(..., ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]])
```

### plane.projectToNegativeHalf

```lua
-- Projects the given point to the negative halfspace of the plane
vec3 = plane.projectToNegativeHalf(...)
```

### plane.projectToPositiveHalf

```lua
-- Projects the given point to the positive halfspace of the plane
vec3 = plane.projectToPositiveHalf(...)
```

### plane.orthoProjection

```lua
-- Return an affine transformation matrix that projects orthographically onto
-- the plane
matrix4x3 = plane.orthoProjection(...)
```

### plane.mirrorMatrix

```lua
-- Returns a transformation matrix that mirrors objects along the plane
matrix4x3 = plane.mirrorMatrix(...)
```

### plane.mirror

```lua
-- Mirrors the given point with respect to the plane
vec3 = plane.mirror(..., point --[[ vec3 ]])
```

### plane.clipLine

```lua
-- Clips a line against the plane, i.e., remove part of the line that lies in
-- the negative halfspace of the plane. Returning:
--   0 - If clipping removes the entire line (the line lies entirely in the
--       negative halfspace),
--   1 - If clipping results in a ray (clipped at the point of intersection),
--   2 - If clipping keeps the entire line (the line lies entirely in the
--       positive halfspace)
result --[[ int ]], v1 --[[ vec3 ]], v2 --[[ vec3 ]] = plane.clipLine(...,
    linePos --[[ vec3 ]],
    lineDir --[[ vec3 ]]
)
```

### plane.clipSegment

```lua
-- Clips a line segment against the plane, i.e., remove part of the line that
-- lies in the negative halfspace of the plane
segStart --[[ vec3 ]], segEnd --[[ vec3 ]] = plane.clipSegment(...,
    segStart --[[ vec3 ]],
    segEnd --[[ vec3 ]]
)
```

### plane.clipTriangle

```lua
-- Clip a triangle against the plane, i.e., create one or more triangles that
-- reside strictly in the positive/negative halfspaces of the plane.
pa,pb,pc --[[ vec3 ]], na,nb,nc --[[ vec3 ]] = plane.clipTriangle(...,
    ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]]
)
```

### plane.closest

```lua
-- Computes the closest point on this plane to the given object
vec3 = plane.closestPointRay(..., rayPos --[[ vec3 ]], rayDir --[[ vec3 ]])
vec3 = plane.closestPointSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
```

### plane.distance

```lua
-- Returns the absolute distance of this plane to the given object
number = plane.distance(..., point --[[ vec3 ]])
number = plane.distanceSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
number = plane.distanceSphere(..., spherePos --[[ vec3 ]], sphereRad --[[ number ]])
```

### plane.signedDistance

```lua
-- Returns the signed distance of this plane to the given point, with a negative
-- distance corresponding to the object lying within the negative halfspace of
-- the plane
number = plane.signedDistance(..., point --[[ vec3 ]])
number = plane.signedDistanceLine(..., linePos --[[ vec3 ]], lineDir --[[ vec3 ]])
number = plane.signedDistanceSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
number = plane.signedDistanceRay(..., rayPos --[[ vec3 ]], rayDir --[[ vec3 ]])
number = plane.signedDistanceAABB(..., aabbMin --[[ vec3 ]], aabbMax --[[ vec3 ]])
number = plane.signedDistanceSphere(..., spherePos --[[ vec3 ]], sphereRad --[[ number ]])
number = plane.signedDistanceTriangle(..., ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]])
```

### plane.contains

```lua
-- Tests if this plane contains the given object(s)
bool = plane.contains(..., point --[[ vec3 ]])
bool = plane.containsLine(..., linePos --[[ vec3 ]], lineDir --[[ vec3 ]])
bool = plane.containsRay(..., rayPos --[[ vec3 ]], rayDir --[[ vec3 ]])
bool = plane.containsSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
bool = plane.containsTriangle(..., ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]])
```

### plane.intersects

```lua
-- Tests whether the plane and the given object intersect
bool,dist --[[ number ]] = plane.intersectsRay(..., rayPos --[[ vec3 ]], rayDir --[[ vec3 ]])
bool,dist --[[ number ]] = plane.intersectsLine(..., linePos --[[ vec3 ]], lineDir --[[ vec3 ]])
bool,dist --[[ number ]] = plane.intersectsSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
bool = plane.intersectsSphere(..., spherePos --[[ vec3 ]], sphereRad --[[ number ]])
bool = plane.intersectsAABB(..., aabbMin --[[ vec3 ]], aabbMax --[[ vec3 ]])
bool = plane.intersectsTriangle(..., ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]])
```

## Polygon

A two-dimensional closed surface in three dimensional space. A **well-formed**
polygon is planar, i.e., all vertices lie on the same plane. Much of the
polygon API assumes the polygon is well-formed and for all practical purposes,
non-planar polygons should be avoided. Note the polygon metatable and the
`lua-glm` polygon library are the same table.

All operators (non-constructors) are of the form:

```lua
-- polygon: A userdata that collects points on a plane
result = polygon.function(poly --[[ userdata ]], ...)
```

### polygon.new

```lua
-- Create a new polygon from an array of points
polygon --[[ userdata ]] = polygon.new(points --[[ table ]])
```

### polygon.\_\_call

```lua
-- Convert a polygon into an array of vector values
table = polygon.__call(...)
```

### polygon.\_\_gc

```lua
-- Garbage collect an allocated polygon userdata
polygon.__gc(...)
```

### polygon.\_\_len

```lua
-- Return the number of points within a polygon
integer = polygon.__len(...)
```

### polygon.\_\_index

```lua
-- Return a point along the polygon:
--   1 <= index and index <= polygon.__length(poly)
vector = polygon.__index(..., index --[[ integer ]])

-- Return the value associated to some "key" in the polygon library
value = polygon.__index(..., key --[[ string ]])
```

### polygon.\_\_newindex

```lua
-- index == __length(poly) + 1: Append a point to the polygon
-- index >= 1 && index <= __length(poly): Set/replace the point on the polygon
polygon.__newindex(..., index --[[ integer ]], point --[[ vec3 ]])
```

### polygon.\_\_pairs

```lua
-- __pairs metamethod
iterator --[[ function ]], polygon --[[ userdata ]], initialValue --[[ integer ]] = polygon.__pairs(...)
```

### polygon.operator\_negate

```lua
-- Negate all vertices in the polygon
polygon --[[ userdata ]] = polygon.operator_negate(...)
polygon --[[ userdata ]] = polygon.__unm(...)
```

### polygon.operator\_add

```lua
-- Add an offset to all vertices
polygon --[[ userdata ]] = polygon.operator_add(..., offset --[[ vec3 ]])
polygon --[[ userdata ]] = polygon.__add(..., offset --[[ vec3 ]])
```

### polygon.operator\_sub

```lua
-- Subtract an offset to from vertices
polygon --[[ userdata ]] = polygon.operator_sub(..., offset --[[ vec3 ]])
polygon --[[ userdata ]] = polygon.__sub(..., offset --[[ vec3 ]])
```

### polygon.operator\_mul

```lua
polygon --[[ userdata ]] = polygon.operator_mul(matrix3x3, ...)
polygon --[[ userdata ]] = polygon.operator_mul(matrix4x3, ...)
polygon --[[ userdata ]] = polygon.operator_mul(matrix4x4, ...)
polygon --[[ userdata ]] = polygon.operator_mul(quat, ...)

polygon --[[ userdata ]] = polygon.__mul(quat, ...)
```

### polygon.equal

```lua
bool = polygon.equal(..., otherPoly --[[ userdata ]])
bool = polygon.equal(..., otherPoly --[[ userdata ]], eps --[[ number ]])
bool = polygon.equal(..., otherPoly --[[ userdata ]], eps --[[ vec3 ]])
bool = polygon.equal(..., otherPoly --[[ userdata ]], ULPs --[[ integer ]])

-- explicit operator==(Polygon, Polygon)
bool = polygon.operator_equals(..., otherPoly --[[ userdata ]])

-- Metamethod
bool = polygon.__eq(..., otherPoly --[[ userdata ]])
```

### polygon.edge

```lua
-- Return a line segment between two adjacent vertices of the polygon
segStart --[[ vec3 ]], segEnd --[[ vec3 ]] = polygon.edge(..., index --[[ integer ]])
```

### polygon.edge2d

```lua
-- Return a line segment between two adjacent vertices of the polygon, in the
-- local space of the polygon
segStart --[[ vec3 ]], segEnd --[[ vec3 ]] = polygon.edge2d(..., index --[[ integer ]])
```

### polygon.diagonal

```lua
--  Returns the diagonal (segment) that joins the two given vertices of the
-- polygon. If |i - j| == 1, then an edge of the polygon is returned
segStart --[[ vec3 ]], segEnd --[[ vec3 ]] = polygon.diagonal(...,
    i --[[ integer ]], j --[[ integer ]]
)
```

### polygon.diagonalExists

```lua
-- Tests whether the diagonal that joins the two given vertices lie inside the
-- polygon and is not intersected by edges of the polygon
bool = polygon.diagonalExists(..., i --[[ integer ]], j --[[ integer ]])
```

### polygon.basisU

```lua
-- Generates the U-vector (i.e., local space "x" axis) of the polygon
vec3 = polygon.basisU(...)
```

### polygon.basisV

```lua
-- Generates the V-vector (i.e., local-space "y" axis) of the polygon
vec3 = polygon.basisV(...)
```

### polygon.mapTo2D

```lua
-- Maps the given (world) space point to the local 2D space of the polygon
vec2 = polygon.mapTo2D(..., point --[[ vec3 ]])
```

### polygon.mapFrom2D

```lua
-- Map the given local 2D space coordinate to a 3D point world space coordinate
vec3 = polygon.mapFrom2D(..., point --[[ vec2 ]])
```

### polygon.area

```lua
-- Return the surface area of the polygon
number = polygon.area(...)
```

### polygon.perimeter

```lua
-- Return the total edge length of the polygon
number = polygon.perimeter(...)
```

### polygon.centroid

```lua
-- Return the center of mass of the polygon
vec3 = polygon.centroid(...)
```

### polygon.isPlanar

```lua
-- Tests if the polygon is planar, i.e., all of its vertices lie on the same
-- plane
bool = polygon.isPlanar(...)
```

### polygon.isSimple

```lua
-- Tests if the polygon is simple, i.e., no two non-consecutive edges have a
-- point in common
bool = polygon.isSimple(...)
```

### polygon.isNull

```lua
-- Tests if the polygon is null, i.e., has no vertices
bool = polygon.isNull(...)
```

### polygon.isfinite

```lua
-- Test if all vertices of the polygon are finite
bool = polygon.isfinite(...)
```

### polygon.isDegenerate

```lua
-- Return true if the polygon is degenerate:
--   1. It has two-or-less vertices
--   2. its surface area is less or equal than a given epsilon
bool = polygon.isDegenerate(..., epsilon --[[ number ]])
```

### polygon.isConvex

```lua
-- Tests whether the polygon is convex, i.e., for each pair of points inside
-- the polygon, the segment joining those points is also completely inside the
-- polygon
bool = polygon.isConvex(...)
```

### polygon.extremePoint

```lua
-- Compute an extreme point along the polygon, i.e., the furthest point in a
-- given direction
point --[[ vec3 ]], distance --[[ number ]] = polygon.extremePoint(..., vec3 --[[ direction ]])
```

### polygon.projectToAxis

```lua
-- Project the polygon onto the provided axis
parametricMin --[[ number ]], parametricMax --[[ number ]] = polygon.projectToAxis(, axis --[[ vec3 ]])
```

### polygon.planeCCW

```lua
-- Computes the plane the polygon is contained in.
--
-- The normal vector of the plane points to the direction from which the
-- vertices wind in counter-clockwise order
planeNormal --[[ vec3 ]], planeOffset --[[ number ]] = polygon.planeCCW(...)
```

### polygon.normalCCW

```lua
-- Compute the normal of the polygon in the counter-clockwise direction
planeNormal --[[ vec3 ]] = polygon.normalCCW(...)
```

### polygon.planeCW

```lua
-- Computes the plane the polygon is contained in.
--
-- The normal vector of the plane points to the direction from which the
-- vertices wind in clockwise order
planeNormal --[[ vec3 ]], planeOffset --[[ number ]] = polygon.planeCW(...)
```

### polygon.normalCW

```lua
-- Compute the normal of the polygon in the clockwise direction
planeNormal --[[ vec3 ]] = polygon.normalCW(..., )
```

### polygon.pointOnEdge

```lua
-- Computes a point on the perimeter of this polygon.
--
-- "dist" is a value between [0, 1] corresponding to a relative location along
-- the polygons perimeter
vec3 = polygon.pointOnEdge(..., dist --[[ number ]])
```

### polygon.edgeNormal

```lua
-- Return the normal vector of the given edge, i.e., the vector perpendicular to
-- the plane the polygon lies in
planeNormal --[[ vec3 ]] = polygon.edgeNormal(..., index --[[ integer ]])
```

### polygon.edgePlane

```lua
-- Return the normal plane of the given edge
planeNormal --[[ vec3 ]], planeOffset --[[ number ]] = polygon.edgePlane(..., index --[[ integer ]])
```

### polygon.minimalEnclosingAABB

```lua
-- Return the smallest AABB that encloses the polygon
aabbMin --[[ vec3 ]], aabbMax --[[ vec3 ]] = polygon.minimalEnclosingAABB(...)
```

### polygon.contains

```lua
-- Tests if the given object (worldspace) are fully contained inside the polygon.
-- The approach used by this function is the Crossings Test.
--
-- Given that a polygon is a 2D object in 3D space, a "thickness" value can be
-- used to describe a height/length in the third dimension. Defaulting to an
-- "epsilon" to handle floating-point inaccuracies.
--
-- Thickness emanates from the plane: 0.5 * thickness in each half-space.
bool = polygon.contains(..., point --[[ vec3 ]], thickness --[[ number ]])
bool = polygon.containsPolygon(..., otherPolygon --[[ userdata ]])
bool = polygon.containsSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]], thickness --[[ number ]]))
bool = polygon.containsTriangle(..., ta --[[ vec3 ]], tb --[[ vec3 ]], tc --[[ vec3 ]], thickness --[[ number ]])

-- Thickness expands only in the positive half-space, emanating from the plane
bool = polygon.containsAbove(..., point --[[ vec3 ]], thickness --[[ number ]])

-- Thickness expands only in the negative half-space, emanating from the plane
bool = polygon.containsBelow(..., point --[[ vec3 ]], thickness --[[ number ]])

-- Tests if the given line segment, expressed in parametric/local coordinates,
-- is fully contained inside the polygon
bool = polygon.containsSegment2D(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
```

### polygon.intersects

```lua
-- Tests whether the polygon and the given object(s) intersect
bool = polygon.intersectsLine(..., linePos --[[ vec3 ]], lineDir --[[ vec3 ]])
bool = polygon.intersectsRay(..., rayPos --[[ vec3 ]], rayDir --[[ vec3 ]])
bool = polygon.intersectsSegment(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
bool = polygon.intersectsPlane(..., planeNormal --[[ vec3 ]], planeOffset --[[ number ]])

-- Tests if the given line segment, expressed in parametric/local coordinates,
-- intersects the polygon
bool = polygon.intersectsSegment2D(..., segStart --[[ vec3 ]], segEnd --[[ vec3 ]])
```

# Preprocessor Header Definitions

Preprocessor definitions used to enable/disable bundling specific GLM headers.
All functions are bundled with the `LUAGLM_INCLUDE_ALL` preprocessor flag.
Note, functions of the same, but with different parameterizations, will not be
included iff no header that declares that function is defined. Template
arguments are resolved at call-time when parsing values from the Lua stack.

# Default

* **COMMON_HPP**
* **CONSTANTS_HPP**
* **EXPONENTIAL_HPP**
* **GEOMETRIC_HPP**
* **INTEGER_HPP**
* **MATRIX_HPP**
* **TRIGONOMETRIC_HPP**
* **VECTOR_RELATIONAL_HPP**
* **PACKING_HPP**
* **STD_RANDOM**

## LUAGLM\_INCLUDE\_EXT

* **EXT_MATRIX_CLIP_SPACE_HPP**
* **EXT_MATRIX_COMMON_HPP**
* **EXT_MATRIX_PROJECTION_HPP**
* **EXT_MATRIX_RELATIONAL_HPP**
* **EXT_MATRIX_TRANSFORM_HPP**
* **EXT_QUATERNION_COMMON_HPP**
* **EXT_QUATERNION_EXPONENTIAL_HPP**
* **EXT_QUATERNION_GEOMETRIC_HPP**
* **EXT_QUATERNION_RELATIONAL_HPP**
* **EXT_QUATERNION_TRIGONOMETRIC_HPP**
* **EXT_SCALAR_COMMON_HPP**
* **EXT_SCALAR_CONSTANTS_HPP**
* **EXT_SCALAR_INTEGER_HPP**
* **EXT_SCALAR_RELATIONAL_HPP**
* **EXT_SCALAR_ULP_HPP**
* **EXT_VECTOR_COMMON_HPP**
* **EXT_VECTOR_INTEGER_HPP**
* **EXT_VECTOR_RELATIONAL_HPP**
* **EXT_VECTOR_ULP_HPP**

## LUAGLM\_INCLUDE\_GTC

* **GTC_BITFIELD_HPP**
* **GTC_COLOR_SPACE_HPP**
* **GTC_EPSILON_HPP**
* **GTC_INTEGER_HPP**
* **GTC_MATRIX_ACCESS_HPP**
* **GTC_MATRIX_INVERSE_HPP**
* **GTC_NOISE_HPP**
* **GTC_QUATERNION_HPP**
* **GTC_RANDOM_HPP**
* **GTC_RECIPROCAL_HPP**
* **GTC_ROUND_HPP**
* **GTC_TYPE_PRECISION_HPP**
* **GTC_ULP_HPP**

## LUAGLM\_INCLUDE\_GTX

* **GTX_BIT_HPP**
* **GTX_CLOSEST_POINT_HPP**
* **GTX_COLOR_ENCODING_HPP**
* **GTX_COLOR_SPACE_HPP**
* **GTX_COLOR_SPACE_YCOCG_HPP**
* **GTX_COMMON_HPP**
* **GTX_COMPATIBILITY_HPP**
* **GTX_COMPONENT_WISE_HPP**
* **GTX_EASING_HPP**
* **GTX_EULER_ANGLES_HPP**
* **GTX_EXTEND_HPP**
* **GTX_EXTERIOR_PRODUCT_HPP**
* **GTX_FAST_EXPONENTIAL_HPP**
* **GTX_FAST_SQUARE_ROOT_HPP**
* **GTX_FAST_TRIGONOMETRY_HPP**
* **GTX_FUNCTIONS_HPP**
* **GTX_GRADIENT_PAINT_HPP**
* **GTX_HANDED_COORDINATE_SPACE_HPP**
* **GTX_INTEGER_HPP**
* **GTX_INTERSECT_HPP**
* **GTX_LOG_BASE_HPP**
* **GTX_LOG_BASE_HPP**
* **GTX_MATRIX_CROSS_PRODUCT_HPP**
* **GTX_MATRIX_DECOMPOSE_HPP**
* **GTX_MATRIX_FACTORISATION_HPP**
* **GTX_MATRIX_INTERPOLATION_HPP**
* **GTX_MATRIX_MAJOR_STORAGE_HPP**
* **GTX_MATRIX_OPERATION_HPP**
* **GTX_MATRIX_QUERY_HPP**
* **GTX_MATRIX_TRANSFORM_2D_HPP**
* **GTX_MIXED_PRODUCT_HPP**
* **GTX_NORMALIZE_DOT_HPP**
* **GTX_NORMAL_HPP**
* **GTX_NORM_HPP**
* **GTX_OPTIMUM_POW_HPP**
* **GTX_ORTHONORMALIZE_HPP**
* **GTX_PCA_HPP**
* **GTX_PERPENDICULAR_HPP**
* **GTX_POLAR_COORDINATES_HPP**
* **GTX_PROJECTION_HPP**
* **GTX_QUATERNION_HPP**
* **GTX_QUATERNION_TRANSFORM_HPP**
* **GTX_RANGE_HPP**
* **GTX_ROTATE_NORMALIZED_AXIS_HPP**
* **GTX_ROTATE_VECTOR_HPP**
* **GTX_SPLINE_HPP**
* **GTX_TEXTURE_HPP**
* **GTX_TRANSFORM2_HPP**
* **GTX_TRANSFORM_HPP**
* **GTX_VECTOR_ANGLE_HPP**
* **GTX_VECTOR_QUERY_HPP**
* **GTX_WRAP_HPP**
