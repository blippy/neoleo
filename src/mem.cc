#include <cstdlib>

#include "mem.h"

void*
mem::add_ptr(void* ptr)
{
	ptrs.push_back(ptr);
}

void
mem::release_all()
{
	for(auto& p: ptrs) free(p);
	ptrs = {};
}

