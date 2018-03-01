/*
 * $Id: parse.y,v 1.11 2001/02/04 00:03:48 pw Exp $
 *
 * Copyright (C) 1990, 1992, 1993, 1999 Free Software Foundation, Inc.
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

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <regex>
#include <string>

//#include "global.h"
//#include "eval.h"
//#include "node.h"
//#include "ref.h"
//#include "hash.h"
//#include "mem.h"


using std::cout;
using std::endl;
using namespace std::string_literals;

typedef std::vector<std::string> strings;

//enum Token { L_CELL, L_CONST, GE, NE, LE, L_FN0, L_FN1, L_FN2, L_FN3, L_FN1R , L_VAR, L_RANGE, 
//	BAD_CHAR, BAD_FUNC, NO_QUOTE, PARSE_ERR};

//char *alt_instr;
//int alt_parse_error = 0;


//* This table contains a list of the infix single-char functions 
//unsigned char alt_fnin[] = {
//	SUM, DIFF, DIV, PROD, MOD, /* AND, OR, */ POW, EQUAL, IF, CONCAT, 0
//};







//constexpr std::string wrap(const std::string& str) { return "^("s + str + ")" ; }

class Re {

	public:
		Re(const std::string& str, const std::string& name);
	std::regex re;
	std::string name;
};

Re::Re(const std::string& str, const std::string& name) : name(name)
{
	re = std::regex( "^(" + str + ")" );
}




int
alt_yylex_a(const std::string& s)
{
	typedef std::vector<Re> Res;
	static Res regexes = { 
		Re("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?", "float"),
		Re("[Rr][0-9]+[Cc][0-9]", "rc"),
		Re("[a-zA-z]+", "word"),
		Re(".", "unknown")

	};


	cout << "alt_yylex_a:input:" << s << "\n";
	std::smatch m;

	auto i0 = s.begin();
	auto iX = s.end();
	while(i0 < iX) {
		while(i0<iX && std::isspace(*i0)) i0++; // eat white
		if(i0 != iX) {
			std::string s2(i0, iX);
			//int matched_size = 0;
			std::string matched_str;
			for(const auto& areg: regexes) {
				if(std::regex_search(s2, m, areg.re)) {
					matched_str = m[0];
					cout << "alt_yylex_a:match:" << areg.name << ":" << matched_str << "\n";
					break;
				}
			}
			i0 += matched_str.size();
		}
	}	

	cout << endl;
}


static
bool test01()
{

	cout << "test01\n";

	std::string s{"  r1C2 12.3e23 13.4"};

	alt_yylex_a(s);

	alt_yylex_a("foo");

	alt_yylex_a("foo(bar)");

	return true;
}

static bool
test02()
{
	cout << "test02 TODO\n";
	//mem yymem;
	//auto res = alt_yyparse_parse("1+2", yymem);
}

bool run_alt_parse_tests()
{
	test01();
	test02();
	return false;
}
