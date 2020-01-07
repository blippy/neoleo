AM_CXXFLAGS = @ASAN_CXXFLAGS@
AM_CXXFLAGS += -Werror # activate to make warnings abort the compile
AM_CXXFLAGS += -std=c++17
AM_CXXFLAGS += -Wno-write-strings # activate to supress warnings about const char*
AM_CXXFLAGS += -DDATADIR="\"$(datadir)\""
AM_CXXFLAGS += @COVERAGE_CXXFLAGS@
AM_CXXFLAGS += -Wfatal-errors

#YFLAGS = -d
#EXTRA_DIST = $(srcdir)/neoleo.i

bin_PROGRAMS = neoleo 
dist_man_MANS = neoleo.1

#BUILT_SOURCES = parse.cc parse.hh
#CLEANFILES = $(BUILT_SOURCES)

neoleo_LDADD = @COVERAGE_LDADD@ @ASAN_LIBS@ -lm @CURSES_LIBS@ -lpanel -lform


neoleo_SOURCES = \
		 args.cc basic.cc \
                 cell.cc cmd.cc  convert.cc \
		 display.cc \
		 errors.cc format.cc \
		 input.cc \
		 io-2019.cc io-headless.cc io-curses.cc io-edit.cc io-term.cc \
		 io-utils.cc \
		 key.cc \
		 logging.cc \
		 mem.cc \
		 oleofile.cc \
		 parser-2019.cc \
		 ref.cc regions.cc \
		 sheet.cc sort.cc spans.cc \
		 tbl.cc \
		 utils.cc \
		 window.cc \
		 main.cc \
		 shell.cc \
		 tests.cc \
		 value.cc 
	


noinst_HEADERS = args.h basic.h cell.h \
		 cmd.h convert.h  display.h \
		 errors.h eval.h format.h \
		 global.h \
		 input.h \
		 io-2019.h io-abstract.h io-headless.h io-curses.h io-edit.h \
		 io-generic.h io-term.h io-utils.h \
		 key.h \
		 logging.h \
		 mem.h \
		 neotypes.h node.h  numeric.h \
		 oleofile.h  \
		 parser-2019.h \
		 ref.h \
		 regions.h sheet.h sort.h spans.h \
		 userpref.h utils.h window.h  \
		 shell.h \
		 tbl.h tests.h \
		 value.h


#byte-compile.o : parse.hh
#ref.o : parse.hh
