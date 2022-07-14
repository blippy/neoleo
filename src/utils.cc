/*
 * $Id: utils.c,v 1.20 2001/02/13 23:38:06 danny Exp $
 *
 * Copyright © 1990, 1992, 1993, 2001 Free Software Foundation, Inc.
 * 
 * This file is part of Oleo, the GNU Spreadsheet.
 * 
 * Oleo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * Oleo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Oleo; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <cassert>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <vector>


#include "config.h"

#include <ctype.h>
#include "utils.h"


#define E100 1E100
//#define NUM_HUNDREDTH 0.01
#define NUM_TEN 10.0

// very useful function!
// https://stackoverflow.com/questions/15106102/how-to-use-c-stdostream-with-printf-like-formatting
/*
std::string format(const std::string& format, ...)
{
	va_list args;
	va_start(args, format);
	size_t len = std::vsnprintf(NULL, 0, format.c_str(), args);
	va_end(args);
	std::vector<char> vec(len + 1);
	va_start(args, format);
	std::vsnprintf(&vec[0], len + 1, format.c_str(), args);
	va_end(args);
	return &vec[0];
}
*/


extern int sys_nerr;

struct id
{
	int flag;
	FILE *fp;
	char *name;
};

struct id *__id_s;
int __id_n;
int __id_f;

int __make_backups;
int __backup_by_copying;

/* Blow chunks! */
	void
panic (const char *s,...)
{
	va_list iggy;

	va_start (iggy, s);
	fprintf (stderr, "%s %s:", PACKAGE_NAME, VERSION);
	vfprintf (stderr, s, iggy);
	putc ('\n', stderr);
	va_end (iggy);
	exit (2);
}



	const char *
__fp_name (FILE *fp)
{
	int n;

	for (n = 0; n < __id_n; n++)
	{
		if (__id_s[n].fp == fp)
			return __id_s[n].name;
	}
	return "{Unknown file pointer}";
}

	void
__set_fp ( FILE *fp, const char *name, int flag)
{
	if (__id_s == 0)
	{
		__id_s = (id*) ck_malloc (20 * sizeof (struct id));
		__id_n = 0;
		__id_f = 20;
	}
	else
	{
		int n;

		for (n = 0; n < __id_n; n++)
			if (__id_s[n].fp == fp)
			{
				free (__id_s[n].name);
				__id_s[n] = __id_s[--__id_n];
				__id_f++;
				break;
			}
	}
	if (__id_f == 0)
	{
		__id_f = 20;
		__id_s = (id*) ck_realloc (__id_s, (__id_f + __id_n) * sizeof (struct id));
	}
	__id_s[__id_n].flag = flag;
	__id_s[__id_n].name = strdup (name);
	__id_s[__id_n].fp = fp;
	__id_n++;
	__id_f--;
}

/* Open a file or a pipe */
	FILE *
xopen ( const char *name, const char *mode)
{
	int flag = 0;
	FILE *ret;

	while (*name == ' ')
		name++;
	if (*name == '!')
	{
		name++;
		ret = popen (name, mode);
		flag = 1;
	}
	else
		ret = fopen (name, mode);
	if (ret == 0)
		return ret;
	__set_fp (ret, name, flag);
	return ret;
}

/* Open a file, creating a backup file if needed. . . */
// mcarter 2019-01-23 Eliminate the backup functionality
	FILE *
fopen_with_backup (char *name, const char *mode)
{
	FILE * ret = fopen (name, mode);
	return ret;
}

/* Open a file or a pipe, creating a backup file if it's a file */
	FILE *
xopen_with_backup (char *name, const char *mode)
{
	int flag;
	FILE *ret;

	while (*name == ' ')
		name++;
	if (*name == '|')
	{
		ret = popen (name + 1, mode);
		flag = 1;
	}
	else
	{
		ret = fopen_with_backup (name, mode);
		flag = 0;
	}
	if (ret == 0)
		return ret;
	__set_fp (ret, name, flag);
	return ret;
}

/* Close something opened with xopen. . . */
	int
