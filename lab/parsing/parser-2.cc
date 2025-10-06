// 25/10 Let's try a new lexer (not yet started)

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

using namespace std;


///////////////////////////////////////////////////////////////////////////////////////////////////
// TYPE DECLARATIONS

class Expr;



typedef vector<Expr> args_t;
typedef double num_t;
typedef variant<num_t, int, string> value_t;
typedef function<value_t(args_t)> function_t;
typedef function_t* funptr;
//class Args { public: vector<Expr> args; };
//class FunCall;

class FunCall { 
	public: 
		funptr fn ; 
		args_t args; 
		//void set(string fnamej
};
class Expr { 
	public: 
		Expr() {};
		Expr(value_t v) : expr(v) {};
		Expr(string fname, Expr x);
		//Expr(string fname, args_t args);
		variant<FunCall, value_t> expr; 
};


//extern map<string, funptr> funcmap;
extern map<string, function_t> funcmap;

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

std::string to_string(const value_t& val)
{
	//if(std::get_if<v>(&val)) return "";
	if(auto v = std::get_if<num_t>(&val)) 	return to_string(*v);
	if(auto v = std::get_if<int>(&val)) 	return to_string(*v);
	if(auto v = std::get_if<std::string>(&val)) 	return "\""s + *v + "\""s;
	throw std::logic_error("Unhandled stringify_value_file_style value type");
}


void parse_error()
{
	throw 666;
}

value_t do_plus(args_t args)
{
	num_t val = 0;
	for(auto& arg: args) val += num_eval(arg);

	/*
	   if(args.size() ==0) return 0;
	   value_t val = eval(args[0]);
	   for(int i=1; i< args.size(); ++i) 
	   val = val + eval(args[i]);
	   */
	return val;
}
value_t do_minus(args_t args)
{
	if(args.size() == 0) return 0.0;
	num_t val = num_eval(args[0]);
	if(args.size() == 1) return -val; // if there is only one argument, then return the negative of it
	for(int i = 1; i<args.size(); ++i) val -= num_eval(args[i]);
	return val;
}
value_t do_mul(args_t args)
{
	num_t val = 1.0;
	for(auto& arg: args) {
		//num_t a = eval(arg);
		val *=  num_eval(arg);
		//cout << "do_mul a and val " << a << " " << val << "\n";
	}
	return val;
}
value_t do_div(args_t args)
{
	if(args.size() == 0) return 0.0;
	num_t val = num_eval(args[0]);
	//cout << "do_div 1/val " << 1.0/val << "\n";
	if(args.size() == 1) return 1.0/val;
	for(int i = 1; i<args.size(); ++i) val /= num_eval(args[i]);
	return val;
}

value_t do_sqrt(args_t args)
{
	if(args.size() !=1) parse_error();
	num_t val = num_eval(args[0]);
	return sqrt(val);
}
value_t do_hypot(args_t args)
{
	if(args.size() !=2) parse_error();
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
	if(args.size() !=1) parse_error();
	return (int) str_eval(args.at(0)).size();
	//num_t val = num_eval(args[0]);
	//return sqrt(val);
}

