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

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <functional>
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

//* This table contains a list of the infix single-char functions 
//unsigned char alt_fnin[] = {
//	SUM, DIFF, DIV, PROD, MOD, /* AND, OR, */ POW, EQUAL, IF, CONCAT, 0
//};

class syntax_error : public std::exception
{
	virtual const char* what() const throw()
	{
		return "#PARSE_ERROR";
	}
};

class unknown_function : public std::exception
{
	virtual const char* what() const throw()
	{
		return "#UNK_FUNC";
	}
};

///////////////////////////////////////////////////////////////////////////
// lexical analysis

// TODO include power, ege. 12^5

enum LexType { LT_FLT , 
	LT_OPE, // = != <= < >= >  
	LT_OPR, // + -
	LT_OPP, // um ..
	LT_OPT, //  * /
	LT_WORD, 
	LT_UNK,
	LT_EOF // end of file
};


typedef struct {
	LexType lextype;
	std::string lexeme;
} lexeme_s;


typedef vector<lexeme_s> lexemes;

class lexemes_c {
	public:
		lexemes_c(lexemes lexs);
		void advance() { idx++;}
		std::string curr() { return idx<len? lexs[idx].lexeme : "" ;}
		LexType curr_type() { return idx<len? lexs[idx].lextype : LT_EOF ;}
		void require(std::string s) { if(this->curr() != s) throw syntax_error(); }
	private:
		lexemes lexs;
		int idx = 0, len;
};


lexemes_c::lexemes_c(lexemes lexs) : lexs(lexs)
{
	len = lexs.size();
}




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
//  P --> v | F | "(" E ")" | "-" T
//  F --> word "(" E ")"



typedef variant<num_t, std::string> value_t;
typedef vector<value_t> values;


num_t num(value_t v) { return std::get<num_t>(v); }

using math_op = std::function<value_t(value_t, value_t)>;

value_t neo_add(value_t v1, value_t v2) { return num(v1) + num(v2); }
value_t neo_sub(value_t v1, value_t v2) { return num(v1) - num(v2); }
value_t neo_mul(value_t v1, value_t v2) { return num(v1) * num(v2); }
value_t neo_div(value_t v1, value_t v2) { return num(v1) / num(v2); }
value_t neo_lt(value_t v1, value_t v2) { return num(v1) < num(v2); }
value_t neo_le(value_t v1, value_t v2) { return num(v1) <= num(v2); }
value_t neo_gt(value_t v1, value_t v2) { return num(v1) > num(v2); }
value_t neo_ge(value_t v1, value_t v2) { return num(v1) >= num(v2); }
value_t neo_eq(value_t v1, value_t v2) { return num(v1) == num(v2); }
value_t neo_ne(value_t v1, value_t v2) { return num(v1) != num(v2); }

map<std::string, math_op> math_ops = { 
	{ "+", neo_add} , {"-", neo_sub }, 
	{"*",  neo_mul}, {"/", neo_div}, 
	{"<", neo_lt}, {"<=", neo_le}, {">", neo_gt}, {">=", neo_ge}, {"=", neo_eq}, {"!=", neo_ne}
};

using neo_func = std::function<value_t(values vs)>;

value_t neo_sqrt(values vs)
{
	if(vs.size() != 1) throw syntax_error();
	return std::sqrt(num(vs[0]));
}

map<std::string, neo_func> neo_funcs = {
	{"sqrt", neo_sqrt}
};


class BaseNode
{
	public:
		virtual ~BaseNode() {};
		virtual value_t  eval() =0 ; //{return v}

};

typedef unique_ptr<BaseNode> base_ptr;
typedef vector<base_ptr> base_ptrs;

/*
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
*/

class EmptyNode : public BaseNode
{
	public:
		value_t eval() {return "";}
		~EmptyNode() {};
};

class FuncNode : public BaseNode
{
	public:
		FuncNode(neo_func f) : f(f) {}
		void append_arg(base_ptr arg) { args.push_back(std::move(arg)); }
		value_t eval();
	private:
		neo_func f;
		base_ptrs args;
		value_t evaluate_arg(base_ptr arg);
};

