#include <cassert>
#include <iostream>
#include <string.h>

#include "neotypes.h"
#include "sheet.h"
#include "value.h"

using namespace std;





ValType get_value_t_type(const value_t& val)
{
	if(is_nul(val)) 	return TYP_NUL;
	if(is_string(val)) 	return TYP_STR;
	if(is_num(val))		return TYP_FLT;
	if(is_err(val))		return TYP_ERR;
	if(is_range(val))	return TYP_RNG;
	if(is_bool(val))	return TYP_BOL;
	ASSERT_UNCALLED();
	return TYP_NUL;
}

bool is_nul(const value_t& val) { return std::holds_alternative<empty_t>(val); }
bool is_range(const value_t& val) { return std::holds_alternative<rng_t>(val); }
bool is_err(const value_t& val) { return std::holds_alternative<err_t>(val); }
bool is_num(const value_t& val) { return std::holds_alternative<num_t>(val); }
bool is_string(const value_t& val) { return std::holds_alternative<string>(val); }
bool is_bool(const value_t& val) { return std::holds_alternative<bool_t>(val); }

bool operator==(const rng_t& r1, const rng_t& r2)
{
	return r1.lr == r2.lr && r1.hr == r2.hr && r1.lr == r2.lr && r1.hr == r2.hr;
}

bool operator==(const value_t& v1, const value_t& v2)
{ 

	ValType t1 = get_value_t_type(v1);
	if(t1 != get_value_t_type(v2))
		return false;
	switch(t1) {
		case TYP_NUL:
			return true;
		case TYP_STR:
			return std::get<string>(v1) == std::get<string>(v2);
		case TYP_FLT:
			return std::get<num_t>(v1) == std::get<num_t>(v2);
		case TYP_ERR:
			return std::get<err_t>(v1).num == std::get<err_t>(v2).num;
		case TYP_RNG:
			return std::get<rng_t>(v1) == std::get<rng_t>(v2);
		case TYP_BOL:
			return std::get<bool_t>(v1).v == std::get<bool_t>(v2).v;
		default:
			ASSERT_UNCALLED();
			return false;
	}
}

bool operator!=(const value_t& lhs, const value_t& rhs)
{
	return !(lhs==rhs);
}

#define REQUIRE(x) if(!(x)) cout << __FILE__ << ":" << __LINE__  << ":"<<  #x << " FAIL\n";
void test_values()
{
	cout << "test_values ... ";
	value_t v1 =1.0, v2 = 1.0;
	REQUIRE(v1==v2);
	v2 = 3.0;
	REQUIRE(v1!= v2);
	cout << "done\n";
}
