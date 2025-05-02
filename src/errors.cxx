module;

#include <stdarg.h>

#include "global.h"

export module errors;

import std;



/* https://www.quora.com/How-does-one-write-a-custom-exception-class-in-C++
 * */
export class OleoJmp : public std::exception
{
	public:
		OleoJmp() {}
		OleoJmp(const std::string& msg) : msg_(msg) {}

		virtual const char* what() const throw()
		{
			return msg_.c_str() ;
		}

	private:
		std::string msg_ = "OleoJmp";
};



export class ValErr : public std::exception
{
	public:
	       ValErr() {}
	       ValErr(const int n) : n(n) {}
	       const char* what() const throw();
	       const int num() const throw();

	private:
	       int n = 0;
	       std::string msg;
};



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





