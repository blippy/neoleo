/*
 * Copyright (c) 1990, 1992, 1993, 2001 Free Software Foundation, Inc.
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



#include "utils.h"

import value;


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
