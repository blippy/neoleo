#
# $Header: /cvs/oleo/doc/Makefile.am,v 1.15 2001/02/14 20:54:49 danny Exp $
#
# Process this file with automake to produce Makefile.in
#
info_TEXINFOS = oleo.texi

TEXIS=		basics.texi bugs.texi cell-refs.texi entering-data.texi \
		examples.texi expressions.texi forth.texi functions.texi \
		interface.texi keymaps.texi \
		macros.texi making-changes.texi options.texi print-plot.texi \
		programs.texi ranlib.texi style.texi texinfo.tex windows.texi \
		motif.texi dbms.texi

EXTRA_DIST=	$(TEXIS) oleo_web.html texi2html oleo1.png
noinst_DATA =	oleo_web.html $(TEXIS)

oleo.info:	oleo.texi $(TEXIS)

#if HavePerl

htmldir=	$(prefix)/share/oleo
html_DATA=	atlast2.html oleo.html

#oleo.html:	$(srcdir)/oleo.texi ${TEXIS} $(top_builddir)/doc/texi2html
#		@HAVE_PERL@ $(top_builddir)/doc/texi2html $(srcdir)/oleo.texi

oleo.html:	$(srcdir)/oleo.texi ${TEXIS}
		texi2html $(srcdir)/oleo.texi

#endif

DISTCLEANFILES = $(html_DATA)
MAINTAINERCLEANFILES = Makefile.in

