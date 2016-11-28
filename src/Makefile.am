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
	


# exclude these for now:
# appres.c io-motif.c plotter.c xbase.cpp


ref.o : parse.h
