#include <cassert>
#include <iostream>
#include <string.h>

//#include "cell.h"
#include "mem.h"
#include "neotypes.h"
#include "sheet.h"
#include "value.h"

using namespace std;

ValType value::get_type() const { return type;}
void value::set_type(ValType t) { type = t;}


void value::sValue(value& newval)
{
	type = newval.type;
	switch(type) {
		case TYP_NUL:
			break;
		case TYP_INT:
			sInt(newval.gInt());
			break;
		//case TYP_FLT:
		//	sFlt(newval.gFlt());
			break;
		case TYP_BOL:
			sBol(newval.gBol());
			break;
		case TYP_ERR:
			sErr(newval.gErr());
		default:
			// strings are going to be a pest
			assert(false);
	}
}

int value::gInt() const { return x.c_i; };
void value::sInt(int newval) { type = TYP_INT; x.c_i = newval; };

const char *value::gString()
{ 
	assert(type == TYP_STR); 
	const char* str = x.c_s;
	assert(str);
	return str; 
}

void value::sString(const char* newval) 
{ 
	//free_string(); // TODO I think that this should actually be called
	type = TYP_STR; 
	x.c_s = strdup(newval);
	assert(x.c_s);
}

void value::sString(const std::string&  newval)
{
	sString(newval.c_str());
}

num_t value::gFlt() const { return x.c_n ;};

void value::sFlt(num_t v) 
{
	//free_string();	
	type = TYP_FLT; 
	x.c_n = v ;
}

int value::gErr() const { return x.c_err ;};
void value::sErr(int newval) { type = TYP_ERR ; x.c_err = newval ;};

int value::gBol() const { return x.c_b ;};
void value::sBol(int newval) { type = TYP_BOL; x.c_b = newval; };

void value::sRng(rng_t r) { type = TYP_RNG; x.c_r = r; }

void value::free_string()
{
	if(type != TYP_STR) return;
	type = TYP_NUL;
	free_nonempty_str(&x.c_s);
	//if(x.c_s != nullptr) free(x.c_s);
	//x.c_s = nullptr;
}


value::value()
{
}

value::~value()
{
	free_string();
}


value_t to_irreducible(CELL* root, value_t val)
{
	if(!is_range(val))
		return val;

	// convert a point range to a value
	rng_t rng{std::get<rng_t>(val)};
	if( rng.lr != rng.hr || rng.lc != rng.hc)
		throw ValErr(BAD_NAME); 
	CELL* cp = find_or_make_cell(rng.lr, rng.lc);
	if(root == cp) throw ValErr(CYCLE);
	cp->eval_cell(); // maybe too much evaluation?
	val = cp->get_value_t();
	return val;
}
ValType get_value_t_type(const value_t& val)
{
	if(is_nul(val)) 	return TYP_NUL;
	if(is_string(val)) 	return TYP_STR;
	if(is_num(val))		return TYP_FLT;
	if(is_err(val))		return TYP_ERR;
	if(is_range(val))	return TYP_RNG;
	ASSERT_UNCALLED();
	return TYP_NUL;
}

bool is_nul(const value_t& val) { return std::holds_alternative<empty_t>(val); }
bool is_range(const value_t& val) { return std::holds_alternative<rng_t>(val); }
bool is_err(const value_t& val) { return std::holds_alternative<err_t>(val); }
bool is_num(const value_t& val) { return std::holds_alternative<num_t>(val); }
bool is_string(const value_t& val) { return std::holds_alternative<string>(val); }
num_t to_num (CELL* root, const value_t& v) { return tox<num_t>(root, v, NON_NUMBER); }
err_t to_err(CELL* root, const value_t& v) { return tox<err_t>(root, v, ERR_CMD); }
string to_str (CELL* root, const value_t& v) { return tox<string>(root, v, NON_STRING); }

rng_t to_range(const value_t& val) 
{
	if(!is_range(val)) throw ValErr(NON_RANGE);
	return std::get<rng_t>(val);
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
		case TYP_RNG: {
				      const rng_t& r1 = std::get<rng_t>(v1);
				      const rng_t& r2 = std::get<rng_t>(v2);
	      			      return r1.lr == r2.lr && r1.hr == r2.hr 
					      && r1.lr == r2.lr && r1.hr == r1.hr;
			      }
		default:
			ASSERT_UNCALLED();
			return false;
	}
}

bool operator!=(const value_t& lhs, const value_t& rhs)
{
	return !(lhs==rhs);
}

void test_values()
{
	cout << "test_values ... ";
	value_t v1 =1, v2 = 1;
	assert(v1==v2);
	v2 = 3;
	assert(v1!= v2);
	cout << "done\n";
}
