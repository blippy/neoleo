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
#include <curses.h>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <unistd.h>
#include <variant>
#include <vector>

//#include "alt-cells.h"
#include "alt-lex.h"
#include "alt-types.h"
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
using std::unique_ptr;
using std::variant;
using std::vector;
using namespace std::string_literals;

typedef std::vector<std::string> strings;
typedef map<string, value_t> varmap_t;


ostream& operator<<(ostream& os, const value_t v);

///////////////////////////////////////////////////////////////////////////
// lexical analysis

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


bool is_str(value_t v) { return std::holds_alternative<string>(v); }
bool is_num(value_t v) { return std::holds_alternative<double>(v); }
//double dbl(value_t v) { return std::get<double>(v); }

string str(value_t v) 
{ 
	if(is_str(v))
		return std::get<string>(v);
	else if(is_num(v))
		return std::to_string(std::get<num_t>(v));
	else
		throw std::runtime_error("#UNHANDLED:str:conversion type");
}

num_t num(value_t v) { return std::get<num_t>(v); }

std::tuple<double, double> two_nums(values vs) 
{ 
	if(vs.size()!=2) 
		throw std::runtime_error("#BAD_CALL: Expected 2 arguments");
	double v1 = num(vs[0]), v2 = num(vs[1]);
	return std::make_tuple(v1, v2);
}




void require_nargs(const values& vs, int n , const std::string& caller)
{
	if(vs.size() != n)
		throw std::runtime_error("#NARGS:" + caller 
				+ ":expected " + std::to_string(n) + " args, got " + std::to_string(vs.size()));
}

void need(values vs, int n, string func_name)
{
	if(vs.size() != n)
		throw std::runtime_error("#FUNC_ARGS:" + func_name 
				+ ": requires " + std::to_string(n)  
				+ " args, got " + std::to_string(vs.size()));
}





value_t blang_add(values vs)
{ 
	need(vs, 2, "+");
	value_t v1 = vs[0], v2 = vs[1];
	if(is_str(v1) && is_str(v2))
		return str(v1) + str(v2);
	else
		return num(v1) + num(v2);
}
value_t blang_eq(values vs) { auto [v1, v2] = two_nums(vs); return v1 == v2; }
value_t blang_ge(values vs) { auto [v1, v2] = two_nums(vs); return v1 >= v2; }
value_t blang_gt(values vs) { auto [v1, v2] = two_nums(vs); return v1 > v2; }
value_t blang_le(values vs) { auto [v1, v2] = two_nums(vs); return v1 <= v2; }
value_t blang_lt(values vs) { auto [v1, v2] = two_nums(vs); return v1 < v2; }
value_t blang_ne(values vs) { auto [v1, v2] = two_nums(vs); return v1 != v2; }
value_t blang_sub(values vs) { auto [v1, v2] = two_nums(vs); return v1 - v2; }
value_t blang_mul(values vs) { auto [v1, v2] = two_nums(vs); return v1 * v2; }
value_t blang_div(values vs) { auto [v1, v2] = two_nums(vs); return v1 / v2; }





/*
using math_op = std::function<value_t(value_t, value_t)>;

value_t blang_add(value_t v1, value_t v2) { return num(v1) + num(v2); }
value_t blang_sub(value_t v1, value_t v2) { return num(v1) - num(v2); }
value_t blang_mul(value_t v1, value_t v2) { return num(v1) * num(v2); }
value_t blang_div(value_t v1, value_t v2) { return num(v1) / num(v2); }
value_t blang_lt(value_t v1, value_t v2) { return num(v1) < num(v2); }
value_t blang_le(value_t v1, value_t v2) { return num(v1) <= num(v2); }
value_t blang_gt(value_t v1, value_t v2) { return num(v1) > num(v2); }
value_t blang_ge(value_t v1, value_t v2) { return num(v1) >= num(v2); }
value_t blang_eq(value_t v1, value_t v2) { return num(v1) == num(v2); }
value_t blang_ne(value_t v1, value_t v2) { return num(v1) != num(v2); }
*/


