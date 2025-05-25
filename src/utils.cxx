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

module;

#include "neotypes.h"
//#include "utils.h"

export module utl;

import std;

import value;



export class defer {
public:
	//defer(std::function<void>() unwind) : m_unwind{unwind} {};
	defer(std::function<void()> fn_unwind) : m_unwind{fn_unwind} {};
	~defer() {m_unwind();};
private:
	std::function<void()> m_unwind;
};


export template <typename R, typename T>
class defer1 {
public:
	//defer(std::function<void>() unwind) : m_unwind{unwind} {};
	defer1(R fn_unwind, T param)  : m_unwind{fn_unwind}, m_param{param}  {};
	~defer1() { m_unwind(m_param) ; };
private:
	R m_unwind;
	T m_param;
};



// 25/4 You can use it like
// auto v = to_int(mystr);
export std::optional<int> to_int(const std::string& str)
{
	try {
		return stoi(str);
	} catch(...) {
		return std::nullopt;
	}
}




export std::string spaces(int n)
{
	n = std::max(0, n);
	char sa[n+1];
	std::fill(sa, sa+n, ' ');
	sa[n] = '\0';
	return sa; 
}

// FN pad_left .
export std::string pad_left(const std::string& s, int width)
{
	return spaces(width-s.size()) + s;
}
// FN-END

// FN pad_right .
export std::string pad_right(const std::string& s, int width)
{
	return s + spaces(width-s.size());
}
// FN-END

// FN pad_centre .
export std::string pad_centre(const std::string& s, int width)
{
	return pad_left(pad_right(s, width/2), width);
}
// FN-END

// FN pad_jst .
export std::string pad_jst(const std::string& s, int width, enum jst j)
{
	std::string txt{s};
	switch(j) {
		case jst::def:
		case jst::rgt:
			txt = pad_left(txt, width);
			break;
		case jst::lft:
			txt = pad_right(txt, width);
			break;
		case jst::cnt:
			txt = pad_centre(txt, width);
			break;
	}
	return txt;
}
// FN-END
