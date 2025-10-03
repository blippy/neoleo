# To run the tests: `make check'

AM_TESTS_ENVIRONMENT = \
	export ASRCDIR=$(abs_srcdir); \
	export ABUILDDIR=$(abs_builddir); \
	export SRCDIR=$(srcdir); \
	. $(srcdir)/neotests.env;


# NB Supressing cpreg-01.src for now due to issue#17
TESTS = basic.sh \
	badrc.sh \
	insert-01.sh \
	issue16.sh issue18.sh \
	issue51.sh \
	nohead-01.sh \
	parser.sh \
	picol-01.sh \
	picol-02.sh \
	spans.sh \
	write.sh


TEST_EXTENSIONS = .scr
#SCR_LOG_COMPILER = ./script.sh
SCR_LOG_COMPILER = $(srcdir)/script.sh
AM_SCR_LOG_FLAGS = -b $(abs_builddir) -s $(abs_srcdir)
#export FOO=bar
export BUILDDIR=$(abs_top_builddir)

EXTRA_DIST = differ neotests.env script.sh $(TESTS)
EXTRA_DIST += badrc.oleo

clean-local : clean-local-out
.PHONY: clean-local-out
clean-local-out:
	-rm -f out/*oleo out/*rep *.log *.trs
