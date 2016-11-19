YFLAGS = -d

bin_PROGRAMS = neoleo

CLEANFILES = parse.c parse.h posixtm.c posixtm.h

neoleo_LDADD = -lm -lncurses ../lib/liboleo.a
neoleo_SOURCES = afm.c args.c basic.c busi.c byte-compile.c cells.c cmd.c date.c decompile.c display.c \
		 epson.c eval.c fallback.c font.c format.c forminfo.c funcs.c graph.c gsl.c hash.c help.c \
		 info.c init.c input.c io-curses.c io-edit.c io-term.c io-utils.c \
		 ir.c key.c legend.c line.c list.c lists.c mdi.c oleofile.c pcl.c plot.c  parse.y \
		 postscript.c print.c prtext.c ref.c regions.c sc.c sort.c string.c stub.c sylk.c utils.c \
		 window.c \
		 posixtm.y \
		 defuns.c \
		 mysql.c


# exclude these for now:
# appres.c io-motif.c io-x11.c plotter.c xrdb.c


ref.o : parse.h
