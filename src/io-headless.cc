#include <assert.h>
#include <algorithm>
#include <errno.h>
#include <functional>
#include <map>
#include <ncurses.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "basic.h"
#include "cell.h"
#include "convert.h"
#include "io-abstract.h"
#include "io-headless.h"
#include "cmd.h"
#include "window.h"
#include "io-curses.h"
#include "io-utils.h"
#include "sheet.h"
#include "tbl.h"
#include "utils.h"


using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::function;
using std::map;
using std::string;
using std::vector;

typedef int T;

void set_cell_input_1 (CELLREF r, CELLREF c, const string& formula)
{
	curow = r;
	cucol = c;
	//CELL* cp = find_or_make_cell(r, c);
	set_and_eval(r, c, formula, true);
}


string hl_getline (int fildes)
{
	bool eof;
	return getline_from_fildes(fildes, eof);
}

string to_hex(long n)
{
	std::stringstream ss;
	ss << std::hex << n << "h";
	return ss.str();
}

string to_oct(long n)
{
	std::stringstream ss;
	ss << "0o" << std::oct << n;
	return ss.str();
}

static void do_nothing(void)
{
	return;
}


static void _io_open_display(void)
{
	/* We fake having a window. This is important because io_init_windows()
	 * will do things like set nwin = 1
	 *
	 * The init was cribbed from io-curses.c
	 */

	io_init_windows();

}


static void _io_repaint_win (struct window *win)
{
	//io_repaint ();
}




// suggested at
// http://stackoverflow.com/questions/8302547/temp-failure-retry-and-use-gnu
#define CALL_RETRY(retvar, expression) do { \
	retvar = (expression); \
} while (retvar == -1 && errno == EINTR);


static int _io_input_avail(void)
{
	int filedes = STDIN_FILENO;
	unsigned int seconds = 0;

	// taken from https://www.gnu.org/software/libc/manual/html_node/Waiting-for-I_002fO.html


	fd_set set;
	struct timeval timeout;


	/* Initialize the file descriptor set. */
	FD_ZERO (&set);
	FD_SET (filedes, &set);

	/* Initialize the timeout data structure. */
	timeout.tv_sec = seconds;
	timeout.tv_usec = 0;

	/* select returns 0 if timeout, 1 if input available, -1 if error. 
	 * Use CALL_RETRY rather than TEMP_FAILURE_RETRY (see above) */
	int res;
	CALL_RETRY (res, select (FD_SETSIZE,
				&set, NULL, NULL,
				&timeout));
	return res;
}



#if 0
static int _io_read_kbd(char *buf, int size)
{        
	//int r = read (0, buf, size);
	int r = read (STDIN_FILENO, buf, size);
	//FD_CLR (0, &read_pending_fd_set);
	//FD_CLR (0, &exception_pending_fd_set);
	return r;
}      
#endif

static void _io_insert (int len)
{ 
	//iv_insert (&input_view, len);
} 

static void _io_pr_cell_win (struct window *win, CELLREF r, CELLREF c, CELL *cp)
{
}

static void info(int fildes)
{
	// print diagnostic information

	typedef struct info_t { string str; int num; string desc; } info_t;
	auto infos = vector<info_t> {
		{"KEY_END",	KEY_END,	"End key"},
			{"KEY_HOME",	KEY_HOME,	"Home key"},
			{"KEY_LEFT", 	KEY_LEFT, 	"Arrow left"},
			{"KEY_NPAGE",	KEY_NPAGE}
	};

	for(const auto& i:infos)
		cout << "curses." << pad_right(i.str,11)  << pad_left(to_oct(i.num),6) 
			<< pad_left(to_hex(i.num), 7)  
			<< pad_left(std::to_string(int(i.num)), 5) 
			<< "     # " << i.desc << "\n";
}

static void insert_columnwise(T fildes)
{
	std::string line;
	while(true) {
		bool eof;
		line = getline_from_fildes(fildes, eof);
		if(line == ".") break;
		if(line == ";") {
			curow = 1;
			cucol++;
			continue;
		}
		if(line.size() ==0) {
			curow++;
			continue;
		}
		if(line[0] == '#') continue;

		set_cell_input_1(curow, cucol, line);
		curow++;

		//cout << "You said " << line <<  (line != "." ) << endl;
		if(eof) return;
	}
}


