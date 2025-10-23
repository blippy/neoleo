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
static int _sys_ret = 0; // store the value of the last system call we make so that we can use it in exit

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

#if 0
// FN getline_from_fildes .
static std::string getline_from_fildes (int fildes, bool& eof)
{
	char ch;
	std::string line;
	while(true) {
		eof = read(fildes, &ch, 1) == 0;
		if(eof) return line;

		if(ch == '\n') {
			//ofs << "line: " << line << endl;
			//exec_cmd(line, fildes);
			//line = "";
			return line;
		} else {
			line += ch;
		}
	}

	return line;
}
// FN-END
#endif

static void hl_eat_ws(stringstream& ss)
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


#if 0
static string hl_getline (int fildes)
{
	bool eof;
	return getline_from_fildes(fildes, eof);
}
#endif

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
	//bool eof;
again:
	//line = getline_from_fildes(fildes, eof);
	getline(is, line);
	if(line == ".") return false;
	if(line.size() == 0 && is.eof()) return false;
	if(line.starts_with('#')) goto again;
	return true;
}

static void insert_columnwise(std::istream& is)
{
	log_func();
	string line;
	while(reading(is, line)) {
		log("line is ", line);
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
	cout << print_cell() << "\n";
}

// FN hl_write_file .
void hl_write_file()
{
	string name = FileGetCurrentFileName();
	
	//log("writing file:", name);
	
	FILE *fp = fopen(name.c_str(), "w");
	if(!fp) {
		_sys_ret = 1; 
		cerr << "? Couldn't open neoleo file for writing:" << name << endl;
		return;
	}

	//assert(fp);
	write_cmd(fp, name.c_str());
	fclose(fp);

}

static void hl_write_file(std::istream& is)
{
	string filename;
	getline(is, filename);
	//log("_write_file:called:", filename);
	if(filename.size() > 0) FileSetCurrentFileName(filename);
	hl_write_file();

}
// FN-END





static void hl_goto_cell(std::istream& is)
{
	//std::size_t pos{};
	//string line;
	log_func();
	try {
		CELLREF r = stol(get_word(is));
		CELLREF c = stol(get_word(is));
		//CELLREF r, c;
		//is >> r;
		log("r=", r);
		//is >> c;
		log("c=", c);

		//getline(is, line);
		//auto r = (CELLREF) stol(line);
		//getline(is, line);
		//auto c = (CELLREF) stol(line);
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
	//cout << "exit called\n";
	string str;
	getline(is, str);

	if(str == "$?") {
		//log("calling hl_exit with status ", _sys_ret);
		exit(_sys_ret ? 1 : 0); // return numbers can be too high for our purposes
	}

	auto ret = to_int(str);
	if(ret.has_value()) exit(ret.value());
	exit(0);
}

static void hl_exec(std::istream& is)
{
	string command;
	getline(is, command);
	//command = is.rdbuf();
	_sys_ret = system(command.c_str());
}

// FN hl_print_row
// 25/05 Started. Very rough at this stage!
static void hl_print_row (std::istream& is)
{
	// assume for now that we only want to print the first row
	// and that there are 80 columns
	//std::array<int, 80> row{-1};

	//log("hl_print_row:enter:", arg);
	stringstream ss;
	ss << is.rdbuf();
	//auto ss = is;
	int row_start = 1;
	int row_end = 1;
	hl_scan_range(ss, row_start, row_end);
	log("row_start ", row_start, " row_end ", row_end);

	int col_start = 1;
	int col_end   = 10;
	if(hl_scan_char(ss, 'c'))
		hl_scan_range(ss, col_start, col_end);



	//int row = row_start;
	//log("hl_print_row: row:", row);
	//int row = std::max(1, atoi(arg.c_str()));
	for (int row = row_start; row <= row_end; row++) {
		for (int col = col_start; col <= col_end; col++) {
			int w = get_width(col);
			CELL *cp = find_cell(row, col);
			if (cp == 0) {
				cout << pad_left("", w);
			} else {
				enum jst just = cp->get_cell_jst();
				string txt { print_cell(cp) };
				txt = pad_jst(txt, w, just);
				cout << txt;
			}
			cout << " ";
		}
		cout << endl;
	}
	//log("hl_print_row:exiting");

}
// FN-END

// FN hl_hi 
// 25/10 added
static void hl_hi () { cout << "neoleo says 'hi'" << endl; }

static void hl_where()
{
	cout << curow << " " << cucol << endl;

}

static void process_headless_line(const std::string& str, istream& rest_of_stream)
{
	string line{str};
	if(line.size() == 0) return;
	if(line[0] == '#') return;

	std::istringstream is(str);
	string cmd{get_word(is)};

	//auto is = [&cmd](string s) { return cmd == s; };
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

#if 1
static void _repl(std::istream& is)
{
	string line;
	while( !Global_definitely_quit && std::getline(is, line)) {
		process_headless_line(line, is);
	}


}

#else
static void _repl(int fildes)
{
	//log("_repl:start");
	while(!Global_definitely_quit) {
		try {
			bool eof;
			string line = getline_from_fildes(fildes, eof);
			process_headless_line(line, fildes);
			if(Global_definitely_quit) close(fildes);
			if(eof) { Global_definitely_quit = true; }
		} catch (OleoJmp& e) {
			cerr << e.what() << endl;
		}
	}
}
#endif


void headless_main() // FN
{
#if 0
	// 25/10 test of parser - seems to work
	ranges_t predecs;
	string s{"life()"};
	Expr expr{parse_string(s, predecs, curow, cucol)};
	Tour tour;
	value_t val{eval_expr(tour, expr)};
	//cout << fmt_value(val) << endl;
#endif

#if 1
	_repl(std::cin);
#else

	//cout << mod_hi() << endl;
	constexpr int fildes = STDIN_FILENO;
	_repl(fildes);
#endif
}

int headless_script(const char* script_file)
{
#if 1
	std::ifstream in(script_file);
	if(!in.is_open()) {
		cerr << "? Couldn't script file:" << script_file << endl;
		return 1;
	}

	_repl(in);

#else
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
#endif
	return 0;
}

int headless_script_run(const std::string& script_file)
{
	return headless_script(script_file.c_str());
}
