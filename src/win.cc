/*
 * Copyright (c) 1992, 1993, 1999, 2001 Free Software Foundation, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

//module;

//#include <ncurses.h>

#include "win.h"

#include <algorithm>
#include <cassert>
#include <ncurses.h>

#include "neotypes.h"
//#include <string>

//export module win;

//import std;

using std::max;
using std::min;
using std::string;

int get_ch (WINDOW *);


// FN win_print .
// these should be nice generic functions
void win_print(WINDOW *w, const char* str) // FN
{
	waddstr(w, str);
}

void win_print(const char* str) // FN
{
	addstr(str);
}

void win_print(WINDOW *w, const std::string& str) // FN
{
	win_print(w, str.c_str());
}

void win_print(const std::string& str) // FN
{
	win_print(str.c_str());
}

void win_print(WINDOW* w, int y, int x, const std::string& str) // FN
{
	//wmove(w, y, x);
	mvwaddstr(w, y, x, str.c_str());
	//win_print(w, str);

}
void win_print(int y, int x, const std::string& str) // FN
{
	win_print(stdscr, y, x, str);
}
// FN-END

// FN win_getyx .
// win defaults to stdscr
std::tuple<int, int> win_getyx(WINDOW *win)
{
	int y, x;
	getyx(win, y, x);
	//log("win_getyx ", y)
	return std::make_tuple(y, x);

}
// FN-END


// FN win_set_line .
void win_set_line(WINDOW *w, const std::string& str)
{
	wmove(w, 0, 0);
	win_print(w, str);
	wclrtoeol(w);
	
}
// FN-END

// 

win_dow::win_dow(int nlines, int ncols, int begin_y, int begin_x) :
	nlines(nlines), ncols(ncols), begin_y(begin_y), begin_x(begin_x) {
	m_w = newwin(nlines, ncols, begin_y, begin_x);
	keypad(m_w, TRUE);
	set_escdelay(10); // lowering the escape delay will enable us to detect a
					  // pure escape (as opposed to arrows)
	assert(m_w);
	wrefresh(m_w);
}

win_dow::~win_dow() { delwin(m_w); }

void win_dow::print_at(int y, int x, const std::string& str)
{
	mvwaddstr(m_w, y, x, str.c_str());
}



// FN win_edln .
win_edln::win_edln(WINDOW *parent, int ncols, int begin_y, int begin_x, const string& desc, const string& input)
{
	//log("win_edln:1");
	curs_set(2); // 0: invis, 1:normal, 2:very vis
	
	//defer1 d4{curs_set, 0};
	m_parent = parent;
	win_print(parent, begin_y, begin_x, desc);
	win_print(parent, input);
	wrefresh(parent);
	//refresh();
	m_begin_y = begin_y;
	m_off_x = begin_x + desc.size();
	m_input = input;
	m_ncols = ncols;
	//get_ch(parent);
	m_desc_len = desc.size();
	m_begin_x = begin_x;
	//log("win_edln:5");
	getbegyx(parent, m_at_y, m_at_x); // where the window starts
	//win_print(parent, m_at_y, m_at_x, "bar");
	//win_print(parent, 0, 0, "ssplob");
	//wrefresh(parent);
	keypad(parent, TRUE); // allow arrow detection
	set_escdelay(10); // lowering the escape delay will enable us to detect a pure escape (as opposed to arrows)
}
// FN-END

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
		string padded{m_input};
		//padded.append(m_ncols- m_input.size(), ' ');
	log("padded:", padded);
		win_print(m_parent, m_begin_y, m_begin_x + m_desc_len, padded);
		//win_set_line(win, input);
		//wmove(win, 1, 3); // set cursor
		//move(3, 16+pos); // place cursor. I don't think this makes sense, but nevermind, wmove doesn't seem to work
		wmove(m_parent, m_begin_y, m_begin_x + m_desc_len + pos);
		//move(m_begin_y + m_at_y, m_off_x + m_at_x + pos);
		//wrefresh(win);	
		wrefresh(m_parent);
		//refresh();
		int ch = get_ch(m_parent);
		if(ch == '\r') break;
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
		} else if(ch == CTRL('g') || ch == 27) { // 27 = ESC
			// positioning here is important due to pesky escaping conditions
			m_cancelled = true;
			return;
		} else {
			//m_input += ch; continue;
			log("m_ncols:", m_ncols);
			if(pos >= m_ncols) continue;
			m_input.insert(pos, string{static_cast<char>(ch)});
			//input += ch;
			//waddch(win, ch);			
			pos++;
		}
	}

}

