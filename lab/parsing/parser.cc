#include <cassert>
#include <ctype.h>
#include <deque>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

using namespace std;


///////////////////////////////////////////////////////////////////////////////////////////////////
// TYPE DECLARATIONS

class Expr;
typedef vector<Expr> args_t;
typedef function<int(args_t)> function_t;
//class Args { public: vector<Expr> args; };
class FunCall;
class FunCall { public: function_t* fn ; args_t args; };
class Expr { 
	public: 
		Expr() {};
		Expr(int i) : expr(i) {};
		/*
		Expr(string fn, Expr arg1, Expr arg2)
		{
			FunCall fc;
			fc.function_name = fn;
			fc.args = vector<Expr>{arg1, arg2};
			expr = fc;
		};
		*/
		variant<FunCall, int> expr; 
};
//typedef vector<Expr> args_t;
//typedef function<int(args_t)> function_t;
//class FunCall { public: string function_name ; function_t &fm ; vector<Expr> args; };
//class FunCall { public: string function_name ; function_t &fm ; Args args; };

/*
class Expr { 
	public: 
		Expr() {};
		Expr(int i) : expr(i) {};
		Expr(string fn, Expr arg1, Expr arg2)
		{
			FunCall fc;
			fc.function_name = fn;
			fc.args = vector<Expr>{arg1, arg2};
			expr = fc;
		};
		variant<FunCall, int> expr; 
};
*/


///////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS

int eval(Expr expr);



int do_plus(args_t args)
{
	int val = 0;
	for(auto& arg: args) val += eval(arg);
	return val;
}
int do_minus(args_t args)
{
	if(args.size() == 0) return 0;
	int val = eval(args[0]);
	if(args.size() == 1) return -val; // if there is only one argument, then return the negative of it
	for(int i = 1; i<args.size(); ++i) val -= eval(args[i]);
	return val;
}

map<string, function_t> funcmap= {
	{"+", &do_plus},
	{"-", &do_minus}
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// LEXER
typedef pair<int, string> token_t;
typedef deque<token_t> tokens_t;

//enum Tokens { EOI, UNK, NUMBER, ID, PLUS };
enum Tokens { EOI = 128, NUMBER, ID };


tokens_t tokenise(string str)
{
	tokens_t tokens;

	auto found = [&tokens](auto toketype, auto token) { tokens.push_back(make_pair(toketype, token)); };
	cout << "Parsing: " << str << "\n";
	const char* cstr = str.c_str();
	int pos = 0;
	auto it = str.begin();
loop:
	string token;
	char ch = cstr[pos];
	if(ch == 0) {
		goto finis;
	} else if ( isdigit(ch)) {
		while(isdigit(ch) || ch == '.' ) { token += ch; ch = cstr[++pos]; }
		found(NUMBER, token);
	} else if (isalpha(ch)) {
		while(isalnum(ch)) { token += ch; ch = cstr[++pos]; }
		found(ID, token);
		cout << "found id: " << token << "\n";
	} else {
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

void parse_error()
{
	throw 666;
}

/*
template<class R, class Q>
R pop(Q& qs)
{
	R front = qs.front();
	qs.pop_front();
	return front;
}
*/

template<class Q>
Q rest(Q qs)
{
	qs.pop_front();
	return qs;
}

/*
tokens_t rest(tokens_t tokes)
{
	tokes.pop_front();
	return tokes;
}
*/

typedef deque<string> ops_t;

Expr parse_e(tokens_t& tokes);

Expr parse_p(tokens_t& tokes)
{
	//Expr t{parse_t(tokes)};
	token_t toke = tokes.front();
	tokes.pop_front();
	switch(toke.first) {
		case EOI:
			return Expr();
		case NUMBER:
			return Expr(stoi(toke.second));
		default:
			parse_error();
	}
	return Expr(); // should never reach here
}
Expr parse_f(tokens_t& tokes) { return parse_p(tokes); }

Expr parse_t(tokens_t& tokes) { return parse_f(tokes); }


Expr parse_e(tokens_t& tokes)
{
	FunCall fc; // assume it's a funcall for now
	//fc.function_name = "+";
	fc.fn = &funcmap["+"];
	fc.args.push_back(parse_t(tokes));

	for(;;) {
		token_t toke = tokes.front();
		tokes.pop_front();
		if(toke.first == EOI) break;
	       	if(toke.first == '+')
			fc.args.push_back(parse_t(tokes));
		else if(toke.first == '-') {
			Expr eneg =parse_t(tokes);
			FunCall fneg;
			fneg.fn = &funcmap["-"];
			fneg.args = args_t{eneg};
			Expr x2;
			x2.expr = fneg;
			fc.args.push_back(x2);
		} else
		       	parse_error();
	}


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

int eval(Expr expr)
{
	int val = 667;
	if(std::holds_alternative<int>(expr.expr))
		val = std::get<int>(expr.expr);
	else { // must be a function call		
		//auto &fn = std::get<FunCall>(expr.fn);
		auto &fc = std::get<FunCall>(expr.expr);
		auto fn = fc.fn;
		return (*fn)(fc.args);
		/*
		if(fc.function_name == "+") {
			return do_plus(fc.args);
		}
		*/
	}

	return val;


}
///////////////////////////////////////////////////////////////////////////////////////////////////
int interpret(string s, int expected)
{
	tokens_t tokes{tokenise(s)};

	if constexpr (0) {
		for(auto& t:tokes) {
			cout << "Found: " << t.first << " " << t.second << "\n";
		}
	}

	Expr expr{parse_e(tokes)};
	int val = eval(expr);
	cout << "Evaluates to " << val << " ";
	if(val == expected) 
		cout << "PASS";
	else
		cout << "FAIL";
	cout << "\n\n";

	return 0;
}
int main()
{
	interpret("42", 42);
	interpret("42+3", 45);
	interpret("1+3+5+7", 16);
	interpret("1-3-5-7", -14);
	interpret("1-3+5-7", -4);
	return 0;
}
