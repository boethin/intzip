ACLOCAL_AMFLAGS = -I m4

SUBDIRS = lib src tests

# distributed headers
include_HEADERS = include/intzip.h

# internal headers
noinst_HEADERS = \
  include/intzip-bitnumber.h \
  include/intzip-bitvector.h \
  include/intzip-def.h \
  include/intzip-io.h \
  include/intzip-rlebuf.h \
  include/intzip-trace.h \
  include/intzip-bitstore.h \
  include/intzip-chunk.h \
  include/intzip-options.h \
  include/intzip-stdint.h \
  include/intzip-uint.h

# Test Coverage:
# recompile everything with gcov flags, run tests and create lib/intzip.cpp.gcov.
gcov-check: intzip.cpp.gcov

.PHONY: gcov-check

intzip.cpp.gcov:
	touch lib/*.cpp src/*.cpp
	$(MAKE) $(AM_MAKEFLAGS) CXXFLAGS='-O0 -fprofile-arcs -ftest-coverage' LDFLAGS='-O0 -lgcov' check
	cd lib && gcov -abf -o .libs intzip.cpp && cp -f intzip.cpp.gcov ../

clean-local:
	-rm -f intzip.cpp.gcov
