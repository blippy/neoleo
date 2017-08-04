# To run the tests: `make check'
EXTRA_DIST = test-suite.sh
#bin_SCRIPTS = . tests/test-suite.sh
#AM_TESTS_ENVIRONMENT = . ./.test-suite.sh
#AM_TESTS_FD_REDIRECT = 9>&2

TESTS = inbuilt-tests.sh test-suite.sh nohead-01.scr nohead-02.scr 
TEST_EXTENSIONS = .scr
SCR_LOG_COMPILER = script.sh

# as suggested at:
# https://www.gnu.org/software/automake/manual/html_node/Clean.html
clean-local : clean-local-out
.PHONY: clean-local-out
clean-local-out:
	-rm -f out/*oleo
