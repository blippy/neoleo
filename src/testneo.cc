/*
 * neotest.cc
 *
 * Doing a general test of using the spreadsheet from a separate program
 *  Created on: 15 Oct 2025
 *      Author: pi
 * Works, too. That was surprisingly easy
 */

//#include <exception>
#include <iostream>
#include <stdio.h>
#include <string>

#include "parser-2019.h"
#include "sheet.h"

bool headless_tests();

using namespace std;



static bool all_pass = true; // all the checks have passed so far

bool run_parser_2019_tests ();
int run_clear_test();

//import value;

void check(std::string got, std::string expected, std::string msg)
{
	//cout << msg << "\t";
	bool ok = got == expected;
	if(ok) {
		cout << "PASS";
	} else {
		cout << "FAIL";
		all_pass = false;
	}
	cout << " " << msg << " Expected "  << expected << ", got " << got << "\n";

}


void check(bool ok, std::string msg)
{
	if(!ok) all_pass = false;
	std::string s = ok? "PASS" : "FAIL";
	std::cout << s << " " << msg << std::endl;

}





template<typename... Args>
std::string format_sub_test(Args... args)
{
	return string_format(args...);
}

bool format_tests()
{
	auto check_fmt = [](num_t v, const std::string& s) {
		bool ok = string(pr_flt(v, &fxt, 2)) == s;
		check(ok, "check_fmt: " + s);
	};
	check_fmt(24.6f, "24.60");
	check_fmt(16.36f, "16.36"); // a source of potential rounding oddities
	check_fmt(0.0f,  "0.00");   // issue #10
	check_fmt(1.0f,  "1.00");


	check(pad_left("hello", 7) == "  hello", "padleft");
	check(pad_right("hello", 7) == "hello  ", "padright");

	string s1{"Hello %s, meaning of life is %d"};
	string s2 = format_sub_test(s1, "world", 42);
	//cout << s2 << "\n";
	check(s2,  "Hello world, meaning of life is 42", "format_sub_test");

	time_t t =1745492070;
	check(fmt_std_date(t), "2025-04-24", "fmt_std_date");
	return all_pass;
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




//#define REQUIRE(x) if(!(x)) cout << __FILE__ << ":" << __LINE__  << ":"<<  #x << " FAIL\n";
bool test_values()
{
	//cout << "test_values ... ";
	value_t v1 =1.0, v2 = 1.0;
	check(v1==v2, "v1==v2");
	v2 = 3.0;
	check(v1!=v2, "v1!=v2");
	return all_pass;
}






bool headless_tests()
{
	//bool all_pass = true;
	cout << "Running tests: " << option_tests_argument << "\n";

	using func_t = struct { string name; std::function<bool()> func; };
	vector<func_t> funcs = {
		{"clear",	run_clear_test},
		//{"decomp-01", test_cecomp_01};
		{"fail",	fail},
		{"fmt",		format_tests},
		{"parser2019",	run_parser_2019_tests},
		{"pass",	pass},
		//{"regular", 	run_regular_tests},
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




bool check_result(CELLREF r, CELLREF c, const string& expecting)
{
	string res = cell_value_string(r, c, 0);
	//cout << std::format("Result  of R{}C{} is`{}' " , r, c, res);
	printf("Result  of R%dC%d is`%s' " , r, c, res.c_str());
	bool pass = res == expecting;
	cout << (pass ? "PASS"s : "FAIL") << "\n";
	return pass;
}

bool interpret (int r, int c, const string& s, const string& expecting)
{
	cout << "Pret: R" << r << "C" << c << ": `" << s << "'";
	string res = set_and_eval(r,c, s);
	cout << " => `" << res << "' ";
	bool pass = res == expecting;
	cout << (pass ? "PASS"s : "FAIL") << "\n";
	return pass;
}

bool interpret(const string& s, const string& expected)
{
	return interpret(1, 1, s, expected);
}
void print_predecs(CELLREF r, CELLREF c)
{
	cout << "PREDECS\n";
	CELL* cp = find_cell(r,c);
	for(const auto& rng: cp->predecs) {
		cout << "R" << rng.lr << ":" << rng.hr << "C" << rng.lc << ":" << rng.hc << "\n";
	}
	cout << "END PREDECS\n";
}

void done() { cout << "Done\n"; }

bool run_parser_2019_tests ()
{
	cout << "Running parser 2019 tests\n";
	bool all_pass = true;

	auto interpret1 = [&](auto& frm, auto& expect) {
		bool pass = interpret(1, 1, frm, expect);
		if(!pass) all_pass = false;
	};
	auto interpret2 = [&](int r, int c, auto& frm, auto& expect) {
		bool pass = interpret(r, c, frm, expect);
		if(!pass) all_pass = false;
	};
	auto check = [&](int r, int c, auto& val) {
		bool pass = check_result(r, c, val);
		if(!pass) all_pass = false;
	};


#if 1
	interpret1("floor(1.2)", "1");
	interpret1("ceil(1.2)", "2");
	interpret1("if(#TRUE, \"hello\", -3)", "hello");
	interpret1("sqrt(4+5)+2", "5");
	//return 0;
	interpret1("42", "42");
	interpret1("42+3", "45");
	interpret1("1+3+5+7", "16");
	interpret1("1-3-5-7", "-14");
	interpret1("1-3+5-7", "-4");
	interpret1("1+2*3", "7");
	interpret1("1+12/3", "5");
	interpret1("1+12/3*2/2", "5");
	interpret1("(1+2)*3", "9");
	interpret1("-(1+2)*-3", "9");
	interpret1("hypot(3,4)+1", "6");
	interpret1("plus()+1", "1");
	interpret1("plus(2)+1", "3");
	interpret1("plus(2,3  +4  )  + 1", "10");
	interpret1(" strlen(\"hello world\") ", "11");
	interpret1("1+", "#PARSE_ERROR");
	interpret1(" strlen(1) ", "#NON_STRING");
	interpret1("strlen(", "#PARSE_ERROR");
	interpret1("life()", "42");

	interpret2(1,1, "1+2", "3");
	interpret2(1,1, "1+", "#PARSE_ERROR");
	interpret2(1,1, "strlen(1)", "#NON_STRING");
	interpret2(1,1, "1 2", "#PARSE_ERROR");
	interpret2(1,1, "", "");

	// check on propagation of point cells
	interpret2(2,2, "2+3", "5");
	interpret2(3,2, "R2C2+1", "6");
	interpret2(3,2, "R2C2", "5");
	interpret2(3,3, "R3C2", "5");

	// check on sum
	interpret2(1,1, "2+3", "5");
	interpret2(1,2, "6", "6");
	interpret2(1,3, "sum(r1c1:2)", "11");
	interpret2(1,4, "r1c3", "11");
	print_predecs(1,3);

	cout << "Check that dependent cells are updated\n";
	interpret2(1, 1, "7", "7");
	check(1, 3, "13");
	check(1, 4, "13");
	done();

	cout << "Cyclic check \n";
	interpret2(10, 1, "r10c1", "#CYCLE");
	interpret2(11, 1, "r10c1", "#CYCLE");
	done();

	cout << "Diamond cycle check\n";
	interpret2(1, 1, "4", "4");
	interpret2(2, 1, "r1c1", "4");
	interpret2(2, 2, "r1c1", "4");
	interpret2(3, 1, "r2c1 + r2c2", "8");
	interpret2(1, 1, "10", "10");
	check(2, 1, "10");
	check(2, 2, "10");
	check(3, 1, "20");
	done();

	cout << "Check triangular cyclicity\n";
	interpret2(12, 1 , "r12c3", "");
	interpret2(12, 2 , "r12c1", "");
	interpret2(12, 3 , "r12c2", "#CYCLE");
	done();

	interpret2(13,1, "12.2", "12.2"); // check floats
	interpret2(13,1, "badfunc(12.2)", "#UNK_FUNC"); // an unknown function
	interpret2(13,1, "ceil(\"oops\")", "#NON_NUMBER");
	interpret2(13,1, "ceil(12 + 0.2)", "13");
	interpret2(13,1, "floor(12.2)", "12");
	interpret2(13,1, "2^(1+1+1)", "8");

	interpret2(13,1, "#TRUE", "#TRUE");
	interpret2(13,1, "#FALSE", "#FALSE");
	interpret2(13,1, "#OOPS", "#PARSE_ERROR");


	interpret2(13,1, "1<2", "#TRUE");
	interpret2(13,1, "1+1=2", "#TRUE");
	interpret2(13,1, "1+10!=2", "#TRUE");
	interpret2(13,1, "1<=2", "#TRUE");
	interpret2(13,1, "2>=1", "#TRUE");
	interpret2(13,1, "2>1", "#TRUE");
	interpret2(13,1, "1>2", "#FALSE");


	interpret2(13,1, "if(1>2, 10, 11)", "11");
	interpret2(13,1, "if(#TRUE, \"hello\", \"world\")", "hello");

	interpret2(14, 1, "14.1", "14.1");
	interpret2(14, 2, "r14c[-1]", "14.1");
#endif

	interpret2(15, 1, "15.2", "15.2");
	interpret2(15, 2, "rc[-1]", "15.2");
	interpret2(16, 2, "r14c+r15c2", "29.3");


	cout << "INFO: Completely finished parser2019 with overall result ";
	cout << (all_pass? "PASS" : "FAIL");
	cout << "\n";
	return all_pass;
}

int run_clear_test()
{
	interpret(1,1, "16", "16");
	clear_spreadsheet();
	CELL* cp = find_or_make_cell(1, 1);
	string res = string_cell(cp);
	cout << (res == "" ? "PASS" : "FAIL") << endl;
	return 0;
}




void exiting(bool all_pass)
{
	if(all_pass)
		exit(EXIT_SUCCESS);
	else
		exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{


	//puts("neotest says hi");
	//auto result = set_and_eval(1, 1, "7", false);
	//cout << "result is " << result << ", should be 7" << endl;

	if(argc<2) {
		exiting(headless_tests());
		//int ret = all_pass ? EXIT_SUCCESS : EXIT_FAILURE;
		//exit(ret);
	}
	string cmd{argv[1]};	
	if(cmd == "pass") { cout << "you want to pass\n"; }
	else if(cmd == "fail") { cout << "you want to fail\n"; return 1;}
	else if(cmd == "parser2019") { exiting(run_parser_2019_tests());}
	else { cout << "Unknown test. Failing\n"; return 1; }

	return 0;
}


