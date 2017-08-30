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
		mem() {}
		mem(bool auto_release);
		void* add_ptr(void* ptr);
		void auto_release(); // destructor releases everything
		void release_all();
		~mem();


	private:
		std::vector<void *> ptrs;
		bool _release_on_destruction = false;
};
