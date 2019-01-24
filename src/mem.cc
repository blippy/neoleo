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

void* mem::gimme(size_t n)
{
	void* ptr = malloc(n);
	assert(ptr);
	add_ptr(ptr);
	return ptr;
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


static void obsmem_reinit(obsmem* obj)
{
	obj->alloc(0);
}

obsmem::obsmem() 
{ 
	obsmem_reinit(this);
}

void obsmem::grow(void* data, int size)
{
	assert(!ptrs.empty());
	int sz = sizes[sizes.size()-1];
	std::byte* ptr = (std::byte*) realloc(ptrs[ptrs.size() -1], std::max(sz+size,1));
	assert(ptr);
	ptrs[ptrs.size() -1] = ptr;
	auto d1 = (std::byte*) data;
	bcopy(d1, ptr+sz, size);
	sizes[sizes.size()-1] += size;

}

void obsmem::grow1(char c)
{
	grow(&c, 1);
}

char* obsmem::alloc(int n)
{
	auto ptr = malloc(n ==0? 1 : n); // I don't like the idea of malloc() returning NULL is n==0
	assert(ptr);
	ptrs.push_back(ptr);
	sizes.push_back(n);
	return (char*) ptr;
}

int obsmem::size()
{
	return sizes[sizes.size()-1];
}

void* obsmem::finish()
{
	auto ptr = ptrs[ptrs.size() -1];			
	this->alloc(0);
	return ptr;
}


void obsmem::free_mem()
{
	// TODO should probably do something here
}

void obsmem::reset()
{
	sizes.clear();
	for(auto p: ptrs)
		if(p) free(p);
	ptrs.clear();
	obsmem_reinit(this);

}

obsmem::~obsmem() 
{ 
	sizes.clear();
	for(auto p: ptrs)
		if(p) free(p);
	ptrs.clear();
}