map<string, function_t> funcmap= {

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

#if 1
//typedef pair<int, string> Lexer;
//typedef deque<Lexer> tokens_t;

//enum Tokens { EOI, UNK, NUMBER, ID, PLUS };
enum Tokens { EOI = 128, NUMBER, ID, STR };

typedef struct {
	enum Tokens type;
	string text;
} 	token_t;

typedef deque<token_t> tokens_t;

class Lexer {
public:
	Lexer(string input);
	//token_t yylex();
	bool isfirst(char c);
	void consume(char ch);
	token_t front();
	void pop_front();
	~Lexer();

private:
	void tokenise(string str);
	void found(int type, string text);
	tokens_t tokens;
	//string m_input;
};


void Lexer::pop_front()
{
	tokens.pop_front();
}
token_t Lexer::front()
{
	return tokens.front();
}

Lexer::Lexer(string input)
{
	tokenise(input);

}

Lexer::~Lexer() {}

bool Lexer::isfirst(char c)
{
	return tokens.front().text == string{c};
}

void Lexer::found(int type, string text)
{
	tokens.push_back(token_t{(Tokens) type, text});
}


void Lexer::consume(char ch)
{
	if(isfirst(ch))
		tokens.pop_front();
	else
		parse_error();
}

void Lexer::tokenise(string str)
{
	//tokens_t tokens;

	//auto found = [&tokens](auto toketype, auto token) { tokens.push_back(token_t{toketype, token}); };
	cout << "Parsing: " << str << "\n";
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
		while(isalnum(ch)) { token += ch; ch = cstr[++pos]; }
		found(ID, token);
		//cout << "found id: " << token << "\n";
	} else if(ch == '"') {
		while(1) {
			ch = cstr[++pos];
			if(ch == 0 || ch == '"') {pos++; break; }
			token += ch;
		}
		cout << "tokenise string is <" << token << ">\n";
		found(STR, token);
	}else {
		token = ch;
		pos++;
		found(ch, token);
	}
	goto loop;
finis:
	found(EOI, "End of stream"); // Add this so that we can look ahead into the void
	//return tokens;
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// SCANNER (the "yacc" side of things)



	template<class Q>
Q rest(Q qs)
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



Expr parse_e(Lexer& lxr);
Expr parse_t(Lexer& lxr);

// parse a function
Expr parse_fn(string fname, Lexer& lxr)
{
	cout << "parse_fn name " << fname << "\n";
	auto fn = fn_lookup(fname);
	//cout << (*fn)(args_t{12}) << "\n";

	//consume('(', tokes);
	lxr.consume('(');
	FunCall fc;
	fc.fn = fn;
loop:
	if(lxr.isfirst(')')) goto finis;
	fc.args.push_back(parse_e(lxr));
	if(lxr.isfirst(',')) {
		lxr.consume(',');
		goto loop;
	} else if(!lxr.isfirst(')'))
		parse_error();

finis:
	lxr.consume(')');

	//args_t args{parse_e(tokes)};

	//fc.args = args;
	Expr x;
	x.expr = fc;

	//tokes.pop_front(); // rrb
	return x;
}

Expr parse_p(Lexer& lxr)
{
	//Expr t{parse_t(tokes)};
	token_t toke{lxr.front()};
	//tokes.pop_front();
	lxr.pop_front();
	switch(toke.type) {
		case EOI:
			return Expr();
		case NUMBER:
			return Expr(stoi(toke.text));
		case STR:
			return Expr(toke.text);
		case ID:
			if(lxr.isfirst('('))
				return parse_fn(toke.text, lxr);
			else
				parse_error(); // although could be a variable name
			break;
		case '(': {
				  //tokes.pop_front();
				  Expr x1{parse_e(lxr)};
				  if(lxr.isfirst(')'))
					  lxr.pop_front();
				  else
					  parse_error();
				  return x1;
			  }
		case '-':
			  return Expr("-", parse_t(lxr));
		default:
			  parse_error();
	}
	return Expr(); // should never reach here
}

Expr parse_f(Lexer& lxr) { return parse_p(lxr); }


FunCall _parse_t(Lexer& lxr)
{
	FunCall fc;
	fc.fn = &funcmap["*"];

	fc.args.push_back(parse_f(lxr));
	//return fc;
	while(1) {
		auto nid = lxr.front().type;
		if(nid == EOI) return fc;
		if(nid == '*') {
			lxr.pop_front();
			fc.args.push_back(parse_f(lxr));
		} else  if(nid == '/') {
			lxr.pop_front();
			Expr eneg =parse_f(lxr);
			FunCall fneg;
			fneg.fn = &funcmap["/"];
			fneg.args = args_t{eneg};
			Expr x2;
			x2.expr = fneg;
			fc.args.push_back(x2);		
		} else {
			return fc;
		}
	}
}
Expr parse_t(Lexer& lxr)
{
	FunCall fc{_parse_t(lxr)};

	if(fc.args.size() == 1)
		return Expr(fc.args[0]);
	else {
		Expr x;
		x.expr = fc;
		return x;
	}

}

FunCall _parse_e(Lexer& lxr)
{
	FunCall fc;
	fc.fn = &funcmap["+"];

	fc.args.push_back(parse_t(lxr));
	while(1) {
		auto nid = lxr.front().type;
		//cout << "nid is " << nid << "\n";
		if(nid == EOI) return fc;
		//tokes.pop_front();
		if(nid == '+') {
			lxr.pop_front();
			//cout <<" nod is +\n";
			fc.args.push_back(parse_t(lxr));
		} else  if(nid == '-') {
			lxr.pop_front();
			Expr eneg =parse_t(lxr);
			FunCall fneg;
			fneg.fn = &funcmap["-"];
			fneg.args = args_t{eneg};
			Expr x2;
			x2.expr = fneg;
			fc.args.push_back(x2);		
		} else {
			return fc;
		}
	}
}
Expr parse_e(Lexer& tokes)
{
	FunCall fc{_parse_e(tokes)};

	if(fc.args.size() == 1)
		return Expr(fc.args[0]);
	else {
		Expr x;
		x.expr = fc;
		return x;
	}

}



///////////////////////////////////////////////////////////////////////////////////////////////////
// EVAL

//template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
//template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

value_t eval (Expr expr)
{
	value_t val = 667;
	if(std::holds_alternative<value_t>(expr.expr))
		val = std::get<value_t>(expr.expr);
	else { // must be a function call		
		//auto &fn = std::get<FunCall>(expr.fn);
		auto &fc = std::get<FunCall>(expr.expr);
		auto fn = fc.fn;
		return (*fn)(fc.args);
	}

	return val;


}

num_t to_num (value_t val)
{
	//if(std::get_if<v>(&val)) return "";
	if(auto v = std::get_if<num_t>(&val)) 	return *v;
	if(auto v = std::get_if<int>(&val)) 	return *v;
	//if(auto v = std::get_if<std::string>(&val)) 	return "\""s + *v + "\""s;
	throw std::logic_error("Unhandled stringify_value_file_style value type");
}


num_t num_eval (Expr expr) { return to_num(eval(expr)); }
string to_str (value_t v) { return std::get<string>(v); }
string str_eval (Expr expr) { return to_str(eval(expr)); }
///////////////////////////////////////////////////////////////////////////////////////////////////
int interpret(string s, int expected)
{
	Lexer lxr(s);

#if 0
	tokens_t tokes{tokenise(s)};

	if constexpr (0) {
		for(auto& t:tokes) {
			cout << "Found: " << t.first << " " << t.second << "\n";
		}
	}
#endif

	Expr expr{parse_e(lxr)};
	auto val= num_eval(expr);
	cout << "Evaluates to " << to_string(val) << " ";
	cout.flush();
	if(to_num(val) == expected)
		cout << "PASS";
	else
		cout << "FAIL";
	cout << "\n\n";

	return 0;
}
int main()
{

	//return 0;
	interpret("42", 42);
	interpret("sqrt(4+5)+2", 5);
	interpret("42+3", 45);
	interpret("1+3+5+7", 16);
	interpret("1-3-5-7", -14);
	interpret("1-3+5-7", -4);
	interpret("1+2*3", 7);
	interpret("1+12/3", 5);
	interpret("1+12/3*2/2", 5);
	interpret("(1+2)*3", 9);
	interpret("-(1+2)*-3", 9);
	interpret("hypot(3,4)+1", 6);
	interpret("plus()+1", 1);
	interpret("plus(2)+1", 3);
	interpret("plus(2,3  +4  )  + 1", 10);
	interpret(" strlen(\"hello world\") ", 11);


	return 0;
}
