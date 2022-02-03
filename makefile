# Developer's makefile for building Lua
# see luaconf.h for further customization

# == CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT =======================

# Warnings valid for both C and C++
CWARNSCPP= \
	-Wfatal-errors \
	-Wextra \
	-Wshadow \
	-Wsign-compare \
	-Wundef \
	-Wwrite-strings \
	-Wredundant-decls \
	-Wdisabled-optimization \
	-Wdouble-promotion \
	-Wmissing-declarations \
	# the next warnings might be useful sometimes,
	# but usually they generate too much noise
	# -Werror \
	# -pedantic   # warns if we use jump tables \
	# -Wconversion  \
	# -Wsign-conversion \
	# -Wstrict-overflow=2 \
	# -Wformat=2 \
	# -Wcast-qual \


# Warnings for gcc, not valid for clang
CWARNGCC= \
	-Wlogical-op \
	-Wno-aggressive-loop-optimizations \
	-Wno-inline \
	-Wno-ignored-qualifiers \


# The next warnings are neither valid nor needed for C++
CWARNSC= -Wdeclaration-after-statement \
	-Wmissing-prototypes \
	-Wnested-externs \
	-Wstrict-prototypes \
	-Wc++-compat \
	-Wold-style-definition \

# Some useful compiler options for internal tests:
# -DLUAI_ASSERT turns on all assertions inside Lua.
# -DHARDSTACKTESTS forces a reallocation of the stack at every point where
# the stack can be reallocated.
# -DHARDMEMTESTS forces a full collection at all points where the collector
# can run.
# -DEMERGENCYGCTESTS forces an emergency collection at every single allocation.
# -DEXTERNMEMCHECK removes internal consistency checking of blocks being
# deallocated (useful when an external tool like valgrind does the check).
# -DMAXINDEXRK=k limits range of constants in RK instruction operands.
# -DLUA_COMPAT_5_3

# -pg -malign-double
# -DLUA_USE_CTYPE -DLUA_USE_APICHECK
# ('-ftrapv' for runtime checks of integer overflows)
# -fsanitize=undefined -ftrapv -fno-inline
# Note: Disable -DNDEBUG
# TESTS= -DLUA_USER_H='"ltests.h"' -O0 -g

# Additional compiler options for internal tests. Useful when compiling with
# GLM_FORCE_DEFAULT_ALIGNED_GENTYPES as additional 'alignment' requirements are
# introduced. Note, the current '*FORCE*_ALIGNED_GENTYPES' implementation may
# cause unaligned reads in other GCObj definitions and still requires cleanup.
#
# Usage: export 'UBSAN_OPTIONS=print_stacktrace=1,log_path=lua_ubsan.out'
# GCC_SANITIZE = -g -fno-omit-frame-pointer -fsanitize=null,undefined,signed-integer-overflow,alignment
# GPP_SANITIZE = -fsanitize=return

# Your platform. See PLATS for possible values.
PLAT= guess

# See LUA_C_LINKAGE definition in source. If "CC" replaces gcc with g++, then
# LUA_LINKAGE needs to be undefined
LUA_LINKAGE= -DLUA_C_LINKAGE

CC= gcc -std=gnu99 $(CWARNSCPP) $(CWARNSC) $(CWARNGCC) $(GCC_SANITIZE)
CPP= g++ -std=c++11 $(CWARNSCPP) $(CWARNGCC) $(GCC_SANITIZE) $(GPP_SANITIZE)
CFLAGS= -O2 -Wall -Wextra -DNDEBUG $(SYSCFLAGS) $(MYCFLAGS)
CPERF_FLAGS = -O3 -march=native -ffast-math -fno-finite-math-only -fno-stack-protector # -flto -fno-plt
LDFLAGS= $(SYSLDFLAGS) $(MYLDFLAGS) $(GCC_SANITIZE) $(GPP_SANITIZE)
LIBS= -lm $(SYSLIBS) $(MYLIBS)

AR= ar rc
RANLIB= ranlib
RM= rm -f
UNAME= uname

SYSCFLAGS=
SYSLDFLAGS=
SYSLIBS=

