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
	//cerr << "Unknown function " << function_name << "\n";
	//throw 667;
	throw ValErr(BAD_FUNC);
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
 *     mar n with a temporary mark
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

void eval_cell (Tour& tour, CELL* cp);


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
bool bool_eval (Tour& tour, Expr expr);
num_t num_eval(Tour& tour, Expr expr);
string str_eval(Tour& tour, Expr expr);


bool to_bool (Tour& tour, const value_t& v) { return tox<bool_t>(tour, v, NON_BOOL).v; }
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
bool parse_error()
{
	throw ValErr(PARSE_ERR);
	return true;
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
		if(is_num(v))
			sum += to_num(tour, v);
	}

	return sum;
}	

num_t one_num(Tour& tour, args_t args)
{
	nargs_eq(args, 1);
	return num_eval(tour, args[0]);
}
value_t do_floor (Tour& tour, args_t args)
{
	num_t n = one_num(tour, args);
	return floor(n);
}
value_t do_ceil (Tour& tour, args_t args)
{
	num_t n = one_num(tour, args);
	return ceil(n);
}



void two_nums(Tour& tour, args_t args, num_t& v1, num_t& v2)
{
	nargs_eq(args, 2);
	v1  = num_eval(tour, args[0]);
	v2  = num_eval(tour, args[1]);
}

value_t do_pow (Tour& tour, args_t args)
{
	num_t x,y;
	two_nums(tour, args, x, y);
	return pow(x, y);

	//return bin_op(tour, args, pow);

}

value_t to_bool (num_t n)
{
	bool_t b;
	b.v = n != 0;
	return b;
}
value_t do_eq (Tour& tour, args_t args)
{
	num_t x,y;
	two_nums(tour, args, x, y);
	return to_bool(x == y);
}
value_t do_ne (Tour& tour, args_t args)
{
	num_t x,y;
	two_nums(tour, args, x, y);
	return to_bool(x != y);
}
value_t do_le (Tour& tour, args_t args)
{
	num_t x,y;
	two_nums(tour, args, x, y);
	return to_bool(x <= y);
}
value_t do_ge (Tour& tour, args_t args)
{
	num_t x,y;
	two_nums(tour, args, x, y);
	return to_bool(x >= y);
}
value_t do_lt (Tour& tour, args_t args)
{
	num_t x,y;
	two_nums(tour, args, x, y);
	return to_bool(x < y);
}
value_t do_gt (Tour& tour, args_t args)
{
	num_t x,y;
	two_nums(tour, args, x, y);
	return to_bool(x > y);
}

value_t do_if (Tour& tour, args_t args)
{
	nargs_eq(args, 3);
	bool test = bool_eval(tour, args[0]);
	if(test)
		return eval_expr(tour, args[1]);
	else
		return eval_expr(tour, args[2]);

}

value_t do_ctime (Tour& tour, args_t args)
{
	nargs_eq(args, 1);
        time_t tim = num_eval(tour, args[0]);
        //strcpy_c s1(ctime(&tim));
        //s1.data()[24] = 0;
	char* s1 = ctime(&tim);
	s1[24] = 0;
	//string s{ctime(&tim)};
	//s[24] = 0;
	return string{s1};
        //p->sString(s1.data());
}



