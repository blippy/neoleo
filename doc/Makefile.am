#
# $Header: /cvs/oleo/doc/Makefile.am,v 1.15 2001/02/14 20:54:49 danny Exp $
#
# Process this file with automake to produce Makefile.in
#
info_TEXINFOS = neoleo.texi

TEXIS=		basics.texi bugs.texi cell-refs.texi entering-data.texi \
		examples.texi expressions.texi forth.texi functions.texi \
		interface.texi keymaps.texi \
		macros.texi making-changes.texi options.texi print-plot.texi \
		programs.texi ranlib.texi style.texi texinfo.tex windows.texi \
		motif.texi dbms.texi

EXTRA_DIST=	$(TEXIS) atlast2.html oleo_web.html oleo1.png
noinst_DATA =	oleo_web.html $(TEXIS)

neoleo.info:	neoleo.texi $(TEXIS)


htmldir=	$(prefix)/share/neoleo
html_DATA=	atlast2.html 

neoleo.html:	$(srcdir)/neoleo.texi ${TEXIS}
		$(MAKEINFOHTML) $(srcdir)/neoleo.texi


DISTCLEANFILES = $(html_DATA)
MAINTAINERCLEANFILES = Makefile.in

