#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <ncursesw/ncurses.h>
#include <unistd.h>
#include <iostream>

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

// FN getline_from_fildes .
std::string getline_from_fildes (int fildes, bool& eof)
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
// 25/10 added
static void hl_column_align_left (const string& arg)
{
	//std::istringstream is(arg);



}
#endif

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


static void info()
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




static void hless_dump_sheet()
{
	extern void dump_sheet();
	dump_sheet();
}


bool reading(T fildes, string& line)
{
	bool eof;
again:
	line = getline_from_fildes(fildes, eof);
	if(line == ".") return false;
	if(line.size() == 0 && eof) return false;
	if(line.starts_with('#')) goto again;
	return true;
}

static void insert_columnwise(T fildes)
{
	string line;
	while(reading(fildes, line)) {
		if(line == ";") { cucol++; 	curow=1; continue;}
		if(line == "")  { curow++; continue;}
		set_cell_input_1(curow, cucol, line);
		curow++;
	}
}

static void insert_rowwise(T fildes)
{
	string line;
	while(reading(fildes, line)) {
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
void hl_write_file(string filename)
{
	//log("_write_file:called:", filename);
	if(filename.size() > 0) FileSetCurrentFileName(filename);
	hl_write_file();

}
// FN-END





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

static void hl_exit(string arg)
{	
	//cout << "exit called\n";
	if(arg == "$?") {
		//log("calling hl_exit with status ", _sys_ret);
		exit(_sys_ret ? 1 : 0); // return numbers can be too high for our purposes
	}
	auto ret = to_int(arg);
	if(ret.has_value()) exit(ret.value());
	exit(0);
}

static void hl_exec(string command)
{
	//log("hl_exec", command);
	//system("ls");
	_sys_ret = system(command.c_str());
	//log("system status:", _sys_ret, ", arg:", _arg);
	//cout << _sys_ret << "\n";
}

// FN hl_print_row
// 25/05 Started. Very rough at this stage!
static void hl_print_row (string arg)
{
	// assume for now that we only want to print the first row
	// and that there are 80 columns
	//std::array<int, 80> row{-1};

	//log("hl_print_row:enter:", arg);
	stringstream ss(arg);
	int row_start = 1;
	int row_end = 1;
	hl_scan_range(ss, row_start, row_end);

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

}
// FN-END

// FN hl_hi 
// 25/10 added
static void hl_hi () { cout << "neoleo says 'hi'" << endl; }

bool process_headless_line(std::string line, int fildes)
{
	// break line down into a command and arguments
	//int len = line.size();
	if(line.size() == 0) return true;
	if(line[0] == '#') return true;
	int i = 0;
	string cmd;
	string arg = ""; // same it for use by any function
	line = trim(line);
	int len = line.size();
	//while(i<len && isspace(line[i])) i++;
	while(i<len && !isspace(line[i])) cmd += line[i++];
	while(i<len && isspace(line[i])) i++;
	while(i<len) arg += line[i++];
	//cout << "'" << cmd << "'\n";

	//auto is = [&cmd](string s) { return cmd == s; };
	if( cmd == "!") { hl_exec(arg); }
	else if(cmd == "cal") {column_align_left();}
	else if(cmd == "dump-sheet") {
		hless_dump_sheet();
	} else if(cmd == "exit") {
		hl_exit(arg);
	} else if(cmd == "g") {
		hl_goto_cell(fildes);
	} else if(cmd == "hi") { 
		hl_hi();
	} else if(cmd == "I") {
		insert_rowwise(fildes);
	} else if(cmd == "i") {
		insert_columnwise(fildes);
	} else if(cmd == "info") {
		info();
	} else if(cmd == "p") {
		hl_print_row(arg);
	//} else if(cmd == "picol") {
	//	extern int picol_interactive();
	//	picol_interactive();
	} else if(cmd == "q") {
		return false;
	} else if(cmd == "ri") {
		hl_insert_row();
	} else if(cmd == "t") {
		_type_sheet();
	} else if(cmd == "recalc") {
		hl_recalc();
	} else if(cmd == "type-cell") {
		type_cell();
	} else if(cmd == "visual") {
		extern void curses_main();
		curses_main();
	} else if( cmd == "w") {
		hl_write_file(arg);
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
#if 0
	// 25/10 test of parser - seems to work
	ranges_t predecs;
	string s{"life()"};
	Expr expr{parse_string(s, predecs, curow, cucol)};
	Tour tour;
	value_t val{eval_expr(tour, expr)};
	//cout << fmt_value(val) << endl;
#endif



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

int headless_script_run(const std::string& script_file)
{
	return headless_script(script_file.c_str());
}
