#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// check for leaks, as suggested at 
// http://stackoverflow.com/questions/33201345/leaksanitizer-get-run-time-leak-reports
//#include <sanitizer/lsan_interface.h>


#include "neoleo_swig.h"
#include "io-abstract.h"
#include "basic.h"
#include "io-headless.h"
#include "io-term.h"
#include "io-utils.h"
#include "cell.h"
#include "mdi.h"
#include "mysql.h"
#include "ref.h"


void *
main1(void *td)
{
        //main0(0, NULL);
        main(0, NULL);
}

void 
start_swig_motif()
{
#ifdef HAVE_MOTIF
        pthread_t tid;
        //motif_main_loop();
        int error = pthread_create(&tid, NULL, main1, NULL);
        if(error != 0) 
                fprintf(stderr, "Couldn't run motif thread\n");
#else
        fprintf(stderr, "No motif available; "
                        "segfault likely.\n");
#endif

}

char * 
get_formula(int curow, int cucol)
{
        CELL *cp = find_cell(curow, cucol);
        return decomp(curow, cucol, cp);
}

/*
void
set_cell(int r, int c, char* str)
{

}
*/

int // returns 1 => OK
swig_read_file_and_run_hooks(char *name, int ismerge)
{
        FILE *fp = fopen(name, "r");
        if(fp == 0) return 0;
        read_file_and_run_hooks(fp, ismerge, name);
        fclose(fp);
        return 1;

}

void FreeGlobals()
{
	/* TODO - make more accassable - neoleo doesn't seem
	 * to clean up after itself.
	 *
	 * This functionality is far from complete */
	FileCloseCurrentFile();
}

void get_set(int r, int c, char* s)
{
	set_cell(r, c, s); // rounding nasty
	printf("Formula at (%d,%d) is:%s\n", r, c, get_formula(r,c));
	decomp_free();
	recalculate(1);
	printf("Cell value at (%d,%d) is:%s\n", r, c, cell_value_string(r,c, 0));
	puts("");
}


void
headless_tests()
{
	//cmd_graphics(); // in leui of calling choose_display
	bool force_cmd_graphics = true;

	//choose_display(argc, argv, force_cmd_graphics);
	headless_graphics();
	io_open_display();

        //# the following causes crash:
        int read_status = swig_read_file_and_run_hooks("/home/mcarter/repos/neoleo/examples/pivot.oleo", 0);
        if(read_status == 1) {
                puts("read worked");
        } else {
                puts("read coultn' find file");
        }

	printf("Formula at (2,2) is:%s\n", get_formula(2,2));
	decomp_free();

	char str[] = "\"foo\"";
	get_set(1, 1, str); // NB must enquote strings otherwise it segfault trying to find or make foo as var
	get_set(1, 1, "63.36"); // rounding nasty
	get_set(2, 1, "1 + R[-1]C");

	printf("Test atof(63.36):%f\n", atof("63.36"));

	puts(pr_flt(1163.36, &fxt, FLOAT_PRECISION));
	puts(pr_flt(2688.9, &fxt, FLOAT_PRECISION));


	FreeGlobals();

        puts("finished test");

	//__lsan_do_leak_check();
}

int neot_test0(int argc, char ** argv)
{
        puts("neot test starting");
	//set_headless(true);
        MdiInitialize();
        //PlotInit
        AllocateDatabaseGlobal();
        InitializeGlobals();
        //# parse_command_line # skip for now
        init_basics();

	headless_tests();
        return 0;
}
