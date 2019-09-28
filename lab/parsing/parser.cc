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

enum Tokens { EOI, UNK, NUMBER, ID };


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
		variant<FunCall, int> expr; 
};

void parse_error()
{
	throw 666;
}

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

Expr parse_e(Expr expr, tokens_t tokes)
{
	auto front = tokes.front();
	auto fid = front.first;
	if(fid == EOI) return expr;
	tokes.pop_front();
	Expr xout;
	auto& next = tokes.front();
	auto nid = tokes.front().first;
	assert(fid == NUMBER);
	int val = stoi(front.second);
	if(next.second == "+") {
		FunCall fc;
		fc.function_name = "+";
		fc.args = vector{Expr(val), parse_e(xout,  rest(tokes))};
		xout.expr = fc;
	} else
		xout.expr = val;
	return xout;
}

Expr parse(tokens_t& tokes)
{
	Expr expr;
	return parse_e(expr, tokes);
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
		if(fc.function_name == "+")
			val = eval(fc.args[0]) + eval(fc.args[1]);
	}

	return val;


}
///////////////////////////////////////////////////////////////////////////////////////////////////
int interpret(string s)
{
	tokens_t tokes{tokenise(s)};
	for(auto& t:tokes) {
		cout << "Found: " << t.first << " " << t.second << "\n";
	}

	Expr expr{parse(tokes)};

	cout << "Evaluates to " << eval(expr) << "\n";

	return 0;
}
int main()
{
	interpret("42");
	interpret("42+3");
	interpret("1+3+5+7");
	return 0;
}
