# 25/10 Using current processes for making info

HTMLS = \
 	html/Command-Index.html \
 	html/Concept-Index.html \
 	html/Function-Index.html \
 	html/Headless-mode.html \
 	html/index.html \
 	html/Key-Index.html \
 	html/Options-Index.html


#distdir=$(datadir)/neoleo
#TEXIS = neoleo.texi headless.texi

#texi_SOURCES = $(TEXIS)
#texi_BIN = neoleo.info



#infodir = $(datadir)/neoleo


#info_DATA = neoleo.texi

info_TEXINFOS = neoleo.texi
neoleo_TEXINFOS = headless.texi

#MAKEINFOHTML = $(MAKEINFO) --html -o html

all : neoleo.info html

# I think this is actually right
htmldir =       $(prefix)/share/doc/neoleo/html

$(HTMLS) : neoleo.texi
	makeinfo --html -o html neoleo.texi

#EXTRA_DIST=	$(neoleo_TEXINFOS) # neoleo_web.html # texi2html
#noinst_DATA =	neoleo_web.html $(TEXIS)

#neoleo.info:	neoleo.texi $(TEXIS)


#htmldir=	$(prefix)/share/neoleo
#html_DATA=	neoleo_html

#html:	$(srcdir)/neoleo.texi ${TEXIS} $(top_builddir)/doc/texi2html
#		@HAVE_PERL@ $(top_builddir)/doc/texi2html $(srcdir)/neoleo.texi
		
#neoleo_html:	$(srcdir)/neoleo.texi ${TEXIS} 
#	texi2any --html $(srcdir)/neoleo.texi


#html: neoleo.texi headless.texi		


#DISTCLEANFILES = $(html_DATA)
#MAINTAINERCLEANFILES = Makefile.in



# 25/10 dvi are an awkward ball of wax, so don't distcheck them
#EMPTY_AUTOMAKE_TARGETS = dvi 
#.PHONY: $(EMPTY_AUTOMAKE_TARGETS)
#$(EMPTY_AUTOMAKE_TARGETS):

#%.info : %.texi
#	makeinfo $<


#html : neoleo.info htmlxref.cnf
#	makeinfo --html neoleo.texi -o html

#neoleo.info : $(TEXIS)
#	echo "mc making info"
#	makeinfo $<

#EXTRA_DIST = neoleo.info $(TEXIS) htmlxref.cnf $(HTMLS)
EXTRA_DIST = $(HTMLS) 
EXTRA_DIST += neoleo.texi
dist_html_DATA = $(HTMLS)
#nodist_info_DATA = neoleo.texi
