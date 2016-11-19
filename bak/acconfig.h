/* acconfig.h
   This file is in the public domain.

   Descriptive text for the C preprocessor macros that
   the distributed Autoconf macros can define.
   No software package will use all of them; autoheader copies the ones
   your configure.in uses into your configuration header file templates.

   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  Although this order
   can split up related entries, it makes it easier to check whether
   a given entry is in the file.

   Leave the following blank line there!!  Autoheader needs it.  */


/* Define if you have the Andrew File System.  */
#undef AFS

/* Define if there is a member named d_ino in the struct describing
   directory headers.  */
#undef D_INO_IN_DIRENT

/* Define to 1 if NLS is requested.  */
#undef ENABLE_NLS

/* Define to rpl_fnmatch if the replacement function should be used.  */
#undef fnmatch

/* Define as rpl_getgroups if getgroups doesn't work right.  */
#undef getgroups

/* Define to gnu_strftime if the replacement function should be used.  */
#undef strftime

/* Define if your system defines TIOCGWINSZ in sys/ioctl.h.  */
#undef GWINSZ_IN_SYS_IOCTL

/* Define as 1 if you have catgets and don't want to use GNU gettext.  */
#undef HAVE_CATGETS

/* Define as 1 if you have gettext and don't want to use GNU gettext.  */
#undef HAVE_GETTEXT

/* Define if your locale.h file contains LC_MESSAGES.  */
#undef HAVE_LC_MESSAGES

/* Define to 1 if you have the obstack functions from GNU libc.  */
#undef HAVE_OBSTACK

/* Define if your struct stat has st_dm_mode.  */
#undef HAVE_ST_DM_MODE

/* Define to 1 if you have the stpcpy function.  */
#undef HAVE_STPCPY

/* Define to `unsigned long' if <sys/types.h> doesn't define.  */
#undef ino_t

/* Define if `struct utimbuf' is declared -- usually in <utime.h>.  */
#undef HAVE_STRUCT_UTIMBUF

/* Define if `sys_errlist' and `sys_nerr' are declared.  Normally in <errno.h> */
#undef HAVE_SYS_ERRLIST

/* Define to gnu_mktime if the replacement function should be used.  */
#undef mktime

/* Define one of the following to indicate how a program can
   get a list of mounted filesystems.  */

/* Define if there is no specific function for reading the list of
   mounted filesystems.  fread will be used to read /etc/mnttab.  [SVR2]  */
#undef MOUNTED_FREAD

/* Define if (like SVR2) there is no specific function for reading the
   list of mounted filesystems, and your system has these header files:
   <sys/fstyp.h> and <sys/statfs.h>.  [SVR3]  */
#undef MOUNTED_FREAD_FSTYP

/* Define if there is a function named getfsstat for reading the list
   of mounted filesystems.  [DEC Alpha running OSF/1]  */
#undef MOUNTED_GETFSSTAT

/* Define if there is a function named getmnt for reading the list of
   mounted filesystems.  [Ultrix]  */
#undef MOUNTED_GETMNT

/* Define if there is a function named getmntent for reading the list
   of mounted filesystems, and that function takes a single argument.
   [4.3BSD, SunOS, HP-UX, Dynix, Irix]  */
#undef MOUNTED_GETMNTENT1

/* Define if there is a function named getmntent for reading the list of
   mounted filesystems, and that function takes two arguments.  [SVR4]  */
#undef MOUNTED_GETMNTENT2

/* Define if there is a function named getmntinfo for reading the list
   of mounted filesystems.  [4.4BSD]  */
#undef MOUNTED_GETMNTINFO

/* Define if there is a function named listmntent that can be used to
   list all mounted filesystems. [UNICOS] */
#undef MOUNTED_LISTMNTENT

/* Define if there is a function named mntctl that can be used to read
   the list of mounted filesystems, and there is a system header file
   that declares `struct vmount.'  [AIX]  */
#undef MOUNTED_VMOUNT

/* Define to the name of the distribution.  */
#undef PACKAGE

/* The concatenation of the strings "GNU ", and PACKAGE.  */
#undef GNU_PACKAGE

/* Define to 1 if ANSI function prototypes are usable.  */
#undef PROTOTYPES


/* Define one of the following to indicate how a program can obtain
   filesystems usage information.  */

/*  Define if  statfs takes 3 args.  [DEC Alpha running OSF/1]  */
#undef STAT_STATFS3_OSF1

/* Define if there is no specific function for reading filesystems usage
   information and you have the <sys/filsys.h> header file.  [SVR2]  */
#undef STAT_READ_FILSYS

/* Define if statfs takes 2 args and struct statfs has a field named f_bsize.
   [4.3BSD, SunOS 4, HP-UX, AIX PS/2]  */
#undef STAT_STATFS2_BSIZE

/* Define if statfs takes 2 args and struct statfs has a field named f_fsize.
   [4.4BSD, NetBSD]  */
#undef STAT_STATFS2_FSIZE

/* Define if statfs takes 2 args and the second argument has
   type struct fs_data.  [Ultrix]  */
#undef STAT_STATFS2_FS_DATA

/* Define if statfs takes 4 args.  [SVR3, Dynix, Irix, Dolphin]  */
#undef STAT_STATFS4

/* Define if there is a function named statvfs.  [SVR4]  */
#undef STAT_STATVFS

/* Define if the block counts reported by statfs may be truncated to 2GB
   and the correct values may be stored in the f_spare array.
   [SunOS 4.1.2, 4.1.3, and 4.1.3_U1 are reported to have this problem.
   SunOS 4.1.1 seems not to be affected.]  */
#undef STATFS_TRUNCATES_BLOCK_COUNTS

/* Define to the version of the distribution.  */
#undef VERSION

/* Define to 1 if GNU regex should be used instead of GNU rx.  */
#undef WITH_REGEX

/* Define to 1 if GTK is present. */
#undef HAVE_LIBGTK

#if HAVE_LOCALE_H
# include <locale.h>
#endif
#if !HAVE_SETLOCALE
# define setlocale(Category, Locale) /* empty */
#endif

/* LessTif or Motif */
#undef HAVE_MOTIF
#undef HAVE_SciPlot_H
#undef HAVE_XmHTML_H
#undef HAVE_XBAE
#undef HAVE_XLT
#undef HAVE_XLT_FONTCHOOSER
#undef HAVE_XPM

/* GNU Plotutils */
#undef	HAVE_LIBPLOT
#undef	HAVE_LIBPLOT_2_2
#undef	HAVE_LIBSCIPLOT

/* GNU scientific library */
#undef	HAVE_GSL

#if ENABLE_NLS
# define _(Text) gettext (Text)
#else
# undef bindtextdomain
# define bindtextdomain(Domain, Directory) /* empty */
# undef textdomain
# define textdomain(Domain) /* empty */
# define _(Text) Text
#endif



/* Leave that blank line there!!  Autoheader needs it.
   If you're adding to this file, keep in mind:
   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  */
