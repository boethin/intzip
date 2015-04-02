#!/bin/bash
# clean up everything
# run './autoreconf.sh' then
rm -vf config.guess config.h config.h.in~ config.log config.status config.sub configure
rm -vf aclocal.m4 depcomp libtool stamp-h1 appxLogFile.log ltmain.sh install-sh missing compile test-driver
rm -vfr autom4te.cache
rm -f *.in
find . -name Makefile -exec rm -vf {} \;
find . -name Makefile.in -exec rm -vf {} \;
for d in src lib tests
do
	rm -vf $d/*.o $d/*.lo
	rm -vfr $d/.deps $d/.libs
	rm -f $d/*.log
done

