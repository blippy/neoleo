#
# $Header: /cvs/oleo/intl/Makefile.am,v 1.6 2001/02/08 00:26:36 pw Exp $
#
noinst_LIBRARIES = libintl.a

gnulocaledir = $(prefix)/share/locale
localedir = $(datadir)/locale

CFLAGS=	@CFLAGS@ \
	-DLOCALEDIR=\"$(localedir)\" \
	-DGNULOCALEDIR=\"$(gnulocaledir)\" \
	-DLOCALE_ALIAS_PATH=\"$(aliaspath)\"


libintl_a_SOURCES = libgettext.h loadinfo.h \
	gettext.h gettextP.h hash-string.h \
	intl-compat.c \
	bindtextdom.c dcgettext.c dgettext.c gettext.c \
	finddomain.c loadmsgcat.c localealias.c textdomain.c l10nflist.c \
	explodename.c

EXTRA_DIST=	po2tbl.sed.in xopen-msg.sed linux-msg.sed
MAINTAINERCLEANFILES = Makefile.in

DISTCLEANFILES = po2tbl.sed

