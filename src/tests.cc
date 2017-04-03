#include <assert.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <vector>

//#include <obstack.h>
using std::string;
using std::cout;
using std::vector;

// check for leaks, as suggested at 
// http://stackoverflow.com/questions/33201345/leaksanitizer-get-run-time-leak-reports
//#include <sanitizer/lsan_interface.h>


#include "tests.h"
#include "io-abstract.h"
#include "basic.h"
#include "io-headless.h"
#include "io-term.h"
#include "io-utils.h"
#include "cell.h"
#include "mdi.h"
#include "mysql.h"
#include "ref.h"
extern "C" {
#include "parse.h"
}
#include "byte-compile.h"



void *
main1(void *td)
{
        //main0(0, NULL);
        main(0, NULL);
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

void FreeGlobals()
{
	/* TODO - make more accassable - neoleo doesn't seem
	 * to clean up after itself.
	 *
	 * This functionality is far from complete */
	FileCloseCurrentFile();
}

void
set_cell_from_string(int r,int  c, const string & s)
{
	vector<char> v(s.begin(), s.end());
	v.push_back(0);
	char *str = &v[0];

	//char * str = (char *) malloc(s.size()+1);
	//strcpy(str, s.c_str());

	//set_cell(r, c, str); 
	new_value(r, c, str); 
}

void get_set(int r, int c, const string& s)
{
	set_cell_from_string(r, c, s);
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

	default_fmt = FMT_GEN;
	set_cell_from_string(2, 2, "23.3");
        CELL *cp = find_cell(2, 2);
	assert(cp);
	cout << "23.3=" << print_cell(cp) << "=\n";

	if(false) {
		extern char * instr; // used for parsing
		instr = (char *) "\"foo\"";
		yyparse();
	}

	if(true) {
		// this causes leak
		char str1[] =  "\"foo\"";
		parse_and_compile(str1);
	}

	if(false) get_set(1, 1, "1.1+2");
	if(false) get_set(1, 1, "1.1+2.2");
	if(false) get_set(1, 1, "63.36");

	if(false) {
		char s1[] = "/home/mcarter/repos/neoleo/examples/pivot.oleo"; // TODO generalise
	        int read_status = swig_read_file_and_run_hooks(s1, 0);
        	if(read_status == 1) {
                	puts("read worked");
	        } else {
        	        puts("read couldn't find file");
	        }
	}

	if(false) {
		printf("Formula at (2,2) is:%s\n", get_formula(2,2));
		decomp_free();
	}

	if(false){
		char str[] = "\"foo\"";
		// NB must enquote strings otherwise it segfault trying to find or make foo as var
		get_set(1, 1, str); 
		//obstack_free (&tmp_mem, tmp_mem_start); // this doesn't help
		/* causes the following output in sanitiser:
		 * Direct leak of 4 byte(s) in 1 object(s) allocated from:
		 *     #0 0x7f68d0831e40 in __interceptor_malloc /build/gcc/src/gcc/libsanitizer/asan/asan_malloc_linux.cc:62
		 *     #1 0x4acd1e in ck_malloc /home/mcarter/repos/neoleo/src/utils.c:388
		 *     #2 0x4c9851 in yylex /home/mcarter/repos/neoleo/src/parse.y:402
		 *     #3 0x4c4a14 in yyparse /home/mcarter/repos/neoleo/src/parse.c:1422
		 *     #4 0x41933e in parse_and_compile /home/mcarter/repos/neoleo/src/byte-compile.cc:456
		 */
	}

	if(true) get_set(2, 1, "1 + R[-1]C");

	//printf("Test atof(63.36):%f\n", atof("63.36"));

	puts(pr_flt(1163.36DL, &fxt, FLOAT_PRECISION));
	puts(pr_flt(-1163.36DL, &fxt, FLOAT_PRECISION));
	puts(pr_flt(2688.9DL, &fxt, FLOAT_PRECISION));
	puts(pr_flt(3575.06DD, &fxt, FLOAT_PRECISION));


	FreeGlobals();

        puts("finished test");

	//__lsan_do_leak_check();
}

