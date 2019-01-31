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
#include <ctype.h>
#include <errno.h>
#include <memory>
#include <stdarg.h>
#include <termios.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "oleox.h"
#include "decompile.h"
#include "sheet.h"
#include "logging.h"
#include "mem.h"



#include "global.h"
#include "cmd.h"
#include "convert.h"
#include "defuns.h"
#include "io-term.h"
#include "io-abstract.h"
#include "io-generic.h"
#include "io-utils.h"
#include "io-edit.h"
#include "init.h"
#include "stub.h"
#include "ref.h"
#include "key.h"
#include "utils.h"

using namespace std::literals::string_literals;

///// obstack stuff begin
// inherits USE_CMD_OBSTACK (or not) from cmd.h
//#undef USE_OBSTACK
//#define USE_OBSTACK 1
#define obstack_chunk_alloc ck_malloc
#define obstack_chunk_free free
#include "obstack.h"

//static obstack* s_obstack;
void obstack_mc_init(cmd_obstack_t* ptr)
{
#ifdef USE_CMD_OBSTACK
	obstack_init(ptr);
#else
	// I don't think I have to do anything
	//ptr->init();
	
#endif
}

void* obstack_mc_alloc(cmd_obstack_t* ptr, int size)
{
#ifdef USE_CMD_OBSTACK
	return obstack_alloc(ptr, size);
#else
	return ptr->alloc(size);
#endif
}

void obstack_mc_grow(cmd_obstack_t* ptr, const void* data, int size)
{
#ifdef USE_CMD_OBSTACK
	obstack_grow(ptr, data, size);
#else
	strcpy_c data1{(const char*) data};
	ptr->grow(data1.data(), size);
#endif
}

void obstack_mc_1grow(cmd_obstack_t* ptr, char c)
{
#ifdef USE_CMD_OBSTACK
	obstack_1grow(ptr, c);
#else
	ptr->grow1(c);
#endif
}

void* obstack_mc_finish(cmd_obstack_t* ptr)
{
#ifdef USE_CMD_OBSTACK
	return obstack_finish(ptr);
#else
	return ptr->finish();
#endif
}

void obstack_mc_free(cmd_obstack_t* ptr, void* mem_start)
{
#ifdef USE_CMD_OBSTACK
	obstack_free(ptr, mem_start);
#else
	ptr->free_from(mem_start);
#endif
}

///// obstack stuff end

/* mcarter 07-12-2016 the command loop is a right tangled mess, so I am trying to 
 * unwind the disaster using state machines. An implementation of state 
 * machines is shown here:
 * http://stackoverflow.com/questions/1371460/state-machines-tutorials
 */

enum state_codes
{
	sc_start = 1,
	sc_new_cycle,
	sc_got_command,
	sc_prefix_cmd_continuation,
	sc_resume_getting_arguments,
	sc_end
};



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

/* These are fd's returned by the last call to select.
 */
SELECT_TYPE read_pending_fd_set;
SELECT_TYPE exception_pending_fd_set;
SELECT_TYPE write_pending_fd_set;

/* Hooks for asynchronos i/o
 */

struct select_hook file_read_hooks[SELECT_SET_SIZE] = { {0} };
struct select_hook file_exception_hooks[SELECT_SET_SIZE] = { {0} };
struct select_hook file_write_hooks[SELECT_SET_SIZE] = { {0} };

int ioerror = 0;

/* The current stream from which commands are being read. */

/*
input_stream_ptr make_input_stream() 
{ 
	auto ptr = new input_stream();
	obstack_mc_init(&ptr->_macro_stack);
	return ptr;
}
*/

input_stream_ptr the_input_stream = nullptr;
//struct input_stream the_default_input_stream;
//void delete_the_input_stream() { if(the_input_stream) delete the_input_stream; }
//exit_c exit_the_input_stream(delete_the_input_stream);

void debug_input_stream(const char* str, struct input_stream *isp)
{
	if constexpr(false) {
		char str1[50];
		sprintf(str1, "cmd.cc:input_stream :%s:%p", str, isp);
		log_debug(str1);
	}
}
input_stream::input_stream()
{
	debug_input_stream("ctor", this);
	obstack_mc_init(&(this->_macro_stack));
}

input_stream::~input_stream()
{
	debug_input_stream("dtor", this);
}

static input_stream_ptr
default_input_stream (void)
{
	if (!the_input_stream) the_input_stream = new input_stream();
	return the_input_stream;
}


/* This constructs an input stream that reads from a macro but never
 * from a keyboard.  EXECUTE_CMD uses this.
 */


static input_stream_ptr
macro_only_input_stream (struct rng *rng, const char *first_line, int len, struct command_frame *frame)
{
	if constexpr(false) { // for debugging purposes
		char cmd[len+1];
		for(int i = 0; i<len; ++i) cmd[i] = first_line[i];
		cmd[len] = '\0';
		log_debug("macro_only_input_stream: "s + cmd);
	}



	//input_stream_ptr ret = make_input_stream();
	input_stream_ptr ret = new input_stream();
	ret->_rmac = (struct macro *) obstack_mc_alloc(&ret->_macro_stack, sizeof (struct macro));
	ret->_rmac->mac_prev = 0;
	ret->_rmac->mac_rng = *rng;
	ret->_rmac->mac_row = rng->lr;
	ret->_rmac->mac_col = rng->lc;
	obstack_mc_grow(&ret->_macro_stack, first_line, len);
	//obstack_mc_grow(&ret->_macro_stack, "", 1);
	obstack_mc_1grow(&ret->_macro_stack, 0);
	ret->_rmac->mac_start = ret->_rmac->mac_exe = (unsigned char *) obstack_mc_finish (&ret->_macro_stack);
	ret->prev_stream = frame->input;
	{
		input_stream_ptr key = frame->input;
		while (frame->input == key)
		  {
			  frame->input = ret;
			  frame = frame->prev;
		  }
	}
	return ret;
}


/* This gets rid of an input stream created by macro_only_input_stream.
 * It fixes the INPUT fields of pending command frames.
 */

void
pop_input_stream (void)
{
	if(!the_cmd_frame->input->prev_stream) return;

	struct command_frame *fr = the_cmd_frame;
	input_stream_ptr key = the_cmd_frame->input;
	while (fr->input == key)
	{
		fr->input = key->prev_stream;
		fr = fr->prev;
	}

	if(key->_macro_start) free(key->_macro_start);
	if(key->_last_macro) free(key->_last_macro);
	obstack_mc_free (&key->_macro_stack, 0);
	delete key;
}

/* Macros 
 * These are the commands the user has to interact with macros.
 */

void
start_entering_macro (void)
{
	if (making_macro)
	  {
		  io_error_msg ("Can't define two macros at once");
		  return;
	  }
	making_macro_size = 20;
	making_macro = making_macro_start = (unsigned char*)ck_malloc (5 + making_macro_size);
}

void
bound_macro (int num)
{
	struct macro *old;
	CELL *cp;

	cp = find_cell (bound_macros[num].lr, bound_macros[num].lc);
	if (!cp || GET_TYP (cp) != TYP_STR || cp->gString()[0] == '\0')
		return;
	old = rmac;
	rmac = (struct macro *) obstack_mc_alloc (&macro_stack, sizeof (struct macro));
	rmac->mac_prev = old;
	rmac->mac_rng = bound_macros[num];
	rmac->mac_row = bound_macros[num].lr;
	rmac->mac_col = bound_macros[num].lc;
	obstack_mc_grow (&macro_stack, cp->gString(), 1 + strlen (cp->gString()));
	rmac->mac_start = rmac->mac_exe = (unsigned char *) obstack_mc_finish (&macro_stack);
}

void
run_string_as_macro (const char *macro)
{
	struct rng rng;
	/* This is going to continue the command loop
	 * as if some other command had been executed.
	 * That command shouldn't receive the same prefix
	 * arg that provided a repeat count.
	 */
	how_many = 1;
	set_line (&raw_prefix, "");
	rng.lr = rng.hr = rng.lc = rng.hc = MIN_ROW;
	/* Reset the keystate. */
	cur_keymap = the_cmd_frame->top_keymap;
	macro_only_input_stream (&rng, macro, strlen (macro), the_cmd_frame);
	command_loop (1, 0);
}

void
call_last_kbd_macro (int count)
{
	if (!last_macro)
		io_error_msg ("No keyboard macro entered.");
	while (count-- > 0)
		run_string_as_macro ((char *) last_macro);
}

/* This command is automaticly inserted into the command stream
 * when the end of a macro is reached.  
 */
