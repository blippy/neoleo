#include <cstdlib>

#include "mem.h"

mem::mem(bool auto_release)
{
	_release_on_destruction = auto_release;
}

void*
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

