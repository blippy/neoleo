#AM_CPPFLAGS=-DDATADIR='$(datadir)'

#VPATH = $(srcdir) $(builddir)

GUI_SRCS = 
GUI_LINK = 
#GUI_DEFINES = -DX_DISPLAY_MISSING
GUI_DEFINES = -DHAVE_X 

# Order of linking of libraries for Motif seems to be important
# I have decided to mandate the use of the Xbae library, rather than
# have it optional.

if UseMotif
GUI_SRCS += io-motif.c appres.c fallback.c oleo_icon.xpm
GUI_LINK += -lXm -lXt -lXbae 
GUI_DEFINES += -DHAVE_MOTIF
endif

GUI_SRCS += io-x11.c xrdb.c
GUI_LINK += -lX11

YFLAGS = -d
EXTRA_DIST = $(srcdir)/neoleo.i

bin_PROGRAMS = neoleo 


BUILT_SOURCES = getdate.c parse.c parse.h posixtm.c posixtm.h 
#BUILT_SOURCES += neoleo_wrap.c
CLEANFILES = $(BUILT_SOURCES)

#lib_LTLIBRARIES = libneoleo.la
neoleo_CFLAGS = $(GUI_DEFINES) -Dmain0=main -DDATADIR="\"$(datadir)\""
neoleo_LDADD = -lm -lncurses -lpthread $(GUI_LINK)
#neoleo_LDFLAGS = -e main0 
#neoleo_la_LDFLAGS = -module -avoid-version -shared
neoleo_SOURCES = afm.c args.c basic.c busi.c byte-compile.c cells.c cmd.c date.c decompile.c display.c \
		 epson.c eval.c font.c format.c forminfo.c funcs.c graph.c gsl.c hash.c help.c \
		 info.c init.c input.c \
		 io-headless.c io-curses.c io-edit.c io-term.c io-utils.c \
		 ir.c key.c legend.c line.c list.c lists.c mdi.c oleofile.c pcl.c plot.c  \
		 postscript.c print.c prtext.c ref.c regions.c sc.c sort.c string.c stub.c sylk.c utils.c \
		 window.c \
		 defuns.c \
		 get_date.h getdate.y \
		 parse.y \
		 posixtm.y \
		 neoleo_swig.c \
		 mysql.c   $(GUI_SRCS)
	

noinst_HEADERS = afm.h appres.h args.h basic.h byte-compile.h cell.h \
		 cmd.h decompile.h defun.h defuns.h display.h epson.h \
		 errors.h eval.h font.h format.h forminfo.h funcdef.h \
		 funcs.h global.h graph.h hash.h help.h info.h init.h \
		 input.h io-abstract.h io-headless.h io-curses.h io-edit.h \
		 io-generic.h io-motif.h io-term.h io-utils.h io-x11.h \
		 ir.h key.h line.h list.h lists.h mdi.h mysql.h node.h \
		 oleofile.h oleo_plot.h oleosql.h oleo_xb.h parse.h pcl.h \
		 posixtm.h postscript.h print.h proto.h prtext.h ref.h \
		 regions.h sc.h sciplot.h sciplotI.h sort.h stub.h stubs.h \
		 sylk.h sysdef.h userpref.h utils.h window.h \
		 neoleo_swig.h

# exclude these for now:
# plotter.c xbase.cpp


ref.o : parse.h

#neoleo_wrap.c : $(srcdir)/neoleo.i neoleo_swig.c neoleo_swig.h
#	swig -tcl8 -o $@ $<