LUA_PATCHES = -DLUA_C99_MATHLIB  \
		-DLUA_COMPAT_5_3 \
		-DLUA_USE_LONGJMP \
		-DLUAGLM_EXT_DEFER_OLD \
		-DLUAGLM_EXT_COMPOUND \
		-DLUAGLM_EXT_INTABLE \
		-DLUAGLM_EXT_TABINIT \
		-DLUAGLM_EXT_SAFENAV \
		-DLUAGLM_EXT_CCOMMENT \
		-DLUAGLM_EXT_JOAAT \
		-DLUAGLM_EXT_LAMBDA \
		-DLUAGLM_EXT_EACH \
		-DLUAGLM_EXT_API \
		-DLUAGLM_EXT_CHRONO \
		-DLUAGLM_EXT_BLOB \
		-DLUAGLM_EXT_READLINE_HISTORY \
		-DLUAGLM_EXT_READONLY \
		# -DLUAGLM_COMPAT_IPAIRS \

GLM_FLAGS = -DLUAGLM_LIBVERSION=999 \
		-DGLM_FORCE_INLINE \
		-DGLM_FORCE_Z_UP \
		-DLUAGLM_INCLUDE_ALL \
		-DLUAGLM_ALIASES_SIMPLE -DLUAGLM_ALIASES_UNITY -DLUAGLM_ALIASES_O3DE \
		-DLUAGLM_INCLUDE_GEOM \
		-DLUAGLM_RECYCLE \
		-DLUAGLM_TYPE_COERCION \
		-DGLM_FORCE_INTRINSICS \
		# -DGLM_FORCE_DEFAULT_ALIGNED_GENTYPES -DLUAGLM_FORCES_ALIGNED_GENTYPES \
		# -DGLM_FORCE_MESSAGES \
		# -DGLM_FORCE_XYZW_ONLY \
		# -DGLM_FORCE_CTOR_INIT \
		# -DGLM_FORCE_QUAT_DATA_XYZW \
		# -DGLM_FORCE_SIZE_T_LENGTH \

MYCFLAGS= $(TESTS) $(LUA_PATCHES) $(GLM_FLAGS) -Ilibs/glm/
MYLDFLAGS= $(TESTS)
MYLIBS=
MYOBJS=

# == END OF USER SETTINGS -- NO NEED TO CHANGE ANYTHING BELOW THIS LINE =======

PLATS= guess aix bsd freebsd generic linux linux-readline macos mingw posix solaris

LUA_A=	liblua.a
CORE_O=	lapi.o lcode.o lctype.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o lmem.o lobject.o lopcodes.o lparser.o lstate.o lstring.o ltable.o ltm.o lundump.o lvm.o lzio.o ltests.o lglm.o
LIB_O=	lauxlib.o lbaselib.o lcorolib.o ldblib.o liolib.o lmathlib.o loadlib.o loslib.o lstrlib.o ltablib.o lutf8lib.o linit.o
BASE_O= $(CORE_O) $(LIB_O) $(MYOBJS)

LUA_T=	lua
LUA_O=	lua.o

LUAC_T=	luac
LUAC_O=	luac.o

ALL_O= $(BASE_O) $(LUA_O) $(LUAC_O)
ALL_T= $(LUA_A) $(LUA_T) $(LUAC_T)
ALL_A= $(LUA_A)

# Targets start here.
default: $(PLAT)

all:	$(ALL_T)

o:	$(ALL_O)

a:	$(ALL_A)

$(LUA_A): $(BASE_O)
	$(AR) $@ $(BASE_O)
	$(RANLIB) $@

$(LUA_T): $(LUA_O) $(LUA_A)
	$(CC) -o $@ $(LDFLAGS) $(LUA_O) $(LUA_A) $(LIBS)

$(LUAC_T): $(LUAC_O) $(LUA_A)
	$(CC) -o $@ $(LDFLAGS) $(LUAC_O) $(LUA_A) $(LIBS)

clean:
	$(RM) $(ALL_T) $(ALL_O) $(GLM_A) minilua.hpp onelua.o lua54.dll glm.dll lua.exe luac.exe

