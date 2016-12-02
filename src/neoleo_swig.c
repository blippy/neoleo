#include <pthread.h>
#include <stdio.h>

#include "neoleo_swig.h"

#ifdef HAVE_MOTIF
#include "io-motif.h"
#endif

#include "io-term.h"
#include "cell.h"

void *main1(void *td)
{
        main0(0, NULL);
}

void start_swig_motif()
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

char * get_formula(int curow, int cucol)
{
        CELL *cp = find_cell(curow, cucol);
        return decomp(curow, cucol, cp);
}
