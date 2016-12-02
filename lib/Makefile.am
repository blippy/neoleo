#
# $Header: /cvs/oleo/lib/Makefile.am,v 1.12 2001/02/08 00:21:23 pw Exp $
#
# Process this file with automake to produce Makefile.in
#
#noinst_LIBRARIES = liboleo.la
lib_LTLIBRARIES = liboleo.la
#
# The list of files here is incomplete :
#	configure.in can add mktime.c and obstack.c through the LIBOBJS
#	mechanism
#
liboleo_la_SOURCES = getdate.y getopt.c getopt1.c random.c
liboleo_la_LDFLAGS = -module -avoid-version -shared
liboleo_la_LIBADD = @LIBOBJS@

noinst_HEADERS = getopt.h get_date.h obstack.h

AM_CPPFLAGS = -I.. -I$(srcdir) -I../intl

CLEANFILES = getdate.c
#MAINTAINERCLEANFILES = Makefile.in

