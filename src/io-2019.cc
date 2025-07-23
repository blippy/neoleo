#include <string.h>

#include <ncurses.h>
#include <form.h>
#include <panel.h>



#include "basic.h"
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

static void col_cmd2019();
extern void hl_write_file();
static void maybe_quit_spreadsheet2019(bool& quit);
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

class nwin_c {
	public:
		nwin_c(int nlines, int ncols, int begin_y, int begin_x);
		~nwin_c();
		void print_at(int y, int x, const std::string& str);
	
		WINDOW* m_w = 0;
		int nlines, ncols, begin_y, begin_x;
};

nwin_c::nwin_c(int nlines, int ncols, int begin_y, int begin_x) :
	nlines(nlines), ncols(ncols), begin_y(begin_y), begin_x(begin_x) {
	m_w = newwin(nlines, ncols, begin_y, begin_x);
	keypad(m_w, TRUE);
	set_escdelay(10); // lowering the escape delay will enable us to detect a
					  // pure escape (as opposed to arrows)
	assert(m_w);
	wrefresh(m_w);
}

nwin_c::~nwin_c() { delwin(m_w); }

void nwin_c::print_at(int y, int x, const std::string& str)
{
	mvwaddstr(m_w, y, x, str.c_str());
}

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

#if 1
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
	set_form_sub(m_f, derwin(m_w, 1, 70, 1, 1));
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

// retun true for normal exit, false if user wants to abort action
// text_field is modified by nform_c
static bool invoke_std_form(const char* desc, std::string& text_field)
{
#if 1
	nwin_c win(12, 75, 10, 5);
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
	nwin_c par(1, 75, 2, 0);
	par.print_at(0, 0, "foogoo");
	wrefresh(par.m_w);
	//get_ch(par.m_w);
	//return;

	//WINDOW * win = par.m_w;
	//win = stdscr;
log("edit_cell2019:1");
	WINDOW* win;
	//= newwin(7, 75, 1, 1);
	win = par.m_w;
log("edit_cell2019:2");
	//defer1(delwin, win);
log("edit_cell2019:3");
	win_edln ed(win, 70, 0, 0 , "=",  formula);
log("edit_cell2019:4");
	ed.run();
	if(ed.m_cancelled) return;
	formula = ed.m_input;
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




bool curses_loop () // FN
{

	bool quit = false;
	auto quitter = [&quit]() { maybe_quit_spreadsheet2019(quit); }; 
	show_menu();
	static auto keymap = keymap_t {
		{CTRL('q'), 	quitter}, // this may (or may not) set quit to true
			{'=', 		edit_cell2019},
			{'%',		set_cell_toggle_percent},
			{'c',		col_cmd2019},
			{'m',		process_menu},
			{'p',		i19_precision},
			{'r',		row_cmd2019},
			{KEY_DC, 	clear_cell_formula}, // delete key
			{KEY_DOWN,	cursor_down},
			{KEY_LEFT,  cursor_left},
			{27,  		complex_key_sequence_27},
			{KEY_RIGHT, cursor_right},
			{KEY_UP, 	cursor_up},
			{KEY_NPAGE,	page_down},
			{KEY_PPAGE,	page_up},
			{CTRL('b'),	set_cell_toggle_bold},
			{CTRL('c'), copy_this_cell_formula},
			{CTRL('i'),	set_cell_toggle_italic},
			{CTRL('l'), set_cell_alignment_left},
			{CTRL('r'), set_cell_alignment_right},
			{CTRL('s'), save_spreadsheet2019},
			{CTRL('v'), paste_this_cell_formula},

	};

	process_key(keymap);
	//cur_io_repaint();
	return quit;


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
	hl_write_file();
}

// return true to go ahead with quit, false otherwise
static void maybe_quit_spreadsheet2019(bool& quit)
{
	quit = false;
	if(Global_modified == false) { quit = true ; return; }
	std::string response = ""; 
	if(!invoke_std_form("Spreadsheet modified; kill anyway? (y/[n])? ", response)) return;
	if(response == "y" || response == "yes") quit = true;

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
	auto insert_left = []() { insert_col_left(); };
	static auto keymap = std::map<int, fn_t> {
		//{'d', delete_1row},
			{'i', insert_left}
			//{'p', paste_1row}
	};

	process_key(keymap);
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
