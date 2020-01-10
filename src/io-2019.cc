#include <cassert>
#include <functional>
#include <iostream>
#include <sstream> // for ostringstream
#include <unistd.h> // for sleep
#include <map>

#include <ncurses.h>
#include <form.h>
#include <panel.h>

using namespace std::string_literals;

#include "basic.h"
#include "cmd.h"
#include "convert.h"
#include "io-2019.h"
#include "io-headless.h"
#include "parser-2019.h"
#include "io-utils.h"
#include "logging.h"
//#include "mem.h"
#include "ref.h"
#include "regions.h"
#include "sheet.h"
#include "window.h"

using std::cout;

constexpr int CTRL(int c) { return c & 037; }

using fn_t = std::function<void()> ;
using keymap_t = std::map<int, fn_t>;


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
		nform_c(char* desc, std::string& text) {
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
static bool invoke_std_form(char* desc, std::string& text_field)
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
	bool ok = invoke_std_form("=", formula);
	if(!ok) return;

	if(use_parser_2019) {
		// cobbled together from parser-2019.cc
		CELL* cp = find_or_make_cell(curow, cucol);
		set_and_eval(curow, cucol, formula, true);
	} else
		edit_cell_str(formula);
	recalculate(1);
}

static void maybe_quit_spreadsheet2019(bool& quit);
static void row_cmd2019();
static void save_spreadsheet2019();

static void cursor_left()  { io_shift_cell_cursor(3, 1); }
static void cursor_right() { io_shift_cell_cursor(2, 1); }
static void cursor_down()  { io_shift_cell_cursor(1, 1); }
static void cursor_up()    { io_shift_cell_cursor(0, 1); }

// TODO this may be more useful that you think
static void clear_cell_formula()
{
	edit_cell_str("");
	recalculate(1);
}

void process_key(const keymap_t& keymap)
{
	int c = getch();
	auto search = keymap.find(c);
	if(search == keymap.end()) { beep(); return; }
	auto fn = search->second;
	fn();
}

void main_command_loop_for2019()
{
	// Tell ncurses to interpret "special keys". It means
	// that KEY_DOWN etc. will work, but ESC won't be
	// read separately
	keypad(stdscr, TRUE);

	bool quit = false;
	auto quitter = [&quit]() { maybe_quit_spreadsheet2019(quit); }; 
	static auto keymap = keymap_t {
		{CTRL('q'), 	quitter}, // this may (or may not) set quit to true
			{'=', 		edit_cell2019},
			{'r',		row_cmd2019},
			{KEY_DC, 	clear_cell_formula}, // delete key
			{KEY_DOWN,	cursor_down},
			{KEY_LEFT,  	cursor_left},
			{KEY_RIGHT, 	cursor_right},
			{KEY_UP, 	cursor_up},
			{CTRL('c'), 	copy_this_cell_formula},
			{CTRL('l'), 	set_cell_alignment_left},
			{CTRL('r'), 	set_cell_alignment_right},
			{CTRL('s'), 	save_spreadsheet2019},
			{CTRL('v'), 	paste_this_cell_formula},

	};


	while(!quit) process_key(keymap);

	endwin();
	exit(0);

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
	if(!invoke_std_form("Spreadsheet modifield; kill anyway? (y/[n])? ", response)) return;
	if(response == "y" || response == "yes") quit = true;

}

void io_error_msg2019_str(const std::string& str)
{
	//log("TODO:io_error_msg2019_str:", str);
	npanel_c pan;
	wprintw(pan.m_w, "%s", str.c_str());
	update_panels();
	doupdate();
	//sleep(1);
	usleep(0.5 * 1'000'000); // half a second

}


static void delete_1row() { delete_row(1); }



static void paste_1row () 
{ 
	std::string response;
	if(!invoke_std_form("Row to copy from?", response)) return;
	bool ok;
	CELLREF src_row = to_long(response, ok);
	if(!ok) { beep(); return;}
	copy_row(curow, src_row);
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
