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
#include "xcept.h"
#include "decompile.h"
#include "sheet.h"
#include "logging.h"
#include "mem.h"



#include "global.h"
#include "cmd.h"
#include "convert.h"
#include "defuns.h"
#include "io-2019.h"
#include "io-term.h"
#include "io-abstract.h"
#include "io-generic.h"
#include "io-utils.h"
#include "io-edit.h"
#include "init.h"
#include "stub.h"
#include "ref.h"
#include "utils.h"

// 2019-02-01 Let's see what we can purge here
#define	ASSERT_UNCALLED() { assert(false); }

using namespace std::literals::string_literals;

char * expand_prompt (char *str);
void expand_prompt(char *str, struct line& line);

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


input_stream_ptr the_input_stream = nullptr;

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
	ASSERT_UNCALLED();
	return nullptr;
}


/* This gets rid of an input stream created by macro_only_input_stream.
 * It fixes the INPUT fields of pending command frames.
 */

	void
pop_input_stream (void)
{
	ASSERT_UNCALLED();
}

/* Macros 
 * These are the commands the user has to interact with macros.
 */

	void
start_entering_macro (void)
{
	ASSERT_UNCALLED();
}

	void
bound_macro (int num)
{
	ASSERT_UNCALLED();
}

	void
run_string_as_macro (const char *macro)
{
	ASSERT_UNCALLED();
}

	void
call_last_kbd_macro (int count)
{
	ASSERT_UNCALLED();
}

/* This command is automaticly inserted into the command stream
 * when the end of a macro is reached.  
 */
	void
end_macro (void)
{
	ASSERT_UNCALLED();
}


/* This command is executed by the user to stop entering a macro.
*/
	void
stop_entering_macro (void)
{
	ASSERT_UNCALLED();
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
	ASSERT_UNCALLED();
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
	ASSERT_UNCALLED();
}


	static void
select_hooks (void)
{
	ASSERT_UNCALLED();
}

/* Block until we get a signal (unless system calls restart), 
 * can do i/o or, until we timeout (timeout is specified in seconds,
 * 0 means block indefinately).  (Front end to select)
 */
	void
block_until_excitement (struct timeval *tv)
{
	ASSERT_UNCALLED();
}

	void
loop_until_char_avail()
{
	ASSERT_UNCALLED();
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
	ASSERT_UNCALLED();
	return '\0';
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
	ASSERT_UNCALLED();
}

/* This is called if an error has been signaled with io_error_msg.
 * It discards any frames that the user has never interacted with
 * and cancels all pending macros.  This is properly followed by 
 * generating an error message for the user and longjmp to error_exception.
 */
	void
recover_from_error (void)
{
	ASSERT_UNCALLED();
}

line_t lineify_expand_prompt(line_t prompt)
{
	ASSERT_UNCALLED();
	line_t result;
	expand_prompt(prompt.buf, result);
	return result;
}

line_t lineify_expand_char(char* prompt)
{
	ASSERT_UNCALLED();
	line_t inp;
	set_line(&inp, prompt);
	return lineify_expand_prompt(inp);
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
	ASSERT_UNCALLED();
	return 1;
}

	void
exit_minibuffer (void)
{
	ASSERT_UNCALLED();
}


	void
setn_arg_text (struct command_arg *arg, const char *text, int len)
{
	ASSERT_UNCALLED();
}

	void
init_arg_text (struct command_arg *arg, const char *text)
{
	ASSERT_UNCALLED();
}

/* This apparently useless alias is here because
 * sometime in the future i want to handle defaults
 * differently.  
 */

	void
set_default_arg (struct command_arg *arg, char *text, int len)
{
	ASSERT_UNCALLED();
}

	int				// new state
prefix_cmd_continuation_loop (bool goto_have_character)
{
	ASSERT_UNCALLED();
}

#define interactive_mode_1 (!rmac || iscmd)

bool turd_1(bool interactive_mode, bool iscmd)
{
	ASSERT_UNCALLED();
	return true;
}


	bool				// return true if we have to jump to new_cycle upon completion 
resume_getting_arguments_loop (bool interactive_mode, bool iscmd)
{
	ASSERT_UNCALLED();
	return false;
}

	void
recompute_numeric_value_of_prefix ()
{
	ASSERT_UNCALLED();
}

	int				// return next state
call_destroy_restart ()
{
	ASSERT_UNCALLED();
	return 0;
}

	int				// return a new state
do_new_cycle ()
{
	ASSERT_UNCALLED();
	return 0;
}

	int				// new state
do_resume_getting_arguments (bool interactive_mode_on, int iscmd)
{
	ASSERT_UNCALLED();
	return 0;
}

	int				// return a new state
do_got_command ()
{
	ASSERT_UNCALLED();
	return 0;
}

	void 
print_state(int state)
{
	ASSERT_UNCALLED();
}

	void
inner_command_loop (int state, int iscmd)
{
	ASSERT_UNCALLED();
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
	ASSERT_UNCALLED();
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
	ASSERT_UNCALLED();
}

	void
execute_command(const char *instr)
{
	ASSERT_UNCALLED();
}

	void
execute_command_str(std::string cmd)
{
	ASSERT_UNCALLED();
}

void execute_command_sv(std::string_view cmd)
{
	ASSERT_UNCALLED();
}



/* Read a character.  If we're in a macro, read from the macro. . . */
	int
get_chr (void)
{
	ASSERT_UNCALLED();
	return 0;
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
	ASSERT_UNCALLED();
}

	void
cmd_io_error_msg (const char *str, ...)
{
	va_list foo;
	char buf[1000];

	io_bell();
	va_start (foo, str);
	vsprintf (buf, str, foo);

	if(use_2019) {
		//std::string msg = string_format(str, 
		io_error_msg2019_str(buf);
	} else {

		char buf2[1020];
		sprintf (buf2, "display-msg %s", buf);
		recover_from_error ();

		if (Global->display_opened)
			execute_command (buf2);
		else
			fprintf (stderr, "oleo: %s\n", buf);

	}

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
	ASSERT_UNCALLED();
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
	ASSERT_UNCALLED();
	return nullptr;

}

void expand_prompt(char *str, struct line& line)
{
	ASSERT_UNCALLED();
}


/* Info commands */




#undef MAX
#define MAX(A,B) (((A) >= (B)) ? (A) : (B))


	void
view_info (char *name, int ignore)
{
	ASSERT_UNCALLED();
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
	ASSERT_UNCALLED();
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

	if(!use_2019) {
		find_func (0, &end_macro_cmd, "end-macro");
		find_func (0, &digit_0_cmd, "digit-0");
		find_func (0, &digit_9_cmd, "digit-9");
		find_func (0, &break_cmd, "break");
		find_func (0, &universal_arg_cmd, "universal-argument");
		create_keymap ("universal", 0);
	}
	push_command_frame (0, 0, 0);
}



	void
init_maps_and_macros()
{
	try {
		init_maps();
		init_named_macro_strings ();
		if(!use_2019) 
			run_init_cmds ();

	} catch (OleoJmp& e) {
		fprintf (stderr, "Error in the builtin init scripts (a bug!).\n");
		io_close_display(69);
		exit (69);
	}
}