map<string, parse_function_t> funcmap= {
	{"ctime", do_ctime},
	{"if", do_if},
	{"=", do_eq},
	{"!=", do_ne},
	{"<=", do_le},
	{">=", do_ge},
	{"<", do_lt},
	{">", do_gt},
	{"^", do_pow},
	{"ceil", do_ceil},
	{"floor", do_floor},
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

enum Tokens : unsigned char { EOI = 128, NUMBER, ID, STR, SYM, NE, GE, LE };


	static tokens_t 
tokenise (string str)
{
	tokens_t tokens;

	auto found = [&tokens](auto toketype, auto token) { tokens.push_back(make_pair(toketype, token)); };
	const char* cstr = str.c_str();
	int pos = 0;
	auto it = str.begin();
	string token;
	char ch;
	auto build_token = [&token, &ch, &pos, cstr] () { token += ch; ch = cstr[++pos]; };
	auto is_rel = [&ch, &pos, cstr](char rel) { 
		if(ch != rel) return false;
		//cout << "is_rel:1\n";
		if(cstr[pos+1] == '=') {
			pos += 2;
			//cout << "is_rel:true\n";
			return true;
		} else
			return false;
	};
loop:
	token = "";
	ch = cstr[pos];
	if(ch == 0) {
		goto finis;
	} else if(isspace(ch)) {
		while(isspace(ch)) { ch = cstr[++pos]; }
	} else if ( isdigit(ch)) {
		while(isdigit(ch) || ch == '.' ) build_token();
		found(NUMBER, token);
	} else if (isalpha(ch)) {
		while(isalpha(ch)) build_token();
		found(ID, token);
		//cout << "found id: " << token << "\n";
	} else if(ch == '"') {
		while(1) {
			ch = cstr[++pos];
			if(ch == 0 || ch == '"') {pos++; break; }
			token += ch;
		}
		found(STR, token);
	} else if(ch =='#') {
		token = "#";
		ch = cstr[++pos];
		while(ch && isalpha(ch)) build_token();
		found(SYM, token);
	} else if(is_rel('!'))
		found(NE, "!=");
	else if(is_rel('<'))
		found(LE, "<=");
	else if(is_rel('>'))
		found(GE, ">=");
	else {
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


//static int m_row, m_col; // I'm not a fan of this approach

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



Expr parse_e (tokens_t& tokes, ranges_t& predecs, CELLREF r, CELLREF c);
Expr parse_t(tokens_t& tokes, ranges_t& predecs, CELLREF r, CELLREF c);

// parse a function
Expr parse_fn (string fname, tokens_t& tokes, ranges_t& predecs, CELLREF r, CELLREF c)
{
	auto fn = fn_lookup(fname);

	consume('(', tokes);
	FunCall fc;
	fc.fn = fn;
loop:
	if(tokes.front().first == ')') goto finis;
	fc.args.push_back(parse_e(tokes, predecs, r, c));
	if(tokes.front().first == ',') {
		consume(',', tokes);
		goto loop;
	} else if(tokes.front().first != ')')
		parse_error();

finis:
	consume(')', tokes);

	Expr x;
	x.expr = fc;
	return x;
}


bool is_char(tokens_t& tokes, char c)
{

	if( tokes.front().first != c)
		return false;
	pop(tokes);
	return true;
}

bool is_cellref(tokens_t& tokes, CELLREF& ref)
{
	bool neg = is_char(tokes, '-');

	if(tokes.front().first != NUMBER)
		return false;
	ref = stoi(tokes.front().second);
	if(neg) ref = - ref;
	pop(tokes);
	return true;
}


bool abs_or_rel(tokens_t& tokes, CELLREF& n, bool& relative) 
{ 
	relative = false;
	if(is_char(tokes, '[') && is_cellref(tokes, n) && is_char(tokes, ']')) {
		relative = true;
		return true;
	}
	return  is_cellref(tokes, n); 
}

// e.g. 12 or 12:13
void parse_slice (tokens_t& tokes, CELLREF& lower, bool& lower_rel, CELLREF& upper, bool& upper_rel)
{
	abs_or_rel(tokes, lower, lower_rel); 
	upper = lower;
	upper_rel = lower_rel;
	is_char(tokes, ':') && abs_or_rel(tokes, upper, upper_rel);
}

Expr parse_rc_1 (tokens_t& tokes, ranges_t& predecs,  CELLREF r, CELLREF c, const string& command)
{
	rng_t rng{r, c, r, c};

	bool lr_rel = false, hr_rel = false, lc_rel = false, hc_rel = false;

	if(command != "rc") {
		parse_slice(tokes, rng.lr, lr_rel, rng.hr, hr_rel);
		if(lr_rel) 
			rng.lr += r;
		if(hr_rel) 
			rng.hr += r;
	}


	if(tolower(tokes.front().second[0]) == 'c') {
		pop(tokes);
	} else if (command != "rc") {
		parse_error();
	}

	parse_slice(tokes, rng.lc, lc_rel, rng.hc, hc_rel);
	if(lc_rel) 
		rng.lc += c;
	if(hc_rel) 
		rng.hc += c;

	predecs.push_back(rng);
	Expr x;
	x.expr = rng;
	return x;
}
Expr parse_p (tokens_t& tokes, ranges_t& predecs, CELLREF r, CELLREF c)
{
	token_t toke = tokes.front();
	tokes.pop_front();
	switch(toke.first) {
		case NUMBER:
			return Expr(stod(toke.second));
		case STR:
			return Expr(toke.second);
		case ID:  {
				  string lower = toke.second;
				  std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
				  if(lower== "r" || lower == "rc" )
					  return parse_rc_1(tokes, predecs, r, c, lower);
				  if(tokes.front().first == '(')
					  return parse_fn(toke.second, tokes, predecs, r, c);
				  else
					  parse_error(); // although could be a variable name
			  }
		case SYM: {
				  bool_t b;
				  if(toke.second == "#TRUE") { 
					  b.v = true;
					  return Expr(b);
				  }
				  if(toke.second == "#FALSE") { 
					  b.v = false;
					  return Expr(b);
				  }
				  parse_error();

			  }
		case '(': {
				  Expr x1{parse_e(tokes, predecs,r ,c)};
				  if(tokes.front().first == ')')
					  tokes.pop_front();
				  else
					  parse_error();
				  return x1;
			  }
		case '-':
			  return Expr("-", parse_t(tokes, predecs, r, c));
		default:
			  parse_error();
	}
	return Expr(); // should never reach here
}

Expr expr_funcall (FunCall fc)
{
	Expr x;
	x.expr = fc;
	return x;
}

Expr parse_f (tokens_t& tokes, ranges_t& predecs, CELLREF r, CELLREF c) 
{ 
	Expr x = parse_p(tokes, predecs, r, c); 
	if(tokes.front().first == '^') {
		tokes.pop_front();
		Expr y = parse_p(tokes, predecs, r, c);
		FunCall fc;
		fc.fn = &funcmap["^"];
		fc.args.push_back(x);
		fc.args.push_back(y);
		/*
		   Expr x1;
		   x1.expr = fc;
		   return x1;
		   */
		return expr_funcall(fc);
	} else
		return x;
}


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
parse_t (tokens_t& tokes, ranges_t& predecs, CELLREF r, CELLREF c)
{
	FunCall fc;
	fc.fn = &funcmap["*"];

	fc.args.push_back(parse_f(tokes, predecs, r , c));
	while(1) {
		auto nid = tokes.front().first;
		if(nid == '*') {
			tokes.pop_front();
			fc.args.push_back(parse_f(tokes, predecs, r, c));
		} else  if(nid == '/') {
			tokes.pop_front();
			Expr eneg =parse_f(tokes, predecs, r , c);
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
parse_e1 (tokens_t& tokes, ranges_t& predecs, CELLREF r, CELLREF c)
{
	FunCall fc;
	fc.fn = &funcmap["+"];

	fc.args.push_back(parse_t(tokes, predecs, r, c));
	while(1) {
		auto nid = tokes.front().first;
		if(nid == '+') {
			tokes.pop_front();
			fc.args.push_back(parse_t(tokes, predecs, r ,c));
		} else  if(nid == '-') {
			tokes.pop_front();
			Expr eneg =parse_t(tokes, predecs, r, c);
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


	Expr 
parse_e (tokens_t& tokes, ranges_t& predecs, CELLREF r, CELLREF c)
{

	Expr x = parse_e1(tokes,  predecs,r ,c);
	static unsigned char rel[] = { '=', NE, LE, GE, '<', '>' };
	auto nid = tokes.front().first;
	unsigned char *pos = std::find(rel, rel + sizeof(rel),  nid);
	if(pos != rel + sizeof(rel)) {
		FunCall fc;
		fc.fn = &funcmap[tokes.front().second];
		tokes.pop_front();
		fc.args.push_back(x);
		x = parse_e1(tokes, predecs, r, c);
		fc.args.push_back(x);
		return expr_funcall(fc);
	} else
		return x;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EVAL

//template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
//template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;


void eval_dependents (CELL* root)
{
	for(auto rc: root->deps_2019) {
		CELL* cp = find_cell(rc);
		if(!cp) continue;
		Tour tour; // we need to start again due to possible diamond shapes mucking things up
		tour.freeze(root); // we already know its value
		eval_cell(tour, cp);
		CELLREF r = get_row(rc);
		CELLREF c = get_col(rc);
		io_pr_cell(r, c, cp);
	}
}

bool bool_eval (Tour& tour, Expr expr)	
{ 
	value_t v = eval_expr(tour, expr); 
	return to_bool(tour, v); 
}
num_t num_eval (Tour& tour, Expr expr)	
{ 
	value_t v = eval_expr(tour, expr); 
	return to_num(tour, v); 
}

void eval_cell (Tour& tour, CELL* cp)
{
	if(tour.frozen(cp)) return;  // its value has been fully resolved

	tour.touch(cp);
	value_t old_value = cp->get_value_2019();
	cp->set_value_2019(eval_expr(tour, cp->parse_tree));

	tour.untouch(cp);
	tour.freeze(cp);
	if(old_value != cp->get_value_2019()) {
		eval_dependents(cp);
	}
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
	return val;
}


string str_eval (Tour& tour, Expr expr) 
{ 
	value_t v{eval_expr(tour, expr)};
	return to_str(tour, v); 
}
///////////////////////////////////////////////////////////////////////////////////////////////////

Expr parse_string (const std::string& s, ranges_t& predecs, CELLREF r, CELLREF c)
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
		Expr x{parse_e(tokes, predecs, r, c)};
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
	} catch(ValErr ex) {
		cp->set_value_2019(err_t{ex.num()});
	}

	if(display_it) // this is really cack-handed
		io_pr_cell(r, c, cp);

	return print_cell(cp);
}

void check_result(CELLREF r, CELLREF c, string expecting)
{

	string res = cell_value_string(r, c, 0);
	cout << "Result  of " << string_coord(r, c) << " is `" << res << "' " ;
	cout << (res == expecting ? "PASS"s : "FAIL") << "\n";
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

void done() { cout << "Done\n"; }

int run_parser_2019_tests ()
{
	cout << "Running parser 2019 tests\n";

	use_parser_2019 = true;

#if 1
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
	check_result(1, 3, "13");
	check_result(1, 4, "13");
	done();

	cout << "Cyclic check \n";
	interpret(10, 1, "r10c1", "#CYCLE");
	interpret(11, 1, "r10c1", "#CYCLE");
	done();

	cout << "Diamond cycle check\n";
	interpret(1, 1, "4", "4");
	interpret(2, 1, "r1c1", "4");
	interpret(2, 2, "r1c1", "4");
	interpret(3, 1, "r2c1 + r2c2", "8");
	interpret(1, 1, "10", "10");
	check_result(2, 1, "10");
	check_result(2, 2, "10");
	check_result(3, 1, "20");
	done();

	cout << "Check triangular cyclicity\n";
	interpret(12, 1 , "r12c3", "");
	interpret(12, 2 , "r12c1", "");
	interpret(12, 3 , "r12c2", "#CYCLE");
	done();

	interpret(13,1, "12.2", "12.2"); // check floats
	interpret(13,1, "badfunc(12.2)", "#UNK_FUNC"); // an unknown function
	interpret(13,1, "ceil(\"oops\")", "#NON_NUMBER"); 
	interpret(13,1, "ceil(12 + 0.2)", "13"); 
	interpret(13,1, "floor(12.2)", "12"); 
	interpret(13,1, "2^(1+1+1)", "8"); 

	interpret(13,1, "#TRUE", "#TRUE"); 
	interpret(13,1, "#FALSE", "#FALSE"); 
	interpret(13,1, "#OOPS", "#PARSE_ERROR"); 


	interpret(13,1, "1<2", "#TRUE"); 
	interpret(13,1, "1+1=2", "#TRUE"); 
	interpret(13,1, "1+10!=2", "#TRUE"); 
	interpret(13,1, "1<=2", "#TRUE"); 
	interpret(13,1, "2>=1", "#TRUE"); 
	interpret(13,1, "2>1", "#TRUE"); 
	interpret(13,1, "1>2", "#FALSE"); 


	interpret(13,1, "if(1>2, 10, 11)", "11"); 
	interpret(13,1, "if(#TRUE, \"hello\", \"world\")", "hello"); 

	interpret(14, 1, "14.1", "14.1");
	interpret(14, 2, "r14c[-1]", "14.1");
#endif

	interpret(15, 1, "15.2", "15.2");
	interpret(15, 2, "rc[-1]", "15.2");


	cout << "INFO: Completely finished parser2019\n";
	return 0;
}
