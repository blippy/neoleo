#include <exception>
#include <getopt.h>
#include <iostream>
#include <string>
#include <sys/auxv.h>
#include <unistd.h>
#include <vector>

#include "config.h"
#include "assert.h"
#include "basic.h"
#include "cmd.h"
#include "io-term.h"
#include "io-utils.h"
#include "logging.h"
#include "parser-2019.h"
#include "sheet.h"
#include "tests.h"
#include "utils.h"
#include "io-curses.h"
#include "window.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

using namespace std::literals;

extern void headless_main();
void curses_main();

static bool	option_tests = false;
//std::string	option_tests_argument = "regular";

bool get_option_tests() { return option_tests;}

static char short_options[] = "VHhpTv";
static struct option long_options[] =
{
	{"version",		0,	NULL,	'V'},
	{"headless",		0,	NULL,	'H'},
	{"help",		0,	NULL,	'h'},
	{"parser",		0,	NULL,	'p'},
	{"tests",		optional_argument,	NULL,	'T'},
	{"version",		0,	NULL,	'v'},
	{NULL,			0,	NULL,	0}
};


// FN panic 
void panic (const char *s,...)
{
	va_list iggy;

	va_start (iggy, s);
	fprintf (stderr, "%s %s:", PACKAGE_NAME, VERSION);
	vfprintf (stderr, s, iggy);
	putc ('\n', stderr);
	va_end (iggy);
	exit (2);
}
// FN-END

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
  -H, --headless           run without all toolkits
  -h, --help               display this help and exit
  -V, --version            output version information and exit
  -T, --tests [x]          run test suite x

Report bugs to https://github.com/blippy/neoleo/issues
)";

	printf("%s", usage);
}



void
parse_command_line(int argc, char **argv)
{
	int opt, optindex;

	while (1) {
		opt = getopt_long (argc, argv, short_options, long_options, &optindex);
		if (opt == EOF)
			break;

		switch (opt)
		{
			case 'v':
			case 'V':
				print_version();
				exit (0);
				break;
			case 'H':
				user_wants_headless = true;
				break;
			case 'h':
				show_usage ();
				exit (0);
				break;
			case 'T':
				option_tests = true;
				//cout << "optindex:" << optind << "\n";
				if(!optarg 
						&& optind < argc
						&& NULL !=argv[optind] 
						&& '\0' != argv[optind][0]
						&& '-' != argv[optind][0])
					option_tests_argument = argv[optind++];
				//exit(1);
				break;
		}
	}


	if (argc - optind > 1)
	{
		show_usage ();
		exit (1);
	}
}


void run_nonexperimental_mode(int argc, char** argv, int command_line_file)
{
	if(get_option_tests()) {
		bool all_pass = headless_tests();
		int ret = all_pass ? EXIT_SUCCESS : EXIT_FAILURE;
		//ret = EXIT_FAILURE;
		exit(ret);
	}



	FD_ZERO (&read_fd_set);
	FD_ZERO (&read_pending_fd_set);
	FD_ZERO (&exception_fd_set);
	FD_ZERO (&exception_pending_fd_set);

	//constexpr bool pesky_abstract = true;
	bool force_cmd_graphics = false;
	choose_display(force_cmd_graphics);


	using namespace std::literals;
	set_def_format(155); // which is "general.float", believe it or not

	if (argc - optind == 1) {
		if (FILE *fp = fopen (argv[optind], "r")) {
			try {
				read_file_and_run_hooks (fp, 0, argv[optind]);
			} catch (OleoJmp& e) {
				cerr << e.what() << endl;
				fprintf (stderr, ", error occured reading '%s'\n", argv[optind]);
				exit(1);
			} 
			fclose (fp);
		}

		command_line_file = 1;
		FileSetCurrentFileName(argv[optind]);
		optind++;
	}

	io_init_windows();
	Global->display_opened = 1;
	Global->modified = 0;
	if(user_wants_headless)
		headless_main();
	else {
		curses_main();
	}
	
}



int 
main (int argc, char **argv)
{
	int command_line_file = 0;	/* was there one? */
	parse_command_line(argc, argv);
	run_nonexperimental_mode(argc, argv, command_line_file);

	return (0);
}
