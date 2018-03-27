#include <iostream>
#include <string>

#include "alt-lex.h"


using std::cout;
using std::string;

class Expression1 {};

Expression1 make_expression(lexemes_c& tokes)
{
	Expression1 expr;

	auto appendq = [](string lexvalue) { return ; };

	while(1) {
		switch(tokes.curr_type()) {
			case LT_FLT:
			       appendq(tokes.curr());
			       break;
			default:
			       cout << "make_expression:I'm confused\n";
		}
		tokes.advance();
	}
	return expr;
}

void test_expr(std::string s)
{
	auto tokes = alt_yylex_a(s);
	make_expression(tokes);
}