xclose ( FILE *fp)
{
	int ret;
	int n;

	for (n = 0; n < __id_n; n++)
	{
		if (__id_s[n].fp == fp)
			break;
	}
	if (n == __id_n)
		panic ("Unknown file pointer %p given to xclose", fp);
	if (__id_s[n].flag)
		ret = pclose (fp);
	else
		ret = fclose (fp);
	return ret;
}


/* fopen or panic */
void *
ck_malloc( size_t size)
{
	void *ret = malloc (size);
	if (ret == (void *) 0)
		panic ("Couldn't allocate %u bytes", size);
	return ret;
}


	void
ck_free (void * mem)
{
	if (mem) free (mem);
}




	void *
ck_calloc (size_t size)
{
	void *ret  = calloc (size, 1);
	if (ret == (void *) 0)
		panic ("Couldn't allocate %u bytes", size);
	return ret;
}

/* Realloc or panic */
	void *
ck_realloc (void *ptr, size_t size)
{
	void *ret;

	if (!ptr)
		ret = malloc (size);
	else
		ret = realloc (ptr, size);
	if (ret == (void *) 0)
		panic ("Couldn't re-allocate %u bytes from %p", size, ptr);
	return ret;
}


/*
 * stricmp - compare string s1 to s2, ignoring case
 */

	int
stricmp (const char * s1, const char * s2)
{
	const char *scan1;
	const char *scan2;
	char chr1, chr2;

	scan1 = s1;
	scan2 = s2;
	do
	{
		chr1 = isupper (*scan1) ? tolower (*scan1) : *scan1;
		chr2 = isupper (*scan2) ? tolower (*scan2) : *scan2;
		scan1++;
		scan2++;
	}
	while (chr1 && chr1 == chr2);

	/*
	 * The following case analysis is necessary so that characters
	 * which look negative collate low against normal characters but
	 * high against the end-of-string NUL.
	 */
	if (chr1 == '\0' && chr2 == '\0')
		return 0;
	else if (chr1 == '\0')
		return -1;
	else if (chr2 == '\0')
		return 1;
	else
		return chr1 - chr2;
}

/* strincmp - compare first N chars of strings S1 and S2 */
	int
strincmp (const char * s1, const char * s2, size_t n)
{
	const char *scan1;
	const char *scan2;
	size_t count;
	char chr1, chr2;

	scan1 = s1;
	scan2 = s2;
	count = n;
	do
	{
		chr1 = isupper (*scan1) ? tolower (*scan1) : *scan1;
		chr2 = isupper (*scan2) ? tolower (*scan2) : *scan2;
		scan1++;
		scan2++;
	}
	while (--count != 0 && chr1 && chr1 == chr2);

	/* if (count == (size_t)-1)
	   return 0; */

	/*
	 * The following case analysis is necessary so that characters
	 * which look negative collate low against normal characters but
	 * high against the end-of-string NUL.
	 */
	if (chr1 == '\0' && chr2 == '\0')
		return 0;
	else if (chr1 == '\0')
		return -1;
	else if (chr2 == '\0')
		return 1;
	else
		return chr1 - chr2;
}

	char *
err_msg (void)
{
	int n = errno;
	static char buf[80];	/* Static to be able to return its address */


#ifdef	HAVE_STRERROR
	p = strerror(n);
	if (p)
		strcpy(buf, p);
	else
		sprintf (buf, "Unknown error code %d (%#x)", n, n);
#else
#if HAVE_SYS_ERRLIST
	/* This was #if-fed away. Why ? */
	if (n < sys_nerr);
	return sys_errlist[n];
#endif

	sprintf (buf, "Unknown error code %d (%#x)", n, n);
#endif	/* HAVE_STRERROR */

	return buf;
}






