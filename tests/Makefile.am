# To run the tests: `make check'
EXTRA_DIST = ref.sh
#bin_SCRIPTS = . tests/test-suite.sh
#AM_TESTS_ENVIRONMENT = . ./.test-suite.sh
#AM_TESTS_FD_REDIRECT = 9>&2

# NB Supressing cpreg-01.src for now due to issue#17
TESTS = busi.scr cpreg-02.scr cpreg-03.scr \
	decompile-01.scr \
	inbuilt-tests.sh \
	issue15.scr issue16.scr issue17.scr issue18.scr issue19.scr \
	issue25.sh \
	ref.sh nohead-01.scr nohead-02.scr stringo.scr
TEST_EXTENSIONS = .scr
SCR_LOG_COMPILER = ./script.sh

# as suggested at:
# https://www.gnu.org/software/automake/manual/html_node/Clean.html
clean-local : clean-local-out
.PHONY: clean-local-out
clean-local-out:
	-rm -f out/*oleo