static void hless_dump_sheet(T fildes)
{
	extern void dump_sheet();
	dump_sheet();
}

static void insert_rowwise(T fildes)
{
	std::string line;
	while(true) {
		bool eof;
		line = getline_from_fildes(fildes, eof);
		if(line == ".") break;
		if(line == ";") {
			curow++;
			cucol=1;
			continue;
		}
		if(line.size() ==0) {
			cucol++;
			continue;
		}
		if(line[0] == '#') continue;

		set_cell_input_1(curow, cucol, line);
		cucol++;

		//cout << "You said " << line <<  (line != "." ) << endl;
		if(eof) return;
	}
}


static void type_cell(int fildes)
{
	//cout << "101 OK value appears on next line\n" 
	cout	<< print_cell(find_cell(curow, cucol))
		<< "\n";
}


static void _write_file(int fildes)
{
	string name = FileGetCurrentFileName();
	FILE *fp = fopen(name.c_str(), "w");
	assert(fp);
	write_cmd(fp, name.c_str());
	fclose(fp);

}
void hl_write_file(){
	_write_file(0);
}

// this seems to crash
function<void(int)> with_int(function<void()> fn)
{
	//return std::bind(fn, 0);
	//return fn;
	return [&](int i) { fn(); };
}

static void hless_tbl(int fildes)
{
	tbl();
}

static void type_dsv(int fildes)
{
	save_dsv(stdout, 0);
}

static void hl_goto_cell(int fildes)
{
	bool ok;
	auto r = (CELLREF) to_long(hl_getline(fildes), ok);
	if(!ok) return;
	auto c = (CELLREF) to_long(hl_getline(fildes), ok);
	if(!ok) return;
	curow = r;
	cucol = c;
	//rng_t a_rng{r, c, r, c};
	//goto_cell(&a_rng);
}

static void hl_recalc(int fildes)
{
	recalculate(1);
}

static void hl_insert_row(int fildes)
{
	insert_1row();
}

static map<string, function<void(T)> > func_map = {
	{"dump-sheet", hless_dump_sheet},
	{"g", hl_goto_cell},
	{"I", insert_rowwise},
	{"i", insert_columnwise},
	{"info", info},
	{"ri", hl_insert_row},
	{"tbl", hless_tbl},
	{"recalc", hl_recalc},
	{"type-cell", type_cell},
	{"type-dsv", type_dsv},
	{"w", _write_file}
};

bool process_headless_line(std::string line, int fildes)
{
	//cout << "process_headless_line: " << line << endl;

	// try to find a canned function and execute it
	auto it = func_map.find(line);
	if(it != func_map.end()) {
		auto fn = it->second;
		fn(fildes);
		cout << std::flush;
		return true;
	}


	if(line == "q") {
		return false;
	}


	cout << std::flush;
	return true;
}

void headless_main()
{
	std::string line;
	constexpr int fildes = STDIN_FILENO;
	bool cont = true;
	while(cont) {
		try {
			bool eof;
			line = getline_from_fildes(fildes, eof);
			cont =	process_headless_line(line, fildes);
			if(eof) { cont = false; }
		} catch (OleoJmp& e) {
			cerr << e.what() << endl;
		}
	}

}



void headless_graphics(void)
{
	io_open_display = _io_open_display;
	//io_redisp = do_nothing;
	io_repaint = do_nothing;
	io_repaint_win = _io_repaint_win;
	io_input_avail = _io_input_avail;
	//io_read_kbd = _io_read_kbd;
	//io_update_status = _io_update_status;
	//io_fix_input = _io_fix_input;
	io_insert = _io_insert;
	//io_flush = do_nothing;
	io_pr_cell_win = _io_pr_cell_win;
	io_hide_cell_cursor = do_nothing;
	io_display_cell_cursor = do_nothing;
}

