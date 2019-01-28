#include <cassert>
#include <iostream>
#include <sstream> // for ostringstream

#include <ncurses.h>
#include <form.h>
#include <panel.h>

#include "basic.h"
#include "decompile.h"
#include "io-2019.h"
#include "logging.h"
#include "mem.h"
#include "ref.h"
#include "window.h"

using std::cout;

constexpr int CTRL(int c) { return c & 037; }

// TODO belongs in logging.h
template<typename... Args>
void log(Args ... args) {
	std::ostringstream ss;
	(ss << ... << args);
	log_debug(ss.str());
}

int scr_width() {
	int x,y;
	getmaxyx(stdscr, y, x);
	return x;
}

class nwin_c {
	public:
		nwin_c() { 
			m_w = newwin(1, scr_width(), 0, 0);
			assert(m_w);
			wrefresh(m_w);
		}
		~nwin_c() {
			delwin(m_w);
		}

	//private:
		WINDOW* m_w;

};

class npanel_c : public nwin_c {
	public:
		npanel_c() {
			m_p = new_panel(m_w);
			assert(m_p);

			update_panels();
			doupdate();
		}

		~npanel_c() {
			del_panel(m_p);
		}

	private:
		//nwin_c w;
		PANEL* m_p;

};

class nform_c : public npanel_c {
	public:
		nform_c(char* desc, char * text) {
			m_fields[0] = new_field(1, strlen(desc), 0, 0, 0, 0);
			m_fields[1] = new_field(1, scr_width() - strlen(desc), 0, strlen(desc), 0, 0);
			m_fields[2] = nullptr;
			m_f = new_form(m_fields);
			assert(m_f);
			set_field_buffer(m_fields[0], 0, desc);
			set_field_opts(m_fields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
			set_field_buffer(m_fields[1], 0, text);
			set_field_opts(m_fields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
			set_field_back(m_fields[1], A_UNDERLINE);
			//set_field_type(m_fields[1], TYPE_ALNUM, 60);
			set_current_field(m_f, m_fields[1]);
			//form_driver(m_f, REQ_NEXT_FIELD);

			set_form_win(m_f, m_w);
			//set_form_sub(m_f, derwin(m_w, 18, 18, 1, 1));
			set_form_sub(m_f, m_w);
			post_form(m_f);
			form_driver(m_f, REQ_END_FIELD);
			refresh();
			wrefresh(m_w);
			//wrefresh(m_f);
			//mvprintw(0,0, "Try pressing x");
			//mvwprintw(m_w, 0, 0, "%s", "Press x to exit");
			//refresh();

			int ch;
			while((ch = getch()) != CTRL('m')) {
				switch(ch) {
					case KEY_HOME:
						form_driver(m_f, REQ_BEG_FIELD);
						break;
					case KEY_END:
						form_driver(m_f, REQ_END_FIELD);
						break;
					case KEY_LEFT:
						form_driver(m_f, REQ_LEFT_CHAR);
						break;
					case KEY_RIGHT:
						form_driver(m_f, REQ_NEXT_CHAR);
						break;
					case KEY_DC:
						form_driver(m_f, REQ_DEL_CHAR);
						break;
					case KEY_BACKSPACE:
						form_driver(m_f, REQ_DEL_PREV);
						break;
					default:
						form_driver(m_f, ch);
						break;
				}
				//form_driver(m_f, ch);
				refresh();
				wrefresh(m_w);
			}

		}
		const char* text() {
			form_driver(m_f, REQ_NEXT_FIELD); // force buffer sync
			return field_buffer(m_fields[1], 0);
		}

		~nform_c() {
			unpost_form(m_f);
			free_form(m_f);
			free_field(m_fields[0]);
			free_field(m_fields[1]);
			//delwin(m_f);
			//refresh();
			//wrefresh(m_w);
		}

	private:
		//FIELD* m_desc, m_edit;
		FIELD *m_fields[3]; //= { m_desc, m_edit, nullptr };;
		FORM* m_f;
		//FIELD* m_fields[] = { m_desc, m_edit, nullptr };

};

void edit_cell2019()
{
	//nwin_c win;
	//printw("Press x to exit");
	//npanel_c pan;

	std::string formula = decompile();
	strcpy_c text{formula};
	nform_c frm("=", text.data());

	const char* newformula = frm.text();
	edit_cell(newformula);
	//log("ui2019:formula`", newformula, "'");

	//while(getch() != CTRL('m')) ;
}

void main_command_loop_for2019()
{
	// Tell ncurses to interpret "special keys". It means
	// that KEY_DOWN etc. will work, but ESC won't be
	// read separately
	keypad(stdscr, TRUE);

	while(1) {
		int c = getch();
		switch(c) {
			case '=':
				edit_cell2019();
				break;
			case CTRL('l'):
				set_cell_alignment_left();
				break;
			case CTRL('r'):
				set_cell_alignment_right();
				break;
			case CTRL('q'):  
				goto finis;
			case KEY_LEFT:
			case 'h':
				io_shift_cell_cursor(3, 1);
				break;
			case KEY_RIGHT:
			case 'l':
				io_shift_cell_cursor(2, 1);
				break;
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
