//#include <exception>
#include <getopt.h>
//#include <iostream>
#include <filesystem>
#include <string>
#include <sys/auxv.h>
#include <unistd.h>
//#include <vector>

//#include <tcl.h>

#include "assert.h"
#include "basic.h"
//#include "blang2.h"
//#include "blx.h"
#include "neotypes.h"
#include "oleofile.h"
#include "parser-2019.h"
#include "sheet.h"



using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

using namespace std::literals;

void curses_main();

static char short_options[] = "e:hm:t:v";
static struct option long_options[] =
{
		{"eval",		required_argument,	NULL,	'e'},
		{"version",		0,	NULL,	'V'},
		{"help",		0,	NULL,	'h'},
		{"mode",		required_argument,	NULL,	'm'},
		{"tcl",			required_argument,	NULL,	't'},
		{"version",		0,	NULL,	'v'},
		{NULL,			0,	NULL,	0}
};




void print_version()
{
	printf("%s %s\n", PACKAGE_NAME, VERSION);
	printf("Copyright (c) 1992-2000 Free Software Foundation, Inc.\n");
	printf("%s comes with ABSOLUTELY NO WARRANTY.\n", PACKAGE_NAME);
	printf("You may redistribute copies of %s\n", PACKAGE);
	printf("under the terms of the GNU General Public License.\n");
	printf("For more information about these matters, ");
	printf("see the files named COPYING.\n");
	printf("\nCompiled: %s %s\n", __DATE__, __TIME__);
	printf("Auxval: %s\n", (char *)getauxval(AT_EXECFN));
}



static void
show_usage (void)
{

	printf("This is %s %s\n\n", PACKAGE, VERSION);
	printf("Usage: %s [OPTION]... [FILE]...\n", PACKAGE);

const char* usage = R"(
  -e, --eval EXPR          Evaluate a Tcl expression
  -h, --help               display this help and exit
  -m, --mode MODE          run in mode MODE, where MODE is one of:
                              0 : do nothing. Exit after running scripts
                              tcl : read and execute Tcl commands
                           Default is to use ncurses
  -t  --tcl FILE           execute a Tcl file
  -V, --version            output version information and exit

Report bugs to https://github.com/blippy/neoleo/issues
)";

	printf("%s", usage);
}


enum class ReplType { none, headless, ncurses};
typedef struct {char type; string  str;} opt_tcl_t; // type is either 'e' for eval or 't' for Tcl file. str is the thing to be evaluated/run
struct {
	string mode;
	vector<opt_tcl_t> tcls; // stuff to either eval or run
} cmd_options; // command-line options

void parse_command_line (int argc, char **argv) //bool& user_wants_headless, strings& blang_files)
{
	int opt, optindex;

	while (1) {
		opt = getopt_long (argc, argv, short_options, long_options, &optindex);
		if (opt == EOF)
			break;

		switch (opt) {
			case 'e':
				cmd_options.tcls.push_back(opt_tcl_t{'e', optarg});
				break;
			case 'v':
			case 'V':
				print_version();
				exit (0);
				break;
			case 'h':
				show_usage ();
				exit (0);
				break;
			case 'm':
				cmd_options.mode = optarg;
				break;
			case 't':
				cmd_options.tcls.push_back(opt_tcl_t{'t', optarg});
				break;
		}
	}


	if (argc - optind > 1)
	{
		show_usage ();
		exit (1);
	}

}

std::string slurp(const char *filename)
{
        std::ifstream in;
        in.open(filename, std::ifstream::in | std::ifstream::binary);
        std::stringstream sstr;
        sstr << in.rdbuf();
        in.close();
        return sstr.str();
}


void run_nonexperimental_mode(int argc, char** argv) //, int command_line_file, bool use_headless, strings blang_files)
{


	extern void tickle_main();
	using namespace std::literals;
	set_def_format(155); // which is "general.float", believe it or not
	FileSetCurrentFileName(string{std::filesystem::current_path()} + "/unnamed.oleo"); // 25/11 In case started up from desktop with no file name

	if (argc - optind == 1) {
		oleo_read_file(argv[optind]);
		FileSetCurrentFileName(argv[optind]);
		optind++;
	}

	Global_modified = 0;

	void tickle_eval_expr(const std::string& expr);
	extern void tickle_run_file(const std::string& path);
	for(auto const& f : cmd_options.tcls) {
		switch(f.type) {
		case 'e' : tickle_eval_expr(f.str); break;
		case 't' : tickle_run_file(f.str); break;
		default: throw std::logic_error("Cannot reach here");
		}
	}

	const string& mode = cmd_options.mode;
	if(mode=="0") { /* do nothing */ }
	else if(mode=="tcl") { tickle_main();}
	else { curses_main(); }
}



int main (int argc, char **argv)
{
	extern void tickle_init(char*);
	tickle_init(argv[0]);
	parse_command_line(argc, argv);
	run_nonexperimental_mode(argc, argv); // , command_line_file, use_headless, blang_files);
	return exit_value;
}
