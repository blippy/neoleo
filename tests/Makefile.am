# To run the tests: `make check'

AM_TESTS_ENVIRONMENT = \
	export ASRCDIR=$(abs_srcdir); \
	export ABUILDDIR=$(abs_builddir); \
	export SRCDIR=$(srcdir); \
	. $(srcdir)/neotests-env.sh;


# NB Supressing cpreg-01.src for now due to issue#17
TESTS = basic.sh \
	issue16.scr issue18.scr issue25.sh \
	nohead-01.scr \
	parser.sh


TEST_EXTENSIONS = .scr
#SCR_LOG_COMPILER = ./script.sh
SCR_LOG_COMPILER = $(srcdir)/script.sh
AM_SCR_LOG_FLAGS = -b $(abs_builddir) -s $(abs_srcdir)
#export FOO=bar
export BUILDDIR=$(abs_top_builddir)

EXTRA_DIST = neotests-env.sh script.sh issue25.oleo $(TESTS)

clean-local : clean-local-out
.PHONY: clean-local-out
clean-local-out:
	-rm -f out/*oleo out/*rep *.log *.trs
