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


#include "neotypes.h"
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




std::string spaces(int n)
{
	n = std::max(0, n);
	char sa[n+1];
	std::fill(sa, sa+n, ' ');
	sa[n] = '\0';
	return sa; 
}

// FN pad_left .
std::string pad_left(const std::string& s, int width)
{
	return spaces(width-s.size()) + s;
}
// FN-END

// FN pad_right .
std::string pad_right(const std::string& s, int width)
{
	return s + spaces(width-s.size());
}
// FN-END

// FN pad_centre .
std::string pad_centre(const std::string& s, int width)
{
	return pad_left(pad_right(s, width/2), width);
}
// FN-END

// FN pad_jst .
std::string pad_jst(const std::string& s, int width, int jst)
{
	std::string txt{s};
	switch(jst) {
		case JST_DEF:
		case JST_RGT:
			txt = pad_left(txt, width);
			break;
		case JST_LFT:
			txt = pad_right(txt, width);
			break;
		case JST_CNT:
			txt = pad_centre(txt, width);
			break;
	}
	return txt;
}
// FN-END
