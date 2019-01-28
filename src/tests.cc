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

// check for leaks, as suggested at 
// http://stackoverflow.com/questions/33201345/leaksanitizer-get-run-time-leak-reports
//#include <sanitizer/lsan_interface.h>


#include "tests.h"
#include "io-abstract.h"
#include "basic.h"
#include "io-headless.h"
#include "io-term.h"
#include "io-utils.h"
#include "cell.h"
#include "mem.h"
#include "neotypes.h"
#include "ref.h"
#include "byte-compile.h"
#include "decompile.h"
#include "sheet.h"
#include "parse_parse.h"

static bool all_pass = true; // all the checks have passed so far

extern bool yyreglex_experiment();

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




void parse_text(const std::string& expr)
{
	FormulaParser yy;
	cout << "Parsing:<" << expr << ">\n";
	yy.parse(expr);
	cout << "\n";
}

bool
test_yyparse_parse()
{
	FormulaParser yy, yy1;

	auto exprs = strings { "12", "13", "13+14", 
		"\"hello\"",
		"",
		"   ",
		"  \t\t     ",
		"     3 + 4   ",
		"1+", 
		"\"hello\"",
		"\"howde doode\"",
		"\"dangling string"
	};
	for(auto const& e: exprs) parse_text(e);
	
	cout << "Finished testing yyparse_parse\n";
	return true;
}


bool
run_regular_tests()
{
	test_formatting();

	check(pad_left("hello", 7) == "  hello", "padleft");
	check(pad_right("hello", 7) == "hello  ", "padright");

	test_yyparse_parse();
	cout << "Finished test\n";
	return all_pass;
}

bool run_cell_tests()
{
	CELL* cp3 = find_or_make_cell(1,1);

	dump_sheet();
	return true;

}

bool run_obsmem_tests()
{
	cout << "Running obsmem tests\n";

	obsmem m;
	cout <<"Should say hello world\n";
	char* s1 = "hello";
	char* s2 = "world";
	m.grow(s1, strlen(s1));
	m.grow1(' ');
	m.grow(s2, strlen(s2));
	m.grow1(0);
	puts((char*) m.finish());
	return true;
}

bool
headless_tests()
{
	bool all_pass = true;
	cout << "Running tests: " << option_tests_argument << "\n";

	map<string, std::function<bool()> > func_map = {
		{"cells",	run_cell_tests},
		{"obsmem",	run_obsmem_tests},
		{"regular", 	run_regular_tests},
		{"yyparse", 	test_yyparse_parse},
		{"yyreglex",	yyreglex_experiment}
	};

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

