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

class value {
	public:
		value();
		~value();


		ValType type = TYP_NUL;
		ValType get_type() const;
		void set_type(ValType t);

		void sValue(value& newval);
		//void sValue(const value_t& newval);

		int gInt() const;
		void sInt(int newval);

		const char *gString();
		void sString(const char* newval);
		void sString(const std::string&  newval);

		num_t gFlt() const;
		void sFlt(num_t v);

		int gErr() const;
		void sErr(int newval);

		int gBol() const;
		void sBol(int newval);

		struct rng gRng() { return x.c_r; }
		void sRng(rng_t r);

		void free_string();
	public:
		union vals x;
};



num_t to_num(const value_t& v);
std::string to_str (const value_t& v);
bool is_string(const value_t& val);
ValType get_value_t_type(const value_t& val);
err_t to_err(const value_t& v);
bool is_num(const value_t& val);
bool is_err(const value_t& val); 
bool is_nul(const value_t& val); 
bool is_range(const value_t& val); 
rng_t to_range(const value_t& val) ;
value_t to_irreducible(value_t val);
	template <class T>
T tox (value_t val, int errtype)
{
	val = to_irreducible(val);

	if(std::holds_alternative<T>(val))
		return std::get<T>(val);
	else
		throw ValErr(errtype);
}
