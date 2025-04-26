module;

//#include <string>

#include "format.h"
#include "errors.h"
#include "global.h"
//#include "xcept.h"

export module errors;

import std;

void  _assert_uncalledXXX(const char* __file__, int __line__)
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


// FN raise_error 
export void raise_error (const char *str, ...) // FN
{
	va_list args;
	char buf[1000];
	va_start (args, str);
	vsprintf (buf, str, args);
	va_end(args);

	throw OleoJmp(buf);
}
// FN-END

export void raise_error (const std::string& msg) // FN
{
	raise_error("%s", msg.c_str());
}

// FN panic
export void panic (const char *s,...)
{
	va_list iggy;

	va_start (iggy, s);
	fprintf (stderr, "%s %s:", PACKAGE_NAME, VERSION);
	vfprintf (stderr, s, iggy);
	putc ('\n', stderr);
	va_end (iggy);
	exit (2);
}
// FN-END
