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

void set_exit (int code);
void ploppy_hi( );
int ploppy_life();
int ploppy_max_col();
int ploppy_max_row();
//int ploppy_get_cell(char** result, int r, int c);
char*  ploppy_get_cell(int r, int c);
int ploppy_load_oleo(char* path);
void ploppy_set_cell(int r, int c, char* formula);
void ploppy_save_oleo_as(char* path);
char*  ploppy_get_cell_fmt(int r, int c);
void ploppy_insert_row();
void ploppy_go(int r, int c);
void ploppy_insert_by_col();


