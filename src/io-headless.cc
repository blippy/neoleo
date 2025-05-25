#include <assert.h>
#include <algorithm>
#include <errno.h>
#include <fcntl.h>
#include <functional>
#include <generator>
#include <map>
#include <ncurses.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "basic.h"
#include "cell.h"
#include "io-utils.h"
#include "sheet.h"
//#include "tbl.h"
#include "utils.h"
#include "oleofile.h"
#include "spans.h"

import errors;
import logging;
//#include "global.h"
import mod;
import utl;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::function;
using std::map;
using std::string;
using std::vector;

typedef int T;

static string _arg; // holds any argument found by process_headless_line()
static int _sys_ret = 0; // store the value of the last system call we make so that we can use it in exit


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




static void hless_dump_sheet(T fildes)
{
	extern void dump_sheet();
	dump_sheet();
}



std::generator<string> reading(T fildes)
{
	while(true) {
		bool eof;
		string line = getline_from_fildes(fildes, eof);
		if(line == ".") break;
		//if(line == ";") { co_yield line ; continue; }
		if(line.size() == 0 && eof) break;
		if(line.starts_with('#')) continue;
		co_yield line;
	}

}

static void insert_columnwise(T fildes)
{
	for(const string& line : reading(fildes)) {
		if(line == ";") { cucol++; 	curow=1; continue;}
		if(line == "")  { curow++; continue;}
		set_cell_input_1(curow, cucol, line);
		curow++;
	}
}

static void insert_rowwise(T fildes)
{
	for(const string& line : reading(fildes)) {
		if(line == ";") { curow++; 	cucol=1; continue;}
		if(line == "")  { cucol++; continue;}
		set_cell_input_1(curow, cucol, line);
		cucol++;
	}
}


static void type_cell(int fildes)
{
	cout << print_cell() << "\n";
}


static void _write_file(int fildes)
{
	//log("_write_file:called");
	if(_arg.size() > 0) FileSetCurrentFileName(_arg);
	string name = FileGetCurrentFileName();
	
	//log("writing file:", name);
	
	FILE *fp = fopen(name.c_str(), "w");
	if(!fp) {
		_sys_ret = 1; 
		cerr << "? Couldn't oleo file for writing:" << name << endl;
		return;
	}

	//assert(fp);
	write_cmd(fp, name.c_str());
	fclose(fp);

}
void hl_write_file(){
	//log("hl_write_file:called");
	_write_file(0);
}

// this seems to crash
function<void(int)> with_int(function<void()> fn)
{
	//return std::bind(fn, 0);
	//return fn;
	return [&](int i) { fn(); };
}



static void hl_goto_cell(int fildes)
{
	std::size_t pos{};
	try {
		auto r = (CELLREF) stol(hl_getline(fildes), &pos);
		auto c = (CELLREF) stol(hl_getline(fildes), &pos);
		curow = r;
		cucol = c;
	} catch(...) {
		return;
	}
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

static void hl_exit(int fildes)
{	
	//cout << "exit called\n";
	if(_arg == "$?") {
		//log("calling hl_exit with status ", _sys_ret);
		exit(_sys_ret ? 1 : 0); // return numbers can be too high for our purposes
	}
	auto ret = to_int(_arg);
	if(ret.has_value()) exit(ret.value());
	exit(0);
}

static void _exc(int fildes)
{
	//system("ls");
	_sys_ret = system(_arg.c_str());
	//log("system status:", _sys_ret, ", arg:", _arg);
	//cout << _sys_ret << "\n";
}

// 25/05 Started. Very rough at this stage!
static void hl_print_row(int fildes)
{
	// assume for now that we only want to print the first row
	// and that there are 80 columns
	//std::array<int, 80> row{-1};

	int row = std::max(1, atoi(_arg.c_str()));
	for(int col =0;col < 10; col++){
		int w = get_width(col);
		CELL* cp = find_cell(row, col);
		if(cp == 0) {
			cout << pad_left("", w);
		} else {
			enum jst just = cp->get_cell_jst();
			string txt{print_cell(cp)};
			txt = pad_jst(txt, w, just);
			cout << txt;
		}
		cout << " ";
	}
	cout << endl;

}

static map<string, function<void(T)> > func_map = {
	{"!",		_exc},
	{"dump-sheet", 	hless_dump_sheet},
	{"exit", 	hl_exit},
	{"g", 		hl_goto_cell},
	{"I", 		insert_rowwise},
	{"i", 		insert_columnwise},
	{"info", 	info},
	{"p",		hl_print_row},
	{"ri", 		hl_insert_row},
	{"t", 		_type_sheet},
	{"recalc", 	hl_recalc},
	{"type-cell", 	type_cell},
	{"w", 		_write_file}
};

bool process_headless_line(std::string line, int fildes)
{
	// break line down into a command and arguments
	int len = line.size();
	if(len == 0) return true;
	if(line[0] == '#') return true;
	int i = 0;
	string cmd;
	_arg = ""; // same it for use by any function
	while(i<len && isspace(line[i])) i++;
	while(i<len && !isspace(line[i])) cmd += line[i++];
	while(i<len && isspace(line[i])) i++;
	while(i<len) _arg += line[i++];
	//cout << "'" << cmd << "'\n";

	if(cmd == "q") {
		//log("quit found");
		return false;
	}

	//log("process_headless_line cmd;", cmd, ";arg:", _arg);
	// try to find a canned function and execute it
	auto it = func_map.find(cmd);
	if(it != func_map.end()) {
		auto fn = it->second;
		fn(fildes);
		cout << std::flush;
		return true;
	} else {
		cerr << "? command not found:" << cmd << endl;
	}





	cout << std::flush;
	return true;
}

static void _repl(int fildes)
{
	//log("_repl:start");
	bool cont = true;
	while(cont) {
		try {
			//log(".");
			bool eof;
			string line = getline_from_fildes(fildes, eof);
			cont =	process_headless_line(line, fildes);
			if(!cont) close(fildes);
			if(eof) { cont = false; }
		} catch (OleoJmp& e) {
			cerr << e.what() << endl;
		}
	}
}

void headless_main() // FN
{
	//cout << mod_hi() << endl;
	constexpr int fildes = STDIN_FILENO;
	_repl(fildes);
}

int headless_script(const char* script_file)
{
	//log("headless_script:started");
	int fildes = open(script_file, O_RDONLY);
	if(fildes == -1) {
		cerr << "? Couldn't script file:" << script_file << endl;
		return 1;
	}

	//log("headless_script:calling repl");
	_repl(fildes);
	//log("headless_script:finished repl");
	close(fildes);
	return 0;
}
