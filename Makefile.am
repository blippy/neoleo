#
# This is the main Makefile for GNU Oleo.
#
# Process this file with automake to produce Makefile.in
#
# $Header: /sources/oleo/oleo/Makefile.am,v 1.41 2011/07/05 05:46:39 delqn Exp $
#
AUTOMAKE_OPTIONS = gnu 1.4 readme-alpha
ACLOCAL_AMFLAGS = -I m4

D=	`date +%G%m%d.%H%M%S`

SUBDIRS = doc lib src m4 Xresources examples afm #po

# Remove this file here (it is created via configure), not from within intl.
# DISTCLEANFILES = intl/libintl.h

EXTRA_DIST = config.rpath  oleobug.in FAQ oleo.spec.in oleo.spec

#
# Use this only for real end-user-ready releases of Oleo
#
release:	oleo-$(VERSION).tar.gz
	scp doc/oleo_web.html gnudist.gnu.org:/home/www/html/software/oleo/oleo.html
	scp oleo-$(VERSION).tar.gz gnudist.gnu.org:/home/ftp/gnu/oleo/oleo-$(VERSION).tar.gz
	scp ChangeLog gnudist.gnu.org:/home/www/html/software/oleo/ChangeLog
	scp TODO gnudist.gnu.org:/home/www/html/software/oleo/TODO
	scp doc/oleo.html gnudist.gnu.org:/home/www/html/software/oleo/doc/oleo.html
	scp doc/oleo1.png gnudist.gnu.org:/home/www/html/software/oleo/doc/oleo1.png

#
# This can be used to put the files in place for alpha releases.
#
release.alpha:	oleo-$(VERSION).tar.gz
	scp oleo-$(VERSION).tar.gz gnudist.gnu.org:/home/alpha/gnu/oleo/oleo-$(VERSION).tar.gz
	scp doc/oleo_web.html gnudist.gnu.org:/home/www/html/software/oleo/oleo.html
	scp ChangeLog gnudist.gnu.org:/home/www/html/software/oleo/ChangeLog
	scp TODO gnudist.gnu.org:/home/www/html/software/oleo/TODO

web:
	scp doc/oleo_web.html gnudist.gnu.org:/home/www/html/software/oleo/oleo.html
	scp ChangeLog gnudist.gnu.org:/home/www/html/software/oleo/ChangeLog
	scp doc/oleo.html gnudist.gnu.org:/home/www/html/software/oleo/doc/oleo.html
	scp doc/oleo1.png gnudist.gnu.org:/home/www/html/software/oleo/doc/oleo1.png

docdir=        $(prefix)/Oleo
doc_DATA=	AUTHORS FAQ

# remove all the symlinks that come from automake
MAINTAINERCLEANFILES = COPYING INSTALL install-sh missing mkinstalldirs \
    Makefile.in aclocal.m4 config.h.in configure stamp-h.in

