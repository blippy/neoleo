# permissive flag is set due to pesky obstack
#COV = --coverage
AM_CXXFLAGS = @ASAN_CXXFLAGS@

AM_CXXFLAGS += -Werror # activate to make warnings abort the compile


AM_CXXFLAGS += -std=c++17
#AM_CXXFLAGS += -fpermissive
AM_CXXFLAGS += -Wno-write-strings # activate to supress warnings about const char*
#AM_CXXFLAGS += -Wformat -Wformat-security
AM_CXXFLAGS += -DDATADIR="\"$(datadir)\""
AM_CXXFLAGS += @COVERAGE_CXXFLAGS@

#AM_CXXFLAGS += -Wfatal-errors 
#AM_CXXFLAGS += $(COV)
#AM_CXXFLAGS += $(CODE_COVERAGE_CXXFLAGS)

YFLAGS = -d
#EXTRA_DIST = $(srcdir)/neoleo.i

bin_PROGRAMS = neoleo 
#man_MANS = neoleo.1
dist_man_MANS = neoleo.1

BUILT_SOURCES = getdate.cc getdate.hh parse.cc parse.hh posixtm.cc posixtm.hh 
CLEANFILES = $(BUILT_SOURCES)

#neoleo_LDADD = -lm @CURSES_LIBS@ -lpanel $(ASAN_FLAGSL)  
neoleo_LDADD = @COVERAGE_LDADD@ @ASAN_LIBS@ -lm @CURSES_LIBS@ -lpanel
#neoleo_LDFLAGS = $(COV)
#neoleo_LIBS= $(CODE_COVERAGE_LIBS)

# Optional component: neobasic:
#if 0
#BLANG_SRCS = blang-lex.cc blang-parse.cc
#BLANG_HDRS = blang-lex.h  blang-parse.h
#AM_CXXFLAGS += -DBLANG
#endif

neoleo_SOURCES = defuns.cc \
		 $(BLANG_SRCS) \
		 args.cc basic.cc busi.cc byte-compile.cc \
                 cell.cc cmd.cc  convert.cc \
		 date.cc decompile.cc display.cc \
		 eval.cc format.cc funcs.cc \
		 init.cc input.cc \
		 io-headless.cc io-curses.cc io-edit.cc io-term.cc \
		 io-utils.cc io-visual.cc \
		 key.cc \
		 line.cc logging.cc \
		 mem.cc \
		 obstack.cc \
		 oleofile.cc \
		 ref.cc regions.cc \
		 sheet.cc sort.cc spans.cc stringo.cc stub.cc \
		 tbl.cc \
		 utils.cc \
		 window.cc \
		 get_date.h getdate.yy \
		 parse.yy \
		 posixtm.yy \
		 main.cc \
		 shell.cc \
		 tests.cc \
		 value.cc \
		 yyreglex.cc
	


noinst_HEADERS = args.h basic.h byte-compile.h busi.h cell.h \
		 $(BLANG_HDRS) \
		 cmd.h convert.h date.h decompile.h defun.h defuns.h display.h \
		 errors.h eval.h format.h \
		 funcs.h global.h init.h \
		 input.h io-abstract.h io-headless.h io-curses.h io-edit.h \
		 io-generic.h io-term.h io-utils.h io-visual.h \
		 key.h \
		 line.h logging.h \
		 mem.h \
		 neotypes.h node.h  numeric.h \
		 obstack.h \
		 oleofile.h  oleox.h \
		 parse.hh parse_parse.h \
		 posixtm.hh ref.h \
		 regions.h sheet.h sort.h spans.h stringo.h stub.h stubs.h \
		 userpref.h utils.h window.h  \
		 shell.h \
		 tbl.h tests.h \
		 value.h


byte-compile.o : parse.hh
ref.o : parse.hh
sc.o : parse.hh	
