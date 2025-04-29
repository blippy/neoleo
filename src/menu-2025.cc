#include <unistd.h>

#include <form.h>
#include <panel.h>
#include "menu-2025.h"
#include "utils.h"
#include "spans.h"
#include "basic.h"
import logging;
#include "io-2019.h"
#include "io-curses.h"



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

static constexpr int CTRL(int c) { return c & 037; }


// the examplar is io-2019.cc:nform_c
bool col_width_form() 
{
	string input{to_string(get_width())};
	//input = "hello";
	WINDOW *w = newwin(7, 30, 2 , 0); // lines cols y x
	defer1 d1(delwin, w);
	box(w, 0 ,0);

	PANEL *p = new_panel(w);
	defer1 d2(del_panel, p);
	show_panel(p);


	

	wrefresh(w);
	

	mvwaddstr(w, 1,2, "Cursor width:");
	//mvwaddstr(w, 3, 2, "Return to accept");
	//mvwaddstr(w, 4, 2, "Ctl-g to reject");

	wrefresh(w);

	
	WINDOW *win = subwin(w, 1, 6, 3, 16); // lines cols y x
	defer1 d3(delwin, win);
	int pos = input.size(), len = 5;
	mvwaddstr(win, 0, 0, input.c_str());
	while(1) {
		int ch = mvwgetch(win, 0, pos);
		//int ch = getch();
		if(ch == '\r') break;
		//if(ch == CTRL('g')) break;
		if((ch == KEY_BACKSPACE || ch == 127) && pos >0) { 
			pos--; 
			wdelch(win);  
			input.erase(pos, 1);
			continue;
		}
		if(pos >= len) continue;
		input += ch;
		waddch(win, ch);
		wrefresh(win);		
		pos++;
	}

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
	//getch();


	//delwin(w);
	//log("current col:", cucol);
	cur_io_repaint();
	return true;

}
