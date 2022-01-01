AM_CXXFLAGS = @ASAN_CXXFLAGS@
AM_CXXFLAGS += -Werror # activate to make warnings abort the compile
AM_CXXFLAGS += -std=c++17
#AM_CXXFLAGS += -Wno-psabi # turn off ABI 7.1 changes
#AM_CXXFLAGS += -Wno-write-strings # activate to supress warnings about const char*
AM_CXXFLAGS += -DDATADIR="\"$(datadir)\""
AM_CXXFLAGS += @COVERAGE_CXXFLAGS@
AM_CXXFLAGS += @FORTIFY_CXXFLAGS@
AM_CXXFLAGS += -Wall
AM_CXXFLAGS += -Wfatal-errors
AM_CXXFLAGS += -Wno-psabi # ignore warnings about GCC 7.1 ABI breakage

# try to add in some stuff that Arch trips up on
AM_CXXFLAGS += -D_FORTIFY_SOURCE=2 -Wformat -Werror=format-security 
AM_CXXFLAGS += -D_GLIBCXX_ASSERTIONS


#YFLAGS = -d
#EXTRA_DIST = $(srcdir)/neoleo.i

bin_PROGRAMS = neoleo 
dist_man_MANS = neoleo.1

#BUILT_SOURCES = parse.cc parse.hh
#CLEANFILES = $(BUILT_SOURCES)

neoleo_LDADD = @COVERAGE_LDADD@ @ASAN_LIBS@ -lm -lncurses -lpanel -lform


neoleo_SOURCES = \
		 basic.cc \
                 cell.cc cmd.cc  convert.cc \
		 errors.cc format.cc \
		 input.cc \
		 io-2019.cc io-headless.cc io-curses.cc io-term.cc \
		 io-utils.cc \
		 logging.cc \
		 oleofile.cc \
		 parser-2019.cc \
		 ref.cc regions.cc \
		 sheet.cc spans.cc \
		 tbl.cc \
		 utils.cc \
		 window.cc \
		 main.cc \
		 tests.cc \
		 value.cc 
	


noinst_HEADERS = basic.h cell.h \
		 cmd.h convert.h  \
		 errors.h format.h \
		 global.h \
		 input.h \
		 io-2019.h io-abstract.h io-headless.h io-curses.h \
		 io-term.h io-utils.h \
		 logging.h \
		 neotypes.h  \
		 oleofile.h  \
		 parser-2019.h \
		 ref.h regions.h \
		 sheet.h spans.h \
		 tbl.h tests.h \
		 utils.h \
		 value.h \
		 window.h

