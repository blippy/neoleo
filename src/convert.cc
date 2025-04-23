#include <stddef.h>
#include <string>

#include "convert.h"



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
