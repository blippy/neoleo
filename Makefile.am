#
# This is the main Makefile for GNU Oleo.
#
# Process this file with automake to produce Makefile.in
#
# $Header: /cvs/oleo/Makefile.am,v 1.37 2001/02/08 00:21:23 pw Exp $
#
AUTOMAKE_OPTIONS = gnu 1.4 readme-alpha
ACLOCAL_AMFLAGS = -I m4

D=	`date +%G%m%d.%H%M%S`

SUBDIRS = doc lib intl src m4 po Xresources examples afm

# Remove this file here (it is created via configure), not from within intl.
DISTCLEANFILES = intl/libintl.h

EXTRA_DIST = oleobug.in FAQ oleo.spec.in oleo.spec

#
# Use this only for real end-user-ready releases of Oleo
#
release:	oleo-$(VERSION).tar.gz
	rcp doc/oleo_web.html gnudist.gnu.org:/home/www/html/software/oleo/oleo.html
	rcp oleo-$(VERSION).tar.gz gnudist.gnu.org:/home/ftp/gnu/oleo/oleo-$(VERSION).tar.gz
	rcp ChangeLog gnudist.gnu.org:/home/www/html/software/oleo/ChangeLog
	rcp TODO gnudist.gnu.org:/home/www/html/software/oleo/TODO
	rcp doc/oleo.html gnudist.gnu.org:/home/www/html/software/oleo/doc/oleo.html
	rcp doc/oleo1.png gnudist.gnu.org:/home/www/html/software/oleo/doc/oleo1.png

#
# This can be used to put the files in place for alpha releases.
#
release.alpha:	oleo-$(VERSION).tar.gz
	rcp oleo-$(VERSION).tar.gz alpha.gnu.org:/fs/share/ftp/gnu/oleo/oleo-$(VERSION).tar.gz
	rcp doc/oleo_web.html gnudist.gnu.org:/home/www/html/software/oleo/oleo.html
	rcp ChangeLog gnudist.gnu.org:/home/www/html/software/oleo/ChangeLog
	rcp TODO gnudist.gnu.org:/home/www/html/software/oleo/TODO

web:
	rcp doc/oleo_web.html gnudist.gnu.org:/home/www/html/software/oleo/oleo.html
	rcp ChangeLog gnudist.gnu.org:/home/www/html/software/oleo/ChangeLog
	rcp doc/oleo.html gnudist.gnu.org:/home/www/html/software/oleo/doc/oleo.html
	rcp doc/oleo1.png gnudist.gnu.org:/home/www/html/software/oleo/doc/oleo1.png

docdir=        $(prefix)/Oleo
doc_DATA=	AUTHORS FAQ

# remove all the symlinks that come from automake
MAINTAINERCLEANFILES = COPYING INSTALL install-sh missing mkinstalldirs \
    Makefile.in aclocal.m4 config.h.in configure stamp-h.in