value_t FuncNode::eval()
{ 
	values the_values;
	//the_values.push_back(12);
	//evaluate_arg(std::move(args.at(0)));
	for(int i = 0 ; i < args.size(); ++i) 
		the_values.push_back(evaluate_arg(std::move(args[i])));
	//std::transform(args.begin(), args.end(), std::back_inserter(the_values),
	//			[](base_ptr arg) -> value_t {return std::move(arg->eval());});
	return f(the_values); 
	//return 666;		
}

value_t FuncNode::evaluate_arg(base_ptr arg)
{ 
	auto a = std::move(arg);
	return a->eval(); 
}

class MultiopNode : public BaseNode
{
	public:
		MultiopNode(base_ptr operand1) {
			operands.push_back(std::move(operand1));
		}
		void append(math_op op, base_ptr operand) {
			operators.push_back(op);
			operands.push_back(std::move(operand));
		}
		value_t eval();
	private:
		vector<base_ptr> operands;
		std::vector<math_op> operators;

};


/*
template<class T, class U>
T::iterator find(T objs, T target)
{
	auto it = std::find(objs.begin(), objs.end(), target);
	if(it != objs.end())
		return nullptr;
	else
		return it;
		
}
*/

value_t MultiopNode::eval()
{
	value_t result = num(operands[0]->eval());
	for(int i = 0; i< operators.size(); ++i) {
		math_op op = operators[i];
		value_t rhs = operands[i+1]->eval();
		result = op(result, rhs);
	}
	return result;
}


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

base_ptr expr_e(lexemes_c& tokes);

/*
base_ptr
binop(std::string op_lexeme, base_ptr lhs, base_ptr rhs)
{
	return make_unique<BinopNode>(op_lexeme, std::move(lhs), std::move(rhs));
}
*/

using sub_expr_func = std::function<base_ptr(lexemes_c&)>;

base_ptr
expr_f(lexemes_c& tokes)
{
	auto fit = neo_funcs.find(tokes.curr());
	if(fit == neo_funcs.end()) throw unknown_function();
	neo_func func = fit->second;
	tokes.advance();
	tokes.require("(");
	tokes.advance();
	auto result = make_unique<FuncNode>(func);
	result->append_arg(expr_e(tokes));
	tokes.require(")");
	tokes.advance();
	return result;
}

base_ptr
expr_p(lexemes_c& tokes)
{
	base_ptr result;
	if(tokes.curr() == "(") {
		tokes.advance();
		result = expr_e(tokes);
		tokes.require(")");
	} else if(tokes.curr_type() == LT_WORD) {
		return expr_f(tokes);
	} else {
		result = make_unique<ValueNode>(stod(tokes.curr()));
	}
	tokes.advance();
	return result;
}

base_ptr multiop(strings operators, lexemes_c& tokes, sub_expr_func expr_f)
{
	auto exprs = make_unique<MultiopNode>(expr_f(tokes));
	while(1) {
		std::string op = tokes.curr();
		strings::iterator opit = std::find(operators.begin(), operators.end(), op);
		if(opit == operators.end()) break;
		tokes.advance();
		math_op fop = (math_ops.find(op)->second);
		exprs->append(fop, expr_f(tokes));
	}
	return exprs;
}
base_ptr
expr_t(lexemes_c& tokes)
{
	return multiop({"*", "/"}, tokes, expr_p);
}

base_ptr
expr_r(lexemes_c& tokes)
{
	return multiop({"+", "-"}, tokes, expr_t);
}


base_ptr
expr_e(lexemes_c& tokes)
{
	return multiop({"<", "<=", ">", ">=", "=", "!="}, tokes, expr_r);
}

void alt_parse(std::string s)
{
	cout << "parsing: " << s << endl;
	lexemes_c tokes{alt_yylex_a(s)};
	base_ptr node = expr_e(tokes);
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
	alt_parse("2*3");
	alt_parse("23<24");
	alt_parse("23+24");
	alt_parse("1+2+3");
	alt_parse("1+2*3");
	alt_parse("4*2+3");
	alt_parse("4/5/6");
	alt_parse("(1+2)*3");
	alt_parse("sqrt(3+9)");

	//unique_ptr<BaseNode> ptr = make_unique<FloatNode>();
	//alt_parse("");
	cout << "Now I am going to deliberately throw an error\n";
	throw syntax_error();
}

bool run_alt_parse_tests()
{
	test01();
	test02();
	return false;
}
