/*
 * A visual mode for headless
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <term.h>
#include <termios.h>
#include <unistd.h>
#include <utility>
#include <vector>


#include "cell.h"
#include "cmd.h"
#include "io-utils.h"
#include "lists.h"
#include "logging.h"
#include "ref.h"
#include "utils.h"

using std::cout;
using std::endl;
using std::flush;
using std::pair;
using std::string;
using std::to_string; 
using std::vector;

bool use_coloured_output = false;

void
colours()
{
	use_coloured_output = true;
}

// use a red background
std::string
on_red(const std::string& str)
{
	if(use_coloured_output)
		return "\E[41m" + str + "\E[40m";
	else return str;
}


// http://www.unix.com/programming/20438-unbuffered-streams.html
// with some modification to allow for escape sequences
std::string
read_in()
{
	//int c=0;

	struct termios org_opts, new_opts;
	int res=0;
	//-----  store old settings -----------
	res=tcgetattr(STDIN_FILENO, &org_opts);
	assert(res==0);
	//---- set new terminal parms --------
	memcpy(&new_opts, &org_opts, sizeof(new_opts));
	new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);

	char buf[10] = {0}; // whole array is set to 0
	//ssize_t n = read_in(buf, sizeof(buf)-1);
	ssize_t n = read(STDIN_FILENO, buf, sizeof(buf)-1);
	buf[sizeof(buf)-1] = 0; // as a backstop against run-away string, which shouldn't happen anyway
	std::string result = buf;

	//------  restore old settings ---------
	res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
	assert(res==0);
	return result;
}

enum meta_key { K_NORM, // The default case. Just a normal char
	K_UNK, // Unknown key sequence
	K_UP, K_DOWN, K_LEFT, K_RIGHT, K_HOME, K_END, K_DEL};

typedef struct keymap_s {
	meta_key mkey;
	std::string code;
	std::string kseq;
	std::string desc;
} keymap_s;

auto keymap = std::vector<keymap_s> {
	{K_UP, "ku", "?", "K_UP"},
		{K_DOWN, 	"kd", "?", "K_DOWN"},
		{K_LEFT,	"kl", "?", "K_LEFT"},
		{K_RIGHT,	"kr", "?", "K_RIGHT"},
		{K_HOME,	"kh", "?", "K_HOME"},
		{K_END,		"@7", "?", "K_END"},
		{K_DEL,		"kD", "?", "K_DEL"}
};

// read from stdin, interpreting escape sequences
int read_and_cook(meta_key *special)
{

	//char buf[10] = {0}; // whole array is set to 0
	//ssize_t n = read_in(buf, sizeof(buf)-1);
	//buf[sizeof(buf)-1] = 0; // as a backstop against run-away string, which shouldn't happen anyway
	std::string input = read_in();

	*special = K_NORM;
	// when n ==1, it could be a normal char (inc. control)
	// or just the normal ESC key.  However, if the buffer
	// returns more than 1 char, then it is an escape
	// seuqnece that needs to be interpreted
	if(input.size()>1) {
		*special = K_UNK; // by default, we don't know what's going on
		for(const auto& km:keymap) {
			if(km.kseq == input) {
				*special = km.mkey;
				break;
			}
		}


	}


	return input[0];

}


static void
gotorc(int r, int c)
{
	cout << "\E[" << r << ";" << c << "H"; // move to row/col
}

static void 
cleareol()
{
	cout << "\E[K";
}



// calculate row headings size required ("R1",R2", ...)
// where hr is the bottom-most row you want to calculated
int row_hdr_size(int hr)
{
	return std::ceil(std::log10(hr)) +2;
}

void
compute_display_range(const point_t&  grid_size, range_t& rng)
{

	// TODO this function is too crude: it assumes that each
	// row occupies 1 line, and each cell occupies 10 cells
	
	// if the current cell is outside the display range, then recentre it
	// into the display
	
	if(curow<rng.lr || curow > rng.hr) {
		//int nrows = w.ws_row -3;
		int nrows = grid_size.r;
		rng.lr = std::max(1, curow - nrows/2);
		rng.hr = rng.lr + nrows -1;
	}
		

	if(cucol< rng.lc || cucol > rng.hc) {
		rng.lc = rng.hc = cucol;
		int grid_cols = row_hdr_size(rng.hr) + get_width(cucol);
		int hamming = 1;
		while(true) {

			if(cucol+hamming <= MAX_ROW) {
				if(grid_cols + get_width(cucol+hamming) > grid_size.c) break; // column fully utilised
				rng.hc = cucol+hamming;
				grid_cols += get_width(rng.hc);
			}

			if(cucol-hamming>0) {
				if(grid_cols + get_width(cucol - hamming) > grid_size.c) break;
				rng.lc = cucol-hamming;
				grid_cols += get_width(rng.lc);
			}

			hamming++;
		}

		//int ncols = grid_size.c/10;
	       	//rng.lc = std::max(1, cucol - ncols/2);
		//rng.hc = rng.lc + ncols;
	}

}

void
show_cells(const range_t& rng)
{
	auto nl = []() { cout << "\n"; };

	// display current cell information
	cout << "r" << curow << "c" << cucol << " " << cell_value_string(curow, cucol, 0) << " [" << get_cell_formula_at(curow, cucol) << "]";
	cleareol();
	nl(); 

	int margin = row_hdr_size(rng.hr);

	// print column headings
	cout << pad_right(" ", margin);
	for(int c=rng.lc; c<= rng.hc ; ++c) {
		int w = get_width(w);
		string hdr = pad_right("C" + to_string(c), w);
		cout << on_red(hdr);
	}
	cleareol();
	nl();

	for(int r=rng.lr; r<=rng.hr; ++r) {
		cout << on_red(pad_right("R" + std::to_string(r), margin));
		for(int c = rng.lc; c<= rng.hc ; ++c) {
			CELL *cp = find_cell(r, c);
			string str = print_cell(cp);
			int w = get_width(c);
			str = pad_left(str, w);
			str = str.substr(0, w); // truncate overlong cells
			if(use_coloured_output && r == curow && c == cucol)
				str = on_red(str); // encase in red, then switch back to black
			cout << str;
			//printf(print_buf);
		}
		nl();
	}
	cout <<flush;
	//cout << "." <<endl;
}


void show_cells()
{
	show_cells(range_t{1,2, 10, 5});
}



static void
edit_cell_visually(int display_row)
{
	std::string formula =  get_cell_formula_at(curow, cucol);
	int col = formula.size() +1;
	auto gotoc = [&display_row](int c) { gotorc(display_row, c); };
	if(false) log_debug("edit_cell_visually(): display_row = " + to_string(display_row) 
			+ ", curow=" + to_string(curow) + ", cucol=" + to_string(cucol) + ", formula:" 
			+ formula);

	bool more = true;
	while(more) {
		// display formula
		gotoc(1);
		cout << formula;
		cleareol();
		gotoc(col);
		cout << flush;

		auto erase_at = [&formula] (int col) { formula.erase(formula.begin() + col -1, formula.begin() + col);};

		meta_key special;
		int ch = read_and_cook(&special);
		int len = (int)formula.size();
		//cout << c << "\n";
		switch(special) {
			case K_NORM:
				switch(ch) {
					case 10:  // return key
						more = false;
						break;
					case 127: // delete backsapce
						if(col>1) erase_at(--col);
						break;
					default:
						formula.insert(formula.begin() +col-1, ch);
						col++;
				}
				break; 
			case K_HOME:
				col = 1;
				break;
			case K_LEFT:
				col = std::max(1, col-1);
				break;
			case K_RIGHT:
				col = col+1;
				col = std::min(col, len+1);
				col = std::min(col , 79); // TODO repleace hard-code
				break;
			case K_END:
				col = std::min(len+1, 79); // TODO repleace hard-code
				break;
			case K_DEL:
				if(col<=len)
					erase_at(col);
				break;

		}
		gotoc(col);
		set_cell_from_string(curow, cucol, formula);
		cout << flush;
	}

	gotoc(1);
	cleareol();
	cout << flush;
}

std::string
get_term_sequence(const std::string& seq)
{
	char buf2[30];
	char tbuf[512];
	char* tbufptr = tbuf;
	//char *cstr;
	//if(cstr = tgetstr("ho", &tbufptr))
	
	char* cstr = tgetstr((char*) seq.c_str(), &tbufptr);
	//	cout << "Found ho" << endl;

	if(cstr==nullptr) cout << "get_term_sequence(): No can do" << endl;
	std::string binding = cstr;

	// kludge for presumably wrong terminfo entries
	if(binding.size()>1 && binding[1] == 'O') binding[1] = '[';

	/*
	cout << "Looking for " << seq;
	for(int i=0; i< binding.size(); ++i) cout << " " << (int)binding[i];
	cout << endl;
	*/
	return binding;
}

