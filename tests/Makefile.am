ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = $(all_includes)
AM_CXXFLAGS = -I$(top_srcdir)/include

check_PROGRAMS = t1
t1_SOURCES = t1.cpp
t1_LDADD = $(top_builddir)/lib/libintzip.la $(AM_LDFLAGS)

TESTS = $(check_PROGRAMS)

