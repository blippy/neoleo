# Order of linking of libraries for Motif seems to be important

if HAVE_XBAE
GUI = io-motif.c appres.c fallback.c oleo_icon.xpm
GUI_LINK = -lXm -lXt -lXbae
GUI_DEFINES = -DHAVE_MOTIF
else
GUI = io-x11.c
GUI_LINK = 
endif

YFLAGS = -d

bin_PROGRAMS = neoleo

CLEANFILES = parse.c parse.h posixtm.c posixtm.h

neoleo_CFLAGS = $(GUI_DEFINES)
neoleo_LDADD = -lm -lncurses ../lib/liboleo.a $(GUI_LINK) -lX11
neoleo_SOURCES = afm.c args.c basic.c busi.c byte-compile.c cells.c cmd.c date.c decompile.c display.c \
		 epson.c eval.c font.c format.c forminfo.c funcs.c graph.c gsl.c hash.c help.c \
		 info.c init.c input.c io-curses.c io-edit.c io-term.c io-utils.c \
		 ir.c key.c legend.c line.c list.c lists.c mdi.c oleofile.c pcl.c plot.c  parse.y \
		 postscript.c print.c prtext.c ref.c regions.c sc.c sort.c string.c stub.c sylk.c utils.c \
		 window.c \
		 posixtm.y \
		 defuns.c \
		 mysql.c \
		 xrdb.c \
		 $(GUI)
	

noinst_HEADERS = afm.h appres.h args.h basic.h byte-compile.h cell.h \
		 cmd.h decompile.h defun.h defuns.h display.h epson.h \
		 errors.h eval.h font.h format.h forminfo.h funcdef.h \
		 funcs.h global.h graph.h hash.h help.h info.h init.h \
		 input.h io-abstract.h io-curses.h io-edit.h \
		 io-generic.h io-motif.h io-term.h io-utils.h io-x11.h \
		 ir.h key.h line.h list.h lists.h mdi.h mysql.h node.h \
		 oleofile.h oleo_plot.h oleosql.h oleo_xb.h parse.h pcl.h \
		 posixtm.h postscript.h print.h proto.h prtext.h ref.h \
		 regions.h sc.h sciplot.h sciplotI.h sort.h stub.h stubs.h \
		 sylk.h sysdef.h userpref.h utils.h window.h

# exclude these for now:
# appres.c io-motif.c plotter.c xbase.cpp


ref.o : parse.h
