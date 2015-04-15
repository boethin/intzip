ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -Wall $(all_includes) -I$(top_srcdir)/include

lib_LTLIBRARIES = libintzip.la
libintzip_la_SOURCES = intzip.cpp

if ENABLE_TRACE
libintzip_la_SOURCES += trace.cpp
endif

clean-local:
	-rm -f *.ii *.s
	-rm -f *.gcov *.gcno *.gcda
