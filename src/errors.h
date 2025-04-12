#pragma once

/*
 * $Id: errors.h,v 1.3 2000/08/10 21:02:50 danny Exp $
 *
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

//#include "format.h"
//#include "errors.h"
//#include "xcept.h"

/* These are all the possible error codes that eval_expression() can return. */
#define ERR_CMD		1
#define BAD_INPUT	2
#define NON_NUMBER	3
#define NON_STRING	4
#define NON_BOOL	5
#define NON_RANGE	6
#define OUT_OF_RANGE	7
#define NO_VALUES	8
#define DIV_ZERO	9
#define BAD_NAME	10
#define NOT_AVAL	11
#define PARSE_ERR	12
#define NO_OPEN		13
#define NO_CLOSE	14
#define NO_QUOTE	15
#define BAD_CHAR	16
#define BAD_FUNC	17
#define CYCLE		18

#define ERR_MAX		18

typedef struct err_t { int num; std::string what; } err_t;
//extern char *ename[];
#define CCC1(x) x
inline const char *ename[] = 
{
	CCC1("#WHAT?"),
	CCC1("#ERROR"), CCC1("#BAD_INPUT"), CCC1("#NON_NUMBER"), CCC1("#NON_STRING"),
	CCC1("#NON_BOOL"), CCC1("#NON_RANGE"), CCC1("#OUT_OF_RANGE"), CCC1("#NO_VALUES"),
	CCC1("#DIV_BY_ZERO"), CCC1("#BAD_NAME"), CCC1("#NOT_AVAIL"), CCC1("#PARSE_ERROR"),
	CCC1("#NEED_OPEN"), CCC1("#NEED_CLOSE"), CCC1("#NEED_QUOTE"), CCC1("#UNK_CHAR"),
	CCC1("#UNK_FUNC"), CCC1("#CYCLE"),
	CCC1(0)
};



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

class SyntaxError : public std::exception
{
	public:
		SyntaxError() {}
		SyntaxError(const std::string& msg) : msg_(msg) {}
		virtual const char* what() const throw() { return msg_.c_str() ; }
	private:
		std::string msg_ = "SyntaxError";
};
 
void  _assert_uncalled(const char* __file__, int __line__); 
#define	ASSERT_UNCALLED() { _assert_uncalled(__FILE__, __LINE__); }

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

void raise_error (const char *str, ...);