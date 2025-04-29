/*
 * $Id: utils.c,v 1.20 2001/02/13 23:38:06 danny Exp $
 *
 * Copyright � 1990, 1992, 1993, 2001 Free Software Foundation, Inc.
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
#include <ctype.h>

#include "utils.h"

import value;

#define E100 1E100
//#define NUM_HUNDREDTH 0.01
#define NUM_TEN 10.0


// 25/4 You can use it like
// auto v = to_int(mystr);
std::optional<int> to_int(const std::string& str)
{
	try {
		return stoi(str);
	} catch(...) {
		return std::nullopt;
	}
}


#if 0
//extern int sys_nerr;

struct id
{
	int flag;
	FILE *fp;
	char *name;
};

//struct id *__id_s;
//int __id_n;
//int __id_f;

//int __make_backups = 1;
//int __backup_by_copying;




const char *__fp_name (FILE *fp)
{
	int n;

	for (n = 0; n < __id_n; n++)
	{
		if (__id_s[n].fp == fp)
			return __id_s[n].name;
	}
	return "{Unknown file pointer}";
}



/* Open a file, creating a backup file if needed. . . */
// mcarter 2019-01-23 Eliminate the backup functionality
FILE *fopen_with_backup (char *name, const char *mode)
{
	FILE * ret = fopen (name, mode);
	return ret;
}
#endif


/*
 * stricmp - compare string s1 to s2, ignoring case
 */

int stricmp (const char * s1, const char * s2)
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
int strincmp (const char * s1, const char * s2, size_t n)
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




std::string spaces(int n)
{
	n = std::max(0, n);
	char sa[n+1];
	std::fill(sa, sa+n, ' ');
	sa[n] = '\0';
	return sa; 
}

std::string pad_left(const std::string& s, int width)
{
	return spaces(width-s.size()) + s;
}

std::string pad_right(const std::string& s, int width)
{
	return s + spaces(width-s.size());
}

size_t strlen(const std::string& s)
{
	return s.size();
}