void 
init_keymap()
{

	// http://c-faq.com/osdep/sd22.html
	// https://nethackwiki.com/wiki/Source:Hack_1.0/hack.termcap.c
	// https://www.gnu.org/software/termutils/manual/termcap-1.3/html_mono/termcap.html#SEC26
	// https://linux.die.net/man/5/termcap
	// http://invisible-island.net/ncurses/man/terminfo.5.html

	char *tmp = getenv("TERM");
	if(!tmp) cout << "Can't get TERM" << endl;
	char *tptr = (char*) alloca(1024);
	if(tgetent(tptr, tmp) <1) cout << "Uknown terminal type " << endl;

	for(auto& km: keymap){
		km.kseq = get_term_sequence(km.code);
	}

}

void 
keyboard_test()
{
	init_keymap();

	cout << "keymap_test(): keys are as follows\n";

	auto describe = [](keymap_s k) -> void {
		cout << k.code << " " << pad_right(k.desc, 8);
		for(char c:k.kseq) {
			if(c=='\E') 
				cout << "\\E";
			else
				cout << c;
		}
	};

	for(const auto& k: keymap) {
		describe(k);
		cout << "\n";
	}

	cout << "Type q to quit\n";

	while(true) {
		meta_key special;
		char ch = read_and_cook(&special);
		cout << "Found: ";
		if(special == K_NORM)
			cout << (char)ch; // TODO handle \E and ^?
		else {
			for(const auto& k:keymap)
				if(special==k.mkey)
					describe(k);
		}
		cout << "\n";
		if(ch == 'q') break;
	}
	
	cout << "keyboard_test() finiahed.\n";
}

