ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -Wall $(all_includes) -I$(top_srcdir)/include

bin_PROGRAMS = intzip
intzip_SOURCES = main.cpp io.cpp
intzip_LDADD = $(top_builddir)/lib/libintzip.la $(AM_LDFLAGS)
intzip_LDFLAGS = -Wl,-rpath='$$ORIGIN/../lib'

clean-local:
	-rm -f *.ii *.s
	-rm -f *.gcov *.gcno *.gcda