/*
map<std::string, math_op> math_ops = { 
	{ "+", neo_add} , {"-", neo_sub }, 
	{"*",  neo_mul}, {"/", neo_div}, 
	{"<", neo_lt}, {"<=", neo_le}, {">", neo_gt}, {">=", neo_ge}, {"=", neo_eq}, {"!=", neo_ne}
};
*/

typedef std::function<value_t(values vs)> blang_func;
typedef map<string, blang_func> blang_funcs_t;



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

value_t neo_gotorc(values vs)
{
	require_nargs(vs, 2, "gotorc");
	curow = std::get<num_t>(vs[0]);
	cucol = std::get<num_t>(vs[1]);
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

value_t neo_refresh(values vs)
{
	refresh();
	return 0;
}

value_t neo_sleeps(values vs)
{
	require_nargs(vs, 1, "sleeps");
	double usecs = std::get<num_t>(vs[0]) * (double) 1'000'000;
	return usleep(usecs);
}

value_t neo_sqrt(values vs)
{
	if(vs.size() != 1) throw std::runtime_error("sqrt nargs");
	return std::sqrt(num(vs[0]));
}

blang_funcs_t blang_funcs = {
	{"<", blang_lt},
	{"<=", blang_le},
        {">", blang_gt},
        {">=", blang_ge},
        {"==", blang_eq},
        {"!=", blang_ne},
        {"+", blang_add},
        {"/", blang_div},
        {"*", blang_mul},
        {"-", blang_sub},

	//{"cell", neo_cell},
	{"cmd", neo_cmd},
	{"gotorc", neo_gotorc},
	{"mod", neo_mod},
	{"pi", neo_pi},
	{"println", neo_println},
	{"refresh", neo_refresh},
	{"sleeps", neo_sleeps},
	{"sqrt", neo_sqrt}
};


class Def;
class Factor;
class For;
class If;
class Let;
class While;



template<typename T>
class Precedence {
	        public: vector<T> operands; vector<blang_func> fops;
};

typedef Precedence<Factor> Term;
typedef Precedence<Term> Relop;
typedef Precedence<Relop> Expression;


typedef variant<Expression,Def,If,Let,For,While> Statement;
typedef vector<Statement> Statements;

class Program { public: Statements statements; };
class If { public: Expression condition; Statements consequent, alternative; };
class Let { public: string varname; Expression expr; };
class For { public: string varname; Expression from, to; Statements statements; };
class While { public: Expression condition; Statements statements; };



Statement make_statement(tokens& tokes);

value_t eval(varmap_t& vars, Expression e);
value_t eval(varmap_t& vars, Statements statements);
value_t eval(varmap_t& vars, Program prog);



class FuncCall{
	public:
		string name; // TODO evaluation would be quicker with a function pointer.
		vector<Expression> exprs;
};

class Variable { public: string name; };

class Factor { public: char sign = '+'; variant<value_t, Expression, FuncCall, Variable> factor; };

class Def{ public: string name; strings args; Statements statements; };


Expression make_expression(tokens& tokes);

template<typename T>
void make_funcargs(tokens& tokes,T make)
{
	require(tokes, "(");
	if(curr(tokes) != ")") {
		make(tokes);
		while(curr(tokes) == ",") {
			advance(tokes);
			make(tokes);
		}
	}
	require(tokes, ")");
}




FuncCall make_funccall(string name, tokens& tokes)
{
	//cout << "make_funccall()\n";
	FuncCall fn;
	fn.name = name;
	auto make = [&fn](tokens& tokes) { fn.exprs.push_back(make_expression(tokes)); };
	make_funcargs(tokes, make);
	return fn;
}
		
	

Variable
make_variable(string name, tokens& tokes)
{
	Variable var;
	var.name = name;
	//cout << "make_variable():variable:" << var.name << "\n";
	//tokes.advance();
	return var;
}

Factor
make_factor(tokens& tokes)
{
	Factor f;
	token toke = take(tokes);

	//optional sign
	if((toke.value == "+") || (toke.value == "-")) {
		f.sign = toke.value[0];
		toke = take(tokes);
	}


	switch(toke.type) {
		case LT_LRB:
			//tokes.advance();
			f.factor = make_expression(tokes);
			require(tokes, ")");
			//tokes.advance();
			break;
		case LT_FLT: 
			f.factor = std::stod(toke.value);
			//tokes.advance();
			break;
		case LT_STR:
			//f.factor = std::get<string>(tokes.curr());
			f.factor = toke.value.substr(1, toke.value.size()-2);
			//tokes.advance();
			break;
		case LT_ID:
			f.factor = make_funccall(toke.value, tokes);
			break;
		case LT_VAR:
			f.factor = make_variable(toke.value, tokes);
			break;
		default:
			throw std::logic_error("#UNHANDLED: make_factor:"s
				       + "type:"s + string(lextypename(toke.type))
				       + ", value:"s + toke.value);

	}

	return f;
}

template <typename T, typename U>
T parse_multiop(tokens& tokes, std::function<U(tokens&)> make, strings ops)
{
	T node;
	node.operands.push_back(make(tokes));
	while(1) {
		string op = curr(tokes);
		strings::iterator opit = std::find(ops.begin(), ops.end(), op);
		if(opit == ops.end()) break;
		tokes.pop_front();
		auto fop = blang_funcs.find(op)->second;
		node.fops.push_back(fop);

		node.operands.push_back(make(tokes));
	}
	return node;
}

Term make_term(tokens& tokes) { return parse_multiop<Term,Factor>(tokes, make_factor, {"*", "/"}); }
Relop make_relop(tokens& tokes) { return parse_multiop<Relop,Term>(tokes, make_term, {"+", "-"}); }
Expression make_expression(tokens& tokes) { return parse_multiop<Expression, Relop>(tokes, make_relop, 
		                {">", ">=", "<", "<=", "==", "!="}); }



If make_if(tokens& tokes)
{
	If an_if;
	require(tokes, "if");
	an_if.condition = make_expression(tokes);
	require(tokes, "then");

	int num_elses = 0;
	while(curr(tokes) != "fi") {
		if(curr(tokes) == "else") { num_elses++; advance(tokes); }
		switch(num_elses) {
			case 0:
				an_if.consequent.push_back(make_statement(tokes));
				break;
			case 1:
				an_if.alternative.push_back(make_statement(tokes));
				break;
			default:
				throw std::runtime_error("Unexpected else in alternative branch of 'if'");
		}
	}
	require(tokes, "fi");
	return an_if;
}



Let make_let(tokens& tokes)
{
	require(tokes, "let");
	//tokes.advance();
	Let let;
	let.varname = take(tokes).value;
	//tokes.advance();
	//cout << "make_let():varname:" << let.varname << "\n";
	require(tokes, ":=");
	//tokes.advance();
	let.expr = make_expression(tokes);
	return let; 
}

Statement make_statement(tokens& tokes);

Statements collect_statements(tokens& tokes, const string& terminator)
{
	Statements stmts;
	while(curr(tokes) != terminator)
		stmts.push_back(make_statement(tokes));
	require(tokes, terminator);
	return stmts;
}

For make_for(tokens& tokes)
{
	//cout << "make_for:in\n";
	require(tokes, "for");
	For a_for;
	a_for.varname = take(tokes).value;
	//cout << "make_for()::varname:" << a_for.varname;
	require(tokes, ":=");
	//cout << "make_for()::=from:" << curr(tokes);
	a_for.from = make_expression(tokes);
	require(tokes, "to");
	a_for.to = make_expression(tokes);
	a_for.statements = collect_statements(tokes, "next");
	//require(tokes, "next");
	//cout << "make_for:out\n";
	return a_for;
}


While make_while(tokens& tokes)
{
	require(tokes, "while");
	While a_while;
	a_while.condition = make_expression(tokes);
	a_while.statements = collect_statements(tokes, "wend");
	return a_while;
}


Def make_def(tokens& tokes);

Statement make_statement(tokens& tokes)
{
	static const map<string, std::function<Statement(tokens&)>> commands = {
		{"def",   make_def},
		{"if",    make_if},
		{"for",   make_for},
		{"let",   make_let},
		{"while", make_while}
	};

	Statement stm;
	auto it = commands.find(curr(tokes));
	if(it != commands.end()) {
		auto make = it->second;
		stm = make(tokes);
	}
	else
		stm = make_expression(tokes);
	return stm;
}
					


Program make_program(tokens& tokes)
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
	tokens tokes{alt_yylex_a(s)};
	return make_expression(tokes);
}


