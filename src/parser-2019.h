#pragma once

#include <deque>
#include <functional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "neotypes.h"

inline bool use_parser_2019 = false;

typedef std::pair<int, std::string> token_t;
typedef std::deque<token_t> tokens_t;

class Expr;

typedef std::vector<Expr> args_t;
typedef double num_t;
//typedef std::variant<num_t, std::string> value_t;
typedef std::function<value_t(args_t)> parse_function_t;
typedef parse_function_t* funptr;
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
		Expr(std::string fname, Expr x);
		//Expr(string fname, args_t args);
		std::variant<FunCall, value_t> expr; 
};

tokens_t tokenise(std::string str);
num_t to_num (value_t v);
num_t num_eval (Expr expr);
std::string to_str (value_t v);
std::string str_eval (Expr expr);
Expr parse_e(tokens_t& tokes);
value_t eval (Expr expr);
bool is_string(value_t val);
//num_t to_num (value_t v) { return std::get<num_t>(v); }
std::string set_and_eval(CELLREF r, CELLREF c, const std::string& formula, bool display_it);
