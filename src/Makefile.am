AM_CXXFLAGS = @ASAN_CXXFLAGS@
AM_CXXFLAGS += -std=c++23
#AM_CXXFLAGS += -Wno-psabi # turn off ABI 7.1 changes
#AM_CXXFLAGS += -Wno-write-strings # activate to supress warnings about const char*
#AM_CXXFLAGS += -DDATADIR="\"$(datadir)\""
AM_CXXFLAGS += @COVERAGE_CXXFLAGS@
AM_CXXFLAGS += @FORM_CXXFLAGS@
AM_CXXFLAGS += @FORTIFY_CXXFLAGS@ # simulate Arch Linux build (--with-fortify)

AM_CXXFLAGS += -Werror=format-security 	-fstack-clash-protection  -Wp,-D_GLIBCXX_ASSERTIONS

# Following won't work on Arms:
#AM_CXXFLAG += -fcf-protection



AM_CXXFLAGS += -Wuninitialized
#AM_CXXFLAGS += -Wfatal-errors  # aborts compilation on first error
AM_CXXFLAGS += -Wno-psabi # ignore warnings about GCC 7.1 ABI breakage
AM_CXXFLAGS += -Wall 
#AM_CXXFLAGS += -Wuse-after-free # Slackware 15.0 doesn't like this flag

#AM_CXXFLAGS += -Werror # treats warnings as errors. See "-Werror is not your friend":
# https://embeddedartistry.com/blog/2017/05/22/werror-is-not-your-friend/


#YFLAGS = -d
#EXTRA_DIST = $(srcdir)/neoleo.i

bin_PROGRAMS = neoleo neotest
dist_man_MANS = neoleo.1

#BUILT_SOURCES = parse.cc parse.hh
#CLEANFILES = $(BUILT_SOURCES)

neoleo_LDADD = @COVERAGE_LDADD@ @ASAN_LIBS@ -lm 
neoleo_LDADD += @NCURSES_LIBS@ 
neoleo_LDADD += @FORM_LIBS@ 
neoleo_LDADD += -lstdc++exp # for stacktrace in blang


neoleo_SOURCES = \
		 basic.cc \
		 blang2.cc \
		 blx.cc \
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
	


noinst_HEADERS = basic.h blang2.h blx.h cell.h \
		 io-2019.h io-curses.h \
		 menu-2025.h \
		 neotypes.h  \
		 oleofile.h  \
		 parser-2019.h \
		 regions.h \
		 sheet.h spans.h \
		 tests.h \
		 win.h


neotest_SOURCES = neotest.cc \
	cell.cc parser-2019.cc regions.cc sheet.cc spans.cc utils.cc
