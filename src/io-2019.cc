#include <string.h>

//#include <ncursesw/ncurses.h>
#include <cursesw.h>
//#define USE_FORM
#ifdef USE_FORM
#include <form.h>
#endif
#include <panel.h>



#include "basic.h"
#include "blang2.h"
#include "io-2019.h"
#include "neotypes.h"
#include "parser-2019.h"
#include "menu-2025.h"
#include "neotypes.h"
#include "oleofile.h"
#include "sheet.h"
#include "io-curses.h"
#include "win.h"

//import std;
//import utl;
//import win;

using namespace std::string_literals;
using std::cout;
using std::cerr;
using std::map;
using std::string;

static void col_cmd2019();
extern void hl_write_file();
static void maybe_quit_spreadsheet2019();
static void row_cmd2019();
static void save_spreadsheet2019();



using fn_t = std::function<void()> ;
using keymap_t = std::map<int, fn_t>;

static int i19_parameter = -1 ; // useful for repeating functions or general parameter setting

int scr_width()
{
	return getmaxx(stdscr);
}

// FN get_ch .
int get_ch ()
{
	return get_ch(stdscr);
	//int c = getch();
	//log(c);
	//write_status(""); // clear the status line
	//return c;
}

int get_ch (WINDOW *win)
{
	int c = wgetch(win);
	log("get_ch:", c);
	//write_status(""); // clear the status line
	return c;
}

// FN-END




#if 0
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
#endif

#ifdef USE_FORM
class nform_c  /*: public npanel_c */ {
	public:
		nform_c(WINDOW *win, const char* desc, std::string& text);
		const std::string text();

		~nform_c(); 

	public:
		FIELD *m_fields[3]; //= { m_desc, m_edit, nullptr };;
		FORM* m_f;
		WINDOW* m_w = nullptr;

};

nform_c::nform_c(WINDOW *win, const char* desc, std::string& text)
{
	curs_set(1);
	assert(win);
	m_w = win;
	int dlen = strlen(desc);
	m_fields[0] = new_field(1, dlen, 0, 0, 0, 0);
	m_fields[1] = new_field(1, 60 - dlen, 0, dlen, 0, 0);
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
	//set_form_sub(m_f, m_w);
	int parent_x, parent_y;
	getmaxyx(m_w, parent_y, parent_x);
	log("nform_c parent_x ", parent_x,  " parent_y ", parent_y);
	//set_form_sub(m_f, derwin(m_w, 1, 70, 0, 0));
	set_form_sub(m_f, derwin(m_w, parent_y, parent_x, 0, 0));
	post_form(m_f);
	form_driver(m_f, REQ_END_FIELD);
	//curs_set(1);
	//refresh();
	wrefresh(m_w);
	
}

nform_c::~nform_c() 
{
	unpost_form(m_f);
	free_form(m_f);
	free_field(m_fields[0]);
	free_field(m_fields[1]);
	curs_set(0);
}

const std::string nform_c::text()
{
	form_driver(m_f, REQ_NEXT_FIELD); // force buffer sync
	return trim(field_buffer(m_fields[1], 0));
}


#endif

// return true for normal exit, false if user wants to abort action
// text_field is modified by nform_c
static bool invoke_std_form(const char* desc, std::string& text_field)
{
#ifndef  USE_FORM
	win_dow par(1, 75, 1, 0);
	//par.print_at(0, 0, "foogoo");
	wrefresh(par.m_w);
	WINDOW* win;
	win = par.m_w;
	win_edln ed(win, 70, 0, 0 , desc,  text_field);
	ed.run();
	if(ed.m_cancelled) return false;
	text_field = ed.m_input;
	return true;

#else
	//win_dow win(12, 75, 10, 5);
	win_dow win(1, 75, 1, 0);
	nform_c frm(win.m_w, desc, text_field);

	auto fdrive = [&frm](int req) { form_driver(frm.m_f, req); } ;
	int ch;
	while((ch = get_ch(win.m_w)) != CTRL('m')) {
		log("invoke_std_form:ch", ch);
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
		//refresh();
		wrefresh(frm.m_w);
	}

	text_field =  frm.text();
	log("frm.text():", text_field);
#endif
	return true;

}


void edit_cell2019()
{
	std::string formula{ formula_text(curow, cucol)};
	std::string old_formula{formula};
#if 1
	if(!invoke_std_form("=", formula)) return;
	//formula = ed.m_input;
#else
	bool ok = invoke_std_form("=", formula);
	if(!ok) return;
#endif
	if(old_formula == formula) return;
	Global_modified = true;
	set_and_eval(curow, cucol, formula, true);
}



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
		log("complex_key_sequence_27:" , c);
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


