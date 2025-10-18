#
# $Header: /cvs/oleo/doc/Makefile.am,v 1.15 2001/02/14 20:54:49 danny Exp $
#
# Process this file with automake to produce Makefile.in
#
info_TEXINFOS = neoleo.texi

TEXIS=		

EXTRA_DIST=	$(TEXIS) neoleo_web.html texi2html
#noinst_DATA =	neoleo_web.html $(TEXIS)

neoleo.info:	neoleo.texi $(TEXIS)


htmldir=	$(prefix)/share/neoleo
html_DATA=	neoleo.html

#neoleo.html:	$(srcdir)/neoleo.texi ${TEXIS} $(top_builddir)/doc/texi2html
#		@HAVE_PERL@ $(top_builddir)/doc/texi2html $(srcdir)/neoleo.texi

DISTCLEANFILES = $(html_DATA)
MAINTAINERCLEANFILES = Makefile.in

