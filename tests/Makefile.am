ACLOCAL_AMFLAGS = -I m4

# generate package.m4
include $(srcdir)/package.m4.am

# generated tests
include $(srcdir)/gentests.am

# testdata
include $(srcdir)/testdata.am 

TESTSUITE_AT = testsuite.at macros.at $(GENTESTS_AT)

EXTRA_DIST = $(TESTSUITE_AT) $(TESTSUITE) $(srcdir)/package.m4 atlocal.in $(TESTDATA)

DISTCLEANFILES       = atconfig $(check_SCRIPTS)
MAINTAINERCLEANFILES = Makefile.in $(TESTSUITE)

TESTSUITE = $(srcdir)/testsuite

check-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

installcheck-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' AUTOTEST_PATH='$(bindir)' \
	$(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || \
	$(SHELL) '$(TESTSUITE)' --clean

AUTOM4TE = $(SHELL) $(top_srcdir)/missing --run autom4te
AUTOTEST = $(AUTOM4TE) --language=autotest
$(TESTSUITE): $(TESTSUITE_AT) package.m4
	$(AUTOTEST) -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@
	
check_SCRIPTS = intzip

