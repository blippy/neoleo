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

