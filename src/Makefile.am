

YFLAGS = -d
#EXTRA_DIST = $(srcdir)/neoleo.i

bin_PROGRAMS = neoleo 


BUILT_SOURCES = getdate.cc getdate.hh parse.cc parse.hh posixtm.cc posixtm.hh 
CLEANFILES = $(BUILT_SOURCES)

if  UseAsan
ASAN_FLAGSC = -fsanitize=address
ASAN_FLAGSL = -lasan
endif

COMMON_FLAGS = $(ASAN_FLAGSC) -DDATADIR="\"$(datadir)\""
#AM_CFLAGS = $(COMMON_FLAGS)
# permissive flag is set due to pesky obstack
AM_CXXFLAGS = -std=gnu++11 -fpermissive -Wno-write-strings $(COMMON_FLAGS) $(GPROF_CXXFLAGS) $(DEBUG_CXXFLAGS)
neoleo_LDADD = $(ASAN_FLAGSL) -lm -lncurses -lpanel
#neoleo_LDADD += -lasan
neoleo_SOURCES = args.cc basic.cc  busi.cc byte-compile.cc \
                 cells.cc cmd.cc \
		 date.cc decompile.cc display.cc \
		 eval.cc format.cc forminfo.cc funcs.cc \
		 gsl.cc hash.cc help.cc \
		 info.cc init.cc input.cc \
		 io-headless.cc io-curses.cc io-edit.cc io-term.cc io-utils.cc \
		 ir.cc key.cc \
		 line.cc list.cc lists.cc logging.cc \
		 mdi.cc oleofile.cc \
		 ref.cc regions.cc \
		 sc.cc sort.cc stringo.cc stub.cc sylk.cc utils.cc \
		 window.cc \
		 defuns.cc \
		 get_date.h getdate.yy \
		 parse.yy \
		 posixtm.yy \
		 main.cc \
		 shell.cc \
		 tests.cc 
	

# not present on some systems like cygwin
# Ideally obstack would be removed as a dependency anyway
neoleo_SOURCES += obstack_a.cc

noinst_HEADERS = args.h basic.h byte-compile.h busi.h cell.h \
		 cmd.h date.h decompile.h defun.h defuns.h display.h \
		 errors.h eval.h format.h forminfo.h funcdef.h \
		 funcs.h global.h gsl.h hash.h help.h info.h init.h \
		 input.h io-abstract.h io-headless.h io-curses.h io-edit.h \
		 io-generic.h io-term.h io-utils.h \
		 ir.h key.h \
		 line.h list.h lists.h logging.h \
		 mdi.h \
		 node.h  numeric.h \
		 oleofile.h oleo_plot.h oleosql.h oleox.hpp oleo_xb.h \
		 parse.hh \
		 posixtm.hh proto.h ref.h \
		 regions.h sc.h sort.h stringo.h stub.h stubs.h \
		 sylk.h sysdef.h userpref.h utils.h window.h  \
		 shell.h \
		 tests.h \
		 $(FORTH_HDRS)

noinst_HEADERS += obstack_a.h

byte-compile.o : parse.hh
ref.o : parse.hh
sc.o : parse.hh	

#parse.cc parse.hh : parse.y
#	bison  -d parse.y -o parse.cc
#	rm -f parse.c

#posixtm.cc posixxtm.hh : posixtm.y
#	bison -d posixtm.y -o posixtm.cc
#	rm -f posixtm.c
