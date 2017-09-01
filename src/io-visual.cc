/*
 * A visual mode for headless
 */

#include <cassert>
#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <utility>
#include <vector>


#include "cell.h"
#include "cmd.h"
#include "io-utils.h"
#include "lists.h"
#include "logging.h"
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
// returns the number of character read
ssize_t read_in(char* buf, int buf_size)
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

      //char buf[100];
      ssize_t n = read(STDIN_FILENO, buf, buf_size);
      //std::array<char, 6> buf;
      //ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
      //cout << "read " << n << endl;
      //c=getchar();

          //------  restore old settings ---------
      res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
      assert(res==0);
      return n;
}

enum meta_keys { K_NORM, // The default case. Just a normal char
	K_UNK, // Unknown key sequence
	K_UP, K_DOWN, K_LEFT, K_RIGHT, K_HOME, K_END};

// read from stdin, interpreting escape sequences
int read_and_cook(meta_keys *special)
{
	char buf[10] = {0}; // whole array is set to 0
	ssize_t n = read_in(buf, sizeof(buf)-1);
	buf[sizeof(buf)-1] = 0; // as a backstop against run-away string, which shouldn't happen anyway
	*special = K_NORM;
	// when n ==1, it could be a normal char (inc. control)
	// or just the normal ESC key.  However, if the buffer
	// returns more than 1 char, then it is an escape
	// seuqnece that needs to be interpreted
	if(n>1) {
		*special = K_UNK; // by default, we don't know what's going on

		static vector<pair<string,meta_keys>> kmap = { 
			{"\E[1~",	K_HOME},
			{"\E[4~",	K_END},
			{"\E[A", 	K_UP},
			{"\E[B", 	K_DOWN},
			{"\E[C", 	K_RIGHT},
			{"\E[D", 	K_LEFT}
		};

		for(auto& km:kmap) {
			if(km.first == buf) {
				*special = km.second;
				break;
			}
		}


	}

	/*
	if(buf[0] == '\E' && buf[1] == '[') {
		if(n == 3) {
			switch(buf[2]) {
				case 'A': *special = K_UP; break;
				case 'B': *special = K_DOWN; break;
				case 'C': *special = K_RIGHT; break;
				case 'D': *special = K_LEFT; break;
				//default:  *special = K_UNK;
			}
		}
	}
	*/

	return buf[0];

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

void
show_cells(int rt, int ct)
{
	//cout << "102 OK Terminated by dot" << endl;
	cout << "r" << curow << "c" << cucol;
	cleareol();
	cout << "\n"; 
	for(int r=1; r<=rt; ++r) {
		cout << on_red("R" + pad_right(std::to_string(r), 3))  << " ";
		for(int c=1; c<= ct ; ++c) {
			CELL *cp = find_cell(r, c);
			string str = print_cell(cp);
			int w = get_width(c);
			str = spaces(w - str.size()) + str;
			if(use_coloured_output && r == curow && c == cucol)
				str = on_red(str); // encase in red, then switch back to black
			cout << str << " ";
			//printf(print_buf);
		}
		cout << "\n";
	}
	//cout << "." <<endl;
}


void show_cells()
{
	show_cells(10, 5);
}



static void
edit_cell_visually(int display_row)
{
	auto gotoc = [&display_row](int c) { gotorc(display_row, c); };
	gotoc(1);
	std::string formula =  get_cell_formula_at(curow, cucol);
	cout << formula;
	int col = formula.size() +1;
	gotoc(col);
	cout << flush;
	if(false) log_debug("edit_cell_visually(): display_row = " + to_string(display_row) 
			+ ", curow=" + to_string(curow) + ", cucol=" + to_string(cucol) + ", formula:" 
			+ formula);

	bool more = true;
	while(more) {
		meta_keys special;
		int ch = read_and_cook(&special);
		//cout << c << "\n";
		switch(special) {
			case K_NORM:
				if(ch == 10) more = false; // return key
				break; 
			case K_HOME:
				col = 1;
				break;
			case K_LEFT:
				col = std::max(1, col-1);
				break;
			case K_RIGHT:
				col = col+1;
				// fallthrough
			case K_END:
				col = std::min((int)formula.size()+1, 79); // TODO repleace hard-code
				break;
		}
		gotoc(col);
		cout << flush;
	}

	cout << " edit_cell_visually() TODO";
}
void
visual_mode()
{
	colours();
	cout << "\E[2J"; // clear screen

	// terminal size code from
	// https://stackoverflow.com/questions/1022957/getting-terminal-width-in-c
	struct winsize w; // get size of terminal
	ioctl(0, TIOCGWINSZ, &w);

	std::string inp;
	while(true){
		cout << "\E[H"; //home
		show_cells(w.ws_row-2, w.ws_col/10);
		meta_keys special;
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


