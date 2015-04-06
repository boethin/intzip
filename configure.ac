#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([intzip], [0.1], [boethin@xn--domain.net])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE
LT_INIT

# Checks for programs.
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_LIBTOOL

AC_LANG([C++])

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.

# Tests
AC_CONFIG_TESTDIR([tests])
AC_CONFIG_FILES([tests/atlocal])
AC_CONFIG_FILES([tests/intzip], [chmod +x tests/intzip])
AC_CONFIG_FILES([tests/encode-decode], [chmod +x tests/encode-decode])

# Add debug support
AC_ARG_ENABLE(debug,
  AS_HELP_STRING(
    [--enable-debug],
    [enable debugging, default: no]),
    [case "${enableval}" in
      yes) debug=true ;;
      no)  debug=false ;;
      *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
    esac],
    [debug=false])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")
AM_COND_IF(DEBUG,
    AC_DEFINE(DEBUG, 1, [Define to 0 if this is a release build]),
    AC_DEFINE(DEBUG, 0, [Define to 1 or higher if this is a debug build])
    AC_DEFINE(NDEBUG, 1, [NDEBUG disables assert()]))

# Makefiles
AC_CONFIG_FILES([
	Makefile
	lib/Makefile
	src/Makefile
	tests/Makefile
])
AC_OUTPUT