static void clear_cell_formula()
{
	set_and_eval(curow, cucol, "", true);
}

static void i19_precision()
{
	int prec = std::max(0, i19_parameter);
	auto cp = find_or_make_cell();
	struct cell_flags_s& flags = cp->cell_flags;

	switch(flags.cell_format) {
	case FMT_DEF:
	case FMT_GEN:
		flags.cell_format = FMT_FXT;
		break;
	case FMT_FXT:
		flags.cell_format = FMT_GEN;
		break;
	}

	//flags.cell_format = FMT_DOL;
	flags.cell_precision = prec;

}

// FN process_key
void process_key(const keymap_t& keymap)
{
	int c = get_ch();
	if ('0' <= c && c <= '9') {
		i19_parameter = c-'0';
	}
	auto search = keymap.find(c);
	if(search == keymap.end()) { beep(); return; }
	auto fn = search->second;
	fn();
	i19_parameter = -1;
}
// FN-END


static map<char, string> custom_bindings; // map between a char and a blang string that must be interpreted

// bind a character to some blang code requiring interpreting
void bind_char(char c, std::string blang_code)
{
	custom_bindings[c] = blang_code;
}

// FN curses_loop .
void curses_loop ()
{

	show_menu();

	int c = get_ch();
	if ('0' <= c && c <= '9') {
		i19_parameter = c - '0';
		return;
	}

	switch (c) {
	case CTRL('q'):		maybe_quit_spreadsheet2019();		break;
	case '=': 			edit_cell2019(); 		break;
	case '%': 			set_cell_toggle_percent();		break;
	case 'c':			col_cmd2019();		break;
	case 'm':			process_menu();		break;
	case 'p':			i19_precision();		break;
	case 'r':			row_cmd2019();		break;
	case KEY_DC:		clear_cell_formula();		break; // delete key
	case KEY_DOWN: 		cursor_down();		break;
	case KEY_LEFT:		cursor_left();		break;
	case 27:			complex_key_sequence_27();		break;
	case 554:			io_shift_cell_cursor(dirn::left, cucol-1); break ; // Ctrl-←  repeat cucol-1 times to bring cursor to col 1
	case 575:			io_shift_cell_cursor(dirn::up, curow-1); break ; // Ctrl-↑  repeat curol-1 times to bring cursor to row 1
	case KEY_RIGHT:		cursor_right();		break;
	case KEY_UP:		cursor_up();		break;
	case KEY_NPAGE:		page_down();		break;
	case KEY_PPAGE:		page_up();		break;
	case CTRL('b'):		set_cell_toggle_bold();		break;
	case CTRL('c'):		copy_this_cell_formula();		break;
	case CTRL('i'):		set_cell_toggle_italic();		break;
	case CTRL('l'):		set_cell_alignment_left();		break;
	case CTRL('r'):		set_cell_alignment_right();		break;
	case CTRL('s'):		save_spreadsheet2019();		break;
	case CTRL('v'):		paste_this_cell_formula();		break;
	}

	if(custom_bindings.contains(c)) {
		log("custom binding found");
		blang::interpret_string(custom_bindings[c]);
	}

	i19_parameter = -1;
}


// FN write_status 
void write_status (const std::string& str)
{
	// 25/4 Persist the error messages
	log("write_status");
	win_print(1, 0, str);
	clrtoeol();

}
// FN-END




static void save_spreadsheet2019(){
	std::string filename = FileGetCurrentFileName();
	//log("Filename before:<", filename, ">");
	if(!invoke_std_form("Save spreadsheet as:", filename)) return;
	FileSetCurrentFileName(filename);
	//log("Filename after:<", FileGetCurrentFileName(), ">");
	oleo_write_file();
}

// return true to go ahead with quit, false otherwise
static void maybe_quit_spreadsheet2019()
{
	Global_definitely_quit = false;
	if(Global_modified == false) { Global_definitely_quit = true ; return; }
	std::string response = ""; 
	if(!invoke_std_form("Spreadsheet modified; kill anyway? (y/[n])? ", response)) return;
	if(response == "y" || response == "yes") Global_definitely_quit = true;

}






void clear_status_line()
{
	write_status("");
}


// user has typed 'c' to perform a col action. This function
// decides which one it is
// 25/5 added
static void col_cmd2019()
{
	//log("col_cmd2019 enter");
	int c = get_ch();
	if(c == 'i') {
		insert_col_left();
	} else if (c == 'a') {
		//log("col_cmd2019 got a");
		c = get_ch();
		if(c == 'l')
			column_align_left();
	}

	i19_parameter = -1;
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
