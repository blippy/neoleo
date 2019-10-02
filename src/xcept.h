#pragma once

#include <string>

//using std::string;

/* https://www.quora.com/How-does-one-write-a-custom-exception-class-in-C++ 
 * */
class OleoJmp : public std::exception
{
	public:
		OleoJmp() {}
		OleoJmp(const std::string& msg) : msg_(msg) {}

		virtual const char* what() const throw()
		{
			return msg_.c_str() ;
		}

	private:
		std::string msg_ = "OleoJmp";
};

class SyntaxError : public std::exception
{
	public:
		SyntaxError() {}
		SyntaxError(const std::string& msg) : msg_(msg) {}
		virtual const char* what() const throw() { return msg_.c_str() ; }
	private:
		std::string msg_ = "SyntaxError";
};
 
void  _assert_uncalled(const char* __file__, int __line__); 
#define	ASSERT_UNCALLED() { _assert_uncalled(__FILE__, __LINE__); }

class ValErr : public std::exception
{
	public:
	       ValErr() {}
	       ValErr(const int n) : n(n) {}
	       const char* what() const throw();
	       const int num() const throw();

	private:
	       int n = 0;
	       std::string msg;
};