depend:
	@$(CC) $(CFLAGS) -MM l*.c

echo:
	@echo "PLAT= $(PLAT)"
	@echo "CC= $(CC)"
	@echo "CPP= $(CPP)"
	@echo "CFLAGS= $(CFLAGS)"
	@echo "LDFLAGS= $(SYSLDFLAGS)"
	@echo "LIBS= $(LIBS)"
	@echo "AR= $(AR)"
	@echo "RANLIB= $(RANLIB)"
	@echo "RM= $(RM)"
	@echo "UNAME= $(UNAME)"

$(ALL_O): makefile ltests.h

# Convenience targets for popular platforms.
ALL= all

help:
	@echo "Do 'make PLATFORM' where PLATFORM is one of these:"
	@echo "   $(PLATS)"
	@echo "See doc/readme.html for complete instructions."

guess:
	@echo Guessing `$(UNAME)`
	@$(MAKE) `$(UNAME)`

AIX aix:
	$(MAKE) $(ALL) CC="xlc" CFLAGS="-O2 -DLUA_USE_POSIX -DLUA_USE_DLOPEN" SYSLIBS="-ldl" SYSLDFLAGS="-brtl -bexpall"

bsd:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_POSIX -DLUA_USE_DLOPEN" SYSLIBS="-Wl,-E"

FreeBSD NetBSD OpenBSD freebsd:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_LINUX -DLUA_USE_READLINE -I/usr/include/edit" SYSLIBS="-Wl,-E -ledit" CC="cc"

generic: $(ALL)

Linux linux:	linux-noreadline

linux-noreadline:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_LINUX" SYSLIBS="-Wl,-E -ldl"

linux-readline:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_LINUX -DLUA_USE_READLINE" SYSLIBS="-Wl,-E -ldl -lreadline"

LINUX_ONE_FLAGS := $(MYCFLAGS) $(CPERF_FLAGS) $(LUA_LINKAGE) -I. -Ilibs/glm-binding
linux-one:
	$(MAKE) linux-readline CC="$(CPP)" LUA_O="onelua.o" BASE_O="onelua.o" LUA_A="" CORE_O="" LIB_O="" LUAC_T="" MYCFLAGS="$(LINUX_ONE_FLAGS)"

Darwin macos macosx:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_MACOSX"

macos-readline:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_MACOSX -DLUA_USE_READLINE" SYSLIBS="-lreadline"

macos-one:
	$(MAKE) macos-readline CC="$(CPP)" LUA_O="onelua.o" BASE_O="onelua.o" LUA_A="" CORE_O="" LIB_O="" LUAC_T="" MYCFLAGS="$(LINUX_ONE_FLAGS)"

mingw:
	$(MAKE) "LUA_A=lua54.dll" "LUA_T=lua.exe" \
	"AR=$(CC) -shared -o" "RANLIB=strip --strip-unneeded" \
	"SYSCFLAGS=-Wno-attributes -DLUA_USE_POSIX -DLUA_BUILD_AS_DLL -D_WIN32" "SYSLIBS=" "SYSLDFLAGS=-s" lua.exe
	$(MAKE) "LUAC_T=luac.exe" luac.exe

posix:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_POSIX"

SunOS solaris:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_POSIX -DLUA_USE_DLOPEN -D_REENTRANT" SYSLIBS="-ldl"

# Targets that do not create files (not all makes understand .PHONY).
.PHONY: all $(PLATS) help test clean default o a depend echo

# lua-glm binding
GLM_A = glm.so
LIBGLM_ALIGN := -DGLM_FORCE_DEFAULT_ALIGNED_GENTYPES
LIBGLM_FLAGS := $(CFLAGS) $(CPERF_FLAGS) $(TESTS) $(LIBGLM_ALIGN) -fno-exceptions -fno-asynchronous-unwind-tables -fPIC -I. -Ilibs/glm-binding -shared

lib-glm:
	$(CPP) $(LUA_LINKAGE) $(LIBGLM_FLAGS) -o $(GLM_A) libs/glm-binding/lglmlib.cpp $(LIBS)

