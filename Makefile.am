#EXTRA_DIST = src/neoleo.i

generaldir = $(datadir)/neoleo

SUBDIRS = doc examples src tests tests/out tests/verified

EXTRA_DIST = FAQ README.md INSTALL-debian INSTALL-fedora
general_DATA = $(EXTRA_DIST)



distclean-local: distclean-local-stamp
.PHONY: distclean-local-stamp
distclean-local-stamp:
	-rm -f stamp-h2