void
end_macro (void)
{
	CELL *cp;
	struct macro *old;

	if (!rmac)
	  {
		  io_error_msg ("Not executing a macro!");
		  return;
	  }
	if ((rmac->mac_row == rmac->mac_rng.hr)
	    && (rmac->mac_col == rmac->mac_rng.hc))
	  {
		  old = rmac->mac_prev;
		  obstack_mc_free(&macro_stack, rmac);
		  rmac = old;
	  }
	else
	  {
		  if (rmac->mac_row == rmac->mac_rng.hr)
		    {
			    rmac->mac_row = rmac->mac_rng.lr;
			    rmac->mac_col++;
		    }
		  else
			  rmac->mac_row++;

		  cp = find_cell (rmac->mac_row, rmac->mac_col);

		  if (!cp || GET_TYP (cp) != TYP_STR
		      || cp->gString()[0] == '\0')
		    {
			    old = rmac->mac_prev;
			    obstack_mc_free (&macro_stack, rmac);
			    rmac = old;
		    }
		  else
		    {
			    obstack_mc_grow(&macro_stack, cp->gString(),
					  1 + strlen (cp->gString()));
			    rmac->mac_exe =
				    (unsigned char *)
				    obstack_mc_finish(&macro_stack);
		    }
	  }
}


/* This command is executed by the user to stop entering a macro.
 */
void
stop_entering_macro (void)
{
	if (!making_macro)
	  {
		  if (rmac)
			  return;
		  io_error_msg ("Not defining a macro!");
		  return;
	  }

	making_macro[0] = '\0';
	making_macro = 0;
	if (last_macro)
		ck_free (last_macro);
	last_macro = making_macro_start;
	making_macro_start = 0;
	free (making_macro_start);
}


/* Scheduling 
 *
 * Scheduling is centered around the function real_get_chr
 * which is allowed to block until an input event has occured.
 * Before blocking, real_get_chr may evaluate cells and/or update 
 * the display.
 */


/* Error messages are delivered to the user by invoking a command 
 * that prompts with the error message, and waits for the user's next
 * keypress.  This command shouldn't wait indefinitely.  After a short time,
 * the error message should disappear.  This is accomplished by counting down
 * a timer, then destorying the error message command frame and throwing an
 * error.  The error is thrown directly rather than with io_error_msg in order
 * to avoid circularity.
 */

static void
error_alarm ()
{
	if (the_cmd_frame->cmd && the_cmd_arg.timeout_seconds)
	  {
		  --the_cmd_arg.timeout_seconds;
		  if (!the_cmd_arg.timeout_seconds)
		    {
			    pop_unfinished_command ();
			    alarm_table[2].freq = 0;
			    //longjmp (Global->error_exception, 1);
			    throw OleoJmp("OleJmp from error_alarm()");
		    }
	  }
	else
		alarm_table[2].freq = 0;
}

struct alarm_entry alarm_table[3] = {
	{cell_alarm, 1, 0},
	{error_alarm, 0, 0},
	{0, 0}
};

/* Function that get called whenever blocking times out. */

static void
alarm_hooks (void)
{
	int x;
	time_t now = time (0);
	for (x = 0; alarm_table[x].fn; ++x)
		if (alarm_table[x].freq
		    && ((now - alarm_table[x].last_time) >=
			alarm_table[x].freq))
		  {
			  alarm_table[x].last_time = now;
			  alarm_table[x].fn ();
		  }
}


static void
select_hooks (void)
{
	int x;
	for (x = 0; x < SELECT_SET_SIZE; ++x)
	  {
		  if (file_read_hooks[x].hook_fn
		      && FD_ISSET (x, &read_pending_fd_set))
			  file_read_hooks[x].hook_fn (x);
		  FD_CLR (x, &read_pending_fd_set);
		  if (file_write_hooks[x].hook_fn
		      && FD_ISSET (x, &write_pending_fd_set))
			  file_write_hooks[x].hook_fn (x);
		  FD_CLR (x, &write_pending_fd_set);
		  if (file_exception_hooks[x].hook_fn
		      && FD_ISSET (x, &exception_pending_fd_set))
			  file_exception_hooks[x].hook_fn (x);
		  FD_CLR (x, &exception_pending_fd_set);
	  }
}

/* Block until we get a signal (unless system calls restart), 
 * can do i/o or, until we timeout (timeout is specified in seconds,
 * 0 means block indefinately).  (Front end to select)
 */
void
block_until_excitement (struct timeval *tv)
{
	int ret;

	bcopy ((char *) &read_fd_set, (char *) &read_pending_fd_set,
	       sizeof (SELECT_TYPE));
	bcopy ((char *) &exception_fd_set,
	       (char *) &exception_pending_fd_set, sizeof (SELECT_TYPE));
	bcopy ((char *) &write_fd_set,
	       (char *) &write_pending_fd_set, sizeof (SELECT_TYPE));
	ret = select (SELECT_SET_SIZE,
		      &read_pending_fd_set, &write_pending_fd_set,
		      &exception_pending_fd_set, tv);
	if (ret < 0)
	  {
		  FD_ZERO (&read_pending_fd_set);
		  FD_ZERO (&write_pending_fd_set);
		  FD_ZERO (&exception_pending_fd_set);
	  }
}

void
loop_until_char_avail()
{
	/* This loop until a character can be read. */
	while (!io_input_avail ())
	{
		alarm_hooks ();
		select_hooks ();
		io_scan_for_input (0);
		if (io_input_avail ())
			break;
		if (Global->auto_recalc && eval_next_cell ())
		{
			if (Global->bkgrnd_recalc)
			{
				int loop = 0;
				while (!io_input_avail ()
						&& eval_next_cell ()
						&& ++loop < 10)
					io_scan_for_input (0);
			}
			else
				while (eval_next_cell ())
					;
			io_scan_for_input (0);
			if (!io_input_avail ())
				io_redisp ();
			io_flush ();
			io_scan_for_input (0);
		}
		else
		{
			io_redisp ();
			io_flush ();
			io_scan_for_input (0);
			if (io_input_avail ())
				break;
			if (!Global->alarm_active)
				block_until_excitement (0);
			else
			{
				struct timeval tv;
				tv.tv_sec =
					Global->alarm_seconds ==
					1 ? 1 : Global->alarm_seconds /
					2;
				tv.tv_usec = 0;
				block_until_excitement (&tv);
			}
		}
	}
}


/*
 * This is the main interact loop.  As quickly as possible
 * it returns a character from the keyboard.  While waiting,
 * it updates cells and the display.  If a macro is being defined,
 * this function save characters in the macro.
 */
int
real_get_chr (void)
{
	int ret;
	unsigned int ch = EOF;	/* The char that will be returned. */

	/* Characters with the meta bit set are returned as
	 * two characters: ESC and a non-meta character.
	 * This buffers the non-meta character between calls.
	 */
	static int saved_char, have_saved_char = 0;

	/* A buffer of characters read in one burst from the kbd. */
	//static unsigned char ibuf[256];
	static char ibuf[256];
	static int i_in;	/* chars buffered */
	static int i_cnt;	/* buffer position */

	alarm_hooks ();
	if (have_saved_char)
	  {
		  ch = saved_char;
		  have_saved_char = 0;
		  goto fini;
	  }

	if (i_cnt)
	  {
		  ch = ibuf[i_cnt++];
		  if (i_cnt == i_in)
			  i_cnt = i_in = 0;
		  goto fini;
	  }

	loop_until_char_avail();

	ret = io_read_kbd (ibuf, sizeof (ibuf));
	if (ret == 1)
	  {
		  ch = ibuf[0];
	  }
	else if (ret > 1)
	  {
		  i_cnt = 1;
		  i_in = ret;
		  ch = ibuf[0];
	  }
	else if (ret == 0 || errno != EINTR)
	  {
		  return EOF;
	  }

      fini:
	if (ch & META_BIT)
	  {
		  switch (ch)
		    {
		    case 229:	/* e */
		    case 228:	/* d */
		    case 246:	/* v */
		    case 197:	/* E */
		    case 196:	/* D */
		    case 214:	/* V */
			    break;
		    default:
			    saved_char = ch;
			    have_saved_char = 1;
			    ch = CTRL_CHAR ('[');
		    }
	  }

	if (making_macro)
	  {
		  /* This is stoopid and should be fixed.
		   * Macros (and other cell strings) should be 
		   * `struct line' and not c-strings.   -tl
		   */
		  if (ch == 0)
		    {
			    *making_macro++ = SPECIAL_CODE_A;
		    }
		  else if (ch == '{')
		    {
			    *making_macro++ = SPECIAL_CODE_B;
		    }
		  else
		    {
			    *making_macro++ = ch;
		    }
		  if (making_macro >=
		      (making_macro_start + making_macro_size))
		    {
			    making_macro_start =
				    (unsigned char *)ck_realloc (making_macro_start,
						5 + making_macro_size * 2);
			    making_macro =
				    (making_macro_start + making_macro_size);
			    making_macro_size *= 2;
		    }
	  }
	return ch;
}

