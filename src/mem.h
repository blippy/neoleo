#pragma once
/* an attempt to clamp down on memory leaks by recording what 
 * what memory has been allocated.
 *
 * Note that memory is not automatically released, because
 * "it all depends"
 */

#include <vector>

class mem {
	public:
		void* add_ptr(void* ptr);
		void release_all();


	private:
		std::vector<void *> ptrs;
};
