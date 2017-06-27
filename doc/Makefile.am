info_TEXINFOS = neoleo.texi

TEXIS=		basics.texi bugs.texi cell-refs.texi entering-data.texi \
		examples.texi expressions.texi forth.texi functions.texi \
		interface.texi keymaps.texi \
		macros.texi making-changes.texi options.texi print-plot.texi \
		programs.texi ranlib.texi style.texi texinfo.tex windows.texi \
		motif.texi dbms.texi


neoleo_TEXINFOS = $(TEXIS)
MAIN_HTML	= neoleo.html

EXTRA_DIST=	$(TEXIS) atlast2.html 
noinst_DATA =	$(TEXIS)

neoleo.info:	neoleo.texi $(TEXIS)


htmldir=	$(prefix)/share/neoleo
html_DATA	= neoleo.html

$(MAIN_HTML):	$(srcdir)/neoleo.texi ${TEXIS}
		makeinfo --html --no-split $(srcdir)/neoleo.texi
		
#$(MAKEINFOHTML) $(srcdir)/neoleo.texi
