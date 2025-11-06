#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <source_location>

#include "basic.h"
#include "cell.h"
#include "sheet.h"
#include "oleofile.h"
#include "spans.h"


using namespace std;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::function;
using std::map;
using std::string;
using std::stringstream;
using std::vector;

typedef int T;

//static string _arg; // holds any argument found by process_headless_line()

void log_func(const std::source_location& location = std::source_location::current())
{
	log(location.function_name());
}


static string get_word(std::istream& is){
	string result;
	is >> std::ws;
	while(1) {
		char c;
		is >> std::noskipws >> c;
		if(is.eof() || isspace(c)) break;
		result += c;
	}
	return result;
}

template <typename T> // T is usuall stringstream or istream
static void hl_eat_ws(T& ss)
{
	while(isspace(ss.peek())) ss.get();
}
static bool hl_scan_pint(stringstream& ss, int& val)
{
	hl_eat_ws(ss);
	bool found = false;
	int res = 0;
	while(isdigit(ss.peek())) {
		found = true;
		res *= 10;
		res += ss.get() - '0';
	}

	if(found) val = res;
	return found;
}

static bool hl_scan_char(stringstream& ss, char c)
{
	hl_eat_ws(ss);
	if(ss.peek() == c) {
		ss.get();
		return true;
	}
	return false;
}


static bool hl_scan_range(stringstream& ss, int& start, int& end)
{
	if (hl_scan_pint(ss, start)) {
		end = start;
		if (hl_scan_char(ss, ','))
			hl_scan_pint(ss, end);
		return true;
	}
	return false;
}


void set_cell_input_1 (CELLREF r, CELLREF c, const string& formula)
{
	curow = r;
	cucol = c;
	//CELL* cp = find_or_make_cell(r, c);
	set_and_eval(r, c, formula, true);
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






static void hless_dump_sheet()
{
	extern void dump_sheet();
	dump_sheet();
}


bool reading(std::istream& is, string& line)
{
again:
	getline(is, line);
	if(line == ".") return false;
	if(line.size() == 0 && is.eof()) return false;
	if(line.starts_with('#')) goto again;
	return true;
}

static void insert_columnwise(std::istream& is)
{
	//log_func();
	string line;
	while(reading(is, line)) {
		if(line == ";") { cucol++; 	curow=1; continue;}
		if(line == "")  { curow++; continue;}
		set_cell_input_1(curow, cucol, line);
		curow++;
	}
}

static void insert_rowwise(std::istream& is)
{
	string line;
	while(reading(is, line)) {
		if(line == ";") { curow++; 	cucol=1; continue;}
		if(line == "")  { cucol++; continue;}
		set_cell_input_1(curow, cucol, line);
		cucol++;
	}
}


static void type_cell()
{
	cout << string_cell() << "\n";
}

// FN hl_write_file .
static void hl_write_file()
{
	oleo_write_file();
}

static void hl_write_file(std::istream& is)
{
	string filename;
	getline(is, filename);
	if(filename.size() > 0) FileSetCurrentFileName(filename);
	hl_write_file();

}
// FN-END





static void hl_goto_cell(std::istream& is)
{
	//log_func();
	try {
		CELLREF r = stol(get_word(is));
		CELLREF c = stol(get_word(is));
		curow = r;
		cucol = c;
	} catch(...) {
		return;
	}
}

static void hl_recalc()
{
	recalculate(1);
}

static void hl_insert_row()
{
	insert_1row();
}

// type the sheet as an oleo file to stdout
static void _type_sheet()
{
	olfos_t olfos;
	oleo_write_file(olfos);
	cout << olfos.str() << flush;
}

static void hl_exit(std::istream& is)
{	
	string str;
	getline(is, str);

	if(str == "$?") {
		exit(exit_value ? 1 : 0); // return numbers can be too high for our purposes
	}

	auto ret = to_int(str);
	if(ret.has_value()) exit(ret.value());
	exit(0);
}

static void hl_exec(std::istream& is)
{
	string command;
	getline(is, command);
	exit_value = system(command.c_str());
}

// FN hl_print_row
// 25/05 Started. Very rough at this stage!
static void hl_print_row (std::istream& is)
{
	stringstream ss;
	ss << is.rdbuf();
	int row_start = 1;
	int row_end = 1;
	hl_scan_range(ss, row_start, row_end);
	log("row_start ", row_start, " row_end ", row_end);

	int col_start = 1;
	int col_end   = 10;
	if(hl_scan_char(ss, 'c'))
		hl_scan_range(ss, col_start, col_end);

	for (int row = row_start; row <= row_end; row++) {
		for (int col = col_start; col <= col_end; col++) {
			int w = get_width(col);
			CELL *cp = find_cell(row, col);
			if (cp == 0) {
				cout << pad_left("", w);
			} else {
				enum jst just = cp->get_cell_jst();
				string txt { string_cell(cp) };
				txt = pad_jst(txt, w, just);
				cout << txt;
			}
			cout << " ";
		}
		cout << endl;
	}

}
// FN-END

// FN hl_hi 
// 25/10 added
static void hl_hi () { cout << "neoleo says 'hi'" << endl; }

static void hl_where()
{
	cout << curow << " " << cucol << endl;

}

static void hl_set_cell (std::istream& is)
{
	//cout << "hl_set_cell called\n";
	CELLREF r = stol(get_word(is));
	CELLREF c = stol(get_word(is));
	hl_eat_ws(is);
	string formula;
	is >> formula;
	//cout << "hl_set_cell  " << r << " " << c << " with " <<formula << endl;
	set_cell_input_1(r, c, formula);

}

static void process_headless_line(const std::string& str, istream& rest_of_stream)
{
	string line{str};
	if(line.size() == 0) return;
	if(line[0] == '#') return;

	std::istringstream is(str);
	string cmd{get_word(is)};

	if( cmd == "!") { hl_exec(is); }
	else if(cmd == "cal") {column_align_left();}
	else if(cmd == "clr-sheet") {clear_spreadsheet();}
	else if(cmd == "dump-sheet") { hless_dump_sheet(); }
	else if(cmd == "exit") {
		hl_exit(is);
	} else if(cmd == "g" || cmd == "go") { hl_goto_cell(is);}
	else if(cmd == "hi") {
		hl_hi();
	} else if(cmd == "I") {
		insert_rowwise(rest_of_stream);
	} else if(cmd == "i") {
		insert_columnwise(rest_of_stream);
	} else if(cmd == "p") {hl_print_row(is); }
	else if(cmd == "q") {	Global_definitely_quit = true;}
	else if(cmd == "recalc") { 	hl_recalc(); }
	else if(cmd == "ri") { hl_insert_row(); }
	else if(cmd == "set-cell") { hl_set_cell(is); }
	else if(cmd == "t") { _type_sheet(); }
	else if(cmd == "type-cell") {
		type_cell();
	} else if(cmd == "visual") {
		extern void curses_main();
		curses_main();
		Global_definitely_quit = true;
	} else if( cmd == "w") { hl_write_file(is); }
	else if( cmd == "where") { hl_where(); }

	cout << std::flush;
}

static void _repl(std::istream& is)
{
	string line;
	while( !Global_definitely_quit && std::getline(is, line)) {
		process_headless_line(line, is);
	}
}



void headless_main() // FN
{
	_repl(std::cin);
}

int headless_script(const char* script_file)
{
	std::ifstream in(script_file);
	if(!in.is_open()) {
		cerr << "? Couldn't script file:" << script_file << endl;
		return 1;
	}

	_repl(in);

	return 0;
}

int headless_script_run(const std::string& script_file)
{
	return headless_script(script_file.c_str());
}
