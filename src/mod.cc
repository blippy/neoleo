// 2025-04-18 Created for testing purposes
// 25/4 Intellisense doesn't support modules yet

//module;

#include <string_view>
//import string_view;


//export module mod;

//#include <string_view>

//import std;

using namespace std::literals;


//#include "global.h"
//#include <string_view>
//#include <typeinfo>
//import <string_view>;

//using namespace std::literals;

const char* mod_hi()
{
	//return c();

	return  "module mod says hi";

}

// 25/4 good luck getting this working, even though it is simple

std::string_view c()
{
	return  "module mod says hi"sv;

}
