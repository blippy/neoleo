#pragma once
/*
 * tickle.h
 *
 *  Created on: 5 Nov 2025
 *      Author: pi
 */

//#include "neotypes.h"
//#include <cstdint>

//typedef uint16_t CELLREF;

void	ploppy_bind_key(char k, const char* str_to_interpret);
void 	ploppy_hi( );
int 	ploppy_life();
int 	ploppy_max_col();
int 	ploppy_max_row();
//int ploppy_get_cell(char** result, int r, int c);
char*  	ploppy_get_cell(int r, int c);
int 	ploppy_load_oleo(char* path);
void	ploppy_log(char* str);
void 	ploppy_set_cell(int r, int c, char* formula);
void 	ploppy_save_oleo_as(char* path);
char*  	ploppy_get_cell_fmt(int r, int c);
void 	ploppy_insert_row();
void 	ploppy_go(int r, int c);
void 	ploppy_insert_by_col();
void 	ploppy_insert_by_row();
void 	ploppy_print_range (int r_lo, int c_lo, int r_hi, int c_hi);
int 	ploppy_replace_first_form (const char* target, const char* with);
void 	ploppy_set_status (const char* status);
void 	ploppy_test_binding();
void	set_exit (int code);
void	tickle_eval_expr(const std::string& expr);

