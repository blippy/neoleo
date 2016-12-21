#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>

#include "neoleo_swig.h"
#include "io-abstract.h"

#ifdef HAVE_MOTIF
#include "io-motif.h"
#endif

#include "basic.h"
#include "io-headless.h"
#include "io-term.h"
#include "cell.h"
#include "mdi.h"
#include "mysql.h"


void *
main1(void *td)
{
        main0(0, NULL);
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

int // returns 1 => OK
swig_read_file_and_run_hooks(char *name, int ismerge)
{
        FILE *fp = fopen(name, "r");
        if(fp == 0) return 0;
        read_file_and_run_hooks(fp, ismerge, name);
        fclose(fp);
        return 1;

}

int neot_test0(int argc, char ** argv)
{
        puts("neot test starting");
	set_headless(true);
        MdiInitialize();
        //PlotInit
        AllocateDatabaseGlobal();
        InitializeGlobals();
        //# parse_command_line # skip for now
        init_basics();

	//cmd_graphics(); // in leui of calling choose_display
	bool force_cmd_graphics = true;
	choose_display(argc, argv, force_cmd_graphics);
	io_open_display();

        //# the following causes crash:
        int read_status = swig_read_file_and_run_hooks("/home/mcarter/repos/neoleo/examples/pivot.oleo", 0);
        if(read_status == 1) {
                puts("read worked");
        } else {
                puts("read coultn' find file");
        }

	printf("Forumal at (2,2) is:%s\n", get_formula(2,2));


        puts("finished test");
        return 0;
}
