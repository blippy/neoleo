module;


#include "neotypes.h"
#include "sheet.h"
//#include "value.h"

export module value;
import std;

import errors;
using namespace std;


const char* ValErr::what() const throw()
{
	return ename_desc[n];

}
const int ValErr::num() const throw()
{
	return n;
}

export bool is_num(const value_t& val) { return std::holds_alternative<num_t>(val); }
export bool is_range(const value_t& val) { return std::holds_alternative<rng_t>(val); }

#if 0
// https://en.cppreference.com/w/cpp/utility/variant/visit2
export bool is_nul(const value_t& val) { return std::holds_alternative<std::monostate>(val); }
export bool is_err(const value_t& val) { return std::holds_alternative<err_t>(val); }

export bool is_string(const value_t& val) { return std::holds_alternative<string>(val); }
export bool is_bool(const value_t& val) { return std::holds_alternative<bool_t>(val); }


export ValType get_value_t_type(const value_t& val)
{
	if(is_nul(val)) 	return TYP_NUL;
	if(is_string(val)) 	return TYP_STR;
	if(is_num(val))		return TYP_FLT;
	if(is_err(val))		return TYP_ERR;
	if(is_range(val))	return TYP_RNG;
	if(is_bool(val))	return TYP_BOL;
	panic("Uncalled get_value_t_type");
	//ASSERT_UNCALLED();
	return TYP_NUL;
}

#endif
