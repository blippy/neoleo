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
#include "io-abstract.h"
#include "basic.h"
#include "io-headless.h"
#include "io-term.h"
#include "io-utils.h"
#include "cell.h"
#include "neotypes.h"
#include "ref.h"
#include "sheet.h"

static bool all_pass = true; // all the checks have passed so far

int run_parser_2019_tests ();
int run_bug44_tests();

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
	check_fmt(24.6DD, "24.60");
	check_fmt(16.36DD, "16.36"); // a source of potential rounding oddities
	check_fmt(0.0DD,  "0.00");   // issue #10
	check_fmt(1.0DD,  "1.00");
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
	bool all_pass = true;
	cout << "Running tests: " << option_tests_argument << "\n";

	map<string, std::function<bool()> > func_map = {
		{"44",		run_bug44_tests},
		{"fmt",		format_tests},
		{"parser2019",	run_parser_2019_tests},
		{"regular", 	run_regular_tests},
		{"vals", 	test_values}
	};

	//format_tests();

	auto it = func_map.find(option_tests_argument);
	if(it != func_map.end()) {
		auto fn = it->second;
		fn();
	} else {
		cout << "Test not found\nTests available are:\n";
		for(auto it=func_map.begin(); it != func_map.end(); ++it)
			cout << it->first << "\n";
	}


	return all_pass;
}

