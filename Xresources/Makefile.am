#
# This directory contains files to be installed as application defaults
# files for oleo. One of the purposes is to internationalise the Motif
# interface, which is not easily done with the GNU Gettext facilities in
# the po/ directory.
#
# $Header: /sources/oleo/oleo/Xresources/Makefile.am,v 1.5 2001/06/26 00:36:35 pw Exp $
#

xdefaultsdir =	$(prefix)/share/oleo

noinst_DATA=	Oleo.nl Oleo.fr Oleo.en
EXTRA_DIST =	Oleo.nl Oleo.fr Oleo.en

nldir =		$(xdefaultsdir)/nl
frdir =		$(xdefaultsdir)/fr
endir =		$(xdefaultsdir)

install-data-hook:
	$(mkinstalldirs) $(DESTDIR)$(nldir) $(DESTDIR)$(frdir) $(DESTDIR)$(endir)
	$(INSTALL_DATA) $(srcdir)/Oleo.nl $(DESTDIR)$(nldir)/Oleo
	$(INSTALL_DATA) $(srcdir)/Oleo.fr $(DESTDIR)$(frdir)/Oleo
	$(INSTALL_DATA) $(srcdir)/Oleo.en $(DESTDIR)$(endir)/Oleo

MAINTAINERCLEANFILES = Makefile.in
