#include <stddef.h>
#include <string>

#include "convert.h"
//#include "xcept.h"

double to_double(const char* strptr, bool &ok)
{
	double d = 666.0;
	ok = true;
	size_t idx;
	try { 
		d = std::stod(strptr, &idx);
	} catch(...) {
		ok = false;
	}
	return d;
}

double to_double(CELL* cp)
{
	double f = 666.0;
	value_t v = cp->get_value_2019();
	switch(cp->get_type()) {
		case TYP_NUL: return 0;
		case TYP_INT: 
		case TYP_FLT: 
			      return std::get<num_t>(v);

		case TYP_STR: {
				      bool ok;
				      f = to_double(std::get<std::string>(v).c_str(), ok);
				      if(!ok) throw ValErr();
				      return f;
			      }
		case TYP_ERR: throw ValErr();
		default:
			      ASSERT_UNCALLED();
	}
	return f;

}

long to_long(const char* strptr, bool &ok)
{
	long n = 666;
	ok = true;
	size_t idx;
	try { 
		n = std::stol(strptr, &idx);
	} catch(...) {
		ok = false;
	}
	return n;
}

long to_long(const std::string& str, bool &ok)
{
	return to_long(str.c_str(), ok);
}

long astol (char **ptr)
{
	long i = 0;
	int c;
	int sign = 1;
	char *s;

	s = *ptr;
	/* Skip whitespace */
	while (isspace (*s))
		if (*s++ == '\0')
		{
			*ptr = s;
			return (0);
		}
	/* Check for - or + */
	if (*s == '-')
	{
		s++;
		sign = -1;
	}
	else if (*s == '+')
		s++;

	/* Read in the digits */
	for (; (c = *s); s++)
	{
		if (!isdigit (c) || i > 214748364 || (i == 214748364 && c > (sign > 0 ? '7' : '8')))
			break;
		i = i * 10 + c - '0';
	}
	*ptr = s;
	return i * sign;
}
