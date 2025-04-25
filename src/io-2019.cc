#include <cassert>
#include <functional>
#include <iostream>
#include <string>
#include <sstream> // for ostringstream
#include <unistd.h> // for sleep
#include <map>

#include <ncurses.h>
//#include <curses.h>
#include <form.h>
#include <panel.h>

using namespace std::string_literals;

#include "basic.h"
#include "io-2019.h"
#include "parser-2019.h"
#include "io-utils.h"
#include "logging.h"
//#include "mem.h"
#include "menu-2025.h"
#include "ref.h"
#include "regions.h"
#include "sheet.h"
//#include "tbl.h"
#include "window.h"
#include "io-curses.h"

import tbl;
using std::cout;
using std::cerr;

extern void hl_write_file();
extern void io_shift_cell_cursor (dirn way, int repeat);

static constexpr int CTRL(int c) { return c & 037; }

using fn_t = std::function<void()> ;
using keymap_t = std::map<int, fn_t>;



int scr_width() {
	/*
	int x,y;
	getmaxyx(stdscr, y, x);
	return x;
	*/
	return getmaxx(stdscr);
}

// FN get_ch 
int get_ch ()
{
	int c = getch();
	write_status(""); // clear the status line
	return c;
}
// FN-END

class nwin_c {
	public:
		nwin_c() { 
			

			m_w = newwin(1, scr_width(), 1, 0);
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
		nform_c(const char* desc, std::string& text) {
			m_fields[0] = new_field(1, strlen(desc), 0, 0, 0, 0);
			m_fields[1] = new_field(1, scr_width() - strlen(desc), 0, strlen(desc), 0, 0);
			m_fields[2] = nullptr;
			m_f = new_form(m_fields);
			assert(m_f);
			set_field_buffer(m_fields[0], 0, desc);
			set_field_opts(m_fields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
			set_field_buffer(m_fields[1], 0, text.c_str());
			set_field_opts(m_fields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
			set_field_back(m_fields[1], A_UNDERLINE);
			//set_field_type(m_fields[1], TYPE_ALNUM, 60);
			set_current_field(m_f, m_fields[1]);

			set_form_win(m_f, m_w);
			set_form_sub(m_f, m_w);
			post_form(m_f);
			form_driver(m_f, REQ_END_FIELD);
			refresh();
			wrefresh(m_w);


		}
		const std::string text() {
			form_driver(m_f, REQ_NEXT_FIELD); // force buffer sync
			return trim(field_buffer(m_fields[1], 0));
		}

		~nform_c() {
			unpost_form(m_f);
			free_form(m_f);
			free_field(m_fields[0]);
			free_field(m_fields[1]);
		}

	public:
		FIELD *m_fields[3]; //= { m_desc, m_edit, nullptr };;
		FORM* m_f;

};

// retun true for normal exit, false if user wants to abort action
// text_field is modified by nform_c
static bool invoke_std_form(const char* desc, std::string& text_field)
{
	nform_c frm(desc, text_field);

	auto fdrive = [&frm](int req) { form_driver(frm.m_f, req); } ;
	int ch;
	while((ch = getch()) != CTRL('m')) {
		switch(ch) {
			case KEY_HOME:
				fdrive(REQ_BEG_FIELD);
				break;
			case KEY_END:
				fdrive(REQ_END_FIELD);
				break;
			case KEY_LEFT:
				fdrive(REQ_LEFT_CHAR);
				break;
			case KEY_RIGHT: {
						/*
						   int y, x;
						   getyx(frm.m_w, y, x);
						   int len = strlen(field_buffer(frm.m_fields[1], 0));
						//log_debug("std frm x:"s + std::to_string(x) + "," + std::to_string(frm.text().size()));
						log_debug("std frm x:"s + std::to_string(x) + "," + std::to_string(len));
						*/
						fdrive(REQ_NEXT_CHAR);
					}
					break;
			case KEY_DC:
					fdrive(REQ_DEL_CHAR);
					break;
			case KEY_BACKSPACE:
			case 127:
			case '\b':
					// backspace goofiness explained here:
					// https://stackoverflow.com/questions/27200597/c-ncurses-key-backspace-not-working#27203263
					fdrive(REQ_DEL_PREV);
					break;
			case CTRL('g'):
					return false;
			default:
					fdrive(ch);
					break;
		}
		refresh();
		wrefresh(frm.m_w);
	}

	text_field =  frm.text();
	return true;
}

void edit_cell2019()
{
	std::string formula{ formula_text(curow, cucol)};
	std::string old_formula{formula};
	bool ok = invoke_std_form("=", formula);
	if(!ok) return;
	if(old_formula == formula) return;
	Global->modified = true;
	set_and_eval(curow, cucol, formula, true);
	cur_io_repaint();
}

static void maybe_quit_spreadsheet2019(bool& quit);
static void row_cmd2019();
static void save_spreadsheet2019();
static void save_csv2019();

//static void cursor_col_0()  { io_shift_cell_cursor(3, 2 + 0*(cucol-1)); } // repeat cucol times to bring cursor to column 0
static void cursor_left()  { io_shift_cell_cursor(dirn::left,1); }
static void cursor_right() { io_shift_cell_cursor(dirn::right, 1); }
static void cursor_down()  { io_shift_cell_cursor(dirn::down, 1); }
static void cursor_up()    { io_shift_cell_cursor(dirn::up, 1); }

// needed for handling Ctl leftarrow and uparrow
static void complex_key_sequence_27()
{
	// key 27 has already been obtained by process_key(). We need to determine the rest

	int c;
	auto get = [&](){ 
		c = get_ch(); 
		//log(c);
		return c;
	};
	if(get() != 91) goto fail;
	if(get() != 49) goto fail;
	if(get() != 59) goto fail;
	if(get() != 53) goto fail;

	
	switch(get()) {
		case 68: // Ctr leftarrow
			io_shift_cell_cursor(dirn::left, cucol-1); // repeat cucol-1 times to bring cursor to column 1
			break;
		case 65: // Ctrl uparrow
			io_shift_cell_cursor(dirn::up, curow-1); // repeat curol-1 times to bring cursor to row 1
			break;
		default:
			goto fail;		
	}
	return;

fail:
	return;
	//failed, so just put back the last read failed char
	//ungetch(c); // 25/4 I don't we really need this
}


// TODO this may be more useful that you think
static void clear_cell_formula()
{
	//edit_cell_str(""); // this doesn't work properly
	set_and_eval(curow, cucol, "", true);
	cur_io_repaint();
}

void process_key(const keymap_t& keymap)
{
	int c = get_ch();
	//clear_status_line();
	//log("process_key:", c);
	auto search = keymap.find(c);
	if(search == keymap.end()) { beep(); return; }
	auto fn = search->second;
	fn();
}




bool curses_loop () // FN
{

	//log("win_over:" ,  cwin->win_over);
	bool quit = false;
	auto quitter = [&quit]() { maybe_quit_spreadsheet2019(quit); }; 
	show_menu();
	static auto keymap = keymap_t {
		{CTRL('q'), 	quitter}, // this may (or may not) set quit to true
			{'=', 		edit_cell2019},
			{'%',		set_cell_toggle_percent},			
			{'m',		process_menu},
			{'r',		row_cmd2019},
			{KEY_DC, 	clear_cell_formula}, // delete key
			{KEY_DOWN,	cursor_down},
			{KEY_LEFT,  	cursor_left},
			{27,  		complex_key_sequence_27},
			{KEY_RIGHT, 	cursor_right},
			{KEY_UP, 	cursor_up},			
			{CTRL('b'),	set_cell_toggle_bold},
			{CTRL('c'), 	copy_this_cell_formula},
			{CTRL('i'),	set_cell_toggle_italic},
			{CTRL('l'), 	set_cell_alignment_left},
			{CTRL('r'), 	set_cell_alignment_right},
			{CTRL('s'), 	save_spreadsheet2019},
			{CTRL('t'), 	save_csv2019},
			{CTRL('v'), 	paste_this_cell_formula},

	};

	process_key(keymap);
	cur_io_repaint();
	return quit;


}


// FN write_status 
void write_status (const std::string& str)
{
	// 25/4 Persist the error messages
	win_print(1, 0, str);
	clrtoeol();

}
// FN-END



static void save_csv2019(){
	std::string filename = FileGetCurrentFileName();
	size_t lastindex = filename.find_last_of(".");
	if(lastindex != std::string::npos)
		filename = filename.substr(0, lastindex);
	filename += ".csv";

	//log("Filename before:<", filename, ">");
	if(!invoke_std_form("Save spreadsheet as CSV:", filename)) return;
	//FileSetCurrentFileName(filename);
	//log("Filename after:<", FileGetCurrentFileName(), ">");
	save_csv(filename, ',');
}

static void save_spreadsheet2019(){
	std::string filename = FileGetCurrentFileName();
	//log("Filename before:<", filename, ">");
	if(!invoke_std_form("Save spreadsheet as:", filename)) return;
	FileSetCurrentFileName(filename);
	//log("Filename after:<", FileGetCurrentFileName(), ">");
	hl_write_file();
}

// return true to go ahead with quit, false otherwise
static void maybe_quit_spreadsheet2019(bool& quit)
{
	quit = false;
	if(Global->modified == false) { quit = true ; return; }
	std::string response = ""; 
	if(!invoke_std_form("Spreadsheet modified; kill anyway? (y/[n])? ", response)) return;
	if(response == "y" || response == "yes") quit = true;

}






void clear_status_line()
{
	write_status("");
}

static void delete_1row() { delete_row(1); }



static void paste_1row () 
{ 
	std::string response;
	if(!invoke_std_form("Row to copy from?", response)) return;
	try {
		size_t idx;
		CELLREF src_row = std::stol(response, &idx);
		copy_row(curow, src_row);
	} catch(...) {
		beep();
	}
}

// user has typed 'r' to perform a row action. This function
// decides which one it is
static void row_cmd2019(){
	static auto keymap = std::map<int, fn_t> {		
		{'d', delete_1row},
			{'i', insert_1row},
			{'p', paste_1row}
	};

	process_key(keymap);
}
