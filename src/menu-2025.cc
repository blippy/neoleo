#include <ncurses.h>
#include <unistd.h>

#include <form.h>
#include <panel.h>
#include "menu-2025.h"
//#include "utils.h"
#include "spans.h"
#include "basic.h"
//import utl;
#include "io-2019.h"
#include "io-curses.h"
#include "win.h"

//import win;

using namespace std;

static bool col_width_form();





void show_menu (bool active) // FN
{
	std::string text{"m to activate menu"};
	if(active) text = "Col c";
	text = pad_right(text, COLS);

	// display menu
	int cp = 1; // color pair
	init_pair(cp, COLOR_BLACK, COLOR_CYAN);
	WINDOW *main_menu = stdscr;
	//WINDOW* main_menu = newwin(1, COLS, 0, 0);
	wmove(main_menu, 0, 0);
	wattron(main_menu, COLOR_PAIR(cp));

	wprintw(main_menu,"%s", text.c_str());
	wattroff(main_menu, COLOR_PAIR(cp));
	//refresh();
}



// shown when you hit the menu button (m key)
void process_menu() // FN
{
	show_menu(true);
	defer1 d(show_menu, false);
	
	switch(get_ch()) {
		case 'c': col_width_form(); break;
	}
}

//static constexpr int CTRL(int c) { return c & 037; }


// the examplar is io-2019.cc:nform_c
bool col_width_form() 
{
	string input{to_string(get_width())};
	//input = "hello";
	WINDOW *w = newwin(7, 30, 2 , 0); // lines cols y x
	defer1 d1(delwin, w);
	box(w, 0 ,0);


	
	win_edln ed(w, 6, 1, 3, "Cursor width:", input);
	ed.run();
	if(ed.m_cancelled) return true;
	input = ed.m_input;
	mvwprintw(w, 2, 2, "You said '%s'", input.c_str());
	//mvwprintw(w, 3, 2, "C for canel");
	wrefresh(w);
	bool accept = false;
	auto new_width = to_int(input);
	if(new_width) {
		mvwprintw(w, 3, 2, "A for accept, C for cancel");
		wrefresh(w);		
		while(1) {
			int ch = wgetch(w);
			if(ch == 'a') { accept = true; break;}
			if(ch == 'c') { break;}
		}
	} else {
		mvwprintw(w, 3, 2, "Bad input. C for cancel");
		wrefresh(w);
		while(wgetch(w) == 'c');
	
	}

	if(accept) {
		set_width(new_width.value());
	}

	//cur_io_repaint();
	return true;

}
