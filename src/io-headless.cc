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
//#include "window.h"
//#include "io-curses.h"
#include "io-utils.h"
#include "sheet.h"
#include "tbl.h"
#include "utils.h"
#include "oleofile.h"
//#include "global.h"
//import mod;

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

// type the sheet as an oleo file to stdout
static void _type_sheet(int fildes) 
{
	oleo_write_file(stdout);
}

static map<string, function<void(T)> > func_map = {
	{"dump-sheet", hless_dump_sheet},
	{"g", hl_goto_cell},
	{"I", insert_rowwise},
	{"i", insert_columnwise},
	{"info", info},
	{"ri", hl_insert_row},
	{"t", _type_sheet},
	{"tbl", hless_tbl},
	{"recalc", hl_recalc},
	{"type-cell", type_cell},
	{"type-dsv", type_dsv},
	{"w", _write_file}
};

bool process_headless_line(std::string line, int fildes)
{

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

void headless_main() // FN
{
	//cout << mod_hi_sv() << endl;

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