void
OleoLog (const char *fmt, ...)
{
#if 0
	va_list ap;
	FILE *log = fopen ("/tmp/oleolog", "a");
	if (log)
	  {
		  va_start (ap, fmt);
		  vfprintf (log, fmt, ap);
		  va_end (ap);
		  fclose (log);
	  }
#endif
}

/*****************************************************************
 * 
 * Command loops
 *
 * The basic cycle is that the user or a macro selects a function
 * (while oleo updates the display and evaluates cells).
 * A new command_frame is allocated in which to evaluate the selected 
 * function.   Arguments to the function will be stored in this frame.
 * The command loop interprets the FUNC_ARGS string of the selected function
 * and builds an argument list.  If the FUNC_ARGS string specifies that
 * the user must be prompted for an argument, an editting mode is entered
 * and the command loop restarts.  The queue of command_frames form
 * a stack of recursively invoked editting modes.
 *
 * When all of the arguments are ready, the command loop executes
 * the function and discards its frame.
 *
 * In principle, any number of command_frames can be created and they
 * could be evaluated in any order.  It is assumed in the code though that
 * the frame the_cmd_frame->prev is the frame the user was in when
 * the_cmd_frame was created (call it the `parent' frame).  Some arguments, 
 * for example the prefix argument and the current row/col, are taken from the
 * parent frame.  This is because those values may have changed in
 * the_cmd_frame as the user editted arguments to the function being called.
 */

/* The active command frame. This is the head of a queue which is used as a
 * stack. 
 */
struct command_frame *the_cmd_frame = 0;

/* This is a list (next field) of frames that are currently running (their 
 * commands are active on the c stack below the error_exception jump buffer).
 */
struct command_frame *running_frames = 0;


/*
 * This is called when the current frame has keymapped
 * down to some function (stored in the_cmd_frame->_cur_cmd.
 * This pushes a new frame in which the arguments to that
 * command will be stored.
 *
 * This can also be called when the_cmd_frame is 0.  In that case,
 * it will create a top-level frame.
 *
 */

void
push_command_frame (struct rng *rng, char *first_line, int len)
{
	//struct command_frame *new_cf = (struct command_frame *) ck_malloc (sizeof (*new_cf));
	auto new_cf = new struct command_frame;

	new_cf->next = new_cf;
	new_cf->prev = new_cf;

	new_cf->input = (rng ? macro_only_input_stream (rng, first_line, len, new_cf) : default_input_stream ());
	//new_cf->input = (rng ? macro_only_input_stream (rng, first_line, len, new_cf) : &the_default_input_stream);

	new_cf->_setrow = NON_ROW;
	new_cf->_setcol = NON_COL;
	new_cf->_curow = MIN_ROW;
	new_cf->_cucol = MIN_COL;
	new_cf->_mkrow = NON_ROW;
	new_cf->_mkcol = NON_COL;
	new_cf->_input_active = 0;
	new_cf->_window_after_input = -1;

	/* These may be reset later. */
	new_cf->top_keymap = map_id ("main");
	if (new_cf->top_keymap < 0)
		new_cf->top_keymap = map_id ("universal");
	new_cf->saved_cur_keymap = -1;
	new_cf->_cur_keymap = map_id ("main");
	new_cf->_how_many = 1;
	new_cf->_cur_cmd = 0;
	new_cf->_cur_vector = 0;
	new_cf->_cur_chr = the_cmd_frame ? cur_chr : 0;

	//init_line (&new_cf->_raw_prefix);
	new_cf->_cmd_argc = 0;
	new_cf->complex_to_user = 0;

	if (!the_cmd_frame)
	  {
		  /* This is a new top-level frame. */
		  the_cmd_frame = new_cf;
		  new_cf->cmd = 0;
		  new_cf->top_keymap = map_id ("main");
		  if (new_cf->top_keymap < 0)
			  new_cf->top_keymap = map_id ("universal");
	  }
	else if (cur_cmd)
	  {
		  new_cf->_cur_arg = 0;
		  new_cf->cmd = cur_cmd;
		  {
			  int argc = 0;
			  char **prompt = new_cf->cmd->func_args;
			  while (prompt && *prompt)
			    {
				    // other initialisation of cfn taken care of by constructor
				    command_arg_t* cfn = &new_cf->argv[argc];
				    cfn->arg_desc = *prompt;
				    //init_line (&cfn->text);
				    set_line (&cfn->text, "");
				    bzero (&cfn->val, sizeof (union command_arg_val));
				    ++argc;
				    ++prompt;
			    }
			  if (argc && new_cf->argv[0].arg_desc[0] == '+')
				  ++new_cf->argv[0].arg_desc;
			  new_cf->_cmd_argc = argc;
			  new_cf->_curow = curow;
			  new_cf->_cucol = cucol;
			  new_cf->_mkrow = mkrow;
			  new_cf->_mkcol = mkcol;
			  new_cf->_setrow = setrow;
			  new_cf->_setcol = setcol;

			  if (!rng)
				  new_cf->input = the_cmd_frame->input;
		  }
	  }

	new_cf->prev = the_cmd_frame;
	new_cf->next = the_cmd_frame->next;
	new_cf->prev->next = new_cf;
	new_cf->next->prev = new_cf;
	the_cmd_frame = new_cf;
}

/* Remove a frame from the queue/stack. */
void
remove_cmd_frame (struct command_frame *frame)
{
	frame->next->prev = frame->prev;
	frame->prev->next = frame->next;
	if (the_cmd_frame == frame)
		the_cmd_frame = frame->prev;
	if (the_cmd_frame == frame)
	  {
		  the_cmd_frame = 0;
		  push_command_frame (0, 0, 0);
	  }
	frame->next = frame->prev = 0;
}


/*
 * This frees all of the memory allocated to FRAME (including
 * the frame itself. 
 */
void
free_cmd_frame (struct command_frame *frame)
{
	if (frame->next)
		remove_cmd_frame (frame);

	free_line (&frame->_raw_prefix);
	if (frame->cmd) {
		int argc;
		for (argc = 0; argc < frame->_cmd_argc; ++argc) {
			if (frame->argv[argc].is_set && frame->argv[argc].style->destroy)
				frame->argv[argc].style->destroy (&frame->argv[argc]);
			free_line (&frame->argv[argc].text);
			if (frame->argv[argc].expanded_prompt && (frame->argv[argc].expanded_prompt != frame->argv[argc].prompt))
				free (frame->argv[argc].expanded_prompt);
		}
	  }
	delete frame;
}

/*
 * Discard the current frame if it contains an unexecuted commnand. 
 * This is used, for example, to handle break.
 */
void
pop_unfinished_command (void)
{
	if (the_cmd_frame->cmd)
	  {
		  int move_cursor = 0;
		  struct command_frame *frame = the_cmd_frame;
		  if (frame->_curow != frame->prev->_curow
		      || frame->_cucol != frame->prev->_cucol)
		    {
			    io_hide_cell_cursor ();
			    move_cursor = 1;
		    }
		  remove_cmd_frame (frame);
		  if (move_cursor)
			  io_display_cell_cursor ();
		  free_cmd_frame (frame);
	  }
}

/* This is called if an error has been signaled with io_error_msg.
 * It discards any frames that the user has never interacted with
 * and cancels all pending macros.  This is properly followed by 
 * generating an error message for the user and longjmp to error_exception.
 */
void
recover_from_error (void)
{
	if(!the_cmd_frame) return; // maybe running headless

	/* pop input streams until the bottom is reached. */
	while (the_cmd_frame->input->prev_stream)
		pop_input_stream();

	/* cancel the current macros */
	{
		input_stream_ptr stream = the_cmd_frame->input;
		if (stream->_macro_start)
			free (stream->_macro_start);
		if (stream->_last_macro)
			free (stream->_last_macro);
		obstack_mc_free (&stream->_macro_stack, 0);
		obstack_mc_init (&stream->_macro_stack);
		stream->_rmac = 0;
		stream->_func_arg = 0;
		stream->_macro = stream->_macro_start = stream->_last_macro =
			0;
		stream->_macro_size = 0;
		stream->_pushed_back_char = -1;
	}

	/* pop command frames until an interactive one is reached. */
	while (the_cmd_frame->prev != the_cmd_frame
	       && !the_cmd_frame->complex_to_user)
	  {
		  struct command_frame *fr = the_cmd_frame;
		  the_cmd_frame = the_cmd_frame->prev;
		  free_cmd_frame (fr);
	  }

	/* Discard any frames that were executing */
	while (running_frames)
	  {
		  struct command_frame *f = running_frames;
		  running_frames = running_frames->next;
		  f->next = 0;
		  free_cmd_frame (f);
	  }
}

