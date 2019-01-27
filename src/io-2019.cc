#include <cassert>
#include <iostream>

#include <ncurses.h>
#include <form.h>
#include <panel.h>

#include "io-2019.h"
#include "logging.h"
#include "window.h"

using std::cout;

constexpr int CTRL(int c) { return c & 037; }

class nwin_c {
	public:
		nwin_c() { 
			m_w = newwin(1, 30, 0, 0);
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

class nform_c : public nwin_c {
	public:
		nform_c() {
			m_fields[0] = new_field(1, 10, 0, 0, 0, 0);
			m_fields[1] = new_field(1, 10, 0, 11, 0, 0);
			m_fields[2] = nullptr;
			m_f = new_form(m_fields);
			assert(m_f);
			set_field_buffer(m_fields[0], 0, "this is a static field");
			field_opts_off(m_fields[0], O_ACTIVE);
			set_field_buffer(m_fields[1], 0, "edit this text");
			field_opts_on(m_fields[1], O_EDIT);
			set_field_type(m_fields[1], TYPE_ALNUM, 60);
			form_driver(m_f, REQ_NEXT_FIELD);

			set_form_win(m_f, m_w);
			//set_form_sub(m_f, derwin(m_w, 18, 18, 1, 1));
			set_form_sub(m_f, m_w);
			post_form(m_f);
			refresh();
			wrefresh(m_w);
			//wrefresh(m_f);
			//mvprintw(0,0, "Try pressing x");
			//mvwprintw(m_w, 0, 0, "%s", "Press x to exit");
			//refresh();

		}
		~nform_c() {
			unpost_form(m_f);
			free_form(m_f);
			free_field(m_fields[0]);
			free_field(m_fields[1]);
			//delwin(m_f);
			refresh();
			wrefresh(m_w);
		}

	private:
		FIELD *m_fields[3];
		FORM* m_f;

};

void test_form()
{
	//nwin_c win;
	//printw("Press x to exit");
	//npanel_c pan;
	nform_c frm;

	while(getch() != CTRL('m')) ;
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
			case 'w':
				test_form();
				break;
			case CTRL('q'):  
				goto finis;
			case KEY_LEFT:
				io_shift_cell_cursor(3, 1);
				break;
			case KEY_RIGHT:
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
