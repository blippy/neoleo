#include <algorithm>
#include <array>
#include <assert.h>
#include <functional>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>

using std::string;
using std::cout;
using std::endl;
using std::map;
using std::vector;

using namespace std::string_literals;

#include "tests.h"
#include "format.h"
#include "basic.h"
#include "io-term.h"
#include "io-utils.h"
#include "cell.h"
#include "neotypes.h"
#include "ref.h"
#include "sheet.h"

static bool all_pass = true; // all the checks have passed so far

int run_parser_2019_tests ();
int run_clear_test();

extern int test_values();

void 
check(bool ok, std::string msg)
{
	if(!ok) all_pass = false;
	std::string s = ok? "PASS" : "FAIL";
	std::cout << s << " " << msg << std::endl;

}



void
check_fmt(num_t v, const std::string& s)
{
	bool ok = string(pr_flt(v, &fxt, 2)) == s;
	check(ok, "check_fmt: " + s);

}
	
void
test_formatting()
{
	check_fmt(24.6f, "24.60");
	check_fmt(16.36f, "16.36"); // a source of potential rounding oddities
	check_fmt(0.0f,  "0.00");   // issue #10
	check_fmt(1.0f,  "1.00");
}


// a test that always fails
bool fail()
{
	return false;
}

// a test that always passes
bool pass()
{
	return true;
}



bool
run_regular_tests()
{
	test_formatting();

	check(pad_left("hello", 7) == "  hello", "padleft");
	check(pad_right("hello", 7) == "hello  ", "padright");

	cout << "Finished regular test\n";
	return all_pass;
}




template<typename... Args>
std::string format_sub_test(Args... args)
{
	return string_format(args...);
}
int format_tests()
{
	cout << "Format tests ...\n";
	cout << format_sub_test("Hello %s, meaning of life is %d\n", "world", 42);
	return 1;
}



bool
headless_tests()
{
	//bool all_pass = true;
	cout << "Running tests: " << option_tests_argument << "\n";

	using func_t = struct { string name; std::function<bool()> func; };
	vector<func_t> funcs = {
		{"clear",	run_clear_test},
		{"fail",	fail},
		{"fmt",		format_tests},
		{"parser2019",	run_parser_2019_tests},
		{"pass",	pass},
		{"regular", 	run_regular_tests},
		{"vals", 	test_values}
	};


	// maybe test is referred by a number
	auto idx = to_int(option_tests_argument).value_or(-1);
	int fsize = static_cast<int>(funcs.size());
	if(0 <= idx && idx < fsize ) {
		auto fn = funcs[idx].func;
		return fn();
	}

	// Wasn't referred by number, so possibly a name
	for(const auto &f : funcs) {
		if(f.name == option_tests_argument) {
			auto fn = f.func;
			return fn();
		}
	}
	
	using std::cerr;
	// use input was neither a number or valid description
	if(option_tests_argument != "") cerr << "Test not found: `" << option_tests_argument << "'\n";
	cerr << "Available tests are:\n";
	for(auto idx1 =0; idx1 < fsize; idx1++) {
		cerr << "\t" << idx1 << " " << funcs[idx1].name << "\n";
	}


	return false;
}

