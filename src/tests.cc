#include <algorithm>
#include <array>
#include <assert.h>
#include <functional>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdbool.h>
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
#include "mdi.h"
#include "mem.h"
#include "ref.h"
#include "byte-compile.h"
#include "decompile.h"
#include "lists.h"
#include "parse_parse.h"
#include "alt-cells.h"
#include "alt-parse.h"

static bool all_pass = true; // all the checks have passed so far

void 
check(bool ok, std::string msg)
{
	if(!ok) all_pass = false;
	std::string s = ok? "PASS" : "FAIL";
	std::cout << s << " " << msg << std::endl;

}





void FreeGlobals()
{
	/* TODO - make more accassable - neoleo doesn't seem
	 * to clean up after itself.
	 *
	 * This functionality is far from complete */
	FileCloseCurrentFile();
}


void get_set(int r, int c, const string& s)
{
	set_cell_from_string(r, c, s);
	printf("Formula at (%d,%d) is:", r, c);
	cout << get_cell_formula_at(r,c) << "\n";
	recalculate(1);
	printf("Cell value at (%d,%d) is:%s\n", r, c, cell_value_string(r,c, 0));
	puts("");
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

//void check_decomp(
void
run_cell_formula_tests()
{
	CELL c1, c2;
	//c2.
	c1 = c2;
	// TODO much expansion required

	string s1 = "r4c4 + 1";
	set_cell_from_string(5, 6, "r4c4 + 1");
	check(decomp_str(5, 6) == s1, "cellfrm-01");
	//decomp_free();

}

void decomp_n(int i, std::string s)
{
	cout << "decomp-" << i << "\n";
	set_cell_from_string(1, 1, s);
	cout << decomp_str(1,1) << "\n";
}

void test_decomp()
{
	decomp_n(1, "\"hello world\""s);

	set_cell_from_string(2, 1, "41");
	decomp_n(2, "1+r2c1"s);
	
	decomp_n(3, "concata(\"foo\", \"bar\")"s);
	decomp_n(4, "12.3"s);
	
}

void
test_yyparse_parse()
{
	// leak fixed by introduction of mem
	{
		mem yymem;
		yyparse_parse("\"foo\"", yymem);
		yymem.release_all();
	}

	// another leak test - which passes
	{
		mem yymem;
		yyparse_parse("len(\"foo\") + len(\"barzy\")", yymem);
		yymem.release_all();
	}

	// something similar used to cause a segfault.
	// Proper allocation is now done.
	{
		mem yymem;
		yyparse_parse("1+X", yymem);
		yymem.release_all();
	}

	cout << "PASS yyparse_parse\n";
}

void
misc_memchecks()
{
	// note that you need to have the sanitizer on to see if everything is OK
	
	// these seem OK as at 28-Aug-2017
	get_set(1, 1, "1.1+2");
	get_set(1, 1, "1.1+2.2");
	get_set(1, 1, "63.36");
	get_set(2, 1, "1 + R[-1]C");

	// leaks fixed as at 28-Aug-2017
	{
		char str1[] =  "\"foo\"";
		mem parser_mem;
		parser_mem.auto_release(); // for when you want to release out of scope
		char* ret = parse_and_compile(str1, parser_mem);
		//parser_mem.release_all();
	}
}

bool
run_regular_tests()
{
	test_formatting();
	run_cell_formula_tests();
	test_yyparse_parse();
	misc_memchecks();

	default_fmt = FMT_GEN;
	set_cell_from_string(2, 2, "23.3");
	CELL *cp = find_cell(2, 2);
	assert(cp);
	cout << "23.3=" << print_cell(cp) << "," << flt_to_str_fmt(cp) << "=\n";



	// Fixed a bug in undeclared variable names
	// This will barf if run under an address sanitizer
	get_set(1, 1, "foo"); 
	cout << "PASS: undeclared variable name\n";


	//printf("Test atof(63.36):%f\n", atof("63.36"));

	puts(pr_flt(1163.36DL, &fxt, FLOAT_PRECISION));
	puts(pr_flt(-1163.36DL, &fxt, FLOAT_PRECISION));
	puts(pr_flt(2688.9DL, &fxt, FLOAT_PRECISION));
	puts(pr_flt(3575.06DD, &fxt, FLOAT_PRECISION));


	FreeGlobals();

	cout << "Finished test\n";

	//__lsan_do_leak_check();
	test_decomp();
	return all_pass;
}

bool
headless_tests()
{
	bool all_pass = true;
	cout << "Running tests: " << option_tests_argument << "\n";

	map<string, std::function<bool()> > func_map = {
		{"alt-cells",	run_alt_cells_tests},
		{"alt-parse",	run_alt_parse_tests},
		{"regular", 	run_regular_tests}
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

