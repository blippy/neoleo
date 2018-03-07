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
#include <cassert>
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
#include <type_traits>
#include <typeinfo>
#include <variant>
#include <vector>

#include "alt-cells.h"
#include "numeric.h"

using std::cout;
using std::endl;
using std::map;
using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::string;
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


///////////////////////////////////////////////////////////////////////////
// lexical analysis

// TODO include power, ege. 12^5

enum LexType { LT_FLT , 
	LT_OPE, // = != <= < >= >  
	LT_OPR, // + -
	LT_OPP, // um ..
	LT_OPT, //  * /
	LT_ID, 
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
		void require(std::string s) { 
			if(this->curr() != s) 
				throw std::runtime_error("#PARSE_ERR: Expecting " + s + ", got " + this->curr()); 
		}
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
		Re("[a-zA-z]+", LexType::LT_ID, "word"),
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
// { } enclose 0 or more repetitions
// | separates alternatives
// Parse rules (incomplete):
//  E --> R {( "<" | "<=" | ">" | ">=" | "==" | "!=" ) R}
//  R --> T {( "+" | "-" ) T}
//  T --> P {( "*" | "/" ) P}
//  P --> v | F | "(" E ")" | "-" T
//  F --> word "(" [E {"," E} ] ")"






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

value_t neo_println(values vs)
{
	cout << "neo_println() says hello\n";
	cout << "number of args: " << vs.size() << "\n";
	for(auto& v: vs)
		cout << num(v)  << ", ";
	cout << endl;
	return 0;
}

value_t neo_mod(values vs)
{
	if(vs.size() != 2) throw std::runtime_error("mod nargs");
	return std::fmod(num(vs[0]), num(vs[1]));
}

value_t neo_pi(values vs)
{
	if(vs.size() != 0) throw std::runtime_error("pi nargs");
	return 3.141592653589793238;
}

value_t neo_sqrt(values vs)
{
	if(vs.size() != 1) throw std::runtime_error("sqrt nargs");
	return std::sqrt(num(vs[0]));
}

map<std::string, neo_func> neo_funcs = {
	{"cell", neo_cell},
	{"mod", neo_mod},
	{"pi", neo_pi},
	{"println", neo_println},
	{"sqrt", neo_sqrt}
};


class Factor;
class Relop;
class Term;


class Expression {
	public:
		vector<Relop> operands; // terms;
		vector<math_op> fops;
		//const strings operators = strings {"+", "-" };
};

class Relop {
	public:
		vector<Term> operands; // terms;
		vector<math_op> fops;
};

class Term {
	public:
		vector<Factor> operands; //factors;
		vector<math_op> fops;
		//const strings operators = strings { "*", "/" };
};


class FuncCall{
	public:
		string name; // TODO evaluation would be quicker with a function pointer.
		vector<Expression> exprs;
};

class Factor {
	public:
		variant<num_t, Expression, FuncCall> factor;
};


typedef Expression expression_t;
typedef vector<Expression> expressions_t;


/*
value_t FuncNode::eval() const
{ 
	values the_values;
	for(int i = 0 ; i < args.size(); ++i) 
		the_values.push_back(evaluate_arg(std::move(args[i])));
	return f(the_values); 
}

value_t FuncNode::evaluate_arg(base_ptr arg)
{ 
	auto a = std::move(arg);
	return a->eval(); 
}
*/

class FunctionDefinition : public BaseNode
{
	public:
		FunctionDefinition(string function_name);
		void append_arg(string identifier) { arguments.push_back(identifier); }
		void append_statement(base_ptr statement) { 
			//statements.push_back(std::move(statement)); 
		} 
		value_t eval() const { 
			cout << "FunctionDefinition:eval()\n";
			return 666; 
		}
		string function_name;
		~FunctionDefinition() { cout << "deleting FunctionDefinition\n"; }
	private:
		strings arguments;
		//base_ptrs statements;

};

typedef shared_ptr<FunctionDefinition> interpreted_function_ptr;

map<std::string, interpreted_function_ptr> interpreted_functions;

FunctionDefinition::FunctionDefinition(string function_name): function_name(function_name) 
{
	//interpreted_functions[function_name] = this;
}

/*
neo_func
standardise_function(FunctionDefinition fin)
{
	neo_func fout = [&](values vs) {
		
		return fin.eval();
		//return 666; 
	};
	return fout;
}
*/

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

Expression expr_e(lexemes_c& tokes);

//using sub_expr_func = std::function<base_ptr(lexemes_c&)>;




//typedef variant<num_t, expression_t> factor_t;






Expression make_expression(lexemes_c& tokes);


FuncCall
make_funccall(lexemes_c& tokes)
{
	FuncCall fn;
	fn.name = tokes.curr();
	tokes.advance();
	tokes.require("(");
	tokes.advance();
	auto make = [&]() { fn.exprs.push_back(make_expression(tokes)); };
	if(tokes.curr() != ")") {
		make();
		while(tokes.curr() == ",") {
			tokes.advance();
			make();
		}
	}
	tokes.advance();
	return fn;
}

Factor
make_factor(lexemes_c& tokes)
{
	Factor f;
	if(tokes.curr() == "(") { // a parenthesised expression
		tokes.advance();
		f.factor = make_expression(tokes);
		tokes.require(")");
		tokes.advance();
	} else if(tokes.curr_type() == LT_ID) {
		f.factor = make_funccall(tokes);
	} else {
		num_t v = std::stod(tokes.curr());
		f.factor = v;
		tokes.advance();
	}

	return f;
}

Term
make_term(lexemes_c& tokes)
{
	Term  t;
	auto operators = strings {"*", "/"};
	auto make = make_factor;
	//cout << "decltype(make_factor):" << type_name<decltype(make)>() << "\n";
	t.operands.push_back(make(tokes));
	while(1) {
		std::string op = tokes.curr();
		strings::iterator opit = std::find(operators.begin(), operators.end(), op);
		if(opit == operators.end()) break;
		tokes.advance();
		math_op fop = (math_ops.find(op)->second);
		t.fops.push_back(fop);
		t.operands.push_back(make(tokes));
	}
	return t;
}

Relop
make_relop(lexemes_c& tokes)
{
	Relop r;
	auto make = make_term;
	auto operators = strings {"<", "<=", ">", ">=", "=", "!"};
	r.operands.push_back(make(tokes));
	while(1) {
		std::string op = tokes.curr();
		strings::iterator opit = std::find(operators.begin(), operators.end(), op);
		if(opit == operators.end()) break;
		tokes.advance();
		math_op fop = (math_ops.find(op)->second);
		r.fops.push_back(fop);
		r.operands.push_back(make(tokes));
	}
	return r;
}
Expression
make_expression(lexemes_c& tokes)
{
	Expression e;
	auto make = make_relop;
	auto operators = strings {"+", "-"};
	e.operands.push_back(make(tokes));
	while(1) {
		std::string op = tokes.curr();
		strings::iterator opit = std::find(operators.begin(), operators.end(), op);
		if(opit == operators.end()) break;
		tokes.advance();
		math_op fop = (math_ops.find(op)->second);
		e.fops.push_back(fop);
		e.operands.push_back(make(tokes));
	}
	return e;
}

Expression
parse_expression(std::string s)
{
	//cout << "parsing: " << s << endl;
	lexemes_c tokes{alt_yylex_a(s)};
	//base_ptr node = expr_e(tokes);
	return make_expression(tokes);
}

/*
bool
function_definition(lexemes_c& tokes)
{
	if(tokes.curr() != "func") return false;
	tokes.advance();
	FunctionDefinition ast{tokes.curr()};
	//interpreted_functions[tokes.curr()] = ast;
	cout << "a1" << endl;
	


	tokes.advance();

	// argument list
	tokes.require("(");
	tokes.advance();
	if(tokes.curr() != ")") {
		ast.append_arg(tokes.curr());
		tokes.advance();
		while(tokes.curr() == ",") {
			tokes.advance();
			ast.append_arg(tokes.curr());
			tokes.advance();
		}
	}
	//tokes.require(")");
	tokes.advance();

	// compound statement
	tokes.require("{");
	tokes.advance();
	if(tokes.curr() != "}") {
		ast.append_statement(expr_e(tokes));
		tokes.require(";");
		tokes.advance();
	}
	tokes.advance();

	//register_user_function(std::move(ast));
	//neo_funcs[tokes.curr()] = standardise_function(ast);
	neo_funcs[tokes.curr()] = [ast](values vs) { 		
		//auto ast1 = std::move(ast);
		//return 667;
		return ast.eval(); 
	};
	return true;
}


void
parse_translation_unit(std::string s)
{
	lexemes_c tokes{alt_yylex_a(s)};
	//base_ptr ast;
	while( tokes.curr_type()!= LT_EOF) {
		if(! function_definition(tokes)) {
			auto expr = expr_e(tokes);
			expr->eval();
		}
	}
	//return nullptr;
}
*/

value_t eval(Expression e);

value_t eval(FuncCall fn)
{	
	values vs;
	for(const auto& e: fn.exprs)
		vs.push_back(eval(e));	

	auto it  =  neo_funcs.find(fn.name);
	if(it == neo_funcs.end()) {
		throw std::runtime_error("#UNK_FUNC: " + fn.name);
	}
	neo_func f = it->second;
	return f(vs);
}

value_t
eval(Factor f)
{
	if(std::holds_alternative<num_t>(f.factor)) {
		return std::get<num_t>(f.factor);
	} else if(std::holds_alternative<Expression>(f.factor)) {
		return eval(std::get<Expression>(f.factor));
	} else if(std::holds_alternative<FuncCall>(f.factor)) {
		return eval(std::get<FuncCall>(f.factor));
	} else {
		throw std::logic_error("eval(Factor f) unhandled alternative");
	}

}



template<class T>
value_t eval_multiop(T expr)
{
	assert(expr.operands.size()>0);
	value_t result = eval(expr.operands[0]);
	for(int i = 0; i< expr.fops.size() ; i++) {
		auto& fop = expr.fops[i];
		result = fop(result, eval(expr.operands[i+1]));
	}
	return result;
}

value_t eval(Term t) { return eval_multiop(t); }
value_t eval(Relop r) { return eval_multiop(r); }
value_t eval(Expression e) { return eval_multiop(e); }

string str(value_t v)
{
	return std::to_string(std::get<num_t>(v));
}
void
test_parse_expression(std::string s)
{
	cout << "test_parse_expression():" << s << "\n";
	Expression e{parse_expression(s)};
	cout << "Done parsing. Now evaluating." << endl;
	cout << str(eval(e)) << "\n" << endl;
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

static void
test02()
{
	cout << "test02\n";
	auto test_expressions = { "66", "1-2-3", "2*3", "23<24",  "23+24", "1+2+3", "1+2*3", "4*2+3", "4/5/6", 
		"(1+2)*3",  "sqrt(3+9)", "pi()", "mod(12, 7)"};

	for(const string& s: test_expressions)
		test_parse_expression(s);


	//cout << "Now I am going to deliberately throw an error\n";
	//throw syntax_error();
}

void
test03()
{
	cout << "test03\n";
	//std::string prog = "func foo (v) { println(v+1); } func bar () { } println(1, 2, 3, 12+13) foo(5)";
	std::string prog = "func foo (v) { println(1+1); } foo(1)";
	//std::string prog = "func foo (v) { println(1+1); }";
	//parse_translation_unit(prog);
}


bool run_alt_parse_tests()
{
	test01();
	test02();
	test03();
	return false;
}
