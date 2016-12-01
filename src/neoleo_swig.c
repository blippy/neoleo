#include <pthread.h>
#include <stdio.h>

#include "neoleo_swig.h"

#include "io-motif.h"
#include "io-term.h"
#include "cell.h"

void *main1(void *td)
{
        main0(0, NULL);
}

void start_swig_motif()
{
        pthread_t tid;
        //motif_main_loop();
        int error = pthread_create(&tid, NULL, main1, NULL);
        if(error != 0) 
                fprintf(stderr, "Couldn't run motif thread\n");

}

char * get_formula(int curow, int cucol)
{
        CELL *cp = find_cell(curow, cucol);
        return decomp(curow, cucol, cp);
}
