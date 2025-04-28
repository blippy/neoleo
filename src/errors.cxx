module;

#include <cstdarg>

#include "global.h"
#include "errors.h"

export module errors;

import std;
//import value;


//using std::va_list;
//using std::va_start;
//using std::va_end;


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
