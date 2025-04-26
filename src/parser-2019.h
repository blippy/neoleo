#pragma once

#include <deque>
#include <functional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "neotypes.h"

//typedef std::pair<int, std::string> token_t;
typedef struct {int type; std::string val;} token_t;
typedef std::deque<token_t> tokens_t;

class Expr;

typedef std::vector<Expr> args_t;


class Tour;

typedef std::function<value_t(Tour& tour, args_t)> parse_function_t;
typedef parse_function_t* funptr;
class FunCall { 
	public: 
		funptr fn ; 
		args_t args; 
};
class Expr { 
	public: 
		Expr();
		Expr(value_t v) : expr(v) {};
		Expr(std::string fname, Expr x);
		std::variant<FunCall, value_t> expr; 
};
Expr parse_string(std::string& s, ranges_t& predecs, CELLREF r, CELLREF c);
std::string set_and_eval(CELLREF r, CELLREF c, const std::string& formula, bool display_it);
