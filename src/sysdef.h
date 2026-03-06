#ifndef SYSDEFH
#define SYSDEFH
/*
 * $Id: sysdef.h,v 1.8 2011/07/05 00:16:13 delqn Exp $
 *
 * Copyright © 1992, 1993 Free Software Foundation, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <sys/types.h>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <sys/stat.h>
  
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
extern int access (char *, int);
extern int getuid (void);
#endif /* HAVE_UNISTD_H */

#ifndef _DEBUG_MALLOC_INC
#if defined(STDC_HEADERS) || defined(HAVE_STRING_H)
#include <string.h>
/* An ANSI string.h and pre-ANSI memory.h might conflict.  */
#if !defined(STDC_HEADERS) && HAVE_MEMORY_H
#include <memory.h>
#endif /* not STDC_HEADERS and HAVE_MEMORY_H */
#ifndef index
#define index strchr
#endif
#ifndef rindex
#define rindex strrchr
#endif
#ifndef bcopy
#define bcopy(s, d, n) memcpy ((d), (s), (n))
#endif
#ifndef bcmp
#define bcmp(s1, s2, n) memcmp ((s1), (s2), (n))
#endif
#ifndef bzero
#define bzero(s, n) memset ((s), 0, (n))
#endif
#else /* not STDC_HEADERS and not HAVE_STRING_H */
#include <strings.h>
/* memory.h and strings.h conflict on some systems.  */
#endif /* not STDC_HEADERS and not HAVE_STRING_H */
#endif /* _DEBUG_MALLOC_INC */


#ifdef STDC_HEADERS
#include <stdlib.h>
#endif
#include <math.h>
#include <time.h>
#include <signal.h>

#ifdef atarist
#undef SIGIO
#ifndef EWOULDBLOCK
#define EWOULDBLOCK (-1)
#endif
#endif

#ifndef HAVE_STRFTIME
extern size_t strfntime (char *, int, char * format, struct tm *);
#endif

#ifndef HAVE_MKTIME
extern time_t mktime (struct tm *);
#endif

extern struct tm * localtime (const time_t *);
extern struct tm * gmtime (const time_t *);

#ifdef __STDC__
#ifndef _DEBUG_MALLOC_INC
#ifndef HAVE_STRDUP
extern char *strdup (const char *);
#endif
#ifndef HAVE_STRICMP
extern int stricmp (const char *, const char *);
#endif
#ifndef HAVE_STRINCMP
extern int strincmp (const char *, const char *, size_t);
#endif
#ifndef HAVE_STRSTR
extern char *strstr (const char *, const char *);
#endif
#endif /* _DEBUG_MALLOC_INC */

#else  /* !defined(__STDC__) */

#ifndef _DEBUG_MALLOC_INC
#ifndef HAVE_STRDUP
extern char *strdup ();
#endif
#ifndef HAVE_STRSTR
extern char *strstr ();
#endif
#endif /* _DEBUG_MALLOC_INC */

#endif /* !defined(__STDC__) */

#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif /* RETSIGTYPE */

#ifndef VOIDSTAR
#define VOIDSTAR void *
#endif

#ifndef __STDC__
#define volatile
#endif

#endif
