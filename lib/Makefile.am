ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -Wall $(all_includes) -I$(top_srcdir)/include

lib_LTLIBRARIES = libintzip.la
libintzip_la_SOURCES = intzip.cpp

clean-local:
	-rm -f *.gcov *.gcno *.gcda
