#pragma once
/*
 * tickle.h
 *
 * 26/3 It seems actually more reliable to use strd::string instead of char*
 *
 *  Created on: 5 Nov 2025
 *      Author: pi
 *
 *
 */

//#include "neotypes.h"
//#include <cstdint>

//typedef uint16_t CELLREF;


#include <string>

void		ploppy_bind_key(char k, const std::string&  str_to_interpret);
void		ploppy_clear_sheet();
void 		ploppy_display_curses();
int 		ploppy_get_col_num();
int			ploppy_get_row_num();
void 		ploppy_hi( );
int 		ploppy_life();
int 		ploppy_max_col();
int 		ploppy_max_row();
std::string	ploppy_get_cell(int r, int c);
std::string	ploppy_get_cell_fmt(int r, int c);
void 		ploppy_go(int r, int c);
void 		ploppy_insert_row();
void 		ploppy_insert_by_col();
void 		ploppy_insert_by_row();
std::string	ploppy_invoke_std_form(const std::string& desc, const std::string& text_field);
int 		ploppy_load_oleo(char* path);
void		ploppy_log(char* str);
void 		ploppy_print_range (int r_lo, int c_lo, int r_hi, int c_hi);
void		ploppy_definitely_quit();
int 		ploppy_replace_first_form (const char* target, const char* with);
void 		ploppy_save_oleo_as(char* path);
void 		ploppy_set_cell(int r, int c, const std::string& formula );
void		ploppy_set_col_prec(int col, int prec);
void 		ploppy_set_status (const char* status);
void 		ploppy_test_binding();

void		set_exit (int code);
void		tickle_eval_expr(const std::string& expr);

