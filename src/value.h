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
		ValType get_type();
		void set_type(ValType t);

		int gInt();
		void sInt(int newval);

		long gLong();
		void sLong(long newval);
		
		 char *gString();
		void sString(char* newval);

		num_t gFlt();
		void sFlt(num_t v);

		int gErr();
		void sErr(int newval);

		int gBol();
		void sBol(int newval);

		struct rng gRng() { return x.c_r; }
	public:
		union vals x;
};
