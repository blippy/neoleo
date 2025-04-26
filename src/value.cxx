module;
/*
#include <cassert>
#include <iostream>
#include <string.h>
*/

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
	//msg = std::string(ename[n]);
	//return msg.c_str();
	//return std::to_string(n).c_str();
}
const int ValErr::num() const throw()
{
	return n;
}



#if 0
/* These are all the possible error codes that eval_expression() can return. */
enum { ERR_CMD =1, BAD_INPUT,  NON_NUMBER,  NON_STRING, NON_BOOL, NON_RANGE,  OUT_OF_RANGE, NO_VALUES,
	DIV_ZERO, BAD_NAME, NOT_AVAL, PARSE_ERR, NO_OPEN, NO_CLOSE, NO_QUOTE, BAD_CHAR, BAD_FUNC, CYCLE};

//#define ERR_MAX		18


//extern char *ename[];
#define CCC1(x) x
export inline const char *ename_desc[] =
{
	CCC1("#WHAT?"),
	CCC1("#ERROR"), CCC1("#BAD_INPUT"), CCC1("#NON_NUMBER"), CCC1("#NON_STRING"),
	CCC1("#NON_BOOL"), CCC1("#NON_RANGE"), CCC1("#OUT_OF_RANGE"), CCC1("#NO_VALUES"),
	CCC1("#DIV_BY_ZERO"), CCC1("#BAD_NAME"), CCC1("#NOT_AVAIL"), CCC1("#PARSE_ERROR"),
	CCC1("#NEED_OPEN"), CCC1("#NEED_CLOSE"), CCC1("#NEED_QUOTE"), CCC1("#UNK_CHAR"),
	CCC1("#UNK_FUNC"), CCC1("#CYCLE"),
	CCC1(0)
};


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
//bool operator!=(const empty_t& lhs, const empty_t& rhs);
#endif

export bool is_nul(const value_t& val) { return std::holds_alternative<std::monostate>(val); }
export bool is_range(const value_t& val) { return std::holds_alternative<rng_t>(val); }
export bool is_err(const value_t& val) { return std::holds_alternative<err_t>(val); }
export bool is_num(const value_t& val) { return std::holds_alternative<num_t>(val); }
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


//export auto operator<=>(const err_t& lhs, const err_t& rhs) = dafault;
//export bool operator!=(const err_t& lhs, const err_t& rhs) { 	return lhs.num != rhs.num; }
//export bool operator==(const err_t& lhs, const err_t& rhs) { 	return lhs.num == rhs.num; }

export bool operator==(const rng_t& r1, const rng_t& r2)
{
	return r1.lr == r2.lr && r1.hr == r2.hr && r1.lr == r2.lr && r1.hr == r2.hr;
}
export bool operator!=(const rng_t& lhs, const rng_t& rhs) { return !(lhs==rhs); }
export bool operator==(const bool_t& lhs, const bool_t& rhs) { return lhs.v==rhs.v; }
export bool operator!=(const bool_t& lhs, const bool_t& rhs) { return !(lhs==rhs); }


#define REQUIRE(x) if(!(x)) cout << __FILE__ << ":" << __LINE__  << ":"<<  #x << " FAIL\n";
export int test_values()
{
	cout << "test_values ... ";
	value_t v1 =1.0, v2 = 1.0;
	REQUIRE(v1==v2);
	v2 = 3.0;
	REQUIRE(v1!= v2);
	cout << "done\n";
	return 1;
}
