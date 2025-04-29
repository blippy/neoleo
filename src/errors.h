#pragma once

/*
 * Copyright (c) 1990, 1992, 1993 Free Software Foundation, Inc.
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

#include <stdarg.h>
#include <string>





/* https://www.quora.com/How-does-one-write-a-custom-exception-class-in-C++ 
 * */
class OleoJmp : public std::exception
{
	public:
		OleoJmp() {}
		OleoJmp(const std::string& msg) : msg_(msg) {}

		virtual const char* what() const throw()
		{
			return msg_.c_str() ;
		}

	private:
		std::string msg_ = "OleoJmp";
};



class ValErr : public std::exception
{
	public:
	       ValErr() {}
	       ValErr(const int n) : n(n) {}
	       const char* what() const throw();
	       const int num() const throw();

	private:
	       int n = 0;
	       std::string msg;
};

