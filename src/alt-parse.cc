/*
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
#include <exception>
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
using std::unique_ptr;
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

// TODO include power, ege. 12^5

enum LexType { LT_FLT , 
	LT_OPE, // = != <= < >= >  
	LT_OPR, // + -
	LT_OPP, // um ..
	LT_OPT, //  * /
	LT_WORD, LT_UNK };


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
		Re("[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?", LexType::LT_FLT, "float"),
		Re("=|!=|<=?|>=?", LT_OPE, "ope"),
		Re("\\+|\\-", LT_OPR, "opr"),
		Re("\\*|/", LT_OPT, "opt"),
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

// https://www.engr.mun.ca/~theo/Misc/exp_parsing.htm#classic
// [ ] enclose an optional part of the expression
// { } encolse 0 or more repetitions
// | separates alternatives
// Parse rules (incomplete):
//  E --> R {( "<" | "<=" | ">" | ">=" | "==" | "!=" ) R}
//  R --> T {( "+" | "-" ) T}
//  T --> P {( "*" | "/" ) P}
//  P --> v | "(" E ")" | "-" T

class syntax_error : public std::exception
{
	virtual const char* what() const throw()
	{
		return "#PARSE_ERROR";
	}
};


typedef variant<num_t, std::string> value_t;



class BaseNode
{
	public:
		virtual ~BaseNode() {};
		virtual value_t  eval() =0 ; //{return v}

};

typedef unique_ptr<BaseNode> base_ptr;

class BinopNode : public BaseNode
{
	public:
		BinopNode(std::string op, base_ptr lhs, base_ptr rhs): op(op) {
			this->lhs = std::move(lhs);
			this->rhs = std::move(rhs);
		}
		value_t eval() { 
			num_t n1 = std::get<num_t>(lhs->eval());
			num_t n2 = std::get<num_t>(rhs->eval());
			cout << "evaluating " << n1 << op << n2 << endl;
			num_t ret;
			if(op == "+")
				ret = n1 + n2;
			else if(op == "*")
				ret = n1*n2;

			return ret;


		}
		std::string op;
		base_ptr lhs, rhs;
};


class EmptyNode : public BaseNode
{
	public:
		value_t eval() {return "";}
		~EmptyNode() {};
};

class ValueNode : public BaseNode
{
	public:
		ValueNode(value_t v) : v(v) {}
		value_t eval() {
			//cout << "evaluating value node " << std::get<num_t>(v) << endl;
			return v; 
		}
		~ValueNode() { 
			//cout << "ValueNode says bye\n";
		}
		value_t v;

};




typedef lexemes::iterator lex_it; // lexeme iterator

base_ptr expr_e(lex_it it, lex_it e);

base_ptr
binop(std::string op_lexeme, base_ptr lhs, base_ptr rhs)
{
	return make_unique<BinopNode>(op_lexeme, std::move(lhs), std::move(rhs));
}

base_ptr
expr_p(lex_it it, lex_it e)
{
	// TODO
	if(it == e) throw syntax_error();
	return make_unique<ValueNode>(stod(it->lexeme));
}

base_ptr
expr_t(lex_it it, lex_it e)
{
	if(it == e) throw syntax_error();
	lex_it op = it+1; 
	if(op<e && op->lextype == LT_OPT)
		return binop(op->lexeme, expr_p(it, e), expr_p(it+2, e));
	else
		return expr_p(it, e);
}

base_ptr
expr_r(lex_it it, lex_it e)
{
	if(it == e) throw syntax_error();
	lex_it op = it+1; 
	if(op<e && op->lextype == LT_OPR)
		return binop(op->lexeme, expr_t(it, e), expr_t(it+2, e));
	else
		return expr_t(it, e);
}

base_ptr
expr_e(lex_it it, lex_it e)
{
	if(it == e) throw syntax_error();
	lex_it op = it+1; 
	if(op<e && op->lextype == LT_OPE)
		return binop(op->lexeme, expr_r(it, e), expr_r(it+2, e));
	else
		return expr_r(it, e);

}

void alt_parse(std::string s)
{
	cout << "parsing: " << s << endl;
	lexemes lexes = alt_yylex_a(s);
	lex_it it = lexes.begin();
	base_ptr node = expr_e(it, lexes.end());
	cout << "Done parsing. Now evaluating." << endl;
	cout << std::get<num_t>(node->eval()) << endl;

}




///////////////////////////////////////////////////////////////////////////
// tests

static
bool test01()
{
	cout << "test01\n";

	lex_and_print("  r1C2 12.3e23 13.4");
	lex_and_print("foo(bar)");
	lex_and_print("1=2");
	lex_and_print("1!=2");
	lex_and_print("1<2");
	lex_and_print("1<=2");
	lex_and_print("1+2-3");
	lex_and_print("1*3/3");


	return true;
}

static bool
test02()
{
	cout << "test02 TODO\n";
	alt_parse("66");
	alt_parse("23<24");
	alt_parse("23+24");
	alt_parse("1+2+3");
	alt_parse("1+2*3");
	alt_parse("4*2+3");

	//unique_ptr<BaseNode> ptr = make_unique<FloatNode>();
	//alt_parse("");
	cout << "Now I am going to delibeartely throw an error\n";
	throw syntax_error();
}

bool run_alt_parse_tests()
{
	test01();
	test02();
	return false;
}
