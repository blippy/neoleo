/*
 * A visual mode for headless
 */

#include <cassert>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>


#include "cell.h"
#include "cmd.h"
#include "io-utils.h"
#include "lists.h"
#include "utils.h"

using std::cout;
using std::endl;
using std::string;

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
	K_DOWN, K_LEFT, K_RIGHT, K_UP };

// read from stdin, interpreting escape sequences
int read_and_cook(meta_keys *special)
{
	char buf[10];
	ssize_t n = read_in(buf, sizeof(buf));
	*special = K_NORM;
	// when n ==1, it could be a normal char (inc. control)
	// or just the normal ESC key.  However, if the buffer
	// returns more than 1 char, then it is an escape
	// seuqnece that needs to be interpreted
	if(n == 3 && buf[0] == '\E'  && buf[1] == '[') {
		switch(buf[2]) {
			case 'A': *special = K_UP; break;
			case 'B': *special = K_DOWN; break;
			case 'C': *special = K_RIGHT; break;
			case 'D': *special = K_LEFT; break;
			default:  *special = K_UNK;
		}
	}
	return buf[0];

}

void
show_cells()
{
	//cout << "102 OK Terminated by dot" << endl;
	cout << "Row: " << curow << " Col: " << cucol << endl;
	for(int r=1; r<10; ++r) {
		cout << on_red("R" + pad_right(std::to_string(r), 3))  << " ";
		for(int c=1; c< 5; ++c) {
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


/* ANSI sequence:
 * \E[A up
 * \E[B down
 * \E[H home
 * \E   escape
 */

void
visual_mode()
{
	colours();
	std::string inp;
	while(true){
		cout << "\E[H"; //home
		show_cells();
		meta_keys special;
		int c = read_and_cook(&special);
		//int c = unbuffered_getch();
		//cout << "Input = " << c << endl;
		if(c == 'q' || (special == K_NORM && c == '\E')) break;
		if(c == 'h' || special == K_LEFT) cucol = std::max(1, cucol-1);
		if(c == 'j' || special == K_DOWN) curow++;
		if(c == 'k' || special == K_UP) curow = std::max(1, curow-1);
		if(c == 'l' || special == K_RIGHT) cucol++;


	}
	cout << "Exited visual mode\n";

}


