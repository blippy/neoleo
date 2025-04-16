#pragma once
/*
 * Copyright (c) 1992, 1993, 2001 Free Software Foundation, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * This file explains the generic interface to interactive functions.
 * This covers how C functions are made available to the user, how
 * keymaps are structured.  This also describes the variables that
 * hold the user's interaction state .
 */
#include "global.h"



inline  CELLREF mkrow = NON_ROW;
inline CELLREF mkcol = NON_COL;
inline int window_after_input = -1;
inline int input_active = 0;

#define SELECT_TYPE fd_set
#define SELECT_SET_SIZE FD_SETSIZE



/* The fd's that are selected on in the interact loop. */
extern SELECT_TYPE read_fd_set;
extern SELECT_TYPE exception_fd_set;
extern SELECT_TYPE write_fd_set;
extern SELECT_TYPE read_pending_fd_set; /* These are the output of select. */
extern SELECT_TYPE exception_pending_fd_set;
extern SELECT_TYPE write_pending_fd_set;

int real_get_chr (void);


void set_curow(int nrow);
void set_cucol(int nrow);
