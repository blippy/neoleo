#include <unistd.h>

#include <form.h>
#include <panel.h>
#include "menu-2025.h"
//#include "utils.h"
#include "spans.h"
#include "basic.h"
import utl;
#include "io-2019.h"
#include "io-curses.h"

import win;

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
	wrefresh(w);

	
	WINDOW *win = subwin(w, 1, 6, 3, 16); // lines cols y x
	defer1 d3(delwin, win);
	curs_set(2); // 0: invis, 1:normal, 2:very vis
	defer1 d4{curs_set, 0};
	// TODO
	int pos = input.size() , max_len = 5;
	//mvwaddstr(win, 0, 0, input.c_str());
	while(1) {
		win_set_line(win, input);
		//wmove(win, 1, 3); // set cursor
		move(3, 16+pos); // place cursor. I don't think this makes sense, but nevermind, wmove doesn't seem to work
		//win_print(win, input);
		wrefresh(win);	
		//move(20, 20);
		//refresh();
		int ch = get_ch();
		//int ch = mvwgetch(win, 0, pos);
		//int ch = getch();
		if(ch == '\r') break;
		//if(ch == CTRL('g')) break;
		if(ch == KEY_LEFT) {
			//input += '<';
			pos = max(pos-1, 0);
		} else if (ch == KEY_END) {
			pos = input.size();
		} else if (ch == KEY_HOME) {
			pos = 0;
		} else 	if(ch == KEY_RIGHT) {
			pos = min(pos+1, max_len);
			pos = min(pos, (int) input.size());
		} else if (ch == KEY_DC) {
			// delete key
			input.erase(pos, 1);
			pos = max(pos-1, 0);
		} else if((ch == KEY_BACKSPACE || ch == 127) && pos >0) { 
			pos--; 
			//wdelch(win);  
			input.erase(pos, 1);
			continue;
		} else {
			if(pos >= max_len) continue;
			input.insert(pos, string{static_cast<char>(ch)});
			//input += ch;
			//waddch(win, ch);			
			pos++;
		}
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

	cur_io_repaint();
	return true;

}
