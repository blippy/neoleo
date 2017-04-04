
GUI_SRCS = 
GUI_LINK =
if UseX
#GUI_DEFINES = -DX_DISPLAY_MISSING
GUI_DEFINES = -DHAVE_X 
GUI_SRCS += io-x11.cc plot.c xrdb.c
GUI_LINK += -lX11
endif

# Oops. Forth assume 64-bit. cygwin is 32-bit
#FORTH_HDRS = atlast.h atldef.h
#FORTH_SRCS = atlast.c atoleo.cc

YFLAGS = -d
#EXTRA_DIST = $(srcdir)/neoleo.i

bin_PROGRAMS = neoleo 


BUILT_SOURCES = getdate.c parse.c parse.h posixtm.c posixtm.h 
CLEANFILES = $(BUILT_SOURCES)

if  UseAsan
ASAN_FLAGSC = -fsanitize=address
ASAN_FLAGSL = -lasan
endif

COMMON_FLAGS = $(ASAN_FLAGSC) $(GUI_DEFINES)  -DDATADIR="\"$(datadir)\""
AM_CFLAGS = $(COMMON_FLAGS)
# permissive flag is set due to pesky obstack
AM_CXXFLAGS = -std=gnu++11 $(COMMON_FLAGS) -fpermissive -Wno-write-strings
neoleo_LDADD = $(ASAN_FLAGSL) -lm -lreadline -lncurses  -lpanel  $(GUI_LINK)
#neoleo_LDADD += -lasan
neoleo_SOURCES = afm.cc args.cc basic.cc  busi.cc byte-compile.cc cells.cc cmd.cc \
		 date.c decompile.c display.c \
		 eval.c font.c format.c forminfo.c funcs.c \
		 graph.c gsl.c hash.c help.c \
		 info.c init.c input.c \
		 io-headless.cc io-curses.cc io-edit.c io-term.cc io-utils.cc \
		 ir.c key.c \
		 legend.c line.c list.c lists.c logging.cc \
		 mdi.c oleofile.c \
		 ref.cc regions.c \
		 sc.c sort.c stringo.c stub.c sylk.c utils.c \
		 window.cc \
		 defuns.c \
		 get_date.h getdate.y \
		 parse.y \
		 posixtm.y \
		 mysql.c   \
		 main.cc \
		 shell.cc \
		 tests.cc \
		 $(GUI_SRCS) \
		 $(FORTH_SRCS)
	

# not present on some systems like cygwin
# Ideally obstack would be removed as a dependency anyway
neoleo_SOURCES += obstack.c

noinst_HEADERS = afm.h args.h atoleo.h basic.h byte-compile.h busi.h cell.h \
		 cmd.h date.h decompile.h defun.h defuns.h display.h \
		 errors.h eval.h font.h format.h forminfo.h funcdef.h \
		 funcs.h global.h graph.h gsl.h hash.h help.h info.h init.h \
		 input.h io-abstract.h io-headless.h io-curses.h io-edit.h \
		 io-generic.h io-term.h io-utils.h io-x11.h \
		 ir.h key.h \
		 line.h list.h lists.h logging.h \
		 mdi.h mysql.h \
		 node.h  numeric.h \
		 oleofile.h oleo_plot.h oleosql.h oleox.hpp oleo_xb.h \
		 parse.h \
		 posixtm.h proto.h ref.h \
		 regions.h sc.h sciplot.h sciplotI.h sort.h stringo.h stub.h stubs.h \
		 sylk.h sysdef.h userpref.h utils.h window.h  \
		 shell.h \
		 tests.h \
		 $(FORTH_HDRS)

noinst_HEADERS += obstack.h

# byte-compile.o : parse.h
ref.o : parse.h
