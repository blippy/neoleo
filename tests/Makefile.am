# To run the tests: `make check'
#EXTRA_DIST = inbuilt-tests.sh ref.sh
#bin_SCRIPTS = . tests/test-suite.sh
#AM_TESTS_ENVIRONMENT = . ./.test-suite.sh
#AM_TESTS_FD_REDIRECT = 9>&2

AM_TESTS_ENVIRONMENT = \
	export ASRCDIR=$(abs_srcdir); \
	export ABUILDDIR=$(abs_builddir); \
	export SRCDIR=$(srcdir); \
	. $(srcdir)/neotests-env.sh;

# NB Supressing cpreg-01.src for now due to issue#17
TESTS = busi.scr cpreg-02.scr cpreg-03.scr \
	decompile-01.scr \
	floor.scr \
	inbuilt-tests.sh \
	issue15.scr issue16.scr issue17.scr issue18.scr issue19.scr \
	neobasic01.sh neobasic02.sh \
	nohead-01.scr nohead-02.scr stringo.scr \
	issue25.sh ref.sh

TEST_EXTENSIONS = .scr
#SCR_LOG_COMPILER = ./script.sh
SCR_LOG_COMPILER = $(srcdir)/script.sh
AM_SCR_LOG_FLAGS = -b $(abs_builddir) -s $(abs_srcdir)

EXTRA_DIST = neotests-env.sh script.sh ref.oleo issue25.oleo $(TESTS)

clean-local : clean-local-out
.PHONY: clean-local-out
clean-local-out:
	-rm -f out/*oleo out/*rep
