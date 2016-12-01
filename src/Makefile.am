# Order of linking of libraries for Motif seems to be important
# I have decided to mandate the use of the Xbae library, rather than
# have it optional.

if UseMotif
GUI = io-motif.c appres.c fallback.c xrdb.c oleo_icon.xpm
GUI_LINK = -lXm -lXt -lXbae -lX11
GUI_DEFINES = -DHAVE_MOTIF
endif


YFLAGS = -d

bin_PROGRAMS = neoleo

CLEANFILES = parse.c parse.h posixtm.c posixtm.h

neoleo_CFLAGS = $(GUI_DEFINES) -DX_DISPLAY_MISSING
neoleo_LDADD = -lm -lncurses ../lib/liboleo.a $(GUI_LINK)
neoleo_SOURCES = afm.c args.c basic.c busi.c byte-compile.c cells.c cmd.c date.c decompile.c display.c \
		 epson.c eval.c font.c format.c forminfo.c funcs.c graph.c gsl.c hash.c help.c \
		 info.c init.c input.c io-curses.c io-edit.c io-term.c io-utils.c \
		 ir.c key.c legend.c line.c list.c lists.c mdi.c oleofile.c pcl.c plot.c  parse.y \
		 postscript.c print.c prtext.c ref.c regions.c sc.c sort.c string.c stub.c sylk.c utils.c \
		 window.c \
		 posixtm.y \
		 defuns.c \
		 mysql.c   $(GUI)
	

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
# io-x11.c plotter.c xbase.cpp


ref.o : parse.h
