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
#include <map>
#include <memory>
#include <stdexcept>
#include <regex>
#include <string>
#include <variant>
#include <vector>

//#include "global.h"
//#include "eval.h"
//#include "node.h"
//#include "ref.h"
//#include "hash.h"
//#include "mem.h"

#include "numeric.h"

using std::cout;
using std::endl;
using std::map;
using std::make_unique;
using std::variant;
using std::vector;
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

///////////////////////////////////////////////////////////////////////////
// lexical analysis

enum LexType { LT_FLT, LT_WORD, LT_UNK };


typedef struct {
	LexType lextype;
	std::string lexeme;
} lexeme_s;

typedef vector<lexeme_s> lexemes;


//constexpr std::string wrap(const std::string& str) { return "^("s + str + ")" ; }

static map<LexType, std::string> type_map;

class Re {

	public:
		Re(const std::string& str, LexType lextype, const std::string& name);
		LexType lextype;
		std::regex re;
		std::string name;
};

Re::Re(const std::string& str, LexType lextype, const std::string& name) : lextype(lextype), name(name)
{
	re = std::regex( "^(" + str + ")" );
	type_map[lextype] = name;
}




lexemes
alt_yylex_a(const std::string& s)
{
	lexemes lexes;
	typedef std::vector<Re> Res;
	static Res regexes = { 
		Re("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?", LexType::LT_FLT, "float"),
		// Re("[Rr][0-9]+[Cc][0-9]", "rc"),
		Re("[a-zA-z]+", LexType::LT_WORD, "word"),
		Re(".", LexType::LT_UNK, "unknown")

	};


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
					lexes.push_back({areg.lextype, matched_str});
					break;
				}
			}
			i0 += matched_str.size();
		}
	}	

	return lexes;
}

static void
lex_and_print(std::string s)
{
	cout << "lex_and_print:input:`" << s << "'\n";	
	lexemes lexes = alt_yylex_a(s);
	for(const auto& l:lexes)
	       	cout << "match:" << type_map[l.lextype] << ":" << l.lexeme << "\n";
	cout << endl;
}

// lexical analysis
///////////////////////////////////////////////////////////////////////////
// scanner

typedef variant<num_t, std::string> value_t;
typedef value_t node_t; // Nodes are not values, but just assume this for now

/*
class BaseNode
{
	public:
		virtual ~BaseNode() {};
		virtual value_t  eval() = 0;// {return  0;}
};


class EmptyNode : public BaseNode
{
	public:
		value_t eval() {return "";}
		~EmptyNode() {};
};

class FloatNode : public BaseNode
{
	public:
		value_t eval() {return 666;};
		~FloatNode() {};

};
*/


// from
// https://gist.github.com/s3rvac/d1f30364ce1f732d75ef0c89a1c8c1ef
template<typename... Ts> struct make_overload: Ts... { using Ts::operator()...; };
template<typename... Ts> make_overload(Ts...) -> make_overload<Ts...>;

template<typename Variant, typename... Alternatives>
decltype(auto) visit_variant(Variant&& variant, Alternatives&&... alternatives) {
	return std::visit(
			make_overload{std::forward<Alternatives>(alternatives)...},
			std::forward<Variant>(variant)
			);
}



std::string
eval(node_t node)
{
	/*
	if(std::holds_alternative<num_t>(node)) {
		return "i am a number " + std::to_string(std::get<num_t>(node));
	} else if(std::holds_alternative<std::string>(node)) {
		return "i am string " + std::get<std::string>(node);
	} else {
		return "eval() :(";
	}
	*/

	std::string result;
	visit_variant(node, 
			[&](num_t n) { result = "i am a number " + std::to_string(n) ; },
			[&](std::string const& s) { result = "i am string " + s;}
		     );
	return result;

}

void alt_parse(std::string s)
{
	lexemes lexes = alt_yylex_a(s);

	//BaseNode top;
	//auto top = make_unique<EmptyNode>();
	node_t top;

	// TODO NOW
	switch(lexes[0].lextype) {
		case LT_FLT:
			top = std::stod(lexes[0].lexeme); // TODO need to cover has ege. 24 + 23
		//	top = FloatNode();
			break;
		default:
			break;
	}

	//top = make_unique<FloatNode>();

	cout << eval(top) << "\n";

}




///////////////////////////////////////////////////////////////////////////
// tests

static
bool test01()
{
	cout << "test01\n";

	lex_and_print("  r1C2 12.3e23 13.4");
	lex_and_print("foo(bar)");

	return true;
}

static bool
test02()
{
	cout << "test02 TODO\n";
	//mem yymem;
	//auto res = alt_yyparse_parse("1+2", yymem);
	alt_parse("23");
}

bool run_alt_parse_tests()
{
	test01();
	test02();
	return false;
}