value_t eval(varmap_t& vars, Statements statements);

// Used to turn a user-defined function into a neo_func
value_t wrap_def(Def def, values vs)
{
	// bind argument identifiers to values
	if(vs.size() != def.args.size()) 
		throw std::runtime_error("#FUNC_ARGS:" + def.name 
				+ "():Expected " + std::to_string(def.args.size()) 
				+ " args, got " + std::to_string(vs.size()));
	varmap_t vars;
	for(int i=0; i< vs.size(); ++i)
		vars[def.args[i]] = vs[i];

	//cout << "wrap_defun():arg[0]:" << num(vs[0]) << "\n";

	return eval(vars, def.statements);
}



Def make_def(tokens& tokes)
{
	require(tokes, "def");
	Def def;
	def.name = take(tokes).value;
	auto make = [&def](tokens& tokes) { 
		auto toke = take(tokes);
		if(toke.type != LT_VAR)
			throw std::runtime_error("#PARSE: def of `" + def.name 
					+ "' has non-var arg " + toke.value);
		def.args.push_back(toke.value); 
	};
	make_funcargs(tokes, make);
	def.statements = collect_statements(tokes, "fed");

	using namespace std::placeholders;
	blang_funcs[def.name] = std::bind(wrap_def, def, _1);
	return def;
}




