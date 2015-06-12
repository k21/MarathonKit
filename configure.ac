AC_INIT([MarathonKit], [0.0.1-dev])
AC_CONFIG_SRCDIR([./LICENSE])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_SILENT_RULES([yes])

AC_PROG_CXX
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11

AC_PROG_RANLIB

ACX_PTHREAD([have_pthread=yes])
AS_IF(
	[test "x$have_pthread" = "xyes"],
	[
		LIBS="$PTHREAD_LIBS $LIBS"
		CFLAGS="$PTHREAD_CFLAGS $CFLAGS"
		CXXFLAGS="$PTHREAD_CFLAGS $CXXFLAGS"
		CC="$PTHREAD_CC"
		GTEST_CPPFLAGS="-DGTEST_HAS_PTHREAD=1"
	],
	[
		GTEST_CPPFLAGS="-DGTEST_HAS_PTHREAD=0"
	]
)
AC_SUBST([GTEST_CPPFLAGS])

AC_MSG_CHECKING([whether to enable warnings])
AC_ARG_ENABLE(
	[warnings],
	[AS_HELP_STRING([--enable-warnings], [turn on warnings (for developers)])],
	[
		AS_CASE(
			["$enableval"],
			[yes], [enable_warnings="-Wall -Wextra -Wconversion -Wshadow -Weffc++"],
			[no], [enable_warnings=""],
			[*], [enable_warnings="$enableval"]
		)
	],
	[enable_warnings=""]
)
AC_MSG_RESULT([$enable_warnings])
AC_SUBST([WARNINGS_CPPFLAGS], "$enable_warnings")

AC_MSG_CHECKING([whether the Sound module is requested])
AC_ARG_ENABLE(
	[sound],
	[AS_HELP_STRING([--enable-sound], [build the Sound module])],
	[request_sound="$enableval"],
	[request_sound="optional"]
)
AC_MSG_RESULT([$request_sound])

AS_IF(
	[test "x$request_sound" = "xno"],
	[enable_sound=no],
	[
		enable_sound=yes
		PKG_CHECK_MODULES(
			[sndfile],
			[sndfile],
			[],
			[enable_sound=no])
	]
)

AS_IF(
	[test "x$request_sound" = "xyes" && test "x$enable_sound" = "xno"],
	[AC_MSG_ERROR([some dependencies of the Sound module are missing])]
)
AC_MSG_CHECKING([whether the Sound module will be built])
AC_MSG_RESULT([$enable_sound])
AM_CONDITIONAL([SOUND_ENABLED], [test "x$enable_sound" = "xyes"])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
