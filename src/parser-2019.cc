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

#include "format.h"
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

// Use the topology algorithm for detecting cyclicty at
// https://en.wikipedia.org/wiki/Topological_sorting
// DFS (Depth First Search)

/* Algorithm is as follows:
 * L ← Empty list that will contain the sorted nodes
 * while exists nodes without a permanent mark do
 *     select an unmarked node n
 *     visit(n)
 *         
 * function visit(node n)
 *     if n has a permanent mark then return
 *     if n has a temporary mark then stop   (not a DAG)
 *     mark n with a temporary mark
 *     for each node m with an edge from n to m do
 *         visit(m)
 *     remove temporary mark from n
 *     mark n with a permanent mark
 *     add n to head of L
 */

class CyclicErr : public std::exception { };


class Tour {
	public:
		using marks_t = std::set<CELL*>;
		bool frozen(CELL* cp) { return pmarks.count(cp) != 0; };
		void freeze(CELL* cp) { pmarks.insert(cp); };
		void touch(CELL* cp) 
		{ 
			if(tmarks.count(cp)) // Existence implies cyclicity
				throw CyclicErr();
			tmarks.insert(cp);
		};
		void untouch(CELL* cp) { tmarks.erase(cp); }
	private:
		marks_t tmarks; // temporary mark
		marks_t pmarks; // permanent marks
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS

//err_t to_err(CELL* root, const value_t& v);
//num_t to_num(CELL* root, const value_t& v);
//std::string to_str (CELL* root, const value_t& v);
//rng_t to_range(const value_t& val) ;

void eval_cell (Tour& tour, CELL* cp);

/*
   bool is_cyclic(const value_t& v)
   {
   return is_err(v) && std::get<err_t>(v).num == CYCLE;
   }
   bool is_cyclic(const CELL* cp)
   {
   return is_cyclic(cp->get_value_2019());
   }

   void throw_if_cyclic(const value_t& v)
   {
   if(is_cyclic(v))
   throw CyclicErr();
   }
   */

value_t to_irreducible(Tour& tour, value_t val)
{
	if(!is_range(val))
		return val;

	// convert a point range to a value
	rng_t rng{std::get<rng_t>(val)};
	if( rng.lr != rng.hr || rng.lc != rng.hc)
		throw ValErr(BAD_NAME); 
	CELL* cp = find_or_make_cell(rng.lr, rng.lc);
	//if(root == cp) throw ValErr(CYCLE);
	//cout << "to_irreducible:" << string_coord(cp->coord) << "\n";
	eval_cell(tour, cp); // maybe too much evaluation?
	val = cp->get_value_2019();
	//throw_if_cyclic(val); // doesn't help
	return val;
}
	template <class T>
T tox (Tour& tour, value_t val, int errtype)
{
	val = to_irreducible(tour, val);

	if(std::holds_alternative<T>(val))
		return std::get<T>(val);
	else
		throw ValErr(errtype);
}
value_t eval(Tour& tour, Expr expr);
num_t num_eval(Tour& tour, Expr expr);
string str_eval(Tour& tour, Expr expr);


num_t to_num (Tour& tour, const value_t& v) { return tox<num_t>(tour, v, NON_NUMBER); }
err_t to_err (Tour& tour, const value_t& v) { return tox<err_t>(tour, v, ERR_CMD); }
string to_str (Tour& tour, const value_t& v) { return tox<string>(tour, v, NON_STRING); }

rng_t to_range(const value_t& val) 
{
	if(!is_range(val)) throw ValErr(NON_RANGE);
	return std::get<rng_t>(val);
}

num_t num_eval (Tour& tour, Expr expr);
std::string str_eval (Expr expr);
value_t eval_expr (Tour& tour, Expr expr);
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

value_t do_plus(Tour& tour, args_t args)
{
	num_t val = 0;
	for(auto& arg: args) val += num_eval(tour, arg);
	return val;
}
value_t do_minus(Tour& tour, args_t args)
{
	if(args.size() == 0) return 0;
	num_t val = num_eval(tour, args[0]);
	if(args.size() == 1) return -val; // if there is only one argument, then return the negative of it
	for(int i = 1; i<args.size(); ++i) val -= num_eval(tour, args[i]);
	return val;
}
value_t do_mul(Tour& tour, args_t args)
{
	num_t val = 1.0;
	for(auto& arg: args) {
		val *=  num_eval(tour, arg);
		//cout << "do_mul a and val " << a << " " << val << "\n";
	}
	return val;
}
value_t do_div(Tour& tour, args_t args)
{
	if(args.size() == 0) return 0;
	num_t val = num_eval(tour, args[0]);
	//cout << "do_div 1/val " << 1.0/val << "\n";
	if(args.size() == 1) return 1.0/val;
	for(int i = 1; i<args.size(); ++i) val /= num_eval(tour, args[i]);
	return val;
}

value_t do_sqrt(Tour& tour, args_t args)
{
	nargs_eq(args, 1);
	num_t val = num_eval(tour, args[0]);
	return sqrt(val);
}
value_t do_hypot(Tour& tour, args_t args)
{
	nargs_eq(args, 2);
	num_t v1 = num_eval(tour, args[0]);
	num_t v2 = num_eval(tour, args[1]);
	return sqrt(v1*v1 + v2*v2);
}
value_t do_plusfn(Tour& tour, args_t args)
{
	num_t val = 0;
	for(auto& v: args) val += num_eval(tour, v);
	return val;
}

value_t do_strlen(Tour& tour, args_t args)
{
	nargs_eq(args, 1);
	string str = str_eval(tour, args[0]);
	return str.size();
}

value_t do_life(Tour& tour, args_t args)
{
	return 42;
}

value_t do_sum (Tour& tour, args_t args)
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
		eval_cell(tour, cp); // too much?
		value_t v = cp->get_value_2019();
		sum += to_num(tour, v);
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


void eval_dependents (Tour& tour, const crefs_t& dependents)
{
	for(auto rc: dependents) {
		CELL* cp = find_cell(rc);
		if(!cp) continue;
		//cout << "eval_dependents: " << string_coord(rc) <<  "\n";
		eval_cell(tour, cp);
		CELLREF r = get_row(rc);
		CELLREF c = get_col(rc);
		io_pr_cell(r, c, cp);
	}
}
num_t num_eval (Tour& tour, Expr expr)	
{ 
	value_t v = eval_expr(tour, expr); 
	return to_num(tour, v); 
}

void eval_cell (Tour& tour, CELL* cp)
{
	//if(cp == root) throw ValErr(CYCLE);

	if(tour.frozen(cp)) return;  // its value has been fully resolved

	tour.touch(cp);
	//try {
	value_t old_value = cp->get_value_2019();
	cp->set_value_2019(eval_expr(tour, cp->parse_tree));

	tour.untouch(cp);
	tour.freeze(cp);
	if(old_value != cp->get_value_2019()) {
		eval_dependents(tour, cp->deps_2019);
	}
	//} catch(ValErr ve) {
	//	cp->set_error(ve);
	//}

	//cp->sValue(cp->the_value_t); // now done in set_value_2019()
	//throw_if_cyclic(cp->the_value_t);
}

value_t eval_expr (Tour& tour, Expr expr)
{
	value_t val;

	if(std::holds_alternative<value_t>(expr.expr)) {
		val = std::get<value_t>(expr.expr);
		val = to_irreducible(tour, val); // resolve single-celled ranges
	} else { // must be a function call		
		auto &fc = std::get<FunCall>(expr.expr);
		auto fn = fc.fn;
		val = (*fn)(tour, fc.args);
	}

	//throw_if_cyclic(val); // doesn't seem to help

	return val;
}


string str_eval (Tour& tour, Expr expr) 
{ 
	value_t v{eval_expr(tour, expr)};
	return to_str(tour, v); 
}
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


std::string set_and_eval (CELLREF r, CELLREF c, const std::string& formula, bool display_it = false)
{
	CELL* cp = find_or_make_cell(r, c);
	cp->set_formula_text(formula);
	try {
		Tour tour;
		eval_cell(tour, cp);
	} catch(CyclicErr ex) {
		cp->set_cyclic();
		//assert(is_cyclic(cp));
	} catch(ValErr ex) {
		cp->set_value_2019(err_t{ex.num()});
	}

	if(display_it) // this is really cack-handed
		io_pr_cell(r, c, cp);

	return print_cell(cp);
}

void check_result(string res, string expecting)
{

	cout << "Result is `" << res << "' " ;
	cout << (res == expecting ? "PASS"s : "FAIL") << "\n\n";
}

int interpret (int r, int c, string s, string expecting)
{
	cout << "Pret: R" << r << "C" << c << ": `" << s << "'";
	string res = set_and_eval(r,c, s);
	cout << " => `" << res << "' ";
	cout << (res == expecting ? "PASS"s : "FAIL") << "\n";
	return 1;
}

int interpret(string s, string expected)
{
	return interpret(1, 1, s, expected);
	/*
	   cout << "Interpreting `" << s << "'\n";

	   ranges_t predecs;
	   Expr expr{parse_string(s, predecs)};
	//cout << "point b\n";

	value_t v = eval(nullptr, expr);
	string res = stringify_value_file_style(v);

	cout << "Evaluates to `" << res << "' ";
	if(res == expected) 
	cout << "PASS";
	else
	cout << "(s/b `" << expected << "') FAIL";
	cout << "\n\n";

	return 0;
	*/
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
	interpret(1,4, "r1c3", "11");
	print_predecs(1,3);

	cout << "Check that dependent cells are updated\n";
	interpret(1, 1, "7", "7");
	check_result(cell_value_string(1, 3, 0), "13");
	check_result(cell_value_string(1, 4, 0), "13");
	cout << "Done\n";

	cout << "Cyclic check 1\n";
	interpret(10, 1, "r10c1", "#CYCLE");
	interpret(11, 1, "r10c1", "#CYCLE");
	cout << "Done\n";

	//value v = val;

	return 0;
}