/*
 * When we begin editting a new argument, this function sets up the
 * appropriate keymap, and then resets the state of the editting commands.
 *
 * The return value is 1 if the user must be prompted, 0 otherwise.
 */
static int
get_argument (char *prompt, struct prompt_style *style)
{
	//log_debug("cmd.cc:get_argment() style keymap=" + std::string(style->keymap));
	the_cmd_arg.style = style;
	the_cmd_arg.prompt = prompt;
	if (!the_cmd_arg.expanded_prompt)
		the_cmd_arg.expanded_prompt = expand_prompt (prompt);
	the_cmd_frame->top_keymap = map_id (the_cmd_arg.style->keymap);
	the_cmd_arg.is_set = 0;
	the_cmd_arg.do_prompt = 1;
	if (the_cmd_frame->top_keymap < 0)
		the_cmd_frame->top_keymap = map_id ("universal");
	if (macro_func_arg)
	  {
		  set_line (&the_cmd_arg.text, macro_func_arg);
		  {
			  char *arg_ptr;
			  const char *error;
			  arg_ptr = the_cmd_arg.text.buf;
			  error = the_cmd_arg.style->verify (&arg_ptr,
							     &the_cmd_arg);
			  if (error)
			    {
				    macro_func_arg = 0;
				    io_error_msg ("%s", error);
			    }
			  else
			    {
				    the_cmd_arg.is_set = 1;
				    if (arg_ptr)
					    while (isspace (*arg_ptr))
						    ++arg_ptr;
				    if (arg_ptr && *arg_ptr)
					    macro_func_arg = arg_ptr;
				    else
					    macro_func_arg = 0;
				    return 0;
			    }
		  }
	  }
	input_active = 1;
	begin_edit ();

	/* Functions can come with macros that initialize arguments for the user. 
	 * As for the call to expand_prompt -- hehehehehe
	 */
	if (the_cmd_frame->cmd->init_code
	    && the_cmd_frame->cmd->init_code[cur_arg])
	  {
		  struct line init_code;
		  //init_line(&init_code);
		  expand_prompt (the_cmd_frame-> cmd->init_code[cur_arg], init_code);
		  struct rng rng;
		  rng.lr = rng.hr = rng.lc = rng.hc = 1;
		  macro_only_input_stream (&rng, init_code.buf, strlen (init_code.buf), the_cmd_frame);
		  command_loop (1, 0);
		  free_line(&init_code);
	  }

	return 1;
}

void
exit_minibuffer (void)
{
	if (check_editting_mode ())
		return;
	else
	  {
		  char *extra = the_cmd_arg.text.buf;
		  const char* error =
			  the_cmd_arg.style->verify (&extra, &the_cmd_arg);
		  if (error)
		    {
			    if (*error)
				    io_error_msg ("%s", error);
		    }
		  else
		    {
			    if (extra)
			      {
				      while (isspace (*extra))
					      ++extra;
				      if (*extra)
					      io_error_msg
						      ("%s: extra characters in argument (%s)",
						       the_cmd_frame->
						       cmd->func_name, extra);
			      }
			    the_cmd_arg.is_set = 1;
			    input_active = 0;
			    window_after_input = -1;
			    Global->topclear = 2;
		    }
	  }
}


void
setn_arg_text (struct command_arg *arg, const char *text, int len)
{
	setn_line (&arg->text, text, len);
	arg->cursor = len;
}

void
init_arg_text (struct command_arg *arg, const char *text)
{
	setn_arg_text (arg, text, strlen (text));
}

/* This apparently useless alias is here because
 * sometime in the future i want to handle defaults
 * differently.  
 */

void
set_default_arg (struct command_arg *arg, char *text, int len)
{
	setn_arg_text (arg, text, len);
}

int				// new state
prefix_cmd_continuation_loop (bool goto_have_character)
{
	int ch;			/* The next character to be keymapped. */
	if (goto_have_character)
		goto have_character;
	while (1)
	  {
		  /* Get the next character.
		   * However, if we are in a macro, and the next character
		   * is '{', then the macro contains a function name
		   * and keymapping is circumvented. 
		   */

		get_next_char:

		  if (pushed_back_char >= 0)
		    {
			    ch = pushed_back_char;
			    pushed_back_char = -1;
		    }
		  else if (!rmac)
		    {
			    io_fix_input ();
			    ch = real_get_chr ();
		    }
		  else
		    {
			    int len;
			    unsigned char *ptr;

			  tryagain:
			    alarm_hooks ();
			    ch = *(rmac->mac_exe++);
			    switch (ch)
			      {
			      case '\0':
				      cur_vector = 0;
				      cur_cmd = end_macro_cmd;
				      cur_chr = 0;
				      //goto got_command;
				      return sc_got_command;	// state machine

			      case SPECIAL_CODE_A:
				      ch = '\0';
				      break;

			      case SPECIAL_CODE_B:
				      ch = '{';
				      break;
			      case SPECIAL_CODE_C:
				      ch = '}';
				      break;

			      case '{':
				      for (ptr = rmac->mac_exe;
					   *ptr && *ptr != ' ' && *ptr != '}';
					   ptr++);
				      len = ptr - rmac->mac_exe;
				      for (cur_vector = 0;
					   cur_vector < num_funcs;
					   cur_vector++)
					      for (cur_cmd =
						   &the_funcs[cur_vector][0];
						   cur_cmd->func_name;
						   cur_cmd++)
						      if (!strincmp
							  ((char
							    *)
							   (rmac->mac_exe),
							   cur_cmd->func_name,
							   len)
							  &&
							  cur_cmd->func_name
							  [len] == '\0')
							{
								cur_chr =
									'\0';
								goto out;
							}
				      io_error_msg
					      ("Ignoring unknown function '%.*s' in macro",
					       len, rmac->mac_exe);
				      while (*ptr != '\0' && *ptr != '}')
					      ptr++;
				      if (*ptr == '}')
					      ptr++;
				      rmac->mac_exe = ptr;
				      goto tryagain;

				    out:
				      if (*ptr == ' ')
					{
						/* ... add argument support here ... */
						if (!cur_cmd->func_args)
						  {
							  io_error_msg
								  ("Ignoring extra operand to %s",
								   cur_cmd->func_name);
							  while (*ptr
								 && *ptr !=
								 '}')
								  ptr++;
							  if (*ptr == '}')
								  ptr++;
						  }
						else if (cur_cmd->func_args[0]
							 [0] == '+')
						  {
							  unsigned char *start
								  = ptr;
							  how_many =
								  astol ((char
									  **)
									 (&ptr));
							  setn_line
								  (&raw_prefix,
								   (char *)
								   start,
								   ptr -
								   start);
							  if (*ptr == '}')
								  ptr++;
						  }
						else
						  {
							  while (isspace
								 (*ptr))
								  ++ptr;
							  macro_func_arg =
								  (char *)
								  ptr;
							  while (*ptr
								 && *ptr !=
								 '}')
							    {
								    switch (*ptr)
								      {
								      case SPECIAL_CODE_B:
									      *ptr = '{';
									      break;
								      case SPECIAL_CODE_C:
									      *ptr = '}';
									      break;
								      }
								    ptr++;
							    }
							  if (*ptr == '}')
								  *ptr++ = '\0';
						  }
						rmac->mac_exe = ptr;
					}
				      else
					      rmac->mac_exe += len + 1;
				      //goto got_command;
				      return sc_got_command;	// state machine
			      }
		    }

		  /* When control comes here, adjust the keystate according 
		   * to the cur_keymap and `ch';
		   */
		have_character:
		  /* This is how keymaps are searched for a binding. */
		  while (1)
		    {
			    struct key *key;
			    key = &(the_maps[cur_keymap]->keys[ch]);
#if 0
			    /* Debug keymap processing */
			    fprintf (stderr,
				     "Key %c Keymap %p (%s) code %d next %p\n",
				     ch, cur_keymap, map_names[cur_keymap],
				     key->code,
				     the_maps[cur_keymap]->map_next);
#endif
			    if (key->vector < 0)
			      {
				      if (key->code >= 0)
					{
						cur_keymap = key->code;
						goto get_next_char;
					}
				      else if (the_maps[cur_keymap]->map_next)
					{
						cur_keymap =
							the_maps
							[cur_keymap]->map_next->
							id;
					}
				      else
					{
						cur_vector = 0;
						cur_cmd = 0;
						cur_chr = ch;
						//goto got_command;
						return sc_got_command;	// state machine
					}
			      }
			    else
			      {
				      cur_vector = key->vector;
				      cur_cmd =
					      &(the_funcs[key->vector]
						[key->code]);
				      cur_chr = ch;
				      //goto got_command;
				      return sc_got_command;	// state machine
			      }
		    }
	  }

	assert ("we never reach here" == 0);
}

