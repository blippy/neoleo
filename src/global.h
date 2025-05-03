#pragma once
/*
 * Copyright (c) 1990, 1992, 1993, 1999, 2000, 2001 Free Software Foundation, Inc.
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

#include <string_view>

#include "neotypes.h"


/* The location of a cell that can never be referenced */
#define NON_ROW		0
#define NON_COL		0


const auto VERSION = "16.0";
const auto  PACKAGE = "neoleo";
const auto PACKAGE_NAME = PACKAGE;


inline CELLREF curow = MIN_ROW;
inline CELLREF cucol = MIN_COL;

inline std::string option_tests_argument = "";

inline bool Global_modified = false;
