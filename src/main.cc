//#include <exception>
#include <getopt.h>
//#include <iostream>
#include <string>
#include <sys/auxv.h>
#include <unistd.h>
//#include <vector>

//#include <tcl.h>

#include "assert.h"
#include "basic.h"
#include "blang2.h"
#include "blx.h"
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

extern void headless_main();
extern int headless_script(const char* script_file);
void curses_main();

static bool	option_tests = false;
//static int	opt_script = 0;
static char*	opt_script_file = 0;
//std::string	option_tests_argument = "regular";

bool get_option_tests() { return option_tests;}

static char short_options[] = "b:Vhm:s:t:v";
static struct option long_options[] =
{
		//{"no-repl",		0,	NULL,	'0'},
		{"blang",		required_argument, NULL, 'b'},
		{"version",		0,	NULL,	'V'},
		//{"headless",	0,	NULL,	'H'},
		{"help",		0,	NULL,	'h'},
		{"mode",		required_argument,	NULL,	'm'},
		//{"parser",		0,	NULL,	'p'},
		{"script",		required_argument,	NULL,	's'},
		{"tcl",			required_argument,	NULL,	't'},
		//{"tests",		optional_argument,	NULL,	'T'},
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
  -0, --no-repl            do not use headless or ncurses repl
  -b, --blang FILE         execute a blang file
  -H, --headless           run without all toolkits
  -h, --help               display this help and exit
  -s, --script FILE        execute a script
  -t  --tcl FILE           execute a Tcl file
  -V, --version            output version information and exit

Report bugs to https://github.com/blippy/neoleo/issues
)";

	printf("%s", usage);
}


enum class ReplType { none, headless, ncurses};
struct {
	//ReplType rt = ReplType::ncurses;
	strings blang_files;
	string mode;
	strings tcl_files;
} cmd_options; // command-line options

void parse_command_line (int argc, char **argv) //bool& user_wants_headless, strings& blang_files)
{
	int opt, optindex;

	while (1) {
		opt = getopt_long (argc, argv, short_options, long_options, &optindex);
		if (opt == EOF)
			break;

		switch (opt) {
			//case '0':
			//	cmd_options.mode = "0";
				break;
			case 'b':
				cmd_options.blang_files.push_back(optarg);
				break;
			case 'v':
			case 'V':
				print_version();
				exit (0);
				break;
			//case 'H':
				// TODO remove
			//	cmd_options.mode = "h";
			//	break;
			case 'h':
				show_usage ();
				exit (0);
				break;
			case 'm':
				cmd_options.mode = optarg;
				break;
			case 's':
				opt_script_file = optarg;
				break;
			case 't':
				cmd_options.tcl_files.push_back(optarg);
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


	using namespace std::literals;
	set_def_format(155); // which is "general.float", believe it or not

	if (argc - optind == 1) {
		if (FILE *fp = fopen (argv[optind], "r")) {
			try {
				read_file_and_run_hooks (fp, argv[optind]);
			} catch (OleoJmp& e) {
				cerr << e.what() << endl;
				fprintf (stderr, ", error occurred reading '%s'\n", argv[optind]);
				exit(1);
			} 
			fclose (fp);
		}

		//command_line_file = 1;
		FileSetCurrentFileName(argv[optind]);
		optind++;
	}

	Global_modified = 0;

	blx_init();
	for(auto const& f : cmd_options.blang_files) {
		auto src = slurp(f.c_str());
		blang::interpret_string(src);
	}

	for(auto const& f : cmd_options.tcl_files) {
		extern void tickle_run_file(const std::string& path);
		tickle_run_file(f);
		//auto src = slurp(f.c_str());
		//blang::interpret_string(src);
	}

	if(opt_script_file) {
		headless_script(opt_script_file);
		//exit(ret);
	}

	const string& mode = cmd_options.mode;
	if(mode == "h") {headless_main(); }
	else if(mode=="0") { /* do nothing */ }
	else { curses_main(); }
	// otherwise we want to run neither, so it will be purely script-based


}



int main (int argc, char **argv)
{
	extern void tickle_init(char*);
	tickle_init(argv[0]);


	//malloc(666);
	//int command_line_file = 0;	/* was there one? */
	//bool use_headless = false;
	//strings blang_files;
	parse_command_line(argc, argv);
	run_nonexperimental_mode(argc, argv); // , command_line_file, use_headless, blang_files);

	//cout << "returning with exit_value " << exit_value << endl;
	//if(exit_value != 0) exit(exit_value);
	return exit_value;
}
