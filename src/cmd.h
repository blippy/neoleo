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


struct command_frame;

typedef void (*direction_function) (int magic, int repeat);


struct command_frame 
{
	//struct command_frame * next;
	//struct command_frame * prev;

	CELLREF _setrow = NON_ROW;
	CELLREF _setcol = NON_COL;

	//long	buf1;
	CELLREF _curow = MIN_ROW;
	//long	buf2;
	CELLREF _cucol = MIN_COL;
	//long	buf3;
	CELLREF _mkrow = NON_ROW;
	//long	buf4;
	CELLREF _mkcol = NON_COL;
	//long	buf5;

	int _window_after_input = -1;
	int _input_active = 0;
	//int top_keymap;
	//int _cur_keymap;
	//int saved_cur_keymap;
	//struct cmd_func *_cur_cmd;
	//short _cur_vector;
	//int _cur_chr;
	//int _how_many;
	//int _cmd_argc;
	//int _cur_arg;
	//struct cmd_func * cmd;

};

/* When a command is executing, this points to the frame it should operate
 * on:
 */

inline struct command_frame cmd_frame;
inline struct command_frame * the_cmd_frame = &cmd_frame;
//extern struct command_frame * running_frames;

/* For most code, the structure of command loops and input streams
 * is unimportant.  To that code, we make it appear that there is just
 * a set of global variables.
 */

#define curow			the_cmd_frame->_curow
#define cucol			the_cmd_frame->_cucol
#define mkrow			the_cmd_frame->_mkrow
#define mkcol			the_cmd_frame->_mkcol

//#define window_after_input	the_cmd_frame->_window_after_input
#define input_active		the_cmd_frame->_input_active


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
void cmd_io_error_msg (const char *str,...);


void set_curow(int nrow);
void set_cucol(int nrow);

//void rebuild_command_frame();
//void init_maps_and_macros();