#define interactive_mode_1 (!rmac || iscmd)

bool turd_1(bool interactive_mode, bool iscmd)
{
	 /* FUNC_ARGS string. To continue this loop, use `goto next_arg;'.
	 *
	 * If user interaction is required, the appropriate keymap,
	 * editing area, etc. is set up, and the command loop resumes
	 * (`goto new_cycle').
	 */
	char *prompt = the_cmd_arg.arg_desc;

	OleoLog ("Prompt [%s]\n", prompt);

	switch (*prompt)
	{	/* Main prompt */
		case 'c':
			{
				int tmp;
				++prompt;
				if (*prompt == '#')
				{
					++prompt;
					the_cmd_arg.val.integer = *prompt;
					the_cmd_arg.is_set = 1;
					the_cmd_arg.do_prompt = 0;
					the_cmd_arg.style = &int_constant_style;
					{
						char c[2];
						c[0] = cur_chr;
						c[1] = '\0';
						init_arg_text (&the_cmd_arg, c);
					}
					goto next_arg;
				}
				else if (*prompt == '\'')
				{
					the_cmd_arg.timeout_seconds
						= 3;
					alarm_table[1].freq =
						1;
					++prompt;
					tmp = get_argument
						(prompt,
						 &char_style);
				}
				else if (*prompt == '!')
				{
					the_cmd_arg.timeout_seconds
						= 3;
					alarm_table[1].freq =
						1;
					++prompt;
					ioerror = 1;
					tmp = get_argument
						(prompt,
						 &char_style);
				}
				else
					tmp = get_argument
						(prompt,
						 &char_style);
				if (tmp)
					//goto new_cycle;
					return true;	// state machine
				goto next_arg;
			}
		case 'C':
			{
				++prompt;
				if (get_argument
						(prompt, &command_style))
					// goto new_cycle;
					return true;	// state machine
				goto next_arg;
			}
		case 'd':
			{
				++prompt;
				if (get_argument
						(prompt, &double_style))
					//goto new_cycle;
					return true;	// state machine
				goto next_arg;
			}
		case 'f':
		       	{
				struct prompt_style *style;
				++prompt;
				char type = *prompt;
				++prompt;
				switch (type)
				{
					case 'r':
						style = &read_file_style;
						break;
					case 'w':
						style = &write_file_style;
						break;
					case 'n':
						style = &file_name_style;
						break;
					default:
						style = 0;	/* shutup gcc -ansi -pendantic -Wall! */
						io_error_msg("func_args bug for %s", the_cmd_frame->cmd->func_name);
				}
				if (get_argument(prompt, style))
					return true;	// state machine
				goto next_arg;
			}
		case 'F':
			{
				++prompt;
				if (get_argument
						(prompt, &format_style))
					//goto new_cycle;
					return true;	// state machine
				goto next_arg;
			}
		case 'k':
			{
				++prompt;
				the_cmd_arg.val.key.cmd.vector =
					-1;
				the_cmd_arg.val.key.cmd.code =
					the_cmd_frame->
					prev->top_keymap;
				the_cmd_arg.val.key.keys =
					&the_cmd_arg.text;
				if (get_argument
						(prompt, &keyseq_style))
					//goto new_cycle;
					return true;	// state machine
				goto next_arg;
			}
		case 'K':
			{
				++prompt;
				if (get_argument
						(prompt, &keymap_style))
					//goto new_cycle;
					return true;	// state machine
				goto next_arg;
			}
		case 'l':
			{
				the_cmd_arg.val.integer =
					cur_chr;
				the_cmd_arg.is_set = 1;
				the_cmd_arg.do_prompt = 0;
				the_cmd_arg.style =
					&int_constant_style;
				{
					char c[2];
					c[0] = cur_chr;
					c[1] = '\0';
					init_arg_text
						(&the_cmd_arg,
						 c);
				}
				goto next_arg;
			}
		case 'm':
			{
				int want_keyseq = 0;
				++prompt;
				want_keyseq = (*prompt == '\'');
				if (want_keyseq)
				{
					char *map;
					++prompt;
					map = expand_prompt
						(prompt);
					the_cmd_arg.val.
						key.cmd.
						vector = -1;
					the_cmd_arg.val.
						key.cmd.code =
						map_id (map);
					the_cmd_arg.val.
						key.keys =
						&the_cmd_arg.text;
				}
				else
				{
					if (mode_style.keymap)
						ck_free (mode_style.keymap);
					mode_style.keymap =
						expand_prompt
						(prompt);
				}
				if (get_argument
						(prompt,
						 (want_keyseq ?
						  &keyseq_style :
						  &mode_style)))
					//goto new_cycle;
					return true;	// state machine
				goto next_arg;
			}
		case 'M':
			if (Global->modified)
			{
				++prompt;
				if (get_argument
						(prompt, &yes_style))
					//goto new_cycle;
					return true;	// state machine
				goto next_arg;
			}
			else
			{
				the_cmd_arg.is_set = 1;
				the_cmd_arg.do_prompt = 1;
				the_cmd_arg.style =
					&yes_style;
				init_arg_text (&the_cmd_arg,
						"yes");
				goto next_arg;
			}
		case 'p':
			{
				++prompt;

				if (*prompt == '?')
				{	/* Command wants to know if prefix provided */
					the_cmd_arg.val.integer = (the_cmd_frame->prev->_raw_prefix.alloc 
							&& the_cmd_frame->prev->_raw_prefix.buf [0]);
					the_cmd_arg.is_set = 1;
					the_cmd_arg.do_prompt = 0;
					the_cmd_arg.style = &int_constant_style;
					init_arg_text (&the_cmd_arg, the_cmd_arg.val.integer ? "1" : "0");
				}
				else
				{
					the_cmd_arg.val.integer = the_cmd_frame->prev->_how_many;
					the_cmd_arg.is_set = 1;
					the_cmd_arg.do_prompt = 0;
					the_cmd_arg.style = &int_constant_style;
					init_arg_text (&the_cmd_arg, long_to_str ((long) the_cmd_arg.val.integer));
				}
				goto next_arg;
			}
		case 'N':
		case 'n':
			{
				long low = 0;
				long high = -1;
				char type = *prompt;
				++prompt;
				if (*prompt == '[')
				{
					++prompt;
					low = astol (&prompt);
					while (isspace
							(*prompt))
						++prompt;
					if (*prompt == ',')
						++prompt;
					high = astol
						(&prompt);
					while (isspace
							(*prompt))
						++prompt;
					if (*prompt == ']')
						++prompt;
				}
				if ((type == 'N') && the_cmd_frame-> prev->_raw_prefix.alloc 
						&& the_cmd_frame-> prev->_raw_prefix.buf[0])
				{
					the_cmd_arg.
						val.integer =
						the_cmd_frame->prev->_how_many;
					the_cmd_arg.is_set =
						1;
					the_cmd_arg.do_prompt
						= 1;
					the_cmd_arg.style =
						&number_style;
					if ((low >= high)
							&&
							((low >
							  the_cmd_arg.
							  val.integer)
							 || (high <
								 the_cmd_arg.val.integer)))
						io_error_msg ("Out of range %d (should be in [%d-%d]).");
					else
						init_arg_text (&the_cmd_arg, long_to_str ((long) the_cmd_arg.val.integer));
				}
				else
				{
					if (get_argument (prompt, &number_style))
						//goto new_cycle;
						return true;	// state machine
				}
				goto next_arg;
			}
			/* This might look slightly tangled, but it makes the command
			 * interface less fickle and more flexible for the user who
			 * wants to macrify a spreadsheet.
			 *
			 * A lowercase 'r' should trigger a prompt for a range in
			 * interactice mode if the mark is not set; if the mark IS set,
			 * then it should grab the range as the implicit answer.  Inside
			 * a macro, 'r' should always expect an explicit argument.
			 * 
			 * The '@' argument should provide a default range with opportunity
			 * for editing (if run interactively), or an implicit argument (if
			 * run non-interactively in a macro).  If the mark is not set, then
			 * the default range or implicit argument is set to the current
			 * cell location.
			 *
			 * If the user really and truly wants interactive behaviour inside a
			 * macro (perhaps because the command is the last in the chain), he
			 * or she can have it by using exec to launch the command.
			 * 
			 * --FB, 1997.12.27
			 */

#define mark_is_set (mkrow != NON_ROW)

		case '@':
		case 'r':
		case 'R':
			{
#if 0
				/*
				 * I don't know what I'm breaking by uncommenting this.
				 * Danny 18/7/2000.
				 *
				 * ... yes, it's all a bit of a mystery, isn't it? mcarter 31-jan-2018
				 */
				if (*prompt != '@' && !mark_is_set)
				{
					/* Default to current cell */
					mkrow = curow;
					mkcol = cucol;
				}
#endif
				if ((*prompt != 'R' && interactive_mode_1 && mark_is_set) || *prompt == '@')
				{
					the_cmd_arg.val.range.lr = std::min(mkrow, curow);
					the_cmd_arg.val.range.hr = std::max(mkrow, curow);
					the_cmd_arg.val.range.lc = std::min(mkcol, cucol);
					the_cmd_arg.val.range.hc = std::max(mkcol, cucol);
					mkrow = NON_ROW;
					mkcol = NON_COL;
					io_update_status ();
					if (*prompt == '@' && interactive_mode_1)
					{
						++prompt;
						if (get_argument (prompt, &range_style))
						{
							init_arg_text
								(&the_cmd_arg,
								 range_name
								 (&the_cmd_arg.val.range));
						}
						return true;	// state machine
					}
					else
					{	/* (Noninteractive mode and @) or r */
						++prompt;
						the_cmd_arg.is_set = 1;
						the_cmd_arg.do_prompt = 1;
						the_cmd_arg.style = &range_style;
						init_arg_text (&the_cmd_arg, range_name (&the_cmd_arg.val.range));
					}
					goto next_arg;
				}
				else
				{	/* R */
					++prompt;
					if (get_argument (prompt, &range_style))
						return true;	// state machine
					goto next_arg;
				}
			}
		case 's':
			{
				{
					++prompt;
					if (get_argument (prompt, &string_style))
						return true;	// state machine
					goto next_arg;
				}
			}
		case 'S':
			{
				{
					++prompt;
					if (*prompt == '\'')
						++prompt;
					if (get_argument (prompt, &symbol_style))
						return true;	// state machine
					goto next_arg;
				}
			}
		case 'V':
			{
				++prompt;
				the_cmd_arg.inc_cmd = io_shift_cell_cursor;
				if (get_argument (prompt, &inc_cmd_style))
					return true;	// state machine
				goto next_arg;
			}
		case 'w':
			{
				{
					++prompt;
					if (*prompt == '\'')
						++prompt;
					if (get_argument (prompt, &word_style))
						return true;	// state machine
					goto next_arg;
				}
			}
		case '#':
			{
				++prompt;

				init_arg_text (&the_cmd_arg, prompt);
				the_cmd_arg.val.integer = astol (&prompt);
				the_cmd_arg.is_set = 1;
				the_cmd_arg.do_prompt = 0;
				the_cmd_arg.style = &int_constant_style;
				goto next_arg;
			}
		case '=':
			{
				++prompt;
				the_cmd_arg.expanded_prompt = expand_prompt (prompt);
				init_arg_text (&the_cmd_arg, the_cmd_arg.expanded_prompt);
				the_cmd_arg.val.string = the_cmd_arg.expanded_prompt;
				the_cmd_arg.is_set = 1;
				the_cmd_arg.do_prompt = 0;
				the_cmd_arg.style =
					&string_style;
				goto next_arg;
			}
		case '.':
			{
				++prompt;
				the_cmd_arg.val.range.lr = curow;
				the_cmd_arg.val.range.lc = cucol;
				if (*prompt == '\'')
				{
					the_cmd_arg.val.range.hr = curow;
					the_cmd_arg.val.range.hc = cucol;
				}
				else
				{
					the_cmd_arg.val.range.hr = mkrow;
					the_cmd_arg.val.range.hc = mkcol;
				}
				the_cmd_arg.is_set = 1;
				the_cmd_arg.do_prompt = 0;
				init_arg_text (&the_cmd_arg, range_name (&the_cmd_arg.val.  range));
				the_cmd_arg.style = &range_constant_style;
				goto next_arg;
			}
		case '[':
			{
				++prompt;
				while (*prompt && (*prompt != ']'))
					if (*prompt != '\\')
						++prompt;
					else {
						++prompt;
						if (*prompt)
							++prompt;
					}
				if (*prompt == ']')
					++prompt;

				if (get_argument (prompt, &menu_style))
					return true;	// state machine
				goto next_arg;
			}
		case '$':
			{
				/* Edit a cell's formula. */
				CELL *cp = find_cell (curow, cucol);
				int do_init;
				++prompt;
				if (*prompt == '\'')
				{
					do_init = 0;
					++prompt;
				}
				else
					do_init = 1;
				if (((!cp || GET_LCK (cp) == LCK_DEF) && (default_lock == LCK_LCK)) 
						|| (cp && GET_LCK (cp) == LCK_LCK))
				{
					io_error_msg ("Cell %s is locked", cell_name (curow, cucol));
					pop_unfinished_command ();
					return true;	// state machine
				}
				the_cmd_frame->prev->_setrow = curow;
				the_cmd_frame->prev->_setcol = cucol;
				if (get_argument (prompt, &formula_style)) {
					if (do_init) {
						int tog = 1;
						if (rmac && !iscmd)
							tog = 0;
						std::string frm = decomp_formula (curow, cucol, cp, tog);
						init_arg_text (&the_cmd_arg, frm.c_str());
					}
					//log_debug("cmd.cc:point a");
					return true;	// state machine
				}
				goto next_arg;
			}

		default:
			{
				io_error_msg ("Interaction-string error!!!");
				pop_unfinished_command ();
				return true;	// state machine
			}
	}
	return true;
next_arg:
	return false;
}


