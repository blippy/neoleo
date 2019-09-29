#include <cassert>
#include <ctype.h>
#include <deque>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

using namespace std;


///////////////////////////////////////////////////////////////////////////////////////////////////
// LEXER
typedef pair<int, string> token_t;
typedef deque<token_t> tokens_t;

enum Tokens { EOI, UNK, NUMBER, ID, PLUS };


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
	} else if (ch == '+') {
		token = ch;
		pos++;
		found(PLUS, token);
	} else {
		token = ch;
		pos++;
		found(UNK, token);
	}
	goto loop;
finis:
	found(EOI, "End of stream"); // Add this so that we can look ahead into the void
	return tokens;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// SCANNER (the "yacc" side of things)


// a little test to show us that we can get recursive definitions
class Expr;
class FunCall;
class FunCall { public: string function_name ; vector<Expr> args; };

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
	fc.function_name = "+";
	fc.args.push_back(parse_t(tokes));

	for(;;) {
		token_t toke = tokes.front();
		tokes.pop_front();
		if(toke.first == EOI) break;
		if(toke.first != PLUS) parse_error();
		fc.args.push_back(parse_t(tokes));
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
		auto &fc = std::get<FunCall>(expr.expr);
		if(fc.function_name == "+") {
			val = 0;
			for(auto& arg: fc.args)
				val += eval(arg);
			//val = eval(fc.args[0]) + eval(fc.args[1]);
		}
	}

	return val;


}
///////////////////////////////////////////////////////////////////////////////////////////////////
int interpret(string s)
{
	tokens_t tokes{tokenise(s)};

	if constexpr (0) {
		for(auto& t:tokes) {
			cout << "Found: " << t.first << " " << t.second << "\n";
		}
	}

	Expr expr{parse_e(tokes)};

	cout << "Evaluates to " << eval(expr) << "\n\n";

	return 0;
}
int main()
{
	interpret("42");
	interpret("42+3");
	interpret("1+3+5+7");
	return 0;
}
