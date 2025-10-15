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

using namespace std;

int main()
{
	puts("neotest says hi");
	auto result = set_and_eval(1, 1, "7", false);
	cout << "result is " << result << ", should be 7" << endl;

	return 0;
}


