#include <iostream>

#include <ncurses.h>

#include "io-2019.h"

using std::cout;

void main_command_loop_for2019()
{
	while(1) {
		char c = getch();
		if(c == 'q') break;
	}

	cout << "Exiting from 2019 io\n";
	exit(0);

}
