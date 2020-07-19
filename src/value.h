#pragma once

enum ValType { TYP_NUL=0, // taken to mean 'undefined'
	TYP_FLT=1, TYP_INT=2, TYP_STR=3, TYP_BOL=4, TYP_ERR=5, TYP_RNG=7 };

typedef std::variant<num_t, std::string, err_t, rng_t, bool_t, empty_t> value_t;


ValType get_value_t_type(const value_t& val);

bool is_string(const value_t& val);
bool is_num(const value_t& val);
bool is_err(const value_t& val); 
bool is_nul(const value_t& val); 
bool is_range(const value_t& val); 
bool is_bool(const value_t& val); 

//bool operator==(const value_t& v1, const value_t& v2);
//bool operator!=(const value_t& lhs, const value_t& rhs);
bool operator==(const err_t& lhs, const err_t& rhs);
bool operator!=(const err_t& lhs, const err_t& rhs);
bool operator!=(const rng_t& lhs, const rng_t& rhs);
bool operator!=(const bool_t& lhs, const bool_t& rhs);
bool operator!=(const empty_t& lhs, const empty_t& rhs);
