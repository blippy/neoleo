#include <cassert>
#include <cstdlib>
#include <cstring>

#include "mem.h"



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


