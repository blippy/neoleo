#include "format.h"
#include "xcept.h"

void  _assert_uncalled(const char* __file__, int __line__)
{
	std::string msg{string_format("%s:%d:ASSERT_UNCALLED failure", __file__, __line__)};
	throw std::logic_error(msg);
}


