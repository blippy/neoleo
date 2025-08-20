#EXTRA_DIST = src/neoleo.i

generaldir = $(datadir)/neoleo

SUBDIRS = examples src tests tests/out tests/verified

EXTRA_DIST = FAQ README.md INSTALL-cygwin INSTALL-debian
general_DATA = $(EXTRA_DIST)



distclean-local: distclean-local-stamp
.PHONY: distclean-local-stamp
distclean-local-stamp:
	-rm -f stamp-h2
