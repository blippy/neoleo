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
#include "cmd.h"
#include "numeric.h"

using std::cout;
using std::endl;
using std::map;
using std::ostream;
using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::string;
using std::string_view;
using std::unique_ptr;
using std::variant;
using std::vector;
using namespace std::string_literals;
using namespace std::string_view_literals;

typedef std::vector<std::string> strings;
typedef map<string, value_t> varmap_t;


ostream& operator<<(ostream& os, const value_t v);

///////////////////////////////////////////////////////////////////////////
// lexical analysis

// TODO include power, ege. 12^5

enum LexType { LT_FLT , 
	LT_ASS,
	LT_OPE, // = != <= < >= >  
	LT_OPR, // + -
	LT_OPP, // um ..
	LT_OPT, //  * /
	LT_LRB, // left round bracket
	LT_ID, 
	LT_STR,
	LT_VAR,
	LT_UNK,
	LT_EOF // end of file
};


const static map<LexType, string_view> typenames = {
	{LT_FLT, "FLT"sv},
	{LT_ASS, "ASS"sv},
	{LT_OPE, "OPE"sv},
	{LT_OPR,  "OPR"sv},
	{LT_OPP, "OPP"sv},
	{LT_OPT, "OPT"sv},
	{LT_LRB, "LRB"sv},
	{LT_ID,  "ID"sv},
	{LT_STR, "STR"sv},
	{LT_VAR, "VAR"sv},
	{LT_UNK, "UNK"sv},
	{LT_EOF, "EOF"sv}
};

//static_assert(typenames.size()>0);
string_view lextypename(LexType ltype)
{
	//assert(sizeof(LexType) == typenames.size());
	return typenames.at(ltype);
}

typedef struct {
	LexType lextype;
	std::string lexeme;
} lexeme_s;

using token = lexeme_s;


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
		bool empty() {return idx >= len; }
		token curr_lex() { return lexs[idx]; }
	private:
		lexemes lexs;
		int idx = 0, len;
};

typedef lexemes_c tokens;

lexemes_c::lexemes_c(lexemes lexs) : lexs(lexs)
{
	len = lexs.size();
}

string curr(tokens& tokes) { return tokes.curr(); }



token take(tokens& tokes) 
{ 
	token toke = tokes.curr_lex();
	tokes.advance();
	return toke;
	//token toke = tokes.front(); tokes.pop_front(); return toke; 
}

