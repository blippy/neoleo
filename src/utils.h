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

#if 0
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




#endif
