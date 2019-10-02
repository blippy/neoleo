#include <string>

#include "format.h"
#include "errors.h"
//#include "xcept.h"

void  _assert_uncalled(const char* __file__, int __line__)
{
	std::string msg{string_format("%s:%d:ASSERT_UNCALLED failure", __file__, __line__)};
	throw std::logic_error(msg);
}


const char* ValErr::what() const throw()
{
	return ename[n];
	//msg = std::string(ename[n]);
	//return msg.c_str();
	//return std::to_string(n).c_str();
}
const int ValErr::num() const throw()
{
	return n;
}
