#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

#include "assert.h"
//#include "atlast.h"
//#include "atoleo.h"
#include "basic.h"
#include "cmd.h"
#include "defuns.h"
#include "init.h"
#include "io-abstract.h"
#include "io-term.h"
#include "io-utils.h"
#include "mdi.h"
#include "tests.h"
//#include "graph.h"
//#include "mysql.h"
//#include "print.h"
#include "logging.h"
#include "utils.h"
#include "io-headless.h"

#include "oleox.hpp"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

#define _(x) (x) // TODO get rid of this line


void
init_native_language_support()
{
#if 0	/* ENABLE_NLS */ // mcarter
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
#endif
}

/* A bland signal handler. */
static RETSIGTYPE
got_sig (int sig)
{
}


static void
init_maps (void)
{
  num_maps = 0;
  the_maps = 0;
  map_names = 0;
  map_prompts = 0;

  the_funcs = (cmd_func**) ck_malloc (sizeof (struct cmd_func *) * 2);
  num_funcs = 1;
  the_funcs[0] = (cmd_func *) get_cmd_funcs();

  find_func (0, &end_macro_cmd, "end-macro");
  find_func (0, &digit_0_cmd, "digit-0");
  find_func (0, &digit_9_cmd, "digit-9");
  find_func (0, &break_cmd, "break");
  find_func (0, &universal_arg_cmd, "universal-argument");

  create_keymap ("universal", 0);
  push_command_frame (0, 0, 0);
}


void
read_init_files()
{
	vector<string> fnames;
	char *home = getenv ("HOME");
	if (home) fnames.push_back(string(home) + "/" + RCFILE);
	fnames.push_back(RCFILE);

	for(const auto& fname:fnames) {
		FILE *fp = fopen(fname.c_str(), "r");
		if(!fp) continue;

		try {
			read_cmds_cmd(fp);
		} catch (OleoJmp& e) {
			cerr << "OleoJmp caught by read_init_file()" << endl;
			string msg = string("   error occured in init file ")
				+ fname
				+ " near line "
				+ std::to_string(Global->sneaky_linec)
				+ "\n";
			const char *m = _(msg.c_str());
			fprintf(stderr, m);
			io_info_msg(m);
		}

		fclose(fp);
	}
}

void
init_maps_and_macros()
{
	try {
		init_maps();
		init_named_macro_strings ();
                run_init_cmds ();
	} catch (OleoJmp& e) {
		fprintf (stderr, _("Error in the builtin init scripts (a bug!).\n"));
                io_close_display(69);
                exit (69);
	}
}
int 
main(int argc, char **argv)
{
	int ignore_init_file = 0;
	int command_line_file = 0;	/* was there one? */


#ifdef HAVE_FORTH
	init_atoleo();
#endif

	init_native_language_support();
	MdiInitialize();	/* Create initial Global structure */
	//PlotInit();
	//AllocateDatabaseGlobal();
	InitializeGlobals();
	Global->argc = argc;
	Global->argv = argv;
	parse_command_line(argc, argv, &ignore_init_file);
	init_basics();
	headless_graphics(); // fallback position

	if(get_option_tests()) {
		headless_tests();
		exit(EXIT_SUCCESS);
	}



	FD_ZERO (&read_fd_set);
	FD_ZERO (&read_pending_fd_set);
	FD_ZERO (&exception_fd_set);
	FD_ZERO (&exception_pending_fd_set);

	bool force_cmd_graphics = false;
	choose_display(force_cmd_graphics);
	io_open_display ();

	//init_graphing ();
	//PrintInit();
	OleoSetEncoding(OLEO_DEFAULT_ENCODING);

	init_maps_and_macros();

	oleo_catch_signals(&got_sig);

	if(!ignore_init_file) read_init_files();

#ifdef HAVE_FORTH
	try_reading_forth_file(command_line_forth_file);
#endif

	if (option_filter) {
		read_file_and_run_hooks(stdin, 0, "stdin");
	} else if (argc - optind == 1) {
		if (FILE *fp = fopen (argv[optind], "r")) {
			try {
				read_file_and_run_hooks (fp, 0, argv[optind]);
			} catch (OleoJmp& e) {
				fprintf (stderr, _(", error occured reading '%s'\n"), argv[optind]);
				io_info_msg(_(", error occured reading '%s'\n"), argv[optind]);
			} 
			fclose (fp);
		}

		command_line_file = 1;
		FileSetCurrentFileName(argv[optind]);
		optind++;
	}

	/* Force the command frame to be rebuilt now that the keymaps exist. */
	{
		struct command_frame * last_of_the_old = the_cmd_frame->next;
		while (the_cmd_frame != last_of_the_old)
			free_cmd_frame (the_cmd_frame);
		free_cmd_frame (last_of_the_old);
	}

	io_recenter_cur_win ();
	Global->display_opened = 1;
	io_run_main_loop();

	return (0); /* Never Reached! */
}
