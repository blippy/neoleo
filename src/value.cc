#include <cassert>
#include <iostream>
#include <string.h>

#include "neotypes.h"
#include "sheet.h"
#include "value.h"

import errors;
using namespace std;





ValType get_value_t_type(const value_t& val)
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

bool is_nul(const value_t& val) { return std::holds_alternative<std::monostate>(val); }
bool is_range(const value_t& val) { return std::holds_alternative<rng_t>(val); }
bool is_err(const value_t& val) { return std::holds_alternative<err_t>(val); }
bool is_num(const value_t& val) { return std::holds_alternative<num_t>(val); }
bool is_string(const value_t& val) { return std::holds_alternative<string>(val); }
bool is_bool(const value_t& val) { return std::holds_alternative<bool_t>(val); }

bool operator!=(const err_t& lhs, const err_t& rhs)
{
	return lhs.num != rhs.num;
}
bool operator==(const err_t& lhs, const err_t& rhs)
{
	return lhs.num == rhs.num;
}

bool operator==(const rng_t& r1, const rng_t& r2)
{
	return r1.lr == r2.lr && r1.hr == r2.hr && r1.lr == r2.lr && r1.hr == r2.hr;
}
bool operator!=(const rng_t& lhs, const rng_t& rhs) { return !(lhs==rhs); }
bool operator==(const bool_t& lhs, const bool_t& rhs) { return lhs.v==rhs.v; }
bool operator!=(const bool_t& lhs, const bool_t& rhs) { return !(lhs==rhs); }


#define REQUIRE(x) if(!(x)) cout << __FILE__ << ":" << __LINE__  << ":"<<  #x << " FAIL\n";
int test_values()
{
	cout << "test_values ... ";
	value_t v1 =1.0, v2 = 1.0;
	REQUIRE(v1==v2);
	v2 = 3.0;
	REQUIRE(v1!= v2);
	cout << "done\n";
	return 1;
}