void
parse_program(varmap_t& vars, std::string s)
{
	//cout << "parse_program:in\n";
	//lex_and_print(s);
	tokens tokes{alt_yylex_a(s)};
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

	auto it  =  blang_funcs.find(fn.name);
	if(it == blang_funcs.end()) {
		throw std::runtime_error("#UNK_FUNC: " + fn.name);
	}
	blang_func f = it->second;
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
		result = fop({result, eval(vars, expr.operands[i+1])});
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



value_t eval(varmap_t& vars, Def def)
{
	// The function was actually created by make_def()
	// and put in blang_funcs, so there is nothing to do here
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


value_t eval(varmap_t& vars, If an_if)
{
	if(num(eval(vars, an_if.condition)))
		eval(vars, an_if.consequent);
	else
		eval(vars, an_if.alternative);
	return 0;
}

value_t eval(varmap_t& vars, While a_while)
{
	//cout << "eval<While>() called\n";
	while(1) {
		value_t test = eval(vars, a_while.condition);
		//cout << "eval<While>():test:" << num(test) << "\n";
		if(num(test) == 0) break;
		eval(vars, a_while.statements);
	}
	return 0;
}
		

value_t eval(varmap_t& vars, Statements statements)
{
	value_t ret;
	for(auto& s: statements) {
		bool executed = eval_holder<Expression>(vars, s, ret) 
			|| eval_holder<Def>(vars, s, ret)
			|| eval_holder<If>(vars, s, ret)
			|| eval_holder<Let>(vars, s, ret)
			|| eval_holder<For>(vars, s, ret)
			|| eval_holder<While>(vars, s, ret)
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




/*
string str(value_t v)
{
	return std::to_string(std::get<num_t>(v));
}
*/
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
