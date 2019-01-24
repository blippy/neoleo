#include <string.h>

#include "mem.h"
#include "value.h"

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

//long value::gLong() { assert(type == TYP_INT); return x.c_l; };
//void value::sLong(long newval) { type = TYP_INT; x.c_l = newval; };
		
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

num_t value::gFlt() const { return x.c_n ;};

void value::sFlt(num_t v) 
{
	free_string();	
	type = TYP_FLT; 
	x.c_n = v ;
}

int value::gErr() const { return x.c_err ;};
void value::sErr(int newval) { type = TYP_ERR ; x.c_err = newval ;};

int value::gBol() const { return x.c_b ;};
void value::sBol(int newval) { type = TYP_BOL; x.c_b = newval; };

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