void require(tokens& tokes, string required) 
{ 
	auto toke = take(tokes);
	string found = toke.lexeme; 
	if(found != required)
		throw std::runtime_error("#PARSE_ERR: Required:" + required + ",found:" + found);
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
		Re(":=", LT_ASS, "ass"),
		Re("[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?", LexType::LT_FLT, "float"),
		Re("=|!=|<=?|>=?", LT_OPE, "ope"),
		Re("\\+|\\-", LT_OPR, "opr"),
		Re("\\*|/", LT_OPT, "opt"),
		// Re("[Rr][0-9]+[Cc][0-9]", "rc"),
		Re("\\?[a-zA=Z]+", LT_VAR, "var"),
		Re("[a-zA-z]+", LexType::LT_ID, "id"),
		Re("\"(?:[^\"\\\\]|\\\\.)*\"", LT_STR, "str"),
		Re("\\(", LT_LRB, "lrb"),
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


bool isstring(value_t v, string& str)
{
	bool is = std::holds_alternative<string>(v);
	if(is) str = std::get<string>(v);
	return is;
}

value_t neo_cmd(values vs)
{
	string cmd;
	if(vs.size()>0 && isstring(vs[0], cmd)) 
		execute_command_str(cmd);
	return 0;
}

value_t neo_println(values vs)
{
	//cout << "neo_println() says hello\n";
	//cout << "number of args: " << vs.size() << "\n";
	//for(auto& v: vs)
	//	cout << v  << ", ";
	for(auto it= vs.begin(); it != vs.end(); ++it) {
		cout << *it;
		if(it+1 != vs.end()) cout << ", ";
	}
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
	{"cmd", neo_cmd},
	{"mod", neo_mod},
	{"pi", neo_pi},
	{"println", neo_println},
	{"sqrt", neo_sqrt}
};


class Factor;
class For;
class Let;
class Relop;
class Term;


class Expression {
	public:
		vector<Relop> operands; // terms;
		vector<math_op> fops;
		//const strings operators = strings {"+", "-" };
};

//typedef variant<Expression,Def,If,Let,For,While> Statement;
typedef variant<Expression,Let,For> Statement;
typedef vector<Statement> Statements;

class Program { public: Statements statements; };
class Let { public: string varname; Expression expr; };
class For { public: string varname; Expression from, to; Statements statements; };


value_t eval(varmap_t& vars, Expression e);
value_t eval(varmap_t& vars, Statements statements);
value_t eval(varmap_t& vars, Program prog);

class Relop {
	public:
		vector<Term> operands;
		vector<math_op> fops;
};

class Term {
	public:
		vector<Factor> operands;
		vector<math_op> fops;
		//const strings operators = strings { "*", "/" };
};


class FuncCall{
	public:
		string name; // TODO evaluation would be quicker with a function pointer.
		vector<Expression> exprs;
};

class Variable {
	public:
		string name;
};

class Factor {
	public:
		variant<value_t, Expression, FuncCall, Variable> factor;
};

class FuncDef{
	public:
		string name;
		strings args;
		vector<Expression> statements; // TODO expressions are not statements
};





typedef lexemes::iterator lex_it;


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

Variable
make_variable(lexemes_c& tokes)
{
	Variable var;
	var.name = tokes.curr();
	cout << "make_variable():variable:" << var.name << "\n";
	tokes.advance();
	return var;
}

Factor
make_factor(lexemes_c& tokes)
{
	Factor f;
	switch(tokes.curr_type()) {
		case LT_LRB:
		       	tokes.advance();
			f.factor = make_expression(tokes);
			tokes.require(")");
			tokes.advance();
			break;
		case LT_ID:
			f.factor = make_funccall(tokes);
			break;
		case LT_VAR:
			f.factor = make_variable(tokes);
			break;
		case LT_STR:
			//f.factor = std::get<string>(tokes.curr());
			f.factor = tokes.curr().substr(1, tokes.curr().size()-2);
			tokes.advance();
			break;
		case LT_FLT: 
			f.factor = std::stod(tokes.curr());
			tokes.advance();
			break;
		default:
			throw std::logic_error("#UNHANDLED: make_factor token: " 
					+ tokes.curr()
					+ ", type:" + string(lextypename(tokes.curr_type())));

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

//token take(lexemes_c& tokes) { token toke = tokes.front(); tokes.pop_front(); return toke; }

Let make_let(lexemes_c& tokes)
{
	tokes.require("let");
	tokes.advance();
	Let let;
	let.varname = tokes.curr();
	tokes.advance();
	//cout << "make_let():varname:" << let.varname << "\n";
	tokes.require(":=");
	tokes.advance();
	let.expr = make_expression(tokes);
	return let; 
}

Statement make_statement(lexemes_c& tokes);

Statements collect_statements(tokens& tokes, const string& terminator)
{
	Statements stmts;
	while(curr(tokes) != terminator)
		stmts.push_back(make_statement(tokes));
	return stmts;
}

For make_for(tokens& tokes)
{
	cout << "make_for:in\n";
	require(tokes, "for");
	For a_for;
	a_for.varname = take(tokes).lexeme;
	cout << "make_for()::varname:" << a_for.varname;
	require(tokes, ":=");
	cout << "make_for()::=from:" << curr(tokes);
	a_for.from = make_expression(tokes);
	require(tokes, "to");
	a_for.to = make_expression(tokes);
	a_for.statements = collect_statements(tokes, "next");
	require(tokes, "next");
	cout << "make_for:out\n";
	return a_for;
}




Statement make_statement(lexemes_c& tokes)
{
	static const map<string, std::function<Statement(lexemes_c&)>> commands = {
		//{"def",   make_def},
		//{"if",    make_if},
		{"for",   make_for},
		{"let",   make_let}
		//{"while", make_while}
	};

	Statement stm;
	auto it = commands.find(tokes.curr());
	if(it != commands.end()) {
		auto make = it->second;
		stm = make(tokes);
	}
	else
		stm = make_expression(tokes);
	return stm;
}
					


Program make_program(lexemes_c& tokes)
{
	Program prog;
	while(!tokes.empty()) 
		prog.statements.push_back(make_statement(tokes));
	return prog;
}



Expression
parse_expression(std::string s)
{
	//cout << "parsing: " << s << endl;
	lexemes_c tokes{alt_yylex_a(s)};
	return make_expression(tokes);
}


// Used to turn a user-defined function into a neo_func
value_t wrap_defun(FuncDef func, values vs)
{
	// bind argument identifiers to values
	if(vs.size() != func.args.size()) 
		throw std::runtime_error("#FUNC_ARGS:" + func.name 
				+ "():Expected " + std::to_string(func.args.size()) 
				+ " args, got " + std::to_string(vs.size()));
	varmap_t vars;
	for(int i=0; i< vs.size(); ++i)
		vars[func.args[i]] = vs[i];

	//cout << "wrap_defun():arg[0]:" << num(vs[0]) << "\n";

	for(const auto& stm: func.statements)
		eval(vars, stm);
	return 666;
}

bool
define_function(lexemes_c& tokes)
{
	if(tokes.curr() != "FUNC") return false;
	tokes.advance();
	FuncDef func;

	func.name = tokes.curr();
	tokes.advance();

	// argument list
	auto append_arg = [&]() { 
		if(tokes.curr_type() != LT_VAR)
			throw std::runtime_error("#PARSE:Function definition of " + func.name 
					+ " has non-var arg " + tokes.curr());
		func.args.push_back(tokes.curr()); 
		tokes.advance(); 
	};
	tokes.require("(");
	tokes.advance();
	if(tokes.curr() != ")") {
		append_arg();
		while(tokes.curr() == ",") {
			tokes.advance();
			append_arg();
		}
	}
	//tokes.require(")");
	tokes.advance();

	// compound statement
	tokes.require("{");
	tokes.advance();
	while(tokes.curr() != "}") {
		func.statements.push_back(make_expression(tokes)); // TODO make_statement()
		//tokes.require(";");
		//tokes.advance();
	}
	tokes.require("}");
	tokes.advance();

	cout << "define_function():name:" << func.name << "\n";
	using namespace std::placeholders;
	neo_funcs[func.name] = std::bind(wrap_defun, func, _1);

	cout << "define_function() returning\n";
	return true;
}


void
parse_program_XXX(varmap_t& vars, std::string s)
{
	lexemes_c tokes{alt_yylex_a(s)};
	while( tokes.curr_type()!= LT_EOF) {
		if(! define_function(tokes)) {
			Expression e{make_expression(tokes)};
			eval(vars, e);
		}
	}
}

void
parse_program(varmap_t& vars, std::string s)
{
	//cout << "parse_program:in\n";
	//lex_and_print(s);
	lexemes_c tokes{alt_yylex_a(s)};
	Program prog{make_program(tokes)};
	//cout << "parse_program: statements:" << prog.statements.size() << "\n";
	eval(vars, prog);
	//cout << "parse_program:out\n";
}

varmap_t global_varmap = { {"?pi", 3.141592653589793238} };

void run_neobasic(std::string program)
{
	parse_program(global_varmap, program);

}

///////////////////////////////////////////////////////////////////////////
// eval


	

value_t eval(varmap_t& vars, Variable var)
{
	if(false) {
		cout << "eval(Variable):name:" << var.name <<"\n";
		cout << "Keys are:";
		for(auto it = vars.begin(); it!= vars.end(); it++) cout << it->first << " ";
		cout << endl;
	}

	auto it = vars.find(var.name);
	if(it == vars.end())
		return 0; // TODO should return Empty
	return it->second;
}

value_t eval(varmap_t& vars, FuncCall fn)
{	
	values vs;
	for(const auto& e: fn.exprs)
		vs.push_back(eval(vars, e));	

	auto it  =  neo_funcs.find(fn.name);
	if(it == neo_funcs.end()) {
		throw std::runtime_error("#UNK_FUNC: " + fn.name);
	}
	neo_func f = it->second;
	return f(vs);
}

value_t
eval(varmap_t& vars, Factor f)
{
	if(std::holds_alternative<value_t>(f.factor)) {
		return std::get<value_t>(f.factor);
	} else if(std::holds_alternative<Expression>(f.factor)) {
		return eval(vars, std::get<Expression>(f.factor));
	} else if(std::holds_alternative<FuncCall>(f.factor)) {
		return eval(vars, std::get<FuncCall>(f.factor));
	} else if(std::holds_alternative<Variable>(f.factor)) {
		return eval(vars, std::get<Variable>(f.factor));
	} else {
		throw std::logic_error("eval(Factor f) unhandled alternative");
	}

}



template<class T>
value_t eval_multiop(varmap_t& vars, T expr)
{
	assert(expr.operands.size()>0);
	value_t result = eval(vars, expr.operands[0]);
	for(int i = 0; i< expr.fops.size() ; i++) {
		auto& fop = expr.fops[i];
		result = fop(result, eval(vars, expr.operands[i+1]));
	}
	return result;
}

value_t eval(varmap_t& vars, Term t) { return eval_multiop(vars, t); }
value_t eval(varmap_t& vars, Relop r) { return eval_multiop(vars, r); }
value_t eval(varmap_t& vars, Expression e) { return eval_multiop(vars, e); }

value_t eval(varmap_t& vars, Let let)
{
	vars[let.varname] = eval(vars, let.expr);
	return 0;
}





template<typename T>
bool eval_holder(varmap_t& vars, Statement statement, value_t& v)
{
	if(std::holds_alternative<T>(statement)) {
		v = eval(vars, std::get<T>(statement));
		//cout << "eval_holder():" << to_string(v) << "\n";
		return true;
	}
	return false;
}
							


value_t eval(varmap_t& vars, For a_for)
{
	double i = num(eval(vars, a_for.from));
	double to = num(eval(vars, a_for.to));
	while(i <= to) {
		vars[a_for.varname] = i;
		eval(vars, a_for.statements);
		i++;
	}
	return 0;
}


value_t eval(varmap_t& vars, Statements statements)
{
	value_t ret;
	for(auto& s: statements) {
		bool executed = eval_holder<Expression>(vars, s, ret) 
			//|| eval_holder<Def>(vars, s, ret)
			//|| eval_holder<If>(vars, s, ret)
			|| eval_holder<Let>(vars, s, ret)
			|| eval_holder<For>(vars, s, ret)
			//|| eval_holder<While>(vars, s, ret)
			;
		if(!executed)
			std::logic_error("eval<Program>(): Unhandled statement type");
	}
	return ret;
}

value_t eval(varmap_t& vars, Program prog)
{
	eval(vars, prog.statements);
	return 0;
}




string str(value_t v)
{
	return std::to_string(std::get<num_t>(v));
}
void
test_parse_expression(varmap_t vars, std::string s)
{
	cout << "test_parse_expression():" << s << "\n";
	Expression e{parse_expression(s)};
	cout << "Done parsing. Now evaluating." << endl;
	cout << str(eval(vars, e)) << "\n" << endl;
}



///////////////////////////////////////////////////////////////////////////
// supporting functions

void bload(FILE* fp)
{
	string program;
	char buffer[1024];
	int n;
loop:
	n = fread(buffer, 1, sizeof(buffer), fp);
	if(n>0) {
	//while(int n = fread(buffer, 1 , sizeof(buffer), fp) >0) {
		for(int i=0; i< n; ++i) program += buffer[i];
		//program += buffer;
		goto loop;
	}

	//cout << "bload:\n" << program << "\n";
	parse_program(global_varmap, program);

}

ostream& operator<<(ostream& os, const value_t v)
{
	if(std::holds_alternative<num_t>(v))
		os << num(v);
	else if(std::holds_alternative<string>(v))
		os << std::get<string>(v);
	else
		throw std::logic_error("#UNHANDLED:conversion to string in alt-parse operator<<");

	return os;
}
///////////////////////////////////////////////////////////////////////////
// tests

static
bool test01()
{
	cout << "test01\n";

	lex_and_print("  r1C2 12.3e23 13.4");
	lex_and_print("goo(bar)");
	lex_and_print("1=2");
	lex_and_print("1!=2");
	lex_and_print("1<2");
	lex_and_print("1<=2");
	lex_and_print("1+2-3");
	lex_and_print("1*3/3");
	lex_and_print("?foo+12");
	lex_and_print("\"A space-delimited string\"");


	return true;
}

static void
test02()
{
	cout << "test02\n";
	auto test_expressions = { "66", "1-2-3", "2*3", "23<24",  "23+24", "1+2+3", "1+2*3", "4*2+3", "4/5/6", 
		"(1+2)*3",  "sqrt(3+9)", "pi()", "mod(12, 7)"};

	for(const string& s: test_expressions)
		test_parse_expression(global_varmap, s);


	//cout << "Now I am going to deliberately throw an error\n";
	//throw syntax_error();
}

void
test03()
{
	cout << "test03\n";
	//std::string prog = "FUNC foo (v) { println(v+1); } func bar () { } println(1, 2, 3, 12+13) foo(5)";
	std::string prog = "FUNC foo (?v) { println(1+1) println(22, 23) println(?v+1) } FUNC bar(?x) { foo(?x*?x) } bar(1+2) println(?pi)";
	//std::string prog = "FUNC foo (v) { println(1+1); }";
	cout << "parsing: " << prog << "\n";
	parse_program(global_varmap, prog);
}


bool run_alt_parse_tests()
{
	test01();
	test02();
	test03();
	return false;
}