bool				// return true if we have to jump to new_cycle upon completion 
resume_getting_arguments_loop (bool interactive_mode, bool iscmd)
{
	while (cur_arg < cmd_argc) {
		if (the_cmd_arg.is_set)
			goto next_arg;
		else if (the_cmd_arg.prompt) {
			begin_edit ();
			return true;	// state machine
		} else {
			bool stinky = turd_1(interactive_mode, iscmd);
			if(stinky) return true;
		}
next_arg:
		++cur_arg;
	}
	return false;
}

void
recompute_numeric_value_of_prefix ()
{
	//puts("recompute_numeric_value_of_prefix()");
	int x = 0;
	int presumed_digits = 0;
	int sign = 1;

	how_many = 1;
	while (raw_prefix.buf[x])
	  {
		  if (isdigit (raw_prefix.buf[x]))
		    {
			    if (presumed_digits)
				    how_many =
					    how_many * 10 +
					    (raw_prefix.buf[x] - '0');
			    else
			      {
				      presumed_digits = 1;
				      how_many = raw_prefix.buf[x] - '0';
			      }
		    }
		  else if (raw_prefix.buf[x] == '-')
			  sign *= -1;
		  else
		    {
			    if (presumed_digits)
			      {
				      presumed_digits = 0;
				      how_many = 1;
			      }
			    how_many *= 4;
		    }
		  ++x;
	  }
	how_many *= sign;
	io_update_status ();
}

int				// return next state
call_destroy_restart ()
{
	int move_cursor = 0;
	struct command_frame *frame = the_cmd_frame;
	cmd_invoker stub = find_stub ();
	if (frame->_curow != frame->prev->_curow
	    || frame->_cucol != frame->prev->_cucol)
	  {
		  move_cursor = 1;
		  io_hide_cell_cursor ();
	  }
	remove_cmd_frame (frame);

	/* Add frame to the list of frames to be freed on error. */
	frame->next = running_frames;
	running_frames = frame;
	if (move_cursor)
		io_display_cell_cursor ();

	if (!stub)
		io_error_msg ("Don't know how to invoke %s!!!",
			      frame->cmd->func_name);
	else
		stub (frame);

	running_frames = running_frames->next;
	frame->next = 0;
	free_cmd_frame (frame);

	/* If command_loop was called by execute_command, it should
	 * return as soon as there is no more macro to evaluate.
	 */
	if ((!rmac && the_cmd_frame->input->prev_stream) || ioerror)
	  {
		  pop_input_stream();
		  ioerror = 0;
		  return sc_end;	//state machine

	  }
	if (the_cmd_frame->cmd)
		//goto resume_getting_arguments;
		return sc_resume_getting_arguments;	// state machine

	//return sc_resume_getting_arguments;   // state machine
	//return sc_end;
	return sc_new_cycle;
}

