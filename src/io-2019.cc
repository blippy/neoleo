#include <iostream>

#include <ncurses.h>

#include "io-2019.h"
#include "logging.h"
#include "window.h"

using std::cout;

constexpr int CTRL(int c) { return c & 037; }

void main_command_loop_for2019()
{
	// Tell ncusres to interpret "special keys". It means
	// that KEY_DOWN etc. will work, but ESC won't be
	// read separately
	keypad(stdscr, TRUE);

	while(1) {
		int c = getch();
		switch(c) {
			case CTRL('q'):  
				goto finis;
			case KEY_DOWN:
			case 'j':
				log_debug("io-2019:down arrow");
				io_shift_cell_cursor(1, 1);
				break;
			case KEY_UP:
			case 'k':
				io_shift_cell_cursor(0, 1);
				break;

		}

	}

finis:
	endwin();
	cout << "Exiting from 2019 io\n";
	exit(0);

}
