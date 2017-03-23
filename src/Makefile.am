GUI_SRCS = 
GUI_LINK = -lreadline
#GUI_DEFINES = -DX_DISPLAY_MISSING
GUI_DEFINES = -DHAVE_X 

# Order of linking of libraries for Motif seems to be important

if UseMotif
GUI_SRCS += io-motif.cc appres.c fallback.c oleo_icon.xpm
GUI_LINK += -lXm -lXt -lXbae 
GUI_DEFINES += -DHAVE_MOTIF
endif

GUI_SRCS += io-x11.cc xrdb.c
GUI_LINK += -lX11

FORTH_HDRS = atlast.h atldef.h
FORTH_SRCS = atlast.c atoleo.cc

YFLAGS = -d
EXTRA_DIST = $(srcdir)/neoleo.i

bin_PROGRAMS = neoleo 


BUILT_SOURCES = getdate.c parse.c parse.h posixtm.c posixtm.h 
CLEANFILES = $(BUILT_SOURCES)

AM_CFLAGS = $(GUI_DEFINES)  -DDATADIR="\"$(datadir)\""
AM_CXXFLAGS = $(GUI_DEFINES)  -DDATADIR="\"$(datadir)\""
neoleo_LDADD = -lm -lncurses -lpthread $(GUI_LINK)
neoleo_SOURCES = afm.c args.c basic.c  busi.c byte-compile.c cells.c cmd.cc \
		 date.c decompile.c display.c \
		 epson.c eval.c font.c format.c forminfo.c funcs.c \
		 graph.c gsl.c hash.c help.c \
		 info.c init.c input.c \
		 io-headless.c io-curses.c io-edit.c io-term.cc io-utils.c \
		 ir.c key.c legend.c line.c list.c lists.c mdi.c oleofile.c \
		 pcl.c plot.c  postscript.c print.c prtext.c \
		 ref.c regions.c \
		 sc.c sort.c string.c stub.c sylk.c utils.c \
		 window.c \
		 defuns.c \
		 get_date.h getdate.y \
		 parse.y \
		 posixtm.y \
		 neoleo_swig.c \
		 mysql.c   \
		 main.cc \
		 shell.cc \
		 $(GUI_SRCS) \
		 $(FORTH_SRCS)
	

noinst_HEADERS = afm.h appres.h args.h basic.h byte-compile.h cell.h \
		 cmd.h decompile.h defun.h defuns.h display.h epson.h \
		 errors.h eval.h font.h format.h forminfo.h funcdef.h \
		 funcs.h global.h graph.h hash.h help.h info.h init.h \
		 input.h io-abstract.h io-headless.h io-curses.h io-edit.h \
		 io-generic.h io-motif.h io-term.h io-utils.h io-x11.h \
		 ir.h key.h line.h list.h lists.h mdi.h mysql.h \
		 node.h  numeric.h \
		 oleofile.h oleo_plot.h oleosql.h oleo_xb.h parse.h pcl.h \
		 posixtm.h postscript.h print.h proto.h prtext.h ref.h \
		 regions.h sc.h sciplot.h sciplotI.h sort.h stub.h stubs.h \
		 sylk.h sysdef.h userpref.h utils.h window.h \
		 shell.h \
		 neoleo_swig.h \
		 $(FORTH_HDRS)

# exclude these for now:
# plotter.c xbase.cpp


ref.o : parse.h
