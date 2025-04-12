/*
 * Copyright (c) 1993, 1999, 2000, 2001 Free Software Foundation, Inc.
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

#include <assert.h>

#include "sheet.h"
#include "logging.h"
#include "global.h"
#include "cmd.h"
#include "convert.h"
#include "io-2019.h"
#include "io-term.h"
#include "io-abstract.h"
#include "io-utils.h"
#include "ref.h"
#include "utils.h"

using namespace std::literals::string_literals;


/* Bogus mapping from KEY->CODE to ranges.  This is how bound
 * macros are represented.
 * This is bogus because the ranges will not be adjusted in 
 * the ways they should.  Variables should be used instead. 
 */
int n_bound_macros;
struct rng *bound_macros;
int bound_macro_vec;

/* Flow of control centers around a select loop.  These are the 
 * fd's selected on.
 */
SELECT_TYPE read_fd_set;
SELECT_TYPE exception_fd_set;
SELECT_TYPE write_fd_set;

/* These are fd's returned by the last call to select.  */
SELECT_TYPE read_pending_fd_set;
SELECT_TYPE exception_pending_fd_set;
SELECT_TYPE write_pending_fd_set;

int ioerror = 0;

void set_curow(int nrow)
{
       if(!the_cmd_frame) return; // maybe running headless
       the_cmd_frame->_curow = nrow;
}


void set_cucol(int ncol)
{
       if(!the_cmd_frame) return; // maybe running headless
       the_cmd_frame->_cucol = ncol;
}

