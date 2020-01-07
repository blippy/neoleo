#pragma once
/* an attempt to clamp down on memory leaks by recording what 
 * what memory has been allocated.
 *
 * Note that memory is not automatically released, because
 * "it all depends"
 */

#include <functional>
#include <vector>


// offers a kludge around const and non-constant strings
// Use with care.
class strcpy_c {
	public:
		strcpy_c(const char* str);
		strcpy_c(const std::string& str);
		char* data() const;
		~strcpy_c();
	private:
		char* null_terminated_str;
};

/* Offers a way of performing automated clearup on exit */
class exit_c {
	public:
		exit_c(std::function<void()> fn) : fn(fn) {}
		~exit_c() { fn(); }
	private:
		std::function<void()> fn;
};

void free_nonempty(void **ptr);
void free_nonempty_str(char **ptr);
void free_nonempty_str(unsigned char **ptr);


