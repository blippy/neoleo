#pragma once

#include <cassert>

#include "global.h"
#include "numeric.h"

enum ValType { TYP_NUL=0, // taken to mean 'undefined'
	TYP_FLT=1, TYP_INT=2, TYP_STR=3, TYP_BOL=4, TYP_ERR=5, TYP_RNG=7 };

union vals {
	num_t c_n;
	char *c_s;
	long c_l;
	int c_i;
	int c_err;
	bool c_b;
	struct rng c_r;
};




ValType get_value_t_type(const value_t& val);

bool is_string(const value_t& val);
bool is_num(const value_t& val);
bool is_err(const value_t& val); 
bool is_nul(const value_t& val); 
bool is_range(const value_t& val); 
bool is_bool(const value_t& val); 

bool operator==(const value_t& v1, const value_t& v2);
bool operator!=(const value_t& lhs, const value_t& rhs);
