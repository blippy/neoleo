# permissive flag is set due to pesky obstack
AM_CXXFLAGS = @ASAN_CXXFLAGS@
AM_CXXFLAGS += -std=c++17
#AM_CXXFLAGS += -fpermissive
#AM_CXXFLAGS += -Wno-write-strings 
#AM_CXXFLAGS += -Wformat -Wformat-security
AM_CXXFLAGS += -DDATADIR="\"$(datadir)\""
AM_CXXFLAGS += -Wfatal-errors

YFLAGS = -d
#EXTRA_DIST = $(srcdir)/neoleo.i

bin_PROGRAMS = neoleo 


BUILT_SOURCES = getdate.cc getdate.hh parse.cc parse.hh posixtm.cc posixtm.hh 
CLEANFILES = $(BUILT_SOURCES)

#neoleo_LDADD = -lm @CURSES_LIBS@ -lpanel $(ASAN_FLAGSL)  
neoleo_LDADD = @ASAN_LIBS@ -lm @CURSES_LIBS@ -lpanel

neoleo_SOURCES = alt-cells.cc alt-lex.cc alt-parse.cc \
		 args.cc basic.cc  busi.cc byte-compile.cc \
                 cells.cc cmd.cc \
		 date.cc decompile.cc display.cc \
		 eval.cc experimental.cc format.cc funcs.cc \
		 init.cc input.cc \
		 io-headless.cc io-curses.cc io-edit.cc io-term.cc \
		 io-utils.cc io-visual.cc \
		 key.cc \
		 line.cc list.cc lists.cc logging.cc \
		 mdi.cc mem.cc \
		 obstack.cc \
		 oleofile.cc \
		 ref.cc regions.cc \
		 sort.cc stringo.cc stub.cc \
		 tbl.cc \
		 utils.cc \
		 window.cc \
		 defuns.cc \
		 get_date.h getdate.yy \
		 parse.yy \
		 posixtm.yy \
		 main.cc \
		 shell.cc \
		 tests.cc \
		 yyreglex.cc
	


noinst_HEADERS = alt-cells.h alt-lex.h alt-parse.h alt-types.h \
		 args.h basic.h byte-compile.h busi.h cell.h \
		 cmd.h date.h decompile.h defun.h defuns.h display.h \
		 errors.h eval.h format.h funcdef.h \
		 funcs.h global.h init.h \
		 input.h io-abstract.h io-headless.h io-curses.h io-edit.h \
		 io-generic.h io-term.h io-utils.h io-visual.h \
		 key.h \
		 line.h list.h lists.h logging.h \
		 mdi.h mem.h \
		 node.h  numeric.h \
		 obstack.h \
		 oleofile.h  oleox.h \
		 parse.hh parse_parse.h \
		 posixtm.hh ref.h \
		 regions.h sort.h stringo.h stub.h stubs.h \
		 sysdef.h userpref.h utils.h window.h  \
		 shell.h \
		 tbl.h tests.h 


byte-compile.o : parse.hh
ref.o : parse.hh
sc.o : parse.hh	
