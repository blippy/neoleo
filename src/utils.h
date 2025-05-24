#pragma once

/*
 * Copyright (c) 1992, 1993 Free Software Foundation, Inc.
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

#include <map>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>
#include <stdio.h>

#include "neotypes.h"


extern void set_usr_stats (int, char **);
std::string pad_centre(const std::string& s, int width);
std::string pad_jst(const std::string& s, int width, enum jst j);
std::string pad_right(const std::string& s, int width);
std::string pad_left(const std::string& n, int width);
std::string spaces(int n);
//size_t strlen(const std::string& s);

std::optional<int> to_int(const std::string& str); // 25/4




class defer {
public:
	//defer(std::function<void>() unwind) : m_unwind{unwind} {};
	defer(std::function<void()> fn_unwind) : m_unwind{fn_unwind} {};
	~defer() {m_unwind();};
private:
	std::function<void()> m_unwind;
};


template <typename R, typename T>
class defer1 {
public:
	//defer(std::function<void>() unwind) : m_unwind{unwind} {};
	defer1(R fn_unwind, T param)  : m_unwind{fn_unwind}, m_param{param}  {};
	~defer1() { m_unwind(m_param) ; };
private:
	R m_unwind;
	T m_param;
};


