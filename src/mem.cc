#include <cassert>
#include <cstdlib>
#include <cstring>

#include "mem.h"


strcpy_c::strcpy_c(const char* str)
{
	null_terminated_str = (char*) malloc(1+ strlen(str));
	assert(null_terminated_str);
	strcpy(null_terminated_str, str);
}

strcpy_c::strcpy_c(const std::string&  str)
{
	null_terminated_str = (char*) malloc(1+ str.size());
	assert(null_terminated_str);
	strcpy(null_terminated_str, str.c_str());
}

char* strcpy_c::data() const
{
	return null_terminated_str;
}

strcpy_c::~strcpy_c()
{
	free(null_terminated_str);
}

void free_nonempty(void **ptr)
{
       if(*ptr) free(*ptr);
       *ptr = nullptr;
}

void free_nonempty_str(char** ptr)
{
	free_nonempty((void**) ptr);
}

void free_nonempty_str(unsigned char** ptr)
{
	free_nonempty((void**) ptr);
}


