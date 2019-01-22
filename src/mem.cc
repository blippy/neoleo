#include <cassert>
#include <cstdlib>
#include <cstring>

#include "mem.h"

mem::mem(bool auto_release)
{
	_release_on_destruction = auto_release;
}

void
mem::add_ptr(void* ptr)
{
	ptrs.push_back(ptr);
}

void mem::auto_release()
{
	_release_on_destruction = true;
}

void
mem::release_all()
{
	for(auto& p: ptrs) free(p);
	ptrs = {};
}

mem::~mem()
{
	if(_release_on_destruction)
		release_all();
}

strcpy_c::strcpy_c(const char* str)
{
	null_terminated_str = (char*) malloc(1+ strlen(str));
	assert(null_terminated_str);
	strcpy(null_terminated_str, str);
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
