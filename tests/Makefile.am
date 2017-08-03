# To run the tests: `make check'
EXTRA_DIST = test-suite.sh
#bin_SCRIPTS = . tests/test-suite.sh
#AM_TESTS_ENVIRONMENT = . ./.test-suite.sh
#AM_TESTS_FD_REDIRECT = 9>&2
TESTS = nohead-01.sh inbuilt-tests.sh test-suite.sh

# as suggested at:
# https://www.gnu.org/software/automake/manual/html_node/Clean.html
clean-local : clean-local-out
.PHONY: clean-local-out
clean-local-out:
	-rm -f out/*oleo
