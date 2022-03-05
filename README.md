# LuaGLM

A Lua 5.4.4 runtime providing vector, quaternion, and matrix data types with a
(near-)complete binding to [GLM 0.9.9.9](https://github.com/g-truc/glm), a C++
mathematics library based on the [OpenGL Shading Language
(GLSL)](https://www.opengl.org/registry/doc/GLSLangSpec.4.50.diff.pdf)
specifications.

## Vectors/Quaternions

Vectors and quaternions are basic types (following nil, boolean, number,
string, function, userdata, thread, and tables) and are viewed (syntactically
and semantically) as **immutable** tables of floats that are accessible by keys
`1, 2, 3, 4`, `x, y, z, w`, and `r, g, b, a`:

```lua
-- Generic vector constructor
> v = vec(math.pi, math.exp(1), (1 + math.sqrt(5)) / 2)

-- Vector constructor with explicit length
> v3 = vec3(math.pi, math.exp(1), (1 + math.sqrt(5)) / 2)

-- Access the vectors fields
> v[3] + v.z
3.2360680103302

-- Vector swizzling
> v.xyzx
vec4(3.141593, 2.718282, 1.618034, 3.141593)

-- vectors and quaternions have an explicit type string even though they are
-- internally represented by the same LUA_TVECTOR tag (grit-lua compatibility)
> print(type(v), type(quat(1,0,0,0)))
vector3 quat

-- Test vectors for equality
> v == v3
true

-- Arithmetic operators on vector types
> ((v + v3) * v) - v3
vec3(16.597618, 12.059830, 3.618034)

-- The length operator returns the magnitude of the vector
> #v
4.4583287239075

-- Vector dimensions
> v.n
3

-- Iterate over each component with 'pairs'
> for k,v in pairs(v3) do print(k,v) end
x       3.1415927410126
y       2.7182817459106
z       1.6180340051651

-- Create a quaternion by rotating an axis by 35 degrees
> quat(35.0, vec(1,0,0))
quat(0.953717, {0.300706, 0.000000, 0.000000})

-- Multiply a direction vector by the quaternion
> quat(35.0, vec(1,0,0)) * norm(vec(1,1,1))
vec3(0.577350, 0.141783, 0.804092)

-- Use vectors as table keys
> t = { }
> t[v // 1.0] = "Hello, World!"
> t[v // 1.0]
Hello, World!
```

When vectors/quaternions are accessed by other values/types some additional
rules exist prior to a `__index` metamethod lookup:

1. If a string key has less-than-or-equal-to four characters it is first passed through a swizzling filter. Returning a vector if all characters are valid fields, e.g., `v.zyx == vec3(v.z, v.y, v.x)`.
    * Note: If swizzling a quaternion results in a four dimensional unit vector, the object remains a quaternion.
1. The `angle` and `axis` strings are reserved for the angle (in degrees) and normalized axis of rotation for quaternion types (grit-lua compatibility).
1. The dimensions of a vector/quaternion can be accessed by the `n` and `dim` strings as the length operator returns the vector magnitude (grit-lua compatibility).

Vector and quaternion types do not maintain an explicit metatable reference.
The Lua functions `getmetatable` and `debug.setmetatable` and C API functions
`lua_setmetatable` and `lua_getmetatable` can be used to define explicit
metatables for these types. The binding library (see **Building**) has the
option to override the metatables for vector and matrix types when loaded.

### C API (Vector)

Vector and quaternions values are represented by the `LUA_TVECTOR` tag and are
internally represented using a struct of floats. On an API level, vectors and
quaternions are effectively tables and accessing their values can be done using
the same C API functions:

* [`lua_rawget`](https://www.lua.org/manual/5.4/manual.html#lua_rawget)
* [`lua_rawgeti`](https://www.lua.org/manual/5.4/manual.html#lua_rawgeti)
* [`lua_geti`](https://www.lua.org/manual/5.4/manual.html#lua_geti)
* [`lua_getfield`](https://www.lua.org/manual/5.4/manual.html#lua_getfield)
* [`lua_rawlen`](https://www.lua.org/manual/5.4/manual.html#lua_rawlen): Returns the dimensions of the vector.
* [`lua_len`](https://www.lua.org/manual/5.4/manual.html#lua_len): Pushes the magnitude of the vector at the given index onto the stack (grit-lua compatibility).
* [`lua_next`](https://www.lua.org/manual/5.4/manual.html#lua_next)

See [lglm.hpp](lglm.hpp) the external header for interfacing with ``glm``
defined structures within Lua. The deprecated grit-lua C API can still be
referenced by [lgrit\_lib.h](lgrit_lib.h).

### Metamethods

For performance reasons, most unary and binary operators have internal
implementations that take precedence over any external metamethod.
The following list corresponds to the reserved metamethods for unary and binary
operators when the objects being operated on are either numbers, vectors,
quaternions, or matrices.

* **Number**: TM\_ADD, TM\_SUB, TM\_MUL, TM\_DIV.
* **Vector**:
  + TM\_EQ, TM\_LEN, TM\_ADD, TM\_SUB, TM\_MUL, TM\_MOD, TM\_POW, TM\_DIV,
  + TM\_IDIV, TM\_BAND, TM\_BOR, TM\_BXOR, TM\_SHL, TM\_SHR, TM\_UNM, TM\_BNOT.
* **Quaternion**:
  + TM\_EQ, TM\_LEN, TM\_ADD, TM\_SUB, TM\_MUL, TM\_POW, TM\_DIV, TM\_UNM.
* **Matrix**: TM\_EQ, TM\_LEN, TM\_ADD, TM\_SUB, TM\_MUL, TM\_DIV, TM\_UNM.

## Matrices

Matrices are another added type and represent **mutable** collections of
**column**(-major) vectors that are accessible by keys `1, 2, 3, 4`. They are
**collectible** objects and beholden to the garbage collector.

```lua
-- Create a matrix
> m = mat(vec(1.0, 0.0, 0.0), vec(0.0, 0.819152, 0.573576), vec(0.0, -0.573576, 0.819152))

-- matrices are a basic type
> type(m)
matrix

-- tostring for matrix and vector types invokes glm::to_string
> tostring(m)
mat3x3((1.000000, 0.000000, 0.000000), (0.000000, 0.819152, 0.573576), (0.000000, -0.573576, 0.819152))

-- The length operator corresponds to the number of column vectors
> #m
3

-- Access a column component
> m[2]
vec3(0.000000, 0.819152, 0.573576)

-- Iterate over each matrix component
> for k,v in pairs(m) do print(k, v) end
1       vec3(1.000000, 0.000000, 0.000000)
2       vec3(0.000000, 0.819152, 0.573576)
3       vec3(0.000000, -0.573576, 0.819152)

-- Multiply a vector by the given matrix
> m * vec(0,1,0)
vec3(0.000000, 0.819152, 0.573576)

-- Append a column vector
> m[4] = vec3(0)
> m
mat4x3((1.000000, 0.000000, 0.000000), (0.000000, 0.819152, 0.573576), (0.000000, -0.573576, 0.819152), (0.000000, 0.000000, 0.000000))

-- Remove column vectors. Note only m[#m] can be removed at a time
> m[3],m[4] = nil,nil
> m
mat2x3((1.000000, 2.000000, 3.000000), (4.000000, 5.000000, 6.000000))
```

### C API (Matrix)

Matrix objects are represented by the `LUA_TMATRIX` tag. On an API level, they
are effectively tables (more specifically arrays) and accessing/modifying their
components can be done using the same C API functions:

* [`lua_gettable`](https://www.lua.org/manual/5.4/manual.html#lua_gettable), [`lua_settable`](https://www.lua.org/manual/5.4/manual.html#lua_settable)
* [`lua_geti`](https://www.lua.org/manual/5.4/manual.html#lua_geti), [`lua_seti`](https://www.lua.org/manual/5.4/manual.html#lua_seti)
* [`lua_rawget`](https://www.lua.org/manual/5.4/manual.html#lua_rawget), [`lua_rawset`](https://www.lua.org/manual/5.4/manual.html#lua_rawset)
* [`lua_rawgeti`](https://www.lua.org/manual/5.4/manual.html#lua_rawgeti), [`lua_rawseti`](https://www.lua.org/manual/5.4/manual.html#lua_rawseti).
* [`lua_rawlen`](https://www.lua.org/manual/5.4/manual.html#lua_rawlen), [`lua_len`](https://www.lua.org/manual/5.4/manual.html#lua_len): Returns the dimensions of the matrix (number of columns).
* [`lua_next`](https://www.lua.org/manual/5.4/manual.html#lua_next)

Following vectors, matrix types do not maintain an explicit metatable
reference. See [lglm.hpp](lglm.hpp) the external header for interfacing with
``glm`` defined matrices within Lua.

## Binding Library

Each function within the [GLM
API](https://glm.g-truc.net/0.9.9/api/modules.html) has an equivalent Lua
[library function](libs/glm-binding) of the same name whose template arguments
are resolved at call-time when parsing values from the Lua stack. For example:

```lua
-- Creates a matrix with four vector components (recall GLM is column-major)
> m = glm.axisAngleMatrix(vec3(1.0, 0.0, 0.0), glm.radians(35.0))

-- Iterate over each matrix component
> for k,v in pairs(m) do print(k, v) end
1       vec4(1.000000, 0.000000, 0.000000, 0.000000)
2       vec4(0.000000, 0.819152, 0.573576, 0.000000)
3       vec4(0.000000, -0.573576, 0.819152, 0.000000)
4       vec4(0.000000, 0.000000, 0.000000, 1.000000)

-- Note: getmetatable(m).__index == glm

-- Convert the matrix to a quaternion
> m:toQuat()
quat(0.953717, {0.300706, 0.000000, 0.000000})

-- Apply a rotation to the matrix (note, there are nine variations of glm::rotate)
m2 = m:rotate(math.rad(35.0), vec3(1, 0, 0))

-- Extract XZY Angles
> glm.degrees(vec(m2:extractEulerAngleXZY()))
vec3(70.000000, -0.000000, 0.000000)

-- spherical linear interpolation of the matrices as quaternions
> glm.slerp(m:toQuat(), m2:toQuat(), 2/glm.pi)
quat(0.877641, {0.479318, 0.000000, 0.000000})

-- Example ported from https://github.com/g-truc/glm/blob/master/readme.md
function camera(Translate, Rotate)
    local Projection = glm.perspective(glm.pi * 0.25, 4.0 / 3.0, 0.1, 100.0)
    local View = glm.translate(glm.mat4(1.0), glm.vec3(0.0, 0.0, -Translate))
    View = glm.rotate(View, Rotate.y, glm.vec3(-1.0, 0.0, 0.0))
    View = glm.rotate(View, Rotate.x, glm.vec3(0.0, 1.0, 0.0))
    local Model = glm.scale(glm.mat4(1.0), glm.vec3(0.5))
    return Projection * View * Model
end
```

The binding library also extends GLM to:

1. Add vector support for all functions declared in [cmath(C99/C++11)](http://www.cplusplus.com/reference/cmath/).
1. Support C++11 [Pseudo-random number generation](https://en.cppreference.com/w/cpp/numeric/random) while [backed](https://en.cppreference.com/w/cpp/named_req/UniformRandomBitGenerator) by Lua's xoshiro256\*\* implementation.
1. Alias (e.g., length vs. magnitude), emulate, and port useful and common functions from other popular vector-math libraries (listed in **Sources & Acknowledgments**).
1. Be a complete superset of Lua's [lmathlib](https://www.lua.org/manual/5.4/manual.html#6.7). Meaning `_G.math` can be replaced by the binding library without compatibility concerns. Note, `math.random` and `math.randomseed` are copied from `lmathlib` when the library is loaded rather than extending/maintaining another pseudorandom-state (see `glm/gtc/random.hpp`).

See [EXTENDED.md](EXTENDED.md) for a list of additional functions.

### Casting Rules

As vector/quaternion types are represented by float-point values, some
additional type-inference rules are required when casting values to and from
floating point values (see sections 4.6-4.9 in your favorite `ISO/IEC 14882`
document):

1. A `glm::vec<1, ...>` structure is represented by `lua_Integer`, `lua_Number`, or `bool` Lua value and all `glm::vec<1, ...>` bindings are templated to those Lua types.
1. All other `glm::vec` structures are float-casted (and/or bound to float-template functions). Consequently, bitfield and integer operations, e.g., [packUnorm](http://glm.g-truc.net/0.9.9/api/a00716.html#gaccd3f27e6ba5163eb7aa9bc8ff96251a) and [floatBitsToInt](http://glm.g-truc.net/0.9.9/api/a00662.html#ga99f7d62f78ac5ea3b49bae715c9488ed), are considered unsafe when operating on multi-dimensional vectors (consider inexact IEEE754).
1. Matrices are represented by a collection of column-vectors that abide by the vector rules above. Prior to GLM 0.9.9.9, there has been little practical use for integer/bool matrix templates given the lack of an API.

### Geometry API

Many of the geometric structures developed for
[MathGeoLib](https://github.com/juj/MathGeoLib/tree/master/src/Geometry) have
been ported to [GLM](https://github.com/g-truc/glm). In turn these structures
are bound to `lua-glm`, providing functional libraries for **AABB**, **Line**,
**Ray**, **Segment**, **Sphere**, **Plane**, and **Polygon**. Each geometrical
structure is stored as a sub-library within the base binding library. For
example:

```lua
-- AABB/Line Raycast
rayOrigin = vec3(-5, -5, -5)
rayDirection = glm.norm(vec3(1, 1, 1))
aabbMin = vec3(-2, -2, -2)
aabbMax = vec3(2, 2, 2)
intersects,tNear,tFar = glm.ray.intersectAABB(rayOrigin, rayDirection, aabbMin, aabbMax)
if intersects then
    enter_point = glm.ray.getPoint(rayOrigin, rayDirection, tNear)
    exit_point = glm.ray.getPoint(rayOrigin, rayDirection, tFar)
    print(("Enters: %s"):format(glm.to_string(enter_point)))
    print(("Exits: %s"):format(glm.to_string(exit_point)))
end

-- Structures may also be rotated
rayOrigin,rayDirection = glm.ray.operator_mul(m, rayOrigin, rayDirection)
```

Polygons (sequences of coplanar points) are represented by a full userdata type:

```lua
p = glm.polygon.new({
  vec3(1809.6550,2611.9644,44.0),
  vec3(1809.8136,2620.5571,44.0),
  -- ...
  vec3(1819.0066,2591.5283,44.0),
  vec3(1818.5493,2612.0737,44.0),
})

> type(p)
userdata
```

See **EXTENDED.md** for the full list of functions.

#### Implementation Details

Modules/functions not bound to LuaGLM due to usefulness or complexity:

* glm/detail/func\_common.hpp: `step(T edge, vec<L, T, Q> const& x)` TODO.
* glm/detail/\_swizzle\_func.hpp: Unnecessary bloat.
* glm/gtx/associated\_min\_max.hpp: `(T x, vec<L, U, Q> const& a, ...)` not supported.
* glm/gtx/range.hpp: `begin`, `end`: useless in Lua context.
* glm/gtx/bit.hpp: `powerOfTwoAbove`, `powerOfTwoBelow`, `powerOfTwoNearest`: deprecated, use GTC\_ROUND\_HPP instead.
* glm/ext/vector\_relational.hpp: `equal(..., vec<L, int, Q> const& ULPs)`, as the current Lua binding cannot differentiate between it and `(..., vec<L, T, Q> const& epsilon)`.
* glm/gtx/pca.hpp: `sortEigenvalues`: Function incorrectly declared and manipulates the parameters in place.

The general rule is that the result of a non-void function is placed first onto
the Lua stack prior to any non-const passed-by-reference parameters. However,
to be compatible with
[Lua](https://www.lua.org/manual/5.4/manual.html#pdf-math.modf), `glm::modf(x)`
returns the integral part of x and then the fractional part of x.

## Power Patches

This runtime [imports](http://lua-users.org/wiki/LuaPowerPatches) many (small)
useful changes to the Lua runtime, all bound to preprocessor flags:

### Compound Operators

Add ``+=, -=, *=, /=, <<=, >>=, &=, |=, ^=``, and ..= to the language. The
increment and decrement operators (``++, --``) have not been implemented due to
one of those operators being reserved.

### Safe Navigation

An indexing operation that suppresses errors on accesses into undefined tables
(similar to the safe-navigation operators in C#, Kotlin, etc.), e.g.,

```lua
t?.x?.y == nil
```

### In Unpacking

Support for unpacking named values from tables using the ``in`` keyword, e.g,

```lua
local a,b,c in t
```

is functionally equivalent to:

```lua
local a,b,c = t.a,t.b,t.c
```

### Set Constructors

Syntactic sugar to improve the syntax for specifying sets, i.e.,

```lua
t = { .a, .b }
```

is functionally equivalent to:

```lua
t = { a = true, b = true }
```

### C-Style Comments

Support for C-style block comments: ``/* Comment */``, e.g.,

```lua
print("Hello, World!") /* Comment */
```

### Compile Time Jenkins' Hashes

String literals wrapped in back-ticks are Jenkins' one-at-a-time hashed when
parsed.

```lua
> `Hello, World!`
1395890823
```

For runtime hashing, the `joaat` function is included in the base library:

```lua
-- joaat(input [, ignore_casing]): Compute the Jenkins hash of the input string.
-- If 'ignore_casing' is true, the byte data is hashed as is. Otherwise, each
-- ASCII character is tolower'd prior to hashing.
> joaat("Hello, World!")
1395890823
```

```lua
> joaat("CPed")
2491553369
```

### Short Function Notation

Syntactic sugar for writing concise anonymous functions of the form `|a, b,
...| expr`. Where `expr` is any expression equivalent to `function(a, b, ...)
return expr end`. For example,

```lua
> f = |x| x^2 - 1 -- function(x) return x^2+1 end

> f(2)
3.0

> f(vec3(1, 2, 3))
vec3(0.000000, 3.000000, 8.000000)

-- 'hexadump' from lua-MessagePack.lua
> hexadump = |s| s:gsub('.', |c| string.format('%02X ', c:byte()))

> hexadump("\221\255\255\255\255Z")
DD FF FF FF FF 5A
```

### \_\_ipairs

Reintroduce compatibility for the ``__ipairs`` metamethod that was deprecated
in 5.3 and removed in 5.4.

### Defer

Import the defer statement from
[Ravi](https://github.com/dibyendumajumdar/ravi/tree/master/patches) into the
runtime. In addition `func2close` from ltests.h has been imported into the base
library.

```lua
-- closing function. Could also be used to supply a to-be-closed variable to a
-- generic for loop
defer numopen = numopen - 1 end
```

### Each Iteration

In Lua 5.4, a generic 'for' loop starts by evaluating its explist to produce
four values: an iterator function, a state, an initial value for the control
variable, and a closing (to-be-closed) value. However, the \_\_pairs metamethod
does not support the optional to-be-closed variable.

This extension introduces a ``__iter`` metamethod to allow ``next, t, nil,
to-be-closed`` forms of iteration bound to a metamethod. To iterate over such a
table, only the table needs to be supplied as an argument to a loop, e.g.,

```lua
t = {1,2,3}
for k,v in each(t) do print(k, v) end
```

which defaults to a ``pairs`` implementation that supports a fourth return
variable (to-be-closed) when no `__iter` metamethod exists. ... This patch is
inspired by the Self-iterating Objects patch; see commit logs for reasoning
behind deviation.

### String Blobs

Introduce a `LUA_TSTRING` variant that is effectively a `LUA_VLNGSTR` but
without the hash caching mechanism. Values of this variant are stored in tables
by reference.

```lua
-- Create a string blob of specified length
blob = string.blob(512)

-- Create a blob variant of another string
blob = string.blob(string.rep('\0', 80))

-- Returns a string blob containing the values v1, v2, etc. serialized in binary
-- form (packed at an optional offset) according to the format string fmt.
--
-- Unlike string.pack, this function attempts to write the resulting
-- serialization onto the provided blob (at an optional offset). If the blob is
-- not of sufficient size, a (byte-)copy of the blob is made and returned.
_blob = string.blob_pack(blob, pos --[[ optional ]], fmt, v1, v2, ···)

-- string.unpack where the "fmt" and "s" parameters have swapped order. It is
-- possible to still use string.unpack on blobs. This function exists for
-- API consistency.
... = string.blob_unpack(blob, pos --[[ optional ]], fmt)
```

With included C API functions:

```c
/*
** Creates a string blob of at least "len" bytes, pushing the zero-terminated
** blob onto the stack. Returning a pointer to that blob inside the Lua state.
*/
const char *lua_pushblob(lua_State *L, size_t len);

/*
** Converts the (explicit) string at the given index to a blob. If the string
** value is not already a blob, then lua_tostringblob changes the actual value
** in the stack to a blob (same lua_tolstring caveats apply).
*/
const char *lua_tostringblob(lua_State *L, int idx, size_t *len);
```

A
[DataView](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView)
API that interfaces with Lua's built in facilities, e.g., string.pack,
string.unpack, and table.concat, is located
[here](libs/scripts/examples/dataview.lua).

The intent is to allow byte data to still be beholden to the garbage collector
while not requiring the allocation of intermediate data when going to and from
the Lua API (unsafe caveats apply).

### Extended API

Expose ``lua_createtable`` and API functions common to other custom Lua runtimes.

```lua
-- Creates a new empty table
-- narr: a hint for how many elements the table will have as a sequence.
-- nrec: a hint for how many other elements the table will have.
t = table.create(narr[, nrec])

-- Restore the table to its initial value (removing its contents) while
-- retaining its internal pointer;
t = table.wipe(t)

-- Request the removal of unused capacity in the given table (shrink_to_fit).
t = table.compact(t)

-- An efficient (implemented using memcpy) table shallow-copy implementation;
t2 = table.clone(t)

-- Return the type of table being used. Note, this function only measures the
-- size of the "array part" of a Lua table and the "root" node of its
-- "hash part". Once an "array" becomes "mixed", or if a table has all of
-- values nil'd out, the table.type will remain "mixed" or "hash".
label = table.type(t) -- "empty", "array", "hash", or "mixed"

-- Joins strings together with a delimiter;
str = string.join(delimiter [, string, ...])

-- Trim characters off the beginning and end of a string;
str = string.trim(input [, chars])

-- Returns a concatenation of all number/string arguments passed;
str = string.concat(...)

-- Splits a string using a delimiter (optionally: into a specified number of pieces);
... = string.split(delimiter, string [, pieces]])

-- string.split that places results into a table.
array = string.splittable(delimiter, string [, pieces]])

-- Converts all arguments to strings;
... = string.tostringall(...)

-- Alias: Returns the number of UTF-8 characters in string s;
utf8.strlenutf8 = utf8.len

-- String comparison accounting for UTF-8 chars. Returning a negative integer,
-- zero, or a positive integer if the first string is less than, equal to, or
-- greater than the second string;
result = utf8.strcmputf8i(stringLH, stringRH)

--- Return all arguments with non-number/boolean/string values changed to nil;
... = scrub(...)

```

### Readonly

Introduce the ability to make a table read-only and prohibit any modifications
to the table.

```lua
-- Mark a table as readonly.
--
-- This behavior is 'shallow', i.e., non-frozen tables stored within 't' are
-- still mutable.
--
-- Frozen tables respect the '__newindex' metamethod. However, any attempt to
-- modify the table by that method (e.g., __newindex = rawset) will lead to an
-- error being thrown.
--
-- Finally, tables with 'protected' metatables, i.e., a '__metatable' field,
-- cannot be frozen.
t = table.freeze(t)

-- Return true if the provided table is configured as readonly; false otherwise.
result = table.isfrozen(t)
```

### Readline History

Keep a persistent list of commands that have been run on the Lua interpreter.
With the `LUA_HISTORY` environment variable used to declare the location
history.

## Building

The Lua core can be compiled as C or as C++ code. All functions required to
integrate GLM into Lua are defined in [lglm\_core.h](lglm_core.h). The mechanism
for aliasing the GLM structures across C/C++ boundaries is provided in
[luaconf.h](luaconf.h) and [lglm.hpp](lglm.hpp). GLM/Lua integration is
implemented in [lglm.cpp](lglm.cpp).

As `lglm.cpp` must be compiled as C++, the preprocessor flag `LUA_C_LINKAGE` is
used to synchronize the calling convention required to link it with the rest of
Lua. Beware of language boundary issues!

### Make

An modified version of the makefile
[bundled](https://www.lua.org/download.html) with release versions of Lua. The
same instructions apply:

```bash
# Ensure the GLM library is initialized
└> git submodule update --init

# Build for the linux platform target, linked with readline;
└> make linux-one

# Compile the glm binding library;
└> make lib-glm

# Run Lua and use Library Preloading to load GLM.
└> ./lua -lglm
```

### CMake

A CMake project that builds the stand-alone interpreter (`lua`), a compiler
(`luac`), the GLM binding library, and shared/static libraries. This project
includes variables for most preprocessor configuration flags supported by Lua
and GLM. See `cmake -LAH` or [cmake-gui](https://cmake.org/runningcmake/) for
the complete list of build options.

```bash
└> git submodule update --init

# Create build directory
└> mkdir -p build ; cd build

# Compile with icc: -DCMAKE_C_COMPILER=icc -DCMAKE_CXX_COMPILER=icpc
# Ensure LD_LIBRARY_PATH contains a reference to: libimf.a
└> cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DONE_LUA=ON ..

# Build
└> make
```

### Lua Preprocessor Configurations

Note, not all Lua-specific options are listed.

* **Compilation**
  + **ONE_LUA**: Compile Lua core, libraries, and interpreter as a single file.
  + **LUA_C_LINKAGE**: An indication to `lglm.cpp` that the Lua core has C linkage.
  + **LUA_NATIVE_ARCH**: Enable compiler optimizations for the native processor architecture.
  + **LUA_NO_DUMP**: Disable the dump module (dumping Lua functions as precompiled chunks).
  + **LUA_NO_BYTECODE**: Disables the usage of lua\_load with binary chunks.
  + **LUA_NO_PARSER**: Compile the Lua core so it does not contain the parsing modules (lcode, llex, lparser). Only binary files and strings, precompiled with luac, can be loaded.
* **Testing**
  + **LUA_INCLUDE_TEST**: Include ltests.h and testing modules. Note this option enables many of the following flags by default.
  + **LUAI_ASSERT**: Turn on all assertions inside Lua.
  + **LUA_USE_APICHECK**: Turns on several consistency checks on the C API.
  + **HARDSTACKTESTS**: Force a reallocation of the stack at every point where the stack can be reallocated.
  + **HARDMEMTESTS**: Force a full collection at all points where the collector can run.
  + **EMERGENCYGCTESTS**: Force an emergency collection at every single allocation.
  + **EXTERNMEMCHECK**: Removes internal consistency checking of blocks being deallocated.
* **LuaGLM Options**:
  + **LUAGLM_EPS_EQUAL**: `luaV_equalobj` uses approximately equal (within glm::epsilon) for vector/matrix types (beware of hashing caveats).
  + **LUAGLM_MUL_DIRECTION**: Define how the runtime handles `TM_MUL(mat4x4, vec3)`.
  + **LUAGLM_NUMBER_TYPE**: Use lua\_Number as the vector primitive; float otherwise.
* **Power Patches**: See Lua Power Patches section.
  + **LUAGLM_COMPAT_IPAIRS**: Enable '\_\_ipairs'.
  + **LUAGLM_EXT_API**: Enable 'Extended API'.
  + **LUAGLM_EXT_BLOB**: Enable 'String Blobs'.
  + **LUAGLM_EXT_CCOMMENT**: Enable 'C-Style Comments'.
  + **LUAGLM_EXT_CHRONO**: Enable nanosecond resolution timers and x86 rdtsc sampling.
  + **LUAGLM_EXT_COMPOUND**: Enable 'Compound Operators'.
  + **LUAGLM_EXT_DEFER**: Enable 'Defer'.
  + **LUAGLM_EXT_EACH**: Enable 'Each Iteration'.
  + **LUAGLM_EXT_INTABLE**:: Enable 'In Unpacking'.
  + **LUAGLM_EXT_JOAAT**: Enable 'Compile Time Jenkins' Hashes'.
  + **LUAGLM_EXT_LAMBDA**: Enable 'Short Function Notation'.
  + **LUAGLM_EXT_READLINE_HISTORY**: Enable 'Readline History'.
  + **LUAGLM_EXT_READONLY**: Enable 'Readonly'
  + **LUAGLM_EXT_SAFENAV**: Enable 'Safe Navigation'.
  + **LUAGLM_EXT_TABINIT**: Enable 'Set Constructors'
  + **LUAGLM_EXT_SCOPE_RESOLUTION**: Replace TK\_DBCOLON tokens with field selection (e.g., emulate C++ scope resolution operator). Note, this invalidates the 'label' rule in the grammar.

#### GLM Preprocessor Configurations

* **GLM_FORCE_MESSAGES**: Platform auto detection and default configuration.
* **GLM_FORCE_INLINE**: Force inline.
* **GLM_FORCE_ALIGNED_GENTYPES**: Force GLM to enable aligned types.
* **GLM_FORCE_DEFAULT_ALIGNED_GENTYPES**: Force GLM to use aligned types by default.
* **GLM_FORCE_INTRINSICS**: Using SIMD optimizations.
* **GLM_FORCE_PRECISION_**: Default precision.
* **GLM_FORCE_SINGLE_ONLY**: Removed explicit 64-bits floating point types.
* **GLM_FORCE_XYZW_ONLY**: Only exposes x, y, z and w components. Note: when enabled disables all **COLOR_SPACE** bindings.
* **GLM_FORCE_LEFT_HANDED**: Force left handed coordinate system.
* **GLM_FORCE_DEPTH_ZERO_TO_ONE**: Force the use of a clip space between 0 to 1.
* **GLM_FORCE_SIZE_T_LENGTH**: Vector and matrix static size type.
* **GLM_FORCE_UNRESTRICTED_GENTYPE**: Removing genType restriction.
* **GLM_FORCE_SILENT_WARNINGS**: Silent C++ warnings from language extensions.
* **GLM_FORCE_QUAT_DATA_WXYZ**: Force GLM to store quat data as w,x,y,z instead of x,y,z,w.

For all GLM preprocessor, see the [GLM manual](https://github.com/g-truc/glm/blob/master/manual.md#section2).

#### Added GLM Preprocessor Configurations

* **GLM_FAST_MATH**: Enable fast math optimizations (see `-ffast-math` caveats).
* **GLM_FORCE_Z_UP**: Unit "up" vector is along the Z-axis (Y-axis otherwise).

#### Binding Library Configuration

* **LUAGLM_INCLUDE_ALL**: Create bindings for all declared GLM headers. To create module-only, extension-only, or header-only bindings see **EXTENDED.md** for a list of all headers.
* **LUAGLM_INCLUDE_EXT**: Include ext headers: Stable extensions not specified by GLSL specification.
* **LUAGLM_INCLUDE_GTC**: Include gtc headers: Recommended extensions not specified by GLSL specification.
* **LUAGLM_INCLUDE_GTX**: Include gtx headers: Experimental extensions not specified by GLSL specification.
* **LUAGLM_INCLUDE_GEOM**: Include support for geometric structures (`ext/geom/`).
* **LUAGLM_BINDING_ALIGNED**: Enable **GLM_FORCE_DEFAULT_ALIGNED_GENTYPES** *only* for the binding library.
* **LUAGLM_ALIASES**: Enable all aliasing (CMake).
* **LUAGLM_ALIASES_SIMPLE**: Include function aliases for common names when registering the library, e.g., length vs. magnitude.
* **LUAGLM_ALIASES_UNITY**: Include Unity Scripting API aliases.
* **LUAGLM_ALIASES_O3DE**: Include O3DE Lua API aliases.
* **LUAGLM_SAFELIB**: Enable a general try/catch wrapper for all binding functions.
* **LUAGLM_TYPE_COERCION**: Enable string-to-number type coercion when parsing arguments from the Lua stack.
* **LUAGLM_RECYCLE**: Treat all trailing and unused values on the Lua stack (but passed as parameters to the `CClosure`) as a 'cache' of recyclable structures.
* **LUAGLM_FORCED_RECYCLE**: Disable this library from allocating memory, i.e., force usage of LUAGLM\_RECYCLE.

Recycling Example:

```lua
-- Some shared matrix
> t = mat(vec(1,1), vec(2,2))

-- When enabled, all arguments after "angle" are recycled.
> m = glm.axisAngleMatrix(vector3(1.0, 0.0, 0.0), math.rad(35.0), t)

-- "t" and "m" reference the same matrix collectible.
> t == m
true
```

#### CRT Allocator

Inspired by `LLVM_INTEGRATED_CRT_ALLOC`, the CMake project includes the ability
to replace the default Windows CRT allocator. Currently, only
[rpmalloc](https://github.com/mjansson/rpmalloc) and
[mimalloc](https://github.com/microsoft/mimalloc) are supported.
Define `-DLUA_CRT_ALLOC="path/to/rpmalloc"`.

## Developer Notes

See [libs/scripts](libs/scripts) for a collection of example/test scripts using
these added features.

### TODO

Ordered by priority.

1. Cleanup documentation and piggyback off GLMs doxygen.
1. Cleanup testing scripts/environment and publish.
1. Half precision vector storage
1. Rewrite build scripts.
1. Update the Benchmarking section.
1. Optimize `glm_createMatrix`. Profiling case '4x4 matrix creation (lua\_Alloc)' is the one of the slowest operations in the added vector/matrix API. Worse when using the default Windows allocator.
1. Improve `lua_CFloatX` struct definitions, i.e., mimic `glm::detail::storage` when anonymous structs are supported.
1. Utility API that resembles `glUniformMatrix*v`-style functions, i.e., extracting/parsing array of matrices/vectors.
1. [ext](libs/glm-binding/ext): Improve SIMD support.
1. [geom](libs/glm-binding/ext/geom): Move to new namespace (library pollutes glm enough).
1. [geom](libs/glm-binding/ext/geom): Configurable epsilon values for the variety of intersection functions (and lingering scale-invariant issues).
1. [geom](libs/glm-binding/ext/geom): Basic support for frustums (both orthographic and perspective) and OBBs.
1. [geom](libs/glm-binding/ext/geom): Support for two-dimensional structures: Ray2D, Line2D, Plane2D.
1. Merge sandboxing changes.
1. Modify implementation to follow updated "Avoid taking the address of a 'TValue' field" (or reference) convention.

### Planned Features

1. One downside to vectors/quaternions being an explicit `Value` is that they increase the minimum Value size to at least 16 bytes. Given that types in Lua are fairly transparent, it may be beneficial to introduce, or at least experiment with, half-precision floating-point formats or a compile-time option to make vector/quaternion types collectible.
1. Support for integer vectors/matrices. Either by introducing an additional type, e.g., `LUA_TVECTORI`, or splitting the vector tag `LUA_TVECTOR` into `LUA_TVECTOR2`, `LUA_TVECTOR3`, `LUA_TVECTOR4`, and `LUA_TQUAT` and use variant bits for the primitive type.
1. Include broad phase collision scripting examples, e.g., dynamic AABB tree and/or multibox sweep-and-prune.
1. A significantly less efficient shared-library implementation, using tables and/or userdata instead of first-class types, for Lua5.1, Lua5.2, Lua5.3, Lua5.4, and [LuaJIT](https://github.com/LuaJIT/LuaJIT).

## Benchmarking

These benchmark values are preliminary and subject to change. Correctness not
guaranteed and no refunds allowed. LuaGLM was compiled with the default
[makefile](./makefile).

### NumPy & PyGLM

[PyGLM vs
NumPy.py](https://github.com/Zuzu-Typ/PyGLM/blob/master/test/PyGLM%20vs%20NumPy.py)
was ported to Lua + LuaGLM. Both were modified to increase the total number of
iterations-per-case to be in the billions. Each value represents the average
throughput in millions of operations per second. Secondary values represent the
operations throughput with object/sink preallocation (see `LUAGLM_RECYCLE`).

| Operation                | NumPy runs | PyGLM runs | LuaGLM runs   |
|--------------------------|------------|------------|---------------|
| vec3 creation            | 10.476     | 19.408     | 57.027        |
| vec3 creation (custom)   | 2.530      | 14.325     | 57.123        |
| dot(vec3)                | 1.933      | 30.340     | 77.995        |
| cross(vec3)              | 0.076      | 29.036     | 78.245        |
| l2norm(vec3)             | 0.538      | 37.832     | 68.900        |
| compadd(vec3)            | 3.457      | 18.094     | 87.579        |
| mat4x4 creation          | 9.481      | 19.573     | 20.033/29.991 |
| mat4x4 identity creation | 0.956      | 23.434     | 30.351/71.164 |
| mat4x4 transposition     | 2.866      | 20.609     | 29.848/72.203 |
| mat4x4 inverse           | 0.236      | 17.441     | 24.281/47.528 |
| mul(mat4x4, mat4x4)      | 3.367      | 11.796     | 29.379/39.614 |
| mul(mat4x4, vec4)        | 1.370      | 15.877     | 71.390        |

### LuaJIT + excessive/CPML

`PyGLM vs NumPy.py` was ported to LuaJIT and
[excessive/CPML](https://github.com/excessive/CPML). Each value represents the
average throughput in millions of operations per second. Secondary values
represent the operations throughput using 'object' preallocation (i.e., the
'out' parameter in many CPML functions).

Note, the JIT compiler may optimize away some of the fairly tight 'profiling'
loops. To compensate for this, each measurement function, i.e., the thing that
invokes each operation repeatedly, is passed through `jit.off(X, true)`. These
results are not a true representation of LuaJITs potential: just FFI and
ancillary in-time optimizations. Real-world comparisons should be preferred
(TBD).

| Operation                | CPML/joff     | CPML\_FFI     | LuaGLM        |
|--------------------------|---------------|---------------|---------------|
| vec3 creation            | 10.851        | 13.314        | 57.027        |
| vec3 creation (custom)   | 8.960         | 8.212         | 57.123        |
| dot(vec3)                | 48.232        | 119.451       | 77.995        |
| cross(vec3)              | 10.230/23.840 | 2.752/105.678 | 78.245        |
| l2norm(vec3)             | 39.737        | 6.581         | 68.900        |
| compadd(vec3)            | 4.482         | 1.675         | 87.579        |
| mat4x4 creation          | 6.224         | 9.128         | 20.033/29.991 |
| mat4x4 identity creation | 5.112/11.756  | 9.367/60.403  | 30.351/71.164 |
| mat4x4 transposition     | 3.219/6.224   | 5.652/39.101  | 29.848/72.203 |
| mat4x4 inverse           | 0.644/0.728   | 4.701/19.800  | 24.281/47.528 |
| mul(mat4x4, mat4x4)      | 0.866/3.427   | 2.966/7.049   | 29.379/39.614 |
| mul(mat4x4, vec4)        | 1.468/9.819   | 1.662/55.578  | 71.390        |

#### + MGL

Modifying the benchmark scripts for
[MGL](https://github.com/ImagicTheCat/MGL#comparisons) to include LuaGLM.
Secondary values represent the benchmarks output with transform recycling
(caching the `ent.transform` object).

| Operation                    | wtime (s)   | utime (s)   | mem (kB)    | ~ ms/tick     | ~ frame % |
|------------------------------|-------------|-------------|-------------|---------------|-----------|
| GLM g++ CPERF+GLM\_FLAGS     | 0.110       | 0.110       | 3648        | 0.092         | 1         |
| GLM g++ -O2                  | 0.430       | 0.430       | 3584        | 0.358         | 2         |
| LuaGLM                       | 1.17/0.92   | 1.14/0.9    | 8932/6576   | 0.975/0.767   | 6/5       |
| CPML LuaJIT                  | 2.4/1.57    | 2.38/1.54   | 10208/5364  | 2/1.308       | 12/8      |
| MGL LuaJIT                   | 14.900      | 14.880      | 12256       | 12.417        | 74        |
| MGL LuaJIT/joff              | 15.190      | 15.150      | 11920       | 12.658        | 76        |
| CPML LuaJIT/joff             | 19.11/18.49 | 19.08/18.46 | 14900/6780  | 15.925/15.408 | 96/92     |
| MGL Lua5.4                   | 40.310      | 40.290      | 9544        | 33.592        | 202       |
| CPML Lua5.4                  | 66.32/61.46 | 66.28/61.43 | 23364/10004 | 55.267/51.217 | 332/307   |

### Benchmark TODO

* Other things not Lua.
* [Unity.Mathematics](https://github.com/Unity-Technologies/Unity.Mathematics)
* Update LuaGLM benchmarks to include unity/jumbo builds.

## Sources & Acknowledgments

1. [grit-lua](https://github.com/grit-engine/grit-lua): Original implementation and inspiration.
1. [MathGeoLib](https://github.com/juj/MathGeoLib/): [Geometry API](libs/glm-binding/geom) is distributed under [Apache License](http://www.apache.org/licenses/LICENSE-2.0.html).
1. [O3DE](https://github.com/o3de/o3de): Reference for Extended API (aliases).
1. [SharpDX](https://github.com/sharpdx/SharpDX): Reference for Extended API (aliases).
1. [Godot Engine](https://docs.godotengine.org/en/stable/classes/): Reference for Extended API (aliases).
1. [Unity](https://docs.unity3d.com/ScriptReference/UnityEngine.CoreModule.html): Reference for Extended API (aliases and emulation of some [mathf](https://docs.unity3d.com/ScriptReference/Mathf.html) functions).

## License

Lua and GLM are distributed under the terms of the [MIT
license](https://opensource.org/licenses/mit-license.html). See the Copyright
Notice in [lua.h](lua.h).