/*
 *	astof - accept a number of the form:
 *		(and ignores leading white space)
 *
 *	null	::=
 *	digit	::= 0|1|2|3|4|5|6|7|8|9
 *	digits	::= <digit>*
 *	DIGITS	::= <digit>+
 *	sign	::= <null> | + | -
 *	-------------------------------
 *		accepted:
 *	-------------------------------
 *	integer	::= <sign><DIGITS>
 *	real	::= <integer> . <digits> | <null> . <DIGITS>
 *	epart	::= e <integer> | E <integer>
 *	float	::= <integer> <epart> | <real> <epart>
 *
 *	Always returned as a double
 *
 *	There is no particular attempt to reduce mpys/divs
 *	those machines that are still out there (eg. PDP11/Small)
 *	that shun floating point arithmetic might rethink this routine.
 *
 *	mcarter 23-Dec-2016: I'm not sure we need this function anymore.
 *	I wrote a much simplified astof(). I'm not sure why this one
 *	is needed.
 */

static num_t exps0[10] = {1E0, 1E1, 1E2, 1E3, 1E4, 1E5, 1E6, 1E7, 1E8, 1E9};
static num_t exps1[10] = {1E00, 1E10, 1E20, 1E30 ,1E40, 1E50, 1E60, 1E70, 1E80, 1E90 };


	num_t
astof (char **sp)
{
	char *s;
	char *cp;
	long ipart, epart;
	int neg = 0;
	num_t res;
	int n;

	s = *sp;
	while (isspace (*s))
	{
		s++;
		if (*s == '\0')
		{
			*sp = s;
			return (0.0);
		}
	}
	// Need to handle sign here due to '-.3' or '-0.3' 
	if (*s == '-')
	{
		++neg;
		++s;
	}
	else if (*s == '+')
		++s;
	cp = s;
	// 	get ipart handling '.n' case 
	res = 0.0;
	while (isdigit (*s))
	{
		for (n = 0, ipart = 0; n < 6 && isdigit (*s); n++)
			ipart = ipart * 10 + *s++ - '0';
		res = res * exps0[n] + (num_t) ipart;
	}
	if (s == cp)
	{
		if (*s == '.')
			ipart = 0;
		else
		{
			*sp = s;
			return (0.0);
		}
	}
	/*
	 *	either we find a '.' or e|E or done
	 */
	if (*s == '.')
	{
		int m;
		++s;

		m = 0;
		while (isdigit (*s))
		{
			for (n = 0, ipart = 0; n < 6 && isdigit (*s); n++)
				ipart = ipart * 10 + *s++ - '0';
			m += n;
			if (m >= 100)
				continue;
			if (m >= 10)
				res += ((num_t) ipart) / (exps1[m / 10] * exps0[m % 10]);
			else
				res += ((num_t) ipart) / exps0[m];
		}
	}
	/*
	 *	In either case (.) handle E part
	 */
	if (*s == 'e' || *s == 'E')
	{
		int eneg;

		++s;
		epart = 0;
		eneg = 0;
		if (*s == '-')
		{
			eneg++;
			s++;
		}
		else if (*s == '+')
			s++;
		while (isdigit (*s))
			epart = epart * 10 + *s++ - '0';
		if (eneg)
		{
#ifndef vax
			while (epart >= 100)
			{
				res /= E100;
				epart -= 100;
			}
#endif
			if (epart > 9)
			{
				res /= exps1[epart / 10];
				epart %= 10;
			}
			if (epart)
				res /= exps0[epart];
		}
		else
		{
#ifndef vax
			while (epart >= 100)
			{
				res *= E100;
				epart -= 100;
			}
#endif
			if (epart > 9)
			{
				res *= exps1[epart / 10];
				epart %= 10;
			}
			if (epart)
				res *= exps0[epart];
		}
	}
	/*
	 *	fix sign
	 */
	if (neg)
		res = -res;
	*sp = s;
	return (res);
}


	std::string 
spaces(int n)
{
	n = std::max(0, n);
	char sa[n+1];
	std::fill(sa, sa+n, ' ');
	sa[n] = '\0';
	return sa; 
}

	std::string 
pad_left(const std::string& s, int width)
{
	return spaces(width-s.size()) + s;
}

	std::string
pad_right(const std::string& s, int width)
{
	return s + spaces(width-s.size());
}

size_t strlen(const std::string& s)
{
	return s.size();
}