lib-glm-mingw:
	$(MAKE) lib-glm SYSCFLAGS="-L . -DLUA_BUILD_AS_DLL -D_WIN32" GLM_A="glm.dll" SYSLIBS="-llua" SYSLDFLAGS="-s"

lib-glm-macos:
	$(MAKE) lib-glm SYSCFLAGS="-L . -DLUA_USE_MACOSX" SYSLIBS="-llua"

# DO NOT EDIT
# automatically made with 'g++ -MM l*.c'

lapi.o: lapi.c lprefix.h lua.h luaconf.h lapi.h llimits.h lstate.h \
 lobject.h ltm.h lzio.h lmem.h ldebug.h ldo.h lfunc.h lgc.h lglm_core.h \
 lstring.h ltable.h lundump.h lvm.h
lauxlib.o: lauxlib.c lprefix.h lua.h luaconf.h lauxlib.h lgrit_lib.h
lbaselib.o: lbaselib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h \
 lgrit_lib.h
lcode.o: lcode.c lprefix.h lua.h luaconf.h lcode.h llex.h lobject.h \
 llimits.h lzio.h lmem.h lopcodes.h lparser.h ldebug.h lstate.h ltm.h \
 ldo.h lgc.h lstring.h ltable.h lvm.h
lcorolib.o: lcorolib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
lctype.o: lctype.c lprefix.h lctype.h lua.h luaconf.h llimits.h
ldblib.o: ldblib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
ldebug.o: ldebug.c lprefix.h lua.h luaconf.h lapi.h llimits.h lstate.h \
 lobject.h ltm.h lzio.h lmem.h lcode.h llex.h lopcodes.h lparser.h \
 ldebug.h ldo.h lfunc.h lstring.h lgc.h ltable.h lvm.h
ldo.o: ldo.c lprefix.h lua.h luaconf.h lapi.h llimits.h lstate.h \
 lobject.h ltm.h lzio.h lmem.h lgrit_lib.h lauxlib.h ldebug.h ldo.h \
 lfunc.h lgc.h lopcodes.h lparser.h lstring.h ltable.h lundump.h lvm.h
ldump.o: ldump.c lprefix.h lua.h luaconf.h lobject.h llimits.h lstate.h \
 ltm.h lzio.h lmem.h lundump.h
lfunc.o: lfunc.c lprefix.h lua.h luaconf.h ldebug.h lstate.h lobject.h \
 llimits.h ltm.h lzio.h lmem.h ldo.h lfunc.h lgc.h
lgc.o: lgc.c lprefix.h lua.h luaconf.h ldebug.h lstate.h lobject.h \
 llimits.h ltm.h lzio.h lmem.h ldo.h lfunc.h lgc.h lstring.h ltable.h
linit.o: linit.c lprefix.h lua.h luaconf.h lualib.h lauxlib.h
liolib.o: liolib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
llex.o: llex.c lprefix.h lua.h luaconf.h lctype.h llimits.h ldebug.h \
 lstate.h lobject.h ltm.h lzio.h lmem.h ldo.h lgc.h llex.h lparser.h \
 lstring.h ltable.h
lmathlib.o: lmathlib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h \
 lgrit_lib.h
lmem.o: lmem.c lprefix.h lua.h luaconf.h ldebug.h lstate.h lobject.h \
 llimits.h ltm.h lzio.h lmem.h ldo.h lgc.h
loadlib.o: loadlib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
lobject.o: lobject.c lprefix.h lua.h luaconf.h lctype.h llimits.h \
 ldebug.h lstate.h lobject.h ltm.h lzio.h lmem.h ldo.h lstring.h lgc.h \
 lvm.h
lopcodes.o: lopcodes.c lprefix.h lopcodes.h llimits.h lua.h luaconf.h
loslib.o: loslib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
lparser.o: lparser.c lprefix.h lua.h luaconf.h lcode.h llex.h lobject.h \
 llimits.h lzio.h lmem.h lopcodes.h lparser.h ldebug.h lstate.h ltm.h \
 ldo.h lfunc.h lglm_core.h lstring.h lgc.h ltable.h
