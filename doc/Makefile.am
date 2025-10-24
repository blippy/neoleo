# 25/10 Using current processes for making info

all : neoleo.info html

#SUFFIXES = .texi



#info_TEXINFOS = neoleo.texi

#neoleo_TEXINFOS = headless.texi

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

html :

html : neoleo.info htmlxref.cnf
	makeinfo --html neoleo.texi -o html

neoleo.info : neoleo.texi headless.texi
	echo "mc making info"
	makeinfo $<

EXTRA_DIST = neoleo.info neoleo.texi headless.texi
