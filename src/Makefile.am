ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = $(all_includes)
AM_CXXFLAGS = -I$(top_srcdir)/include

bin_PROGRAMS = intzip
intzip_SOURCES = main.cpp io.cpp
intzip_LDADD = $(top_builddir)/lib/libintzip.la $(AM_LDFLAGS)