lstate.o: lstate.c lprefix.h lua.h luaconf.h lapi.h llimits.h lstate.h \
 lobject.h ltm.h lzio.h lmem.h ldebug.h ldo.h lfunc.h lgc.h llex.h \
 lstring.h ltable.h
lstring.o: lstring.c lprefix.h lua.h luaconf.h ldebug.h lstate.h \
 lobject.h llimits.h ltm.h lzio.h lmem.h ldo.h lstring.h lgc.h
lstrlib.o: lstrlib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
ltable.o: ltable.c lprefix.h lua.h luaconf.h ldebug.h lstate.h lobject.h \
 llimits.h ltm.h lzio.h lmem.h ldo.h lgc.h lglm_core.h lstring.h ltable.h \
 lvm.h
ltablib.o: ltablib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h \
 lgrit_lib.h
ltests.o: ltests.c lprefix.h lua.h luaconf.h lapi.h llimits.h lstate.h \
 lobject.h ltm.h lzio.h lmem.h lauxlib.h lgrit_lib.h lcode.h llex.h \
 lopcodes.h lparser.h lctype.h ldebug.h ldo.h lfunc.h lopnames.h \
 lstring.h lgc.h ltable.h lualib.h
ltm.o: ltm.c lprefix.h lua.h luaconf.h ldebug.h lstate.h lobject.h \
 llimits.h ltm.h lzio.h lmem.h ldo.h lgc.h lglm_core.h lstring.h ltable.h \
 lvm.h
lua.o: lua.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
luac.o: luac.c lprefix.h lua.h luaconf.h lauxlib.h ldebug.h lstate.h \
 lobject.h llimits.h ltm.h lzio.h lmem.h lopcodes.h lopnames.h lundump.h
lundump.o: lundump.c lprefix.h lua.h luaconf.h ldebug.h lstate.h \
 lobject.h llimits.h ltm.h lzio.h lmem.h ldo.h lfunc.h lstring.h lgc.h \
 lundump.h
lutf8lib.o: lutf8lib.c lprefix.h lua.h luaconf.h lauxlib.h lualib.h
lvm.o: lvm.c lprefix.h lua.h luaconf.h ldebug.h lstate.h lobject.h \
 llimits.h ltm.h lzio.h lmem.h ldo.h lfunc.h lgc.h lglm_core.h lopcodes.h \
 lstring.h ltable.h lvm.h ljumptab.h
lzio.o: lzio.c lprefix.h lua.h luaconf.h llimits.h lmem.h lstate.h \
 lobject.h ltm.h lzio.h
onelua.o: onelua.c lprefix.h luaconf.h lzio.c lua.h llimits.h lmem.h \
 lstate.h lobject.h ltm.h lzio.h lctype.c lctype.h lopcodes.c lopcodes.h \
 lmem.c ldebug.h ldo.h lgc.h lundump.c lfunc.h lstring.h lundump.h \
 ldump.c lstate.c lapi.h llex.h ltable.h lgc.c llex.c lparser.h lcode.c \
 lcode.h lvm.h lparser.c lglm_core.h ldebug.c lfunc.c lobject.c ltm.c \
 lstring.c ltable.c ldo.c lgrit_lib.h lauxlib.h lvm.c ljumptab.h lapi.c \
 lglm.cpp lglm.hpp lua.hpp lualib.h lglm_string.hpp lauxlib.c lbaselib.c \
 lcorolib.c ldblib.c liolib.c lmathlib.c loadlib.c loslib.c lstrlib.c \
 ltablib.c lutf8lib.c linit.c lua.c
lglm.o: lglm.cpp lua.h luaconf.h lglm.hpp lua.hpp lualib.h \
 lauxlib.h lglm_core.h llimits.h ltm.h lobject.h lglm_string.hpp \
 lgrit_lib.h lapi.h lstate.h lzio.h lmem.h ldebug.h lfunc.h lgc.h \
 lstring.h ltable.h lvm.h ldo.h
	$(CPP) $(LUA_LINKAGE) $(CFLAGS) $(CPERF_FLAGS) $(TESTS) -c -o lglm.o lglm.cpp

# (end of Makefile)