void
visual_mode()
{

	init_keymap();
	//exit(0);

	colours();
	cout << "\E[2J"; // clear screen

	// terminal size code from
	// https://stackoverflow.com/questions/1022957/getting-terminal-width-in-c
	struct winsize w; // get size of terminal
	ioctl(0, TIOCGWINSZ, &w);
	point_t grid{w.ws_row-3, w.ws_col};

	range_t grid_range{1,1,0,0} ;
	compute_display_range(grid, grid_range);

	//range_t drng = init_display_range(disp_win_size);
	std::string inp;
	while(true){
		cout << "\E[H"; //k

		compute_display_range(grid, grid_range);
		show_cells(grid_range);

		meta_key special;
		int c = read_and_cook(&special);
		//int c = unbuffered_getch();
		//cout << "Input = " << c << endl;
		if(special == K_NORM) {
			if(c == '=') edit_cell_visually(w.ws_row);
			if(c == 'q' || c == '\E') break;
		} else if(special != K_UNK) {
			if(c == 'h' || special == K_LEFT) cucol = std::max(1, cucol-1);
			if(c == 'j' || special == K_DOWN) curow++;
			if(c == 'k' || special == K_UP) curow = std::max(1, curow-1);
			if(c == 'l' || special == K_RIGHT) cucol++;
		}
	}
	cout << "Exited visual mode\n";

}