int				// return a new state
do_new_cycle ()
{
	// puts("command_loop(): new_cycle");

	if (!the_cmd_frame)
		push_command_frame (0, 0, 0);

	/* Reset the prefix argument. */
	how_many = 1;
	set_line (&raw_prefix, "");
	io_update_status ();

	/* Reset the keystate. */
	cur_keymap = the_cmd_frame->top_keymap;

	return sc_prefix_cmd_continuation;
}

int				// new state
do_resume_getting_arguments (bool interactive_mode_on, int iscmd)
{

	if (resume_getting_arguments_loop (interactive_mode_on, iscmd))
		return sc_new_cycle;
	/* Make sure that all the args are really there. */
	for (cur_arg = 0; cur_arg < cmd_argc; ++cur_arg)
		if (the_cmd_arg.do_prompt && !the_cmd_arg.is_set)
			return sc_resume_getting_arguments;

	/* If this point is reached, call the interactive function,
	 * destroy its frame, and restart the cycle.
	 */
	return call_destroy_restart ();
}

int				// return a new state
do_got_command ()
{
	/* There are some commands that are implemented right here. */
	if (cur_cmd == break_cmd)
	  {
		  io_bell ();
		  if (input_active)
			  pop_unfinished_command ();	/* Abort a complex command. */
		  //goto new_cycle;
		  return sc_new_cycle;	// state machine
	  }

	/* The binding of all keys associated with the prefix arg. */
	if (cur_cmd == universal_arg_cmd)
	  {
		  int prefix_map = map_id ("prefix");
		  /* Make sure the prefix-arg keymap is in place. */
		  if (cur_keymap != prefix_map)
		    {
			    the_cmd_frame->saved_cur_keymap
				    = the_cmd_frame->top_keymap;
			    cur_keymap = prefix_map;
		    }
		  /* Store the last character typed in the raw-prefix. */
		  std::string ch{(char) cur_chr};
		  //catn_line (&raw_prefix, &ch, 1);
		  catn_line (&raw_prefix, ch);
		  /* Recompute the numeric value of the prefix. */
		  recompute_numeric_value_of_prefix ();
		  //goto prefix_cmd_continuation;
		  return sc_prefix_cmd_continuation;
	  }

	/* Make sure we really mapped to a command. */
	if (!cur_cmd || !cur_cmd->func_func)
	  {
		  /* If a character is unmapped in the prefix map,
		   * retry mapping in the last-used normal keymap.
		   */
		  if (the_cmd_frame->saved_cur_keymap >= 0)
		    {
			    cur_keymap = the_cmd_frame->saved_cur_keymap;
			    the_cmd_frame->saved_cur_keymap = -1;
			    //goto have_character;
			    prefix_cmd_continuation_loop (true);
			    return sc_resume_getting_arguments;	// state machine
		    }
		  /* Otherwise, signal an error and start from the top keymap. */
		  io_bell ();
		  //goto new_cycle;
		  return sc_new_cycle;	// state machine
	  }

	/* The next step is to gather the arguments with which to call
	 * the function interactively.
	 */
	/* Whever a new command is encountered, we begin by creating a 
	 * frame in which to store its arguments.
	 * This initializes the new frame on the basis of cur_cmd in 
	 * the_cmd_frame.  
	 */
	push_command_frame (0, 0, 0);

	return sc_resume_getting_arguments;
}

void 
print_state(int state)
{
	//char* state_str = 0;
	char state_str[80];
#define SET_ST_STR(x) strcpy(state_str, x)
	switch(state) {
		case sc_start:
			SET_ST_STR("sc_start\0");
			break;
		case sc_new_cycle:
			SET_ST_STR("sc_new_cycle\0");
			break;
		case sc_prefix_cmd_continuation:
			SET_ST_STR("sc_prefix_cmd_continuation\0");
			break;
		case sc_got_command:
			SET_ST_STR("sc_got_command\0");
			break;
		case sc_resume_getting_arguments:
			SET_ST_STR("sc_resume_getting_arguments\0");
			break;
		case sc_end:
			SET_ST_STR("sc_end\0");
			break;
		default:
			assert(false);
	}

	OleoLog("cmd.c state=%s\n", state_str);



}

void
inner_command_loop (int state, int iscmd)
{

	while (1)
	  {
		  print_state(state);
		  switch (state)
		    {
		    case sc_start:

		    case sc_new_cycle:

			    //new_cycle:

			    /* Some commands are prefix commands: they effect the 
			     * user's state without beginnging a new command cyle.
			     * Those commands return here:
			     */

			    state = do_new_cycle ();
			    assert (state == sc_prefix_cmd_continuation);
			    break;
		    case sc_prefix_cmd_continuation:
			    //prefix_cmd_continuation:
			    /* In this loop, we look for the next command to
			     * execute.  This may involve reading from a macro, 
			     * or the keyboard.  If there is time to kill, updates
			     * and evalutations are done.
			     *
			     * This loop is exited by `goto got_command'.
			     */
			    state = prefix_cmd_continuation_loop (false);
			    assert (state == sc_got_command);
			    break;

			    // fallthrough

		    case sc_got_command:
			    /* Now the next command to begin has been read from a macro
			     * or the keyboard.
			     */
			    //got_command:

			    state = do_got_command ();
			    assert (state == sc_resume_getting_arguments
				    || state == sc_new_cycle
				    || state == sc_prefix_cmd_continuation);
			    break;

			    /* After some other command finishes from underneath a complex command,
			     * flow returns here. 
			     */

			    // fallthrough

		    case sc_resume_getting_arguments:
			    //resume_getting_arguments:
			    state = do_resume_getting_arguments
				    (interactive_mode_1, iscmd);

			    assert (state == sc_new_cycle
				    || state == sc_resume_getting_arguments
				    || state == sc_end);
			    break;
		    case sc_end:
			    return;

		    default:
			    fprintf (stderr,
				     "call_destroy_restart(): unknown state\n");
			    fflush (stderr);
			    abort ();	// should NEVER reach here



		    }
	  }
}

/*
 * This is the main loop of oleo.
 *
 * It reads commands and their arguments, and evaluates them.
 * It (via real_get_chr) udpates the display and performs background recomputation.
 *
 * This function can also be used to evaluate a function without doing any
 * interaction.
 *
 * This is done by pushing a macro_only command frame (see execute_command).
 */

void
command_loop (int prefix, int iscmd)
{

	//puts("command_loop() started");
	int state = sc_start;

	if (the_cmd_frame->cmd)
	  {
		  /* We might be re-entering after a longjmp caused by an error.
		   * In that case, we use an alternate entry point:
		   */
		  //goto resume_getting_arguments;
		  state = sc_resume_getting_arguments;
	  }
	else if (prefix)
	  {

		  /*
		   * Commands (notably execute_command) just tweek the command_frame
		   * state for some other command.  To accomplish this, there is an 
		   * entry point that avoid reinitializing the command_frame.
		   */
		  prefix = 0;
		  //goto prefix_cmd_continuation;
		  state = sc_prefix_cmd_continuation;
	  }

	inner_command_loop (state, iscmd);
}

/* Exectute the command called in `string'.
 * If the string begins with a proper command name,
 * it is executed as if it were embedded in "{}" in 
 * a macro.  Otherwise, if the string can be interpreted 
 * as a range address, the macro at that address is executed.
 */

//static struct line exec_cmd_line = { 0, 0 };
static struct line exec_cmd_line;

/* execute_command buils a macro expression of the from `{command args}'.
 * This function quotes the braces in ARGS so that the macro reader knows
 * they are literal rather than macro syntax.
 */
static void
quote_macro_args (char *args)
{
	while (*args)
	  {
		  switch (*args)
		    {
		    case '{':
			    *args = SPECIAL_CODE_B;
			    break;
		    case '}':
			    *args = SPECIAL_CODE_C;
			    break;
		    }
		  ++args;
	  }
}

