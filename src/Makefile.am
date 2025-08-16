AM_CXXFLAGS = @ASAN_CXXFLAGS@
AM_CXXFLAGS += -std=c++23
#AM_CXXFLAGS += -Wno-psabi # turn off ABI 7.1 changes
#AM_CXXFLAGS += -Wno-write-strings # activate to supress warnings about const char*
AM_CXXFLAGS += -DDATADIR="\"$(datadir)\""
AM_CXXFLAGS += @COVERAGE_CXXFLAGS@
AM_CXXFLAGS += @FORTIFY_CXXFLAGS@ # simulate Arch Linux build (--with-fortify)

AM_CXXFLAGS += -Werror=format-security 	-fstack-clash-protection -fcf-protection -Wp,-D_GLIBCXX_ASSERTIONS



AM_CXXFLAGS += -Wuninitialized
#AM_CXXFLAGS += -Wfatal-errors  # aborts compilation on first error
AM_CXXFLAGS += -Wno-psabi # ignore warnings about GCC 7.1 ABI breakage
AM_CXXFLAGS += -Wall 
#AM_CXXFLAGS += -Wuse-after-free # Slackware 15.0 doesn't like this flag

#AM_CXXFLAGS += -Werror # treats warnings as errors. See "-Werror is not your friend":
# https://embeddedartistry.com/blog/2017/05/22/werror-is-not-your-friend/


#YFLAGS = -d
#EXTRA_DIST = $(srcdir)/neoleo.i

bin_PROGRAMS = neoleo 
dist_man_MANS = neoleo.1

#BUILT_SOURCES = parse.cc parse.hh
#CLEANFILES = $(BUILT_SOURCES)

neoleo_LDADD = @COVERAGE_LDADD@ @ASAN_LIBS@ -lm 
neoleo_LDADD += @NCURSES_LIBS@ -lpanel -lform


neoleo_SOURCES = \
		 basic.cc \
         cell.cc \
		 io-2019.cc io-headless.cc io-curses.cc \
		 menu-2025.cc \
		 oleofile.cc \
		 parser-2019.cc \
		 regions.cc \
		 sheet.cc spans.cc \
		 utils.cc \
		 win.cc \
		 main.cc \
		 tests.cc
	


noinst_HEADERS = basic.h cell.h \
		 io-2019.h io-curses.h \
		 neotypes.h  \
		 oleofile.h  \
		 parser-2019.h \
		 regions.h \
		 sheet.h spans.h \
		 tests.h \
		 win.h

