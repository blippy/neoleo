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


typedef void (*alarm_fn)(void);

struct alarm_entry
{
	alarm_fn fn;
	int freq; // in seconds
	time_t last_time;
};


extern struct alarm_entry alarm_table[];


struct command_frame;

typedef void (*direction_function) (int magic, int repeat);


struct command_frame 
{
	struct command_frame * next;
	struct command_frame * prev;

	CELLREF _setrow;
	CELLREF _setcol;

	long	buf1;
	CELLREF _curow;
	long	buf2;
	CELLREF _cucol;
	long	buf3;
	CELLREF _mkrow;
	long	buf4;
	CELLREF _mkcol;
	long	buf5;

	int _window_after_input;
	int _input_active;
	int top_keymap;
	int _cur_keymap;
	int saved_cur_keymap;
	struct cmd_func *_cur_cmd;
	short _cur_vector;
	int _cur_chr;
	int _how_many;
	//int complex_to_user; 
	int _cmd_argc;
	int _cur_arg;
	struct cmd_func * cmd;

};

/* When a command is executing, this points to the frame it should operate
 * on:
 */

extern struct command_frame * the_cmd_frame;
extern struct command_frame * running_frames;

/* For most code, the structure of command loops and input streams
 * is unimportant.  To that code, we make it appear that there is just
 * a set of global variables.
 */

#define setrow			the_cmd_frame->_setrow
#define setcol			the_cmd_frame->_setcol
#define curow			the_cmd_frame->_curow
#define cucol			the_cmd_frame->_cucol
#define mkrow			the_cmd_frame->_mkrow
#define mkcol			the_cmd_frame->_mkcol

#define window_after_input	the_cmd_frame->_window_after_input
#define input_active		the_cmd_frame->_input_active



#ifdef FD_SET

#define SELECT_TYPE fd_set
#define SELECT_SET_SIZE FD_SETSIZE

#else /* no FD_SET */

/* Define the macros to access a single-int bitmap of descriptors.  */
#define SELECT_SET_SIZE 32
#define SELECT_TYPE int
#define FD_SET(n, p) (*(p) |= (1 << (n)))
#define FD_CLR(n, p) (*(p) &= ~(1 << (n)))
#define FD_ISSET(n, p) (*(p) & (1 << (n)))
#define FD_ZERO(p) (*(p) = 0)

#endif /* no FD_SET */

/* The fd's that are selected on in the interact loop. */
extern SELECT_TYPE read_fd_set;
extern SELECT_TYPE exception_fd_set;
extern SELECT_TYPE write_fd_set;
extern SELECT_TYPE read_pending_fd_set; /* These are the output of select. */
extern SELECT_TYPE exception_pending_fd_set;
extern SELECT_TYPE write_pending_fd_set;

#ifdef __STDC__
typedef void (*select_hook_fn) (int fd);
#else
typedef void (*select_hook_fn) ();
#endif

struct select_hook
{
	select_hook_fn hook_fn;
	void * jrandom;
};

extern struct select_hook file_read_hooks[SELECT_SET_SIZE];
extern struct select_hook file_exception_hooks[SELECT_SET_SIZE];
extern struct select_hook file_write_hooks[SELECT_SET_SIZE];


extern void free_input_stream (struct input_stream * stream);
extern void pop_input_stream (void);
extern void start_entering_macro (void);
extern void bound_macro (int num);
void run_string_as_macro(const char * macro);
extern void call_last_kbd_macro (int count);
extern void end_macro (void);
extern void stop_entering_macro (void);
extern int real_get_chr (void);
extern void block_until_excitement(struct timeval *tv);
extern void push_command_frame (struct rng * rng, char * first_line, int len);
extern void remove_cmd_frame (struct command_frame * frame);
extern void free_cmd_frame (struct command_frame * frame);
extern void pop_unfinished_command (void);
extern void recover_from_error (void);
extern void exit_minibuffer (void);
extern void setn_arg_text (struct command_arg * arg, const char * text, int len);
extern void init_arg_text (struct command_arg * arg, const char * text);
extern void set_default_arg (struct command_arg * arg, char * text, int len);
extern void command_loop (int prefix, int iscmd);
extern int get_chr (void);
extern void display_msg (char * msg, int c);
extern void pushback_keystroke (int c);
void cmd_io_error_msg (const char *str,...);
extern void io_info_msg (const char *str,...);
extern char * expand_prompt (char * str);
void expand_prompt (char * str, struct line& line);
extern void set_info (char * name);
extern void page_info_backwards (int rep);
extern void page_info (int rep);
extern void view_info (char * name, int ignore);
void with_keymap (char * mapname);
extern void one_cmd_with_keymap (char * mapname, struct key_sequence * keyseq);

void set_curow(int nrow);
void set_cucol(int nrow);

void rebuild_command_frame();
void init_maps_and_macros();