void
execute_command(const char *instr)
{
	// kludge to ensure we can pass in a const string
	strcpy_c mem(instr);
	char *str = mem.data();
	int iscmd = 0;
	char *ptr = str;
	const char *run;		/* The first string to execute. */
	/* The address of the macro to execute.  If the user typed a 
	 * command name and not a range name, then this range will be
	 * set to a one cell region.
	 */
	struct rng rng;
	//static struct line exec_buf = { 0, 0 };
	static struct line exec_buf;
	int count = 1;
	ioerror = 0;


	/* Chop off the first word. */
	while (isspace (*str))
		++str;
	if (!*str || *str == '#')
		return;
	for (ptr = str; *ptr && !isspace (*ptr); ptr++);
	if (*ptr)
	  {
		  setn_line (&exec_buf, str, ptr - str + 1);
		  exec_buf.buf[ptr - str] = 0;
		  str = exec_buf.buf;
		  ++ptr;
	  }
	else
		ptr = 0;


	/* First, look for a command name. */
	{
		int vector;
		struct cmd_func *cmd;

		if (!find_function (&vector, &cmd, str, strlen (str)))
		  {
			  if (ptr)
			    {
				    quote_macro_args (ptr);
				    sprint_line (&exec_cmd_line, "{%s %s}",
						 str, ptr);
			    }
			  else
				  sprint_line (&exec_cmd_line, "{%s}", str);
			  run = exec_cmd_line.buf;
			  rng.lr = rng.hr = 1;
			  rng.lc = rng.hc = 1;
			  iscmd = 1;
			  goto found_command;
		  }
	}

	{
		/* Try for a range address. */
		CELL *cp;
		if (get_abs_rng (&str, &rng))
		  {
			  io_error_msg ("Unknown command %s", str);
			  return;
		  }
		if (ptr)
		  {
			  io_error_msg ("Macros can't take arguments");
			  return;
		  }

		cp = find_cell (rng.lr, rng.lc);
		if (!cp || GET_TYP (cp) != TYP_STR || cp->gString()[0] == '\0')
		  {
			  io_error_msg ("No macro found at %s.",
					range_name (&rng));
			  return;
		  }

		run = cp->gString();
		/* Reset the keystate. */
		cur_keymap = the_cmd_frame->top_keymap;
		count = how_many;
		how_many = 1;	/* s.o.p when executing macros; */
		set_line (&raw_prefix, "");	/* see run_string_as_macro for more info */
	}

      found_command:
	while (count-- > 0)
	  {
		  macro_only_input_stream (&rng, run, strlen (run), the_cmd_frame);
		  command_loop (1, iscmd);
	  }
}

void
execute_command_str(std::string cmd)
{
	execute_command((char*) cmd.c_str());
}

void execute_command_sv(std::string_view cmd)
{
	execute_command_str(std::string(cmd));
}



/* Read a character.  If we're in a macro, read from the macro. . . */
int
get_chr (void)
{
	int ch;

	if (rmac)
	  {
		  ch = *(rmac->mac_exe++);
		  switch (ch)
		    {
		    case '{':	/* What else can we do? */
		    case '\0':
			    --rmac->mac_exe;
			    break;

		    case (SPECIAL_CODE_A):
			    ch = 0;
			    break;

		    case (SPECIAL_CODE_B):
			    ch = '{';
			    break;
		    default:
			    break;
		    }
	  }
	else
		ch = real_get_chr ();
	return ch;
}


/* This is an entirely magical function.  All of it's work is done 
 * by the argument prompting system.  All that remains to be done
 * when this is called is to push back a character the user may have
 * typed to cause the error message to go away.
 */

void
display_msg (char *msg, int c)
{
	if (c > 0)
		pushed_back_char = c;
}

void
pushback_keystroke (int c)
{
	if (c > 0)
		pushed_back_char = c;
}

void
cmd_io_error_msg (const char *str, ...)
{
	va_list foo;
	char buf[1000];

	io_bell();
	va_start (foo, str);
	vsprintf (buf, str, foo);

	char buf2[1020];
	sprintf (buf2, "display-msg %s", buf);
	recover_from_error ();

	if (Global->display_opened)
		execute_command (buf2);
	else
		fprintf (stderr, "oleo: %s\n", buf);


	//longjmp (Global->error_exception, 1);
	throw OleoJmp("OleoJmp from io_error_msg()");

}


void
io_info_msg (const char *str, ...)
{
	va_list foo;
	char buf[1000];
	char buf2[1020];

	va_start (foo, str);
	vsprintf (buf, str, foo);
	sprintf (buf2, "display-msg %s", buf);
	execute_command (buf2);
}


// refactoring. Only called by expand_prompt()
void inner_prompt_expansion(char*& str, struct line& expanded)
{
	char *last_pos = str;
	char *src_pos;

	for (src_pos = index (str, '%'); src_pos;
			src_pos = index (src_pos, '%'))
	{
		catn_line (&expanded, last_pos, src_pos - last_pos);
		++src_pos;
		switch (*src_pos)
		{
			case '%':
				catn_line (&expanded, src_pos, 1);
				++src_pos;
				break;
			case 'c':
				{
					struct rng rng;
					char *str;
					rng.lr = rng.hr = the_cmd_frame-> prev->_setrow;
					rng.lc = rng.hc = the_cmd_frame-> prev->_setcol;
					str = range_name (&rng);
					catn_line (&expanded, str);
					++src_pos;
					break;
				}
			case '.':
				{
					struct rng rng;
					char *str;
					rng.lr = rng.hr = the_cmd_frame-> prev->_curow;
					rng.lc = rng.hc = the_cmd_frame-> prev->_cucol;
					str = range_name (&rng);
					catn_line (&expanded, str);
					++src_pos;
					break;
				}
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				{
					int argn = *src_pos - '0';
					if ((cmd_argc > argn) && the_cmd_frame-> argv[argn].is_set 
							&& the_cmd_frame-> argv[argn].text.buf)
						catn_line (&expanded, the_cmd_frame->argv[argn].text.buf);
					else
						catn_line (&expanded, "????");
					++src_pos;
					break;
				}
			default:
				catn_line (&expanded, "%");
				break;
		}
		last_pos = src_pos;
	}
	catn_line (&expanded, last_pos);
}

/* Expands a string that will be used to prompt for an argument.
 *    %n expands to the text of argument n (if defined -- ??? otherwise).
 *    %% expands to %
 *    %c expands to the name of the_cmd_frame->prev->_set{row,col}
 * If no expansion is needed, the argument is returned.  Otherwise,
 * malloced memory is returned.
 */

char *
expand_prompt (char *str)
{
	if (!str || !index (str, '%')) 
		return str;

	struct line expanded;
	//init_line (&expanded);
	inner_prompt_expansion(str, expanded);
	return expanded.buf;
	
}

void expand_prompt(char *str, struct line& line)
{
	if (!str || !index (str, '%')) {
		set_line(&line, str);
	} else {
		inner_prompt_expansion(str, line);
	}
}


/* Info commands */




#undef MAX
#define MAX(A,B) (((A) >= (B)) ? (A) : (B))


void
view_info (char *name, int ignore)
{
}

/* The C part of this function is uninteresting.  The interesting part
 * is in defun.h.
 */

void
with_keymap (char *mapname)
{
}

void
one_cmd_with_keymap (char *mapname, struct key_sequence *keyseq)
{
	if (keyseq->cmd.vector < 0 && keyseq->cmd.code < 0)
		io_bell ();
	else if (keyseq->cmd.vector < 0)
		io_error_msg
			("one-command-with-keymap: %s maps to a keymap (%s), not a command.",
			 keyseq->keys->buf, map_names[keyseq->cmd.code]);
	else
		execute_command
			(the_funcs[keyseq->cmd.vector]
			 [keyseq->cmd.code].func_name);
}

void 
set_curow(int nrow)
{
	if(!the_cmd_frame) return; // maybe running headless
	the_cmd_frame->_curow = nrow;
}

void
set_cucol(int ncol)
{
	if(!the_cmd_frame) return; // maybe running headless
	the_cmd_frame->_cucol = ncol;
}



void rebuild_command_frame()
{
	/* Force the command frame to be rebuilt now that the keymaps exist. */
	struct command_frame * last_of_the_old = the_cmd_frame->next;
	while (the_cmd_frame != last_of_the_old)
		free_cmd_frame (the_cmd_frame);
	free_cmd_frame (last_of_the_old);
}
static void
init_maps (void)
{
  num_maps = 0;
  the_maps = 0;
  map_names = 0;
  map_prompts = 0;

  the_funcs = (cmd_func**) ck_malloc (sizeof (struct cmd_func *) * 2);
  num_funcs = 1;
  the_funcs[0] = (cmd_func *) get_cmd_funcs();

  find_func (0, &end_macro_cmd, "end-macro");
  find_func (0, &digit_0_cmd, "digit-0");
  find_func (0, &digit_9_cmd, "digit-9");
  find_func (0, &break_cmd, "break");
  find_func (0, &universal_arg_cmd, "universal-argument");

  create_keymap ("universal", 0);
  push_command_frame (0, 0, 0);
}



void
init_maps_and_macros()
{
	try {
		init_maps();
		init_named_macro_strings ();
                run_init_cmds ();
	} catch (OleoJmp& e) {
		fprintf (stderr, "Error in the builtin init scripts (a bug!).\n");
                io_close_display(69);
                exit (69);
	}
}
