#pragma once

#include <string>

using std::string;

/* https://www.quora.com/How-does-one-write-a-custom-exception-class-in-C++ 
 * */
class OleoJmp : public std::exception
{
	public:
		OleoJmp() {}
		OleoJmp(const string& msg) : msg_(msg) {}

		virtual const char* what() const throw()
		{
			return msg_.c_str() ;
		}

	private:
		string msg_ = "OleoJmp";
};

class SyntaxError : public std::exception
{
	public:
		SyntaxError() {}
		SyntaxError(const string& msg) : msg_(msg) {}
		virtual const char* what() const throw() { return msg_.c_str() ; }
	private:
		string msg_ = "SyntaxError";
};
 
void  _assert_uncalled(const char* __file__, int __line__); 
#define	ASSERT_UNCALLED() { _assert_uncalled(__FILE__, __LINE__); }

