#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([intzip], [0.1], [boethin@xn--domain.net])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE
LT_INIT

AC_CONFIG_TESTDIR([tests])

# Checks for programs.
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_LIBTOOL

AC_LANG([C++])

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([tests/intzip], [chmod +x tests/intzip])

AC_CONFIG_FILES([
	Makefile
	lib/Makefile
	src/Makefile
	tests/Makefile
	tests/atlocal
])
AC_OUTPUT

