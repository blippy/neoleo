#include <cassert>
#include <ctype.h>
#include <deque>
#include <functional>
#include <iostream>
#include <map>
#include <cmath>
#include <string>
#include <string.h>
#include <variant>
#include <vector>

#include "cell.h"

#include "io-abstract.h"
#include "io-utils.h"
//#include "neotypes.h"
#include "regions.h"
#include "sheet.h"
//#include "xcept.h"

#include "parser-2019.h"

using namespace std;


///////////////////////////////////////////////////////////////////////////////////////////////////
// TYPE DECLARATIONS



extern map<string, parse_function_t> funcmap;

void unknown_function(string function_name)
{
	cerr << "Unknown function " << function_name << "\n";
	throw 667;
}

funptr fn_lookup(string function_name)
{
	if(funcmap.count(function_name) == 0)
		unknown_function(function_name);
	return &funcmap[function_name];
}

Expr::Expr()
{
	expr = empty_t{};
}

Expr::Expr(string fname, Expr x)
{
	FunCall fc;
	fc.fn = fn_lookup(fname);
	fc.args.push_back(x);
	this->expr = fc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS

value_t eval(Expr expr);
num_t num_eval(Expr expr);
string str_eval(Expr expr);


void parse_error()
{
	throw ValErr(PARSE_ERR);
	//throw 666;
}

void nargs_eq(const args_t& args, int n)
{
	if(args.size() != n)
		throw ValErr(BAD_FUNC);
}

value_t do_plus(args_t args)
{
	num_t val = 0;
	for(auto& arg: args) val += num_eval(arg);
	return val;
}
value_t do_minus(args_t args)
{
	if(args.size() == 0) return 0;
	num_t val = num_eval(args[0]);
	if(args.size() == 1) return -val; // if there is only one argument, then return the negative of it
	for(int i = 1; i<args.size(); ++i) val -= num_eval(args[i]);
	return val;
}
value_t do_mul(args_t args)
{
	num_t val = 1.0;
	for(auto& arg: args) {
		val *=  num_eval(arg);
		//cout << "do_mul a and val " << a << " " << val << "\n";
	}
	return val;
}
value_t do_div(args_t args)
{
	if(args.size() == 0) return 0;
	num_t val = num_eval(args[0]);
	//cout << "do_div 1/val " << 1.0/val << "\n";
	if(args.size() == 1) return 1.0/val;
	for(int i = 1; i<args.size(); ++i) val /= num_eval(args[i]);
	return val;
}

value_t do_sqrt(args_t args)
{
	nargs_eq(args, 1);
	num_t val = num_eval(args[0]);
	return sqrt(val);
}
value_t do_hypot(args_t args)
{
	nargs_eq(args, 2);
	num_t v1 = num_eval(args[0]);
	num_t v2 = num_eval(args[1]);
	return sqrt(v1*v1 + v2*v2);
}
value_t do_plusfn(args_t args)
{
	num_t val = 0;
	for(auto& v: args) val += num_eval(v);
	return val;
}

value_t do_strlen(args_t args)
{
	nargs_eq(args, 1);
	return strlen(str_eval(args.at(0)).c_str());
}

value_t do_life(args_t args)
{
	return 42;
}

value_t do_sum (args_t args)
{
	nargs_eq(args, 1);
	Expr& x = args[0];
	// TODO might hold fn instead
	value_t v = std::get<value_t>(x.expr);
	rng_t rng = to_range(v);
	num_t sum = 0;
	crefs_t coords = coords_in_range(rng);
	for(auto& coord: coords) {
		CELL* cp = find_cell(coord);
		if(!cp) continue;
		cp->eval_cell(); // too much?
		value_t v = cp->get_value_t();
		sum += to_num(v);
	}

	return sum;
}	

map<string, parse_function_t> funcmap= {
	{"sum", do_sum},
	{"life", do_life},
	{"strlen", do_strlen},
	{"+", &do_plus},
	{"-", &do_minus},
	{"*", &do_mul},
	{"/", &do_div},
	{"sqrt", do_sqrt},
	{"hypot", do_hypot},
	{"plus", do_plusfn}
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// LEXER

enum Tokens { EOI = 128, NUMBER, ID, STR };


	static tokens_t 
tokenise (string str)
{
	tokens_t tokens;

	auto found = [&tokens](auto toketype, auto token) { tokens.push_back(make_pair(toketype, token)); };
	//cout << "Parsing: " << str << "\n";
	const char* cstr = str.c_str();
	int pos = 0;
	auto it = str.begin();
loop:
	string token;
	char ch = cstr[pos];
	if(ch == 0) {
		goto finis;
	} else if(isspace(ch)) {
		while(isspace(ch)) { ch = cstr[++pos]; }
	} else if ( isdigit(ch)) {
		while(isdigit(ch) || ch == '.' ) { token += ch; ch = cstr[++pos]; }
		found(NUMBER, token);
	} else if (isalpha(ch)) {
		while(isalpha(ch)) { token += ch; ch = cstr[++pos]; }
		found(ID, token);
		//cout << "found id: " << token << "\n";
	} else if(ch == '"') {
		while(1) {
			ch = cstr[++pos];
			if(ch == 0 || ch == '"') {pos++; break; }
			token += ch;
		}
		//cout << "tokenise string is <" << token << ">\n";
		found(STR, token);
	}else {
		token = ch;
		pos++;
		found(ch, token);
	}
	goto loop;
finis:
	found(EOI, "End of stream"); // Add this so that we can look ahead into the void
	return tokens;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// SCANNER (the "yacc" side of things)


void consume(char ch, tokens_t& tokes)
{
	if(ch == tokes.front().first)
		tokes.pop_front();
	else
		parse_error();
}

token_t pop(tokens_t& tokes)
{
	token_t front = tokes.front();
	tokes.pop_front();
	return front;
}

	template<class Q>
Q rest (Q qs)
{
	qs.pop_front();
	return qs;
}


typedef deque<string> ops_t;

// ARITHMETIC
// Adopt the algorithm at 
// https://www.engr.mun.ca/~theo/Misc/exp_parsing.htm#classic
// for computing arithmetic
//
// Here's the original derivations
//  E --> T {( "+" | "-" ) T}
//  T --> F {( "*" | "/" ) F}
//  F --> P ["^" F]
//  P --> v | "(" E ")" | "-" T
//
// Here's mine:
// I extend BNF with the notion of a function, prefixed by &
// {} zero or more repetitions
// [] optional
//  &M(o, X) --> X {o X} // e.g. &M(("+"|"-"), T) --> T { ("+"|"-") T }
//  E --> &M(( "<" | "<=" | ">" | ">=" | "==" | "!=" ), R)
//  R --> &M(( "+" | "-" ), T)
//  T --> &M(( "*" | "/" ), F)
//  F --> ["+"|"-"] (v | "(" E ")")



Expr parse_e (tokens_t& tokes, ranges_t& predecs);
Expr parse_t(tokens_t& tokes, ranges_t& predecs);

// parse a function
Expr parse_fn (string fname, tokens_t& tokes, ranges_t& predecs)
{
	//cout << "parse_fn name " << fname << "\n";
	auto fn = fn_lookup(fname);
	//cout << (*fn)(args_t{12}) << "\n";

	consume('(', tokes);
	FunCall fc;
	fc.fn = fn;
loop:
	if(tokes.front().first == ')') goto finis;
	fc.args.push_back(parse_e(tokes, predecs));
	if(tokes.front().first == ',') {
		consume(',', tokes);
		goto loop;
	} else if(tokes.front().first != ')')
		parse_error();

finis:
	consume(')', tokes);

	//args_t args{parse_e(tokes)};

	//fc.args = args;
	Expr x;
	x.expr = fc;

	//tokes.pop_front(); // rrb
	return x;
}

void parse_slice (tokens_t& tokes, CELLREF& lower, CELLREF& upper)
{
	//cout << "parse_slice: front token:" << tokes.front().second << "\n";

	if(tokes.front().first != NUMBER)
		parse_error();
	lower = stoi(tokes.front().second);
	upper = lower;
	pop(tokes);

	if(tokes.front().first == ':') {
		pop(tokes);
		if(tokes.front().first != NUMBER)
			parse_error();
		upper = stoi(tokes.front().second);
		//cout << "parse_slice: upper:" << upper << "\n";
		pop(tokes);
	}
}

Expr parse_rc (tokens_t& tokes, ranges_t& predecs)
{
	rng_t rng;

	parse_slice(tokes, rng.lr, rng.hr);

	if(! ((tokes.front().second != "C")
				|| (tokes.front().second != "c")))
		parse_error();
	pop(tokes);

	parse_slice(tokes, rng.lc, rng.hc);
	predecs.push_back(rng);
	Expr x;
	x.expr = rng;
	return x;
}
Expr parse_p (tokens_t& tokes, ranges_t& predecs)
{
	//Expr t{parse_t(tokes)};
	token_t toke = tokes.front();
	tokes.pop_front();
	switch(toke.first) {
		//case EOI:
		//	return Expr();
		case NUMBER:
			return Expr(stoi(toke.second));
		case STR:
			return Expr(toke.second);
		case ID:  {
				  if(toke.second == "R" || toke.second == "r")
					  return parse_rc(tokes, predecs);
				  if(tokes.front().first == '(')
					  return parse_fn(toke.second, tokes, predecs);
				  else
					  parse_error(); // although could be a variable name
			  }
		case '(': {
				  Expr x1{parse_e(tokes, predecs)};
				  if(tokes.front().first == ')')
					  tokes.pop_front();
				  else
					  parse_error();
				  return x1;
			  }
		case '-':
			  return Expr("-", parse_t(tokes, predecs));
		default:
			  parse_error();
	}
	return Expr(); // should never reach here
}
Expr parse_f (tokens_t& tokes, ranges_t& predecs) { return parse_p(tokes, predecs); }


Expr simplify(const FunCall& fc)
{
	if(fc.args.size() ==0)
		return Expr();

	if(fc.args.size() == 1)
		return Expr(fc.args[0]);

	Expr x;
	x.expr = fc;
	return x;
}

	Expr 
parse_t (tokens_t& tokes, ranges_t& predecs)
{
	FunCall fc;
	fc.fn = &funcmap["*"];

	fc.args.push_back(parse_f(tokes, predecs));
	//return fc;
	while(1) {
		auto nid = tokes.front().first;
		if(nid == '*') {
			tokes.pop_front();
			fc.args.push_back(parse_f(tokes, predecs));
		} else  if(nid == '/') {
			tokes.pop_front();
			Expr eneg =parse_f(tokes, predecs);
			FunCall fneg;
			fneg.fn = &funcmap["/"];
			fneg.args = args_t{eneg};
			Expr x2;
			x2.expr = fneg;
			fc.args.push_back(x2);		
		} else break; 
	}
	return simplify(fc);
}



	Expr 
parse_e (tokens_t& tokes, ranges_t& predecs)
{
	FunCall fc;
	fc.fn = &funcmap["+"];

	fc.args.push_back(parse_t(tokes, predecs));
	while(1) {
		auto nid = tokes.front().first;
		if(nid == '+') {
			tokes.pop_front();
			fc.args.push_back(parse_t(tokes, predecs));
		} else  if(nid == '-') {
			tokes.pop_front();
			Expr eneg =parse_t(tokes, predecs);
			FunCall fneg;
			fneg.fn = &funcmap["-"];
			fneg.args = args_t{eneg};
			Expr x2;
			x2.expr = fneg;
			fc.args.push_back(x2);
		} else break;
	}
	return simplify(fc);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// EVAL

//template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
//template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;


num_t num_eval (Expr expr) { return to_num(eval(expr)); }
value_t eval (Expr expr)
{
	value_t val = 667;

	try {
		if(std::holds_alternative<value_t>(expr.expr)) {
			val = std::get<value_t>(expr.expr);
			val = to_irreducible(val); // resolve single-celled ranges
		} else { // must be a function call		
			//auto &fn = std::get<FunCall>(expr.fn);
			auto &fc = std::get<FunCall>(expr.expr);
			auto fn = fc.fn;
			return (*fn)(fc.args);
		}
	} catch(ValErr ve) {
		val = err_t{ve.num()};
	}

	return val;
}



string str_eval (Expr expr) { return to_str(eval(expr)); }
///////////////////////////////////////////////////////////////////////////////////////////////////

Expr parse_string (const std::string& s, ranges_t& predecs)
{
	predecs.clear();
	tokens_t tokes{tokenise(s)};

	if constexpr (0) {
		for(auto& t:tokes) {
			cout << "Found: " << t.first << " " << t.second << "\n";

		}
	}

	if(tokes.size() == 1) return Expr(); // the empty expression, EOI

	try {
		Expr x{parse_e(tokes, predecs)};
		//cout << "tokes remaining: " << tokes.size() << "\n";
		if(tokes.size() !=1) parse_error(); // remaining token s/b EOI
		return x;
	} catch (ValErr ve) {
		err_t err{ve.num()};
		return Expr{err};
	}
}

int interpret(string s, string expected)
{
	cout << "Interpreting `" << s << "'\n";

	ranges_t predecs;
	Expr expr{parse_string(s, predecs)};
	//cout << "point b\n";

	value_t v = eval(expr);
	string res = stringify_value_file_style(v);

	cout << "Evaluates to `" << res << "' ";
	if(res == expected) 
		cout << "PASS";
	else
		cout << "(s/b `" << expected << "') FAIL";
	cout << "\n\n";

	return 0;
}

std::string set_and_eval(CELLREF r, CELLREF c, const std::string& formula, bool display_it = false)
{
	CELL* cp = find_or_make_cell(r, c);
	cp->set_formula_text(formula);
	cp->eval_cell();
	//value_t val = eval(cp->parse_tree);
	//cp->sValue(val);
	if(display_it) // this is really cack-handed
		io_pr_cell(r, c, cp);

	return print_cell(cp);
}

void check_result(string res, string expecting)
{

	cout << "Result is `" << res << "' " ;
	cout << (res == expecting ? "PASS"s : "FAIL") << "\n\n";
}

int interpret(int r, int c, string s, string expecting)
{
	cout << "Interpreting R" << r << "C" << c << ": `" << s << "'\n";
	string res = set_and_eval(r,c, s);
	check_result(res, expecting);
	return 1;
}

void print_predecs(CELLREF r, CELLREF c)
{
	cout << "PREDECS\n";
	CELL* cp = find_cell(r,c);
	for(const auto& rng: cp->predecs) {
		cout << "R" << rng.lr << ":" << rng.hr << "C" << rng.lc << ":" << rng.hc << "\n";
	}
	cout << "END PREDECS\n";
}
int run_parser_2019_tests ()
{
	cout << "Running parser 2019 tests\n";

	use_parser_2019 = true;

	interpret("sqrt(4+5)+2", "5");
	//return 0;
	interpret("42", "42");
	interpret("42+3", "45");
	interpret("1+3+5+7", "16");
	interpret("1-3-5-7", "-14");
	interpret("1-3+5-7", "-4");
	interpret("1+2*3", "7");
	interpret("1+12/3", "5");
	interpret("1+12/3*2/2", "5");
	interpret("(1+2)*3", "9");
	interpret("-(1+2)*-3", "9");
	interpret("hypot(3,4)+1", "6");
	interpret("plus()+1", "1");
	interpret("plus(2)+1", "3");
	interpret("plus(2,3  +4  )  + 1", "10");
	interpret(" strlen(\"hello world\") ", "11");
	interpret("1+", "#PARSE_ERROR");
	interpret(" strlen(1) ", "#NON_STRING");
	interpret("strlen(", "#PARSE_ERROR");
	interpret("life()", "42");

	interpret(1,1, "1+2", "3");
	interpret(1,1, "1+", "#PARSE_ERROR");
	interpret(1,1, "strlen(1)", "#NON_STRING");
	interpret(1,1, "1 2", "#PARSE_ERROR");
	interpret(1,1, "", "");

	// check on propagation of point cells
	interpret(2,2, "2+3", "5");
	interpret(3,2, "R2C2+1", "6");
	interpret(3,2, "R2C2", "5");
	interpret(3,3, "R3C2", "5");

	// check on sum
	interpret(1,1, "2+3", "5");
	interpret(1,2, "6", "6");
	interpret(1,3, "sum(r1c1:2)", "11");
	print_predecs(1,3);

	cout << "Check that dependent cells are updated\n";
	interpret(1, 1, "7", "7");
	check_result(cell_value_string(1, 3, 0), "13");

	//value v = val;

	return 0;
}
