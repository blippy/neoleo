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
	string res = print_cell(cp);
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


