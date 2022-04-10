# See License in lua.h.
CMAKE_MINIMUM_REQUIRED( VERSION 3.9.0 )

PROJECT( lua VERSION 5.4.4 LANGUAGES C )

INCLUDE(CheckCCompilerFlag)
IF( CMAKE_C_COMPILER_ID MATCHES "Clang" )
  SET(CMAKE_COMPILER_IS_CLANG 1) # using Clang or AppleClang
ENDIF()

# Enable CXX for the glm bits.
ENABLE_LANGUAGE(CXX)
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CXX_EXTENSIONS OFF)

INCLUDE(CheckCXXCompilerFlag)
IF( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
  SET(CMAKE_COMPILER_IS_CLANGXX 1) # using Clang or AppleClang
ENDIF()

################################################################################
# Path/Library Setup.
################################################################################

SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/etc/")
IF( NOT CMAKE_BUILD_TYPE )
  SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, \
  options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF()

SET(LIB_LUA_VER "54" CACHE STRING "Suffix of version")
IF( CMAKE_SIZEOF_VOID_P EQUAL 8 ) # TODO: Use w/ LUA_BIT32
  SET(LIB_SUFFIX "64" CACHE STRING "Suffix of the directory name, e.g. 64 for lib64")
ELSE( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  SET(LIB_SUFFIX "" CACHE STRING "Suffix of the directory name, e.g. 64 for lib64")
ENDIF()

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/" CACHE STRING "Library build output directory")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")
SET(EXECUTABLE_OUTPUT_PATH         "${CMAKE_BINARY_DIR}/")

SET(CMAKE_INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Installation directory for executables")
SET(CMAKE_INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Installation directory for libraries")
SET(CMAKE_INSTALL_INC_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Installation directory for headers")

################################################################################
# Compiler Setup
################################################################################

# Linux: Enable all instruction subsets supported by host.
# MSVC: Enables the use of Intel AVX2 if/when possible; @TODO: Follow Linux.
OPTION(LUA_NATIVE_ARCH "Enable compiler optimizations for the native processor architecture" OFF)

# Include additional optimizations for glm-specific vector and matrix operations;
OPTION(LUA_FAST_MATH "Enable fast math optimizations" OFF)

# A subset of LUA_NATIVE_ARCH for only the GLM bindings
OPTION(GLM_NATIVE_ARCH "Enable compiler optimizations for the native processor architecture" OFF)

# Include additional optimizations for glm-specific vector and matrix operations;
# This option is intended to be a subset of LUA_FAST_MATH
OPTION(GLM_FAST_MATH "Enable fast math optimizations" OFF)

# Compilation flags specific to the luaglm object.
SET(GLM_OPTIMIZATION_FLAGS "")

# Windows systems
IF( MSVC )
  OPTION(LUA_USE_WINDOWS "Windows specific build" ON)
  OPTION(LUA_BUILD_AS_DLL "Build Lua library as DLL" ON)
  IF( NOT( ${CMAKE_COMPILER_IS_CLANG} ) )
    MESSAGE(WARNING "MSVC detected! For MSVC/msbuild environments, it is advised to use the LLVM/ClangCL toolset")
  ENDIF()

  # CRT allocator replacement
  SET(LUA_CRT_ALLOC "" CACHE PATH "Replace the Windows CRT allocator with rpmalloc or mimalloc; requires /MT")

  IF(CMAKE_BUILD_TYPE MATCHES Debug)
    ADD_COMPILE_OPTIONS(/Oi)
  ELSE()
    ADD_COMPILE_OPTIONS(/O2 /GS- /EHsc /DNDEBUG) # /Og /Oi /Ot /Oy /Ob2 /GF /Gy
  ENDIF()

  STRING(APPEND GLM_OPTIMIZATION_FLAGS " ${MSVC_OPT_FLAG} /O2 /GF /GS-")

  IF( LUA_FAST_MATH )
    ADD_COMPILE_OPTIONS(/fp:fast /fp:except-)
  ELSEIF( GLM_FAST_MATH )
    STRING(APPEND GLM_OPTIMIZATION_FLAGS " /fp:fast /fp:except-")
  ENDIF()

  IF( LUA_NATIVE_ARCH OR GLM_NATIVE_ARCH )
    CHECK_C_COMPILER_FLAG("/arch:AVX" COMPILER_OPT_ARCH_AVX_SUPPORTED)
    IF( LUA_NATIVE_ARCH AND COMPILER_OPT_ARCH_AVX_SUPPORTED )
      ADD_COMPILE_OPTIONS(/arch:AVX)
    ELSEIF( GLM_NATIVE_ARCH AND COMPILER_OPT_ARCH_AVX_SUPPORTED )
      STRING(APPEND GLM_OPTIMIZATION_FLAGS " /arch:AVX")
    ENDIF()
  ENDIF()

  ADD_COMPILE_OPTIONS(/nologo /Zc:__cplusplus)
  ADD_COMPILE_DEFINITIONS(NOMINMAX)
  ADD_COMPILE_DEFINITIONS(_CRT_SECURE_NO_DEPRECATE)
  ADD_COMPILE_DEFINITIONS(_CRT_NONSTDC_NO_DEPRECATE)

  SET(LUA_C_FLAGS "")
  SET(LUA_REL_FLAGS "/GL")
  SET(LUA_LINKFLAGS "/LTCG")

  # Visual Studio 2019 feature
  CHECK_C_COMPILER_FLAG("/Ob3" COMPILER_OPT_OB3)
  IF( COMPILER_OPT_OB3 AND NOT CMAKE_COMPILER_IS_CLANGXX )
    STRING(REPLACE "/Ob2" "/Ob3" LUA_C_FLAGS "${LUA_C_FLAGS}")
    STRING(REGEX REPLACE "[-/]Ob[1-4]" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    STRING(REGEX REPLACE "[-/]Ob[1-4]" "" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
    STRING(REGEX REPLACE "[-/]Ob[1-4]" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    STRING(REGEX REPLACE "[-/]Ob[1-4]" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

    STRING(APPEND CMAKE_C_FLAGS_RELEASE " /Ob3")
    STRING(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO " /Ob3")
    STRING(APPEND CMAKE_CXX_FLAGS_RELEASE " /Ob3")
    STRING(APPEND CMAKE_CXX_FLAGS_RELWITHDEBINFO " /Ob3")
  ELSEIF( NOT CMAKE_BUILD_TYPE MATCHES Debug )
    ADD_COMPILE_OPTIONS(/Ob2)
  ENDIF()

  # Override the C runtime allocator on Windows
  IF( LUA_CRT_ALLOC )
    IF( CMAKE_BUILD_TYPE AND ${CMAKE_BUILD_TYPE} MATCHES Debug )
      MESSAGE(FATAL_ERROR "The Debug target isn't supported along with LUA_CRT_ALLOC!")
    ENDIF()

    STRING(REGEX REPLACE "/" "\\\\" LUA_CRT_ALLOC ${LUA_CRT_ALLOC})
    IF( LUA_CRT_ALLOC MATCHES "rpmalloc$" )
      ADD_DEFINITIONS(-DENABLE_OVERRIDE -DENABLE_PRELOAD)
      STRING(APPEND LUA_REL_FLAGS " /MT")

      SET(LUA_ALLOCATOR_INCLUDE "${LUA_CRT_ALLOC}")
      SET(LUA_ALLOCATOR_FILES "${LUA_CRT_ALLOC}/rpmalloc/rpmalloc.c")

      # Search LUA_ALLOCATOR_FILES before the CRT library
      ADD_LINK_OPTIONS(/INCLUDE:realloc)
    ELSEIF( LUA_CRT_ALLOC MATCHES "mimalloc$" )
      SET(MIMALLOC_LIB "${LUA_CRT_ALLOC}/out/msvc-x64/Release/mimalloc-static.lib")

      # Additionally: '#elif defined(_MSC_VER)' needs to be commented out to
      # ensure realloc/free are symbols exported by the library.
      IF(NOT EXISTS ${MIMALLOC_LIB})
        MESSAGE(FATAL_ERROR "Cannot find the mimalloc static library. To build it, "
        "first apply the patch from https://github.com/microsoft/mimalloc/issues/268 "
        "then build the Release x64 target through ${LUA_CRT_ALLOC}\\ide\\vs2019\\mimalloc.sln")
      ENDIF()

      STRING(APPEND LUA_REL_FLAGS " /MT")
      STRING(APPEND LUA_LINKFLAGS " -INCLUDE:realloc")
      LIST(APPEND LIBS "${MIMALLOC_LIB}" "-INCLUDE:realloc")

      # This runtime should never be using new/delete and all memory allocations
      # should be routed through lua_Alloc. Worst case scenario a LuaCrtAllocator
      # implementation exists for STL containers.
      # ADD_COMPILE_DEFINITIONS(/FI"${LUA_CRT_ALLOC}/include/mimalloc-new-delete.h")
      # ADD_COMPILE_DEFINITIONS(/FI"${LUA_CRT_ALLOC}/include/mimalloc-override.h")

      ADD_LINK_OPTIONS(/INCLUDE:mi_version)
      STRING(APPEND CMAKE_SHARED_LINKER_FLAGS " -INCLUDE:realloc")
    ELSE()
      MESSAGE(FATAL_ERROR "(${LUA_CRT_ALLOC}) Cannot find the path to for the CRT allocator; currently only rpmalloc and mimalloc are supported")
    ENDIF()
  ENDIF()

  STRING(APPEND CMAKE_C_FLAGS_RELEASE " ${LUA_C_FLAGS} ${LUA_REL_FLAGS} /Oy")
  STRING(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO " ${LUA_C_FLAGS} ${LUA_REL_FLAGS}")

  STRING(APPEND CMAKE_CXX_FLAGS_RELEASE " ${LUA_C_FLAGS} ${LUA_REL_FLAGS} /Oy")
  STRING(APPEND CMAKE_CXX_FLAGS_RELWITHDEBINFO " ${LUA_C_FLAGS} ${LUA_REL_FLAGS}")

  STRING(APPEND CMAKE_EXE_LINKER_FLAGS_RELEASE " ${LUA_LINKFLAGS}")
  STRING(APPEND CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO " ${LUA_LINKFLAGS}")

  STRING(APPEND CMAKE_SHARED_LINKER_FLAGS_RELEASE " ${LUA_LINKFLAGS}")
  STRING(APPEND CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO " ${LUA_LINKFLAGS}")

  STRING(APPEND CMAKE_MODULE_LINKER_FLAGS_RELEASE " ${LUA_LINKFLAGS}")
  STRING(APPEND CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO " ${LUA_LINKFLAGS}")

  # W4 to turn compiler warnings all the way up...
  STRING(REGEX REPLACE "[-/]W[1-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  STRING(REGEX REPLACE "[-/]W[1-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

  STRING(APPEND CMAKE_C_FLAGS " /W3")
  STRING(APPEND CMAKE_CXX_FLAGS " /W3")

# Posix systems (including msys2/cygwin)
ELSE()
  OPTION(LUA_BIT32 "32bit compile" OFF)
  OPTION(LUA_USE_POSIX "Use POSIX functionality" ON)
  OPTION(LUA_USE_DLOPEN "Use dynamic linker to load modules" ON)

  # Replace -O3 w/ -O2
  # STRING(REGEX REPLACE "-O3" "-O2" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
  # STRING(REGEX REPLACE "-O3" "-O2" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
  SET(GLM_OPTIMIZATION_FLAGS "-O3")

  IF( LUA_BIT32 )
    # ADD_REQUIRED_C_COMPILER_FLAG(-m32)
    ADD_COMPILE_DEFINITIONS(LUA_32BITS=1)
    IF( "${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" OR CMAKE_COMPILER_IS_CLANG )
      SET(CMAKE_C_FLAGS "-m32 ${CMAKE_C_FLAGS}")
      SET(CMAKE_CXX_FLAGS "-m32 ${CMAKE_CXX_FLAGS}")
    ENDIF()
  ENDIF()

  # Compile everything, or lglm.cpp, or nothing with -ffast-math
  IF( LUA_FAST_MATH )
    ADD_COMPILE_OPTIONS(-ffast-math -fno-finite-math-only)
  ELSEIF( GLM_FAST_MATH )
    STRING(APPEND GLM_OPTIMIZATION_FLAGS " -ffast-math -fno-finite-math-only")
  ENDIF()

  # Compile everything, lglm.cpp, or nothing with common performance tweaks
  IF( LUA_NATIVE_ARCH )
    ADD_COMPILE_OPTIONS(-fno-stack-protector)
    IF (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
      ADD_COMPILE_OPTIONS(-fno-plt)
    ENDIF()
  ELSEIF( GLM_NATIVE_ARCH )
    STRING(APPEND GLM_OPTIMIZATION_FLAGS " -fno-stack-protector")
    IF (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
      STRING(APPEND GLM_OPTIMIZATION_FLAGS " -fno-plt")
    ENDIF()
  ENDIF()

  # Compile everything, or lglm.cpp, or nothing with -march=native
  IF( LUA_NATIVE_ARCH OR GLM_NATIVE_ARCH )
    CHECK_C_COMPILER_FLAG("-march=native" COMPILER_OPT_ARCH_NATIVE_SUPPORTED)
    IF( LUA_NATIVE_ARCH AND COMPILER_OPT_ARCH_NATIVE_SUPPORTED )
      ADD_COMPILE_OPTIONS(-march=native)
    ELSEIF( GLM_NATIVE_ARCH AND COMPILER_OPT_ARCH_NATIVE_SUPPORTED )
      STRING(APPEND GLM_OPTIMIZATION_FLAGS " -march=native")
    ENDIF()
  ENDIF()

  # Temporary fix: CMake no longer defines WIN32 on MSYS.
  IF( CYGWIN OR MSYS )
    SET(LUA_BUILD_AS_DLL ON)
    ADD_COMPILE_DEFINITIONS(_WIN32)
    ADD_COMPILE_OPTIONS(-Wno-attributes)
  ENDIF()

  # Compiler options taken from Lua: lua/blob/master/makefile
  ADD_COMPILE_OPTIONS(
    -Wall -Wextra -Wshadow -Wsign-compare -Wundef -Wwrite-strings
    -Wdisabled-optimization -Wno-inline -Wunused-parameter -Wformat=2
    # -Werror
    # -Wconversion
    # -Wsign-conversion
    # -Wcast-qual
  )

  IF (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    ADD_COMPILE_OPTIONS(
        -Wredundant-decls -Wdouble-promotion -Wstrict-overflow=2
        -Wno-format-nonliteral
    )
  ENDIF()

  IF( CMAKE_COMPILER_IS_CLANG )
    ADD_COMPILE_OPTIONS(-ferror-limit=5 -fno-common -Wno-deprecated)
  ELSEIF( CMAKE_COMPILER_IS_GNUCC )
    ADD_COMPILE_OPTIONS(-fmax-errors=5 -Wduplicated-cond -Wrestrict -Wlogical-op
      -Wno-aggressive-loop-optimizations -Wno-ignored-qualifiers
      #-Wnull-dereference
      #-Wduplicated-branches
    )

    # Remove symbols and relocation information from the binding library
    # STRING(APPEND GLM_OPTIMIZATION_FLAGS " -s")
  ENDIF()

  # On POSIX Lua links to standard math library "m"
  IF( LUA_USE_POSIX )
    ADD_COMPILE_DEFINITIONS(LUA_USE_POSIX)
    LIST(APPEND LIBS m)
  ENDIF()

  # Link to dynamic linker library "dl"
  IF( LUA_USE_DLOPEN )
    ADD_COMPILE_DEFINITIONS(LUA_USE_DLOPEN)
    IF( NOT APPLE AND NOT _WIN32 )
      LIST(APPEND LIBS dl)
    ENDIF()
  ENDIF()
ENDIF()

# Clang Everything support for debugging.
IF( CMAKE_COMPILER_IS_CLANG )
  ADD_COMPILE_OPTIONS(-fstrict-aliasing)
  ADD_COMPILE_OPTIONS(
    #-Weverything
    #-Wno-gnu-zero-variadic-macro-arguments
    #-Wno-covered-switch-default
    #-Wno-reserved-id-macro
    #-Wno-unused-macros
    #-Wno-padded
    #-Wno-old-style-cast
    #-Wno-extra-semi-stmt
    #-Wno-c++98-compat-pedantic
    #-Wno-zero-as-null-pointer-constant
    #-Wno-gnu-anonymous-struct
    #-Wno-gnu-label-as-value
    #-Wno-nested-anon-types
    #-Wno-enum-enum-conversion
    #-Wno-documentation-unknown-command
    #-Wno-float-equal
    #-Wcast-align
  )
ENDIF()

################################################################################
# Build Options
################################################################################
# TODO luaconf: LUA_DIRSEP, Manual LUA_INT_TYPE/LUA_FLOAT_TYPE
# TODO lstrlib: LUA_MAXCAPTURES, MAXCCALLS, L_FMTFLAGS, LUAL_PACKPADBYTE
# TODO loadlib: LUA_CSUBSEP, LUA_LSUBSEP, LUA_LLE_FLAGS, LUA_PATH_VAR, LUA_CPATH_VAR
# TODO loslib: LUA_TMPNAMTEMPLATE

OPTION(LUA_INCLUDE_TEST "Include ltests.h" OFF)
OPTION(LUA_USE_APICHECK "Turns on several consistency checks on the C API" OFF)
OPTION(LUAI_ASSERT "Turn on all assertions inside Lua" OFF)
OPTION(HARDSTACKTESTS "Forces a reallocation of the stack at every point where the stack can be reallocated" OFF)
OPTION(HARDMEMTESTS "Forces a full collection at all points where the collector can run" OFF)
OPTION(EMERGENCYGCTESTS "Forces an emergency collection at every single allocation" OFF)
OPTION(EXTERNMEMCHECK "Removes internal consistency checking of blocks being deallocated " OFF)
OPTION(LUA_NOCVTN2S "Turn off automatic coercion from numbers to strings" OFF)
OPTION(LUA_NOCVTS2N "Turn off automatic coercion from strings to numbers" OFF)
OPTION(LUA_COMPAT_5_3 "Controls other macros for compatibility with Lua 5.3" ON)
OPTION(LUA_COMPAT_5_4_0 "Controls other macros for compatibility with Lua 5.4.0" OFF)
OPTION(LUA_COMPAT_MATHLIB "Controls the presence of several deprecated functions in the mathematical library." ON)
OPTION(LUA_USE_JUMPTABLE "Force the use of jump tables in the main interpreter loop" OFF)
OPTION(LUA_USE_LONGJMP "handles errors with _longjmp/_setjmp when compiling as C++" ON)
OPTION(LUA_CPP_EXCEPTIONS "unprotected calls are wrapped in typed C++ exceptions" OFF)

# IF( CMAKE_BUILD_TYPE STREQUAL Debug )
#   SET(LUA_INCLUDE_TEST ON)
# ENDIF()

SET(LUA_F2IMOD_ENUM F2Ieq F2Ifloor F2Iceil)
SET(LUA_F2IMOD F2Ieq CACHE STRING "Rounding modes for float-to-integer coercion")
SET_PROPERTY(CACHE LUA_F2IMOD PROPERTY STRINGS ${LUA_F2IMOD_ENUM})

SET(LUAI_MAXCCALLS CACHE STRING "Minimum size for string buffer")
SET(MAXIWTHABS CACHE STRING
  "MAXimum number of successive Instructions WiTHout ABSolute line information \
  Must be a power of two to allow fast divisions."
)

SET(LUA_MINBUFFER CACHE STRING
  "Maximum depth for nested C calls, syntactical nested non-terminals, and \
  other features implemented through recursion in C. (Value must fit in a \
  16-bit unsigned integer. It must also be compatible with the size of the C stack.)"
)

SET(LUAI_MAXSHORTLEN CACHE STRING
  "Maximum length for short strings, that is, strings that are internalized. \
  (Cannot be smaller than reserved words or tags for metamethods, as these \
  strings must be internalized; #(\"function\") = 8, #(\"__newindex\") = 10.)"
)

SET(MINSTRTABSIZE CACHE STRING
  "Initial size for the string table (must be power of 2). The Lua core alone \
  registers ~50 strings (reserved words + metaevent keys + a few others). \
  Libraries would typically add a few dozens more."
)

SET(STRCACHE_M CACHE STRING "")
SET(STRCACHE_N CACHE STRING
  "Size of cache for strings in the API. \'N\' is the number of sets (better \
  be a prime) and \"M\" is the size of each set (M == 1 makes a direct cache.)"
)

IF( LUA_USE_APICHECK )
  ADD_COMPILE_DEFINITIONS(LUA_USE_APICHECK)
ENDIF()

IF( LUAI_ASSERT )
  ADD_COMPILE_DEFINITIONS(LUAI_ASSERT)
ENDIF()

IF( HARDSTACKTESTS )
  ADD_COMPILE_DEFINITIONS(HARDSTACKTESTS)
ENDIF()

IF( HARDMEMTESTS )
  ADD_COMPILE_DEFINITIONS(HARDMEMTESTS)
ENDIF()

IF( EMERGENCYGCTESTS )
  ADD_COMPILE_DEFINITIONS(EMERGENCYGCTESTS)
ENDIF()

IF( EXTERNMEMCHECK )
  ADD_COMPILE_DEFINITIONS(EXTERNMEMCHECK)
ENDIF()

IF( LUA_NOCVTN2S )
  ADD_COMPILE_DEFINITIONS(LUA_NOCVTN2S)
ENDIF()

IF( LUA_NOCVTS2N )
  ADD_COMPILE_DEFINITIONS(LUA_NOCVTS2N)
ENDIF()

LIST(FIND LUA_F2IMOD_ENUM ${LUA_F2IMOD} index)
IF( NOT index EQUAL -1 )
  ADD_COMPILE_DEFINITIONS(LUA_FLOORN2I=${LUA_F2IMOD})
ENDIF()

IF( LUA_COMPAT_5_3 )
  ADD_COMPILE_DEFINITIONS(LUA_COMPAT_5_3)
  SET(LUA_COMPAT_MATHLIB OFF CACHE INTERNAL "" FORCE)
ENDIF()

IF( LUA_COMPAT_5_4_0 )
  ADD_COMPILE_DEFINITIONS(LUA_COMPAT_5_4_0)
ENDIF()

IF( LUA_COMPAT_MATHLIB )
  ADD_COMPILE_DEFINITIONS(LUA_COMPAT_MATHLIB)
ENDIF()

IF( LUA_USE_JUMPTABLE )
  ADD_COMPILE_DEFINITIONS(LUA_USE_JUMPTABLE=1)
ENDIF()

IF( LUA_USE_LONGJMP )
  ADD_COMPILE_DEFINITIONS(LUA_USE_LONGJMP)
ENDIF()

IF( LUA_CPP_EXCEPTIONS )
  ADD_COMPILE_DEFINITIONS(LUA_CPP_EXCEPTIONS)
ENDIF()

IF( LUAI_MAXCCALLS )
  ADD_COMPILE_DEFINITIONS(LUAI_MAXCCALLS=${LUAI_MAXCCALLS})
ENDIF()

IF( LUA_MINBUFFER )
  ADD_COMPILE_DEFINITIONS(LUA_MINBUFFER=${LUA_MINBUFFER})
ENDIF()

IF( LUAI_MAXSHORTLEN )
  ADD_COMPILE_DEFINITIONS(LUAI_MAXSHORTLEN=${LUAI_MAXSHORTLEN})
ENDIF()

LIST(FIND LUA_F2IMOD_ENUM ${LUA_F2IMOD} index)
IF( NOT index EQUAL -1 )
  ADD_COMPILE_DEFINITIONS(LUA_FLOORN2I=${LUA_F2IMOD})
ENDIF()

# Definitions redefined during testing
IF( NOT LUA_INCLUDE_TEST )
  IF( MAXIWTHABS )
    ADD_COMPILE_DEFINITIONS(MAXIWTHABS=${MAXIWTHABS})
  ENDIF()

  IF( MINSTRTABSIZE )
    ADD_COMPILE_DEFINITIONS(MINSTRTABSIZE=${MINSTRTABSIZE})
  ENDIF()

  IF( STRCACHE_N )
    ADD_COMPILE_DEFINITIONS(STRCACHE_N=${STRCACHE_N})
  ENDIF()

  IF( STRCACHE_M )
    ADD_COMPILE_DEFINITIONS(STRCACHE_M=${STRCACHE_M})
  ENDIF()
ENDIF()

#######################################
# Lua Extensions & PowerPatches
#######################################

OPTION(LUA_C99_MATHLIB "Enable extended C99/C++11 " ON)
OPTION(LUA_SANDBOX_DBLIB "Sandboxing support for ldblib.c" OFF)
OPTION(LUA_NO_DUMP "Disable the dump module (dump Lua functions as precompiled chunk) " OFF)
OPTION(LUA_NO_BYTECODE "Disables the usage of lua_load with binary (precompiled) chunks" OFF)
OPTION(LUA_NO_PARSER "Compile the Lua core so it does not contain the parsing \
  modules (lcode, llex, lparser); only binary files and strings, precompiled \
  with luac, can be loaded." OFF
)

OPTION(LUAGLM_NUMBER_TYPE "Use lua_Number as the vector primitive; float otherwise" OFF)
OPTION(LUAGLM_EPS_EQUAL "luaV_equalobj uses approximately equal (within glm::epsilon) for vector/matrix types (beware of hashing caveats)" OFF)
OPTION(LUAGLM_MUL_DIRECTION "How operator*(glm::mat4x4, glm::vec3) is handled" OFF)

OPTION(LUAGLM_COMPAT_IPAIRS "Reintroduce compatibility for the __ipairs metamethod that was deprecated in 5.3 and removed in 5.4" OFF)
OPTION(LUAGLM_EXT_DEFER "Enable the defer statement" OFF)
OPTION(LUAGLM_EXT_DEFER_OLD "Import func2close from ltests.h into the base library as `defer`" ON)
OPTION(LUAGLM_EXT_JOAAT "Enable compile time Jenkins' one-at-a-time hashing" ON)
OPTION(LUAGLM_EXT_LAMBDA "Enable short function notation" ON)
OPTION(LUAGLM_EXT_COMPOUND "Add compound operators (+=, -=, *=, /=, <<=, >>=, &=, |=, and ^=) to the language." ON)
OPTION(LUAGLM_EXT_INTABLE "Support for unpacking named values from tables using the in keyword" ON)
OPTION(LUAGLM_EXT_TABINIT "Syntactic sugar to improve the syntax for specifying sets" ON)
OPTION(LUAGLM_EXT_SAFENAV "An indexing operation that suppresses errors on accesses into undefined table" ON)
OPTION(LUAGLM_EXT_CCOMMENT "Support for C-style block comments" ON)
OPTION(LUAGLM_EXT_SCOPE_RESOLUTION "Allow double-colon tokens to be used as field selection (emulate C++ scope resolution operator)" OFF)
OPTION(LUAGLM_EXT_API "Expose lua_createtable and other WowLua compatibility functions" ON)
OPTION(LUAGLM_EXT_READONLY "Enable readonly table API" ON)
OPTION(LUAGLM_EXT_CHRONO "Enable nanosecond resolution timers and x86 rdtsc sampling" ON)
OPTION(LUAGLM_EXT_EACH "__iter metamethod support; see documentation" ON)
OPTION(LUAGLM_EXT_BLOB "Enable an API to create non-internalized contiguous byte sequences" ON)
OPTION(LUAGLM_EXT_READLINE_HISTORY "" ON)

IF( LUA_C99_MATHLIB )
  ADD_COMPILE_DEFINITIONS(LUA_C99_MATHLIB)
ENDIF()

IF( LUA_SANDBOX_DBLIB )
  ADD_COMPILE_DEFINITIONS(LUA_SANDBOX_DBLIB)
ENDIF()

IF( LUAGLM_NUMBER_TYPE )
  ADD_COMPILE_DEFINITIONS(LUAGLM_NUMBER_TYPE)
ENDIF()

IF( LUAGLM_MUL_DIRECTION )
  ADD_COMPILE_DEFINITIONS(LUAGLM_MUL_DIRECTION)
ENDIF()

IF( LUAGLM_EPS_EQUAL )
  ADD_COMPILE_DEFINITIONS(LUAGLM_EPS_EQUAL)
ENDIF()

IF( LUAGLM_EXT_DEFER )
  ADD_COMPILE_DEFINITIONS(LUAGLM_EXT_DEFER)
ELSEIF( LUAGLM_EXT_DEFER_OLD )
  ADD_COMPILE_DEFINITIONS(LUAGLM_EXT_DEFER_OLD)
ENDIF()

IF( LUAGLM_EXT_JOAAT )
  ADD_COMPILE_DEFINITIONS(LUAGLM_EXT_JOAAT)
ENDIF()

IF( LUAGLM_EXT_LAMBDA )
  ADD_COMPILE_DEFINITIONS(LUAGLM_EXT_LAMBDA)
ENDIF()

IF( LUAGLM_COMPAT_IPAIRS )
  ADD_COMPILE_DEFINITIONS(LUAGLM_COMPAT_IPAIRS)
ENDIF()

IF( LUAGLM_EXT_COMPOUND )
  ADD_COMPILE_DEFINITIONS(LUAGLM_EXT_COMPOUND)
ENDIF()

IF( LUAGLM_EXT_INTABLE )
  ADD_COMPILE_DEFINITIONS(LUAGLM_EXT_INTABLE)
ENDIF()

IF( LUAGLM_EXT_TABINIT )
  ADD_COMPILE_DEFINITIONS(LUAGLM_EXT_TABINIT)
ENDIF()

IF( LUAGLM_EXT_SAFENAV )
  ADD_COMPILE_DEFINITIONS(LUAGLM_EXT_SAFENAV)
ENDIF()

IF( LUAGLM_EXT_CCOMMENT )
  ADD_COMPILE_DEFINITIONS(LUAGLM_EXT_CCOMMENT)
ENDIF()

IF( LUAGLM_EXT_SCOPE_RESOLUTION )
  ADD_COMPILE_DEFINITIONS(LUAGLM_EXT_SCOPE_RESOLUTION)
ENDIF()

IF( LUAGLM_EXT_EACH )
  ADD_COMPILE_DEFINITIONS(LUAGLM_EXT_EACH)
ENDIF()

IF( LUAGLM_EXT_BLOB )
  ADD_COMPILE_DEFINITIONS(LUAGLM_EXT_BLOB)
ENDIF()

IF( LUAGLM_EXT_API )
  ADD_COMPILE_DEFINITIONS(LUAGLM_EXT_API)
ENDIF()

IF( LUAGLM_EXT_READONLY )
  ADD_COMPILE_DEFINITIONS(LUAGLM_EXT_READONLY)
ENDIF()

IF( LUAGLM_EXT_CHRONO )
  ADD_COMPILE_DEFINITIONS(LUAGLM_EXT_CHRONO)
ENDIF()

IF( LUAGLM_EXT_READLINE_HISTORY )
  ADD_COMPILE_DEFINITIONS(LUAGLM_EXT_READLINE_HISTORY)
ENDIF()

#######################################
# GLM Options
#######################################

OPTION(GLM_FORCE_MESSAGES "Platform auto detection and default configuration" OFF)
OPTION(GLM_FORCE_CTOR_INIT "Force default initialization of vector, matrix and quaternion types" OFF)
OPTION(GLM_FORCE_INLINE "Force inline" ON)
OPTION(GLM_FORCE_ALIGNED_GENTYPES "Force GLM to enable aligned types" OFF)
OPTION(GLM_FORCE_DEFAULT_ALIGNED_GENTYPES "Force GLM to use aligned types by default" OFF)
OPTION(GLM_FORCE_INTRINSICS "Using SIMD optimizations" ON)
OPTION(GLM_FORCE_XYZW_ONLY "Only exposes x, y, z and w components" OFF)
OPTION(GLM_FORCE_LEFT_HANDED "Force left handed coordinate system" OFF)
OPTION(GLM_FORCE_DEPTH_ZERO_TO_ONE "Force the use of a clip space between 0 to 1" OFF)
OPTION(GLM_FORCE_SIZE_T_LENGTH "length_t is a typedef of size_t, otherwise length_t is a typedef of int like GLSL defines it." OFF)
OPTION(GLM_FORCE_UNRESTRICTED_GENTYPE "Removing genType restriction" OFF)
OPTION(GLM_FORCE_SILENT_WARNINGS "Silent C++ warnings from language extensions" OFF)

# Formerly known as 'GLM_FORCE_QUAT_DATA_WXYZ', however, changed without support
# for backwards compatibility in-mind; must be compensated for in source.
OPTION(GLM_FORCE_QUAT_DATA_XYZW "Force GLM to store quat data as x,y,z,w instead of w,x,y,z" OFF)
# GLM_FORCE_PRECISION_{ LOWP_FLOAT, MEDIUMP_FLOAT, HIGHP_FLOAT }

# lua-glm Options
OPTION(GLM_FORCE_Z_UP "Unit up vector is along the Z-axis (Y-axis otherwise)" ON)
OPTION(LUAGLM_INCLUDE_ALL "Include all GLM modules, glm/gtc/gtx" ON)
OPTION(LUAGLM_INCLUDE_EXT "Include ext headers: Stable extensions not specified by GLSL specification" OFF)
OPTION(LUAGLM_INCLUDE_GTC "Include gtc headers: Recommended extensions not specified by GLSL specification" OFF)
OPTION(LUAGLM_INCLUDE_GTX "Include gtx headers: Experimental extensions not specified by GLSL specification" OFF)
OPTION(LUAGLM_ALIASES "Include all function aliases" ON)
OPTION(LUAGLM_ALIASES_SIMPLE "Include simple function aliases, e.g., length vs. magnitude" OFF)
OPTION(LUAGLM_ALIASES_UNITY "Include Unity Scripting API aliases" OFF)
OPTION(LUAGLM_ALIASES_O3DE "Include O3DE Lua API aliases" OFF)
OPTION(LUAGLM_TYPE_COERCION "Enable string-to-number type coercion when parsing arguments from the Lua stack" ON)
OPTION(LUAGLM_INCLUDE_GEOM "Extend geometry API" ON)
OPTION(LUAGLM_RECYCLE "Recycle trailing (unused) function parameters" ON)
OPTION(LUAGLM_FORCED_RECYCLE
  "Experiment: All function results must be preallocated, i.e., functions that return \
  matrices must have preallocated matrix trailing function arguments." OFF
)
OPTION(LUAGLM_DRIFT "Experiment: implicitly correct floating point drift in direction vectors and quaternions" OFF)

IF( GLM_FORCE_MESSAGES )
  ADD_COMPILE_DEFINITIONS(GLM_FORCE_MESSAGES)
ENDIF()

# g++ doesn't support force-inlined variadic functions, see glm::to_string.
# Technically this is a GLM error that should be reported

IF( GLM_FORCE_CTOR_INIT )
  ADD_COMPILE_DEFINITIONS(GLM_FORCE_CTOR_INIT)
ENDIF()

IF( GLM_FORCE_INLINE )
  ADD_COMPILE_DEFINITIONS(GLM_FORCE_INLINE)
ENDIF()

IF( GLM_FORCE_DEFAULT_ALIGNED_GENTYPES )
  ADD_COMPILE_DEFINITIONS(GLM_FORCE_DEFAULT_ALIGNED_GENTYPES)
  ADD_COMPILE_DEFINITIONS(LUAGLM_FORCES_ALIGNED_GENTYPES)
ELSEIF( GLM_FORCE_ALIGNED_GENTYPES )
  ADD_COMPILE_DEFINITIONS(GLM_FORCE_ALIGNED_GENTYPES)
ENDIF()

IF( GLM_FORCE_INTRINSICS )
  ADD_COMPILE_DEFINITIONS(GLM_FORCE_INTRINSICS)
ENDIF()

IF( GLM_FORCE_XYZW_ONLY )
  ADD_COMPILE_DEFINITIONS(GLM_FORCE_XYZW_ONLY)
ENDIF()

IF( GLM_FORCE_LEFT_HANDED )
  ADD_COMPILE_DEFINITIONS(GLM_FORCE_LEFT_HANDED)
ENDIF()

IF( GLM_FORCE_Z_UP )
  ADD_COMPILE_DEFINITIONS(GLM_FORCE_Z_UP)
ENDIF()

IF( GLM_FORCE_DEPTH_ZERO_TO_ONE )
  ADD_COMPILE_DEFINITIONS(GLM_FORCE_DEPTH_ZERO_TO_ONE)
ENDIF()

IF( GLM_FORCE_SIZE_T_LENGTH )
  ADD_COMPILE_DEFINITIONS(GLM_FORCE_SIZE_T_LENGTH)
ENDIF()

IF( GLM_FORCE_UNRESTRICTED_GENTYPE )
  ADD_COMPILE_DEFINITIONS(GLM_FORCE_UNRESTRICTED_GENTYPE)
ENDIF()

IF( GLM_FORCE_SILENT_WARNINGS )
  ADD_COMPILE_DEFINITIONS(GLM_FORCE_SILENT_WARNINGS)
ENDIF()

IF( GLM_FORCE_QUAT_DATA_XYZW )
  MESSAGE(FATAL_ERROR "Disable GLM_FORCE_QUAT_DATA_XYZW" )
  # ADD_COMPILE_DEFINITIONS(GLM_FORCE_QUAT_DATA_XYZW)
ENDIF()

IF( LUAGLM_INCLUDE_EXT )
  ADD_COMPILE_DEFINITIONS(LUAGLM_INCLUDE_EXT)
ENDIF()

IF( LUAGLM_INCLUDE_GTC )
  ADD_COMPILE_DEFINITIONS(LUAGLM_INCLUDE_GTC)
ENDIF()

IF( LUAGLM_INCLUDE_GTX )
  ADD_COMPILE_DEFINITIONS(LUAGLM_INCLUDE_GTX)
ENDIF()

IF( LUAGLM_INCLUDE_ALL )
  ADD_COMPILE_DEFINITIONS(LUAGLM_INCLUDE_ALL)

  # Future-proof by including the above options:
  # ADD_COMPILE_DEFINITIONS(LUAGLM_INCLUDE_EXT)
  # ADD_COMPILE_DEFINITIONS(LUAGLM_INCLUDE_GTC)
  # ADD_COMPILE_DEFINITIONS(LUAGLM_INCLUDE_GTX)
ENDIF()

# Enable all aliasing configurations
IF( LUAGLM_ALIASES OR LUAGLM_ALIASES_SIMPLE )
  ADD_COMPILE_DEFINITIONS(LUAGLM_ALIASES_SIMPLE)
ENDIF()

IF( LUAGLM_ALIASES OR LUAGLM_ALIASES_UNITY )
  ADD_COMPILE_DEFINITIONS(LUAGLM_ALIASES_UNITY)
ENDIF()

IF( LUAGLM_ALIASES OR LUAGLM_ALIASES_O3DE )
  ADD_COMPILE_DEFINITIONS(LUAGLM_ALIASES_O3DE)
ENDIF()

IF( LUAGLM_TYPE_COERCION )
  ADD_COMPILE_DEFINITIONS(LUAGLM_TYPE_COERCION)
ENDIF()

IF( LUAGLM_INCLUDE_GEOM )
  ADD_COMPILE_DEFINITIONS(LUAGLM_INCLUDE_GEOM)
ENDIF()

IF( LUAGLM_RECYCLE )
  ADD_COMPILE_DEFINITIONS(LUAGLM_RECYCLE)
  IF( LUAGLM_FORCED_RECYCLE )
    ADD_COMPILE_DEFINITIONS(LUAGLM_FORCED_RECYCLE)
  ENDIF()
ENDIF()

IF( LUAGLM_DRIFT )
  ADD_COMPILE_DEFINITIONS(LUAGLM_DRIFT)
ENDIF()

OPTION(LUAGLM_TYPE_SANITIZE "Experiment: narrow_cast" ON)
IF( LUAGLM_TYPE_SANITIZE )
  ADD_COMPILE_DEFINITIONS(LUAGLM_TYPE_SANITIZE)
ENDIF()

################################################################################
# Compilation
################################################################################

OPTION(ONE_LUA "Compile Lua core, libraries, and interpreter in a single file" OFF)
OPTION(LUA_HPP "Compile Lua as cplusplus (note: this requires changing the extern field of lua.hpp" OFF)
OPTION(LUA_INCLUDE_LIBGLM "Build lglm into the runtime and include it in the initialization libraries" OFF)

INCLUDE_DIRECTORIES( . )
INCLUDE_DIRECTORIES( ./libs/glm/ )
INCLUDE_DIRECTORIES( ./libs/glm-binding/ )

SET(LUA_PUBLIC_HDRS luaconf.h lua.h lauxlib.h lgritlib.h lualib.h lua.hpp lglm.hpp )

SET(SRC_LUA lua.c)
SET(SRC_LUAC luac.c)
SET(SRC_ONELUA onelua.c)
SET(SRC_LUAGLM lglm.cpp)
SET(SRC_LIB
  lapi.c lauxlib.c lbaselib.c lcode.c lcorolib.c lctype.c ldblib.c ldebug.c
  ldo.c ldump.c lfunc.c lgc.c linit.c liolib.c llex.c lmathlib.c lmem.c
  loadlib.c lobject.c lopcodes.c loslib.c lparser.c lstate.c lstring.c
  lstrlib.c ltable.c ltablib.c ltm.c lundump.c lutf8lib.c lvm.c lzio.c
)

SET(SRC_LIBGLM libs/glm-binding/lglmlib.cpp)

IF( LUA_INCLUDE_TEST )
  ADD_COMPILE_DEFINITIONS(LUA_USER_H="ltests.h")
  LIST(APPEND SRC_LIB ltests.c)
ENDIF()

IF( LUA_HPP )
  MESSAGE(WARNING "Ensure extern \"C++\" is defined in lua.hpp")
  SET_SOURCE_FILES_PROPERTIES(${SRC_LIB} ${SRC_LUA} ${SRC_LUAC} PROPERTIES LANGUAGE CXX )
ELSE()
  SET_SOURCE_FILES_PROPERTIES(${SRC_LIB} ${SRC_LUA} ${SRC_LUAC} PROPERTIES LANGUAGE C )
  ADD_COMPILE_DEFINITIONS(LUA_C_LINKAGE) # Setup ${SRC_LUAGLM} linkage
ENDIF()

IF( LUA_INCLUDE_LIBGLM )
  ADD_COMPILE_DEFINITIONS(LUA_INCLUDE_LIBGLM)
  SET(SRC_LUAGLM ${SRC_LUAGLM} ${SRC_LIBGLM})
ENDIF()

# Setup ${SRC_LUAGLM}
SET_SOURCE_FILES_PROPERTIES(${SRC_ONELUA} ${SRC_LUAGLM} PROPERTIES LANGUAGE CXX )
IF( NOT CMAKE_BUILD_TYPE STREQUAL Debug )
  SET_SOURCE_FILES_PROPERTIES(${SRC_ONELUA} PROPERTIES COMPILE_FLAGS ${GLM_OPTIMIZATION_FLAGS})
  SET_SOURCE_FILES_PROPERTIES(${SRC_LUAGLM} PROPERTIES COMPILE_FLAGS ${GLM_OPTIMIZATION_FLAGS})
  SET_SOURCE_FILES_PROPERTIES(${SRC_LIBGLM} PROPERTIES COMPILE_FLAGS ${GLM_OPTIMIZATION_FLAGS})
ENDIF()

#######################################
# Library Properties
#######################################

# @TODO: Ideally, the shared and static libraries would only be compiled once
# with a 'shared' position independent object library. However, LUA_NO_DUMP,
# LUA_NO_BYTECODE, and LUA_NO_PARSER are compile time options instead of
# link-time options.

# Create dynamic library (dll, so)
IF( NOT LUA_INCLUDE_TEST AND NOT ONE_LUA )
  ADD_LIBRARY(liblua SHARED ${SRC_LIB} ${SRC_LUAGLM} ${LUA_RC})

  TARGET_LINK_LIBRARIES(liblua ${LIBS})
  TARGET_INCLUDE_DIRECTORIES(liblua SYSTEM PRIVATE ${LUA_ALLOCATOR_INCLUDE})
  TARGET_SOURCES(liblua PRIVATE ${LUA_ALLOCATOR_FILES})

  SET_TARGET_PROPERTIES(liblua PROPERTIES OUTPUT_NAME lua${LIB_LUA_VER} CLEAN_DIRECT_OUTPUT 1)
  IF( LUA_BIT32 )
    SET_TARGET_PROPERTIES(liblua PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
  ENDIF()

  IF( LUA_BUILD_AS_DLL )
    TARGET_COMPILE_DEFINITIONS(liblua PRIVATE LUA_BUILD_AS_DLL)
  ENDIF()

  IF( LUA_NO_DUMP )
    TARGET_COMPILE_DEFINITIONS(liblua PRIVATE LUA_NO_DUMP)
  ENDIF()

  IF( LUA_NO_BYTECODE )
    TARGET_COMPILE_DEFINITIONS(liblua PRIVATE LUA_NO_BYTECODE)
  ENDIF()

  IF( LUA_NO_PARSER )
    TARGET_COMPILE_DEFINITIONS(liblua PRIVATE LUA_NO_PARSER)
  ENDIF()
ENDIF()

# Create static library
IF( NOT ONE_LUA )
  ADD_LIBRARY(liblua_static STATIC ${SRC_LIB} ${SRC_LUAGLM})
  TARGET_INCLUDE_DIRECTORIES(liblua_static SYSTEM PRIVATE ${LUA_ALLOCATOR_INCLUDE})
  TARGET_SOURCES(liblua_static PRIVATE ${LUA_ALLOCATOR_FILES})

  TARGET_LINK_LIBRARIES(liblua_static PUBLIC ${LIBS})
  SET_TARGET_PROPERTIES(liblua_static PROPERTIES POSITION_INDEPENDENT_CODE ON)
  SET_TARGET_PROPERTIES(liblua_static PROPERTIES OUTPUT_NAME lua${LIB_LUA_VER}_static CLEAN_DIRECT_OUTPUT 1)
  IF( LUA_BIT32 )
    SET_TARGET_PROPERTIES(liblua_static PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
  ENDIF()
ELSE()
  ADD_COMPILE_DEFINITIONS(LUA_INCLUDE_LIBGLM)
ENDIF()

# Add extra rc files to the windows build
IF( (MSVC OR CYGWIN OR MSYS) AND NOT LUA_INCLUDE_TEST )
  IF( NOT CMAKE_RC_COMPILER )
    SET(CMAKE_RC_COMPILER windres.exe)
  ENDIF()

  SET(LUA_RC etc/lua.rc)
ENDIF()

#######################################
# Stand-Alone Interpreter
#######################################

IF( ONE_LUA )
  ADD_EXECUTABLE(lua ${SRC_ONELUA} ${LUA_RC})
  TARGET_COMPILE_DEFINITIONS(lua PRIVATE MAKE_LUA)
  TARGET_INCLUDE_DIRECTORIES(lua SYSTEM PRIVATE ${LUA_ALLOCATOR_INCLUDE})
  TARGET_SOURCES(lua PRIVATE ${LUA_ALLOCATOR_FILES})
ELSE()
  ADD_EXECUTABLE(lua ${SRC_LUA} ${LUA_RC})
ENDIF()

SET(LUA_PROGNAME CACHE STRING "Interpreter program name")
SET(LUA_PROMPT CACHE STRING "Default prompt used by stand-alone Lua")
SET(LUA_PROMPT2 CACHE STRING "Default continuation prompt used by stand-alone Lua")
SET(LUA_MAXINPUT CACHE STRING "The maximum length for an input line in the stand-alone interpreter.")

IF( LUA_PROGNAME )
  TARGET_COMPILE_DEFINITIONS(lua PRIVATE LUA_PROGNAME="${LUA_PROGNAME}")
ENDIF()

IF( LUA_PROMPT )
  # echo ${PROMPT}: "└> "
  TARGET_COMPILE_DEFINITIONS(lua PRIVATE LUA_PROMPT="${LUA_PROMPT}")
ENDIF()

IF( LUA_PROMPT2 )
  TARGET_COMPILE_DEFINITIONS(lua PRIVATE LUA_PROMPT2="${LUA_PROMPT2}")
ENDIF()

IF( LUA_MAXINPUT )
  TARGET_COMPILE_DEFINITIONS(lua PRIVATE LUA_MAXINPUT=${LUA_MAXINPUT})
ENDIF()

# Ensure libreadline linkage
IF( APPLE )
  SET(SKIP_BUILD_RPATH TRUE)
  SET(CMAKE_MACOSX_RPATH 1)

  LIST(APPEND LUA_REPL_LIBS readline)
  TARGET_COMPILE_DEFINITIONS(lua PRIVATE "LUA_USE_READLINE")
ELSEIF( UNIX )
  FIND_PACKAGE (Readline)
  IF( READLINE_FOUND )
    SET(CMAKE_REQUIRED_LIBRARIES "readline")

    LIST(APPEND LUA_REPL_LIBS readline)
    TARGET_COMPILE_DEFINITIONS(lua PRIVATE "LUA_USE_READLINE")
  ENDIF()
ENDIF()

# Ensure exports are enabled for dlopen
SET_TARGET_PROPERTIES(lua PROPERTIES ENABLE_EXPORTS 1)
IF( LUA_BIT32 )
  SET_TARGET_PROPERTIES(lua PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
ENDIF()

# Lua Libraries
IF( ONE_LUA )
  TARGET_LINK_LIBRARIES(lua PUBLIC ${LIBS} PRIVATE ${LUA_REPL_LIBS})
ELSE()
  IF( LUA_INCLUDE_TEST )
    SET(interpretor_target liblua_static)
  ELSE()
    SET(interpretor_target liblua)
  ENDIF()
  TARGET_LINK_LIBRARIES(lua PUBLIC ${interpretor_target} PRIVATE ${LUA_REPL_LIBS})
ENDIF()

#######################################
# Compiler
#######################################

# Use static library.
IF( NOT LUA_INCLUDE_TEST )
  IF( ONE_LUA )
    ADD_EXECUTABLE(luac ${SRC_ONELUA} ${LUA_RC})
    TARGET_COMPILE_DEFINITIONS(luac PRIVATE MAKE_LUAC)
  ELSE()
    ADD_EXECUTABLE(luac ${SRC_LUAC} ${LUA_RC})
    SET(compiler_target liblua_static)
  ENDIF()

  TARGET_LINK_LIBRARIES(luac PRIVATE ${compiler_target} PUBLIC ${LIBS})
  IF( LUA_BIT32 )
    SET_TARGET_PROPERTIES(luac PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
  ENDIF()
ENDIF()

#######################################
# Binding Library
#######################################

IF( NOT LUA_INCLUDE_LIBGLM AND NOT ONE_LUA )
  ADD_LIBRARY(glm MODULE ${SRC_LIBGLM})
  TARGET_INCLUDE_DIRECTORIES(glm PRIVATE ${INCLUDE_DIRECTORIES})
  TARGET_LINK_LIBRARIES(glm PRIVATE ${LIBS} ${interpretor_target})

  # SET_TARGET_PROPERTIES(glm PROPERTIES LINK_FLAGS_RELEASE -s)
  SET_TARGET_PROPERTIES(glm PROPERTIES PREFIX "" OUTPUT_NAME "glm" CLEAN_DIRECT_OUTPUT 1)
  IF( LUA_BIT32 )
    SET_TARGET_PROPERTIES(glm PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
  ENDIF()

  OPTION(LUAGLM_BINDING_ALIGNED "Force the binding library to use aligned types by default" OFF)
  IF( LUAGLM_BINDING_ALIGNED )
    TARGET_COMPILE_DEFINITIONS(glm PRIVATE GLM_FORCE_DEFAULT_ALIGNED_GENTYPES)
  ENDIF()

  IF( LUA_BUILD_AS_DLL )
    TARGET_COMPILE_DEFINITIONS(glm PRIVATE LUA_BUILD_AS_DLL)
  ENDIF()

  IF( NOT MSVC )
    TARGET_COMPILE_OPTIONS(glm PRIVATE $<$<CONFIG:Release>:-fno-exceptions -fno-asynchronous-unwind-tables>)
  ENDIF()
ENDIF()

#######################################
# Install
#######################################

# Don't allow installation if including test builds
SET(SKIP_INSTALL SKIP_INSTALL_ALL OR LUA_INCLUDE_TEST)

IF( NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL )
  INSTALL(FILES ${LUA_PUBLIC_HDRS} DESTINATION "${CMAKE_INSTALL_INC_DIR}")
ENDIF()

IF( NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL )
  IF( UNIX AND NOT APPLE )
    SET_TARGET_PROPERTIES( lua PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_LIB_DIR}" )
  ENDIF()

  INSTALL(TARGETS lua ${interpretor_target} ${compiler_target}
    RUNTIME DESTINATION "${CMAKE_INSTALL_BIN_DIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIB_DIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIB_DIR}"
  )
ENDIF()

#######################################
# Testing
#######################################

IF( LUA_INCLUDE_TEST ) # ctest --verbose
  MESSAGE("CTest enabled: make test ARGS=\"-V\" for verbose output")

  ENABLE_TESTING()
  ADD_TEST(NAME test_all
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/testes
    COMMAND lua -e "_port=true" ${PROJECT_SOURCE_DIR}/testes/all.lua
  )
ENDIF()
