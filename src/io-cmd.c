#include "io-abstract.h"

void 
do_nothing(void)
{
	return;
}

void
cmd_graphics (void)
{
	// not sure if the following are useful:
	//FD_SET (0, &read_fd_set);
	//FD_SET (0, &exception_fd_set);
	
	/* I'm bored by most of this, although it is probably (?) useful
	 */

	/*
	io_command_loop = _io_command_loop;
	io_open_display = _io_open_display;
	io_redisp = _io_redisp;
	*/
	io_repaint = do_nothing;
	/*
	io_repaint_win = _io_repaint_win;
	io_close_display = _io_close_display;
	io_input_avail = _io_input_avail;
	io_scan_for_input = _io_scan_for_input;
	io_wait_for_input = _io_wait_for_input;
	io_read_kbd = _io_read_kbd;
	io_nodelay = _io_nodelay;
	io_getch = _io_getch;
	io_bell = _io_bell;
	io_get_chr = _io_get_chr;
	io_update_status = _io_update_status;
	io_fix_input = _io_fix_input;
	io_move_cursor = _io_move_cursor;
	io_erase = _io_erase;
	io_insert = _io_insert;
	io_over = _io_over;
	io_flush = _io_flush;
	io_clear_input_before = _io_clear_input_before;
	io_clear_input_after = _io_clear_input_after;
	io_pr_cell_win = _io_pr_cell_win;
	io_hide_cell_cursor = _io_hide_cell_cursor;
	io_cellize_cursor = _io_cellize_cursor;
	io_inputize_cursor = _io_inputize_cursor;
	io_display_cell_cursor = _io_display_cell_cursor;
	*/

	//nwin = 1;
}

