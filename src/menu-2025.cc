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

class win_edln {
	public:
		win_edln(WINDOW *parent, int ncols, int begin_y, int begin_x, const string& desc, const string& input);
		~win_edln();
		void run();
		//WINDOW *m_win;
		WINDOW* m_parent;
		int m_begin_y, m_off_x, m_ncols, m_at_y, m_at_x;
		string m_input;
		bool m_cancelled = false;
	
};

win_edln::win_edln(WINDOW *parent, int ncols, int begin_y, int begin_x, const string& desc, const string& input)
{
	curs_set(2); // 0: invis, 1:normal, 2:very vis
	
	//defer1 d4{curs_set, 0};
	m_parent = parent;

	win_print(parent, begin_y, begin_x, desc);
	win_print(parent, input);
	wrefresh(parent);
	m_begin_y = begin_y;
	m_off_x = begin_x + desc.size();
	m_input = input;
	m_ncols = ncols;
	getbegyx(parent, m_at_y, m_at_x); // where the window starts
	//keypad(parent, TRUE); // might also be necessary for ESC key detection
	notimeout(parent, FALSE); // capture escape
	//nodelay(stdscr, TRUE); // we want to detect keys immediately
}

win_edln::~win_edln()
{
	//delwin(win);
	curs_set(0); // invisible
}

void win_edln::run()
{
// log("win_edln::run");

	int pos = m_input.size(); // , max_len = 5;
	//mvwaddstr(win, 0, 0, input.c_str());
	while(1) {
		win_print(m_parent, m_begin_y, m_off_x, pad_right(m_input, m_ncols));
		//win_set_line(win, input);
		//wmove(win, 1, 3); // set cursor
		//move(3, 16+pos); // place cursor. I don't think this makes sense, but nevermind, wmove doesn't seem to work
		//wmove(m_parent, m_begin_y, m_off_x + pos);
		move(m_begin_y + m_at_y, m_off_x + m_at_x + pos);
		//wrefresh(win);	
		wrefresh(m_parent);
		refresh();
		int ch = get_ch();
		if(ch == '\r') break;
		if(ch == CTRL('g') || ch == 27 ) { // 27 is ESC key
			m_cancelled = true;
			return;
		}
		if(ch == KEY_LEFT) {
			//input += '<';
			pos = max(pos-1, 0);
		} else if (ch == KEY_END) {
			pos = m_input.size();
		} else if (ch == KEY_HOME) {
			pos = 0;
		} else 	if(ch == KEY_RIGHT) {
			pos = min(pos+1, m_ncols);
			pos = min(pos, (int) m_input.size());
		} else if (ch == KEY_DC) {
			// delete key
			m_input.erase(pos, 1);
			pos = max(pos-1, 0);
		} else if((ch == KEY_BACKSPACE || ch == 127) && pos >0) { 
			pos--; 
			//wdelch(win);  
			m_input.erase(pos, 1);
			continue;
		} else {
			if(pos >= m_ncols) continue;
			m_input.insert(pos, string{static_cast<char>(ch)});
			//input += ch;
			//waddch(win, ch);			
			pos++;
		}
	}

}



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
