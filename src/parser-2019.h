#pragma once

#include <deque>
#include <functional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "neotypes.h"
#include "value.h"

inline bool use_parser_2019 = false;

typedef std::pair<int, std::string> token_t;
typedef std::deque<token_t> tokens_t;

class Expr;

typedef std::vector<Expr> args_t;
typedef double num_t;
typedef std::function<value_t(args_t)> parse_function_t;
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

num_t num_eval (Expr expr);
std::string str_eval (Expr expr);
Expr parse_string(const std::string& s, ranges_t& predecs);
value_t eval (Expr expr);
std::string set_and_eval(CELLREF r, CELLREF c, const std::string& formula, bool display_it);
