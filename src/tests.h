#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//void *main1(void *td);
//char * get_formula(int curow, int cucol);
int swig_read_file_and_run_hooks(char *name, int ismerge);

int neot_test0(int argc, char ** argv);
void headless_tests();
#ifdef __cplusplus
}
#endif
