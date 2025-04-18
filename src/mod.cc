// 2025-04-18 Created for testing purposes
// 25/4 Intellisense doesn't support modules yet

//#include <string_view>

export module mod;

//#include "global.h"
//#include <string_view>
//#include <typeinfo>
//import <string_view>;

//using namespace std::literals;

export const char* mod_hi()
{
	return  "module mod says hi";

}

// 25/4 good luck getting this working, even though it is simple
/*
export std::string_view mod_hi_sv()
{
	return  "module mod says hi"sv;

}
*/	