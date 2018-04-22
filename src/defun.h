/*
 * $Id: defun.h,v 1.7 2000/08/10 21:02:50 danny Exp $
 *
 * Copyright (c) 1993, 2000 Free Software Foundation, Inc.
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
 * This file is include multiple times in defuns.h to do all kinds of
 * crazy things.
 *
 * Lousy design :-(
 *
 * There is no need to wrap the function in `to_vptr()', as this
 * is actually taken care of in the macro definitions in defuns.cc
 * now.
 */

/* Commands to Stop and to Suspend the Beast */

DEFUN ("kill-oleo", FDkill_oleo, FAkill_oleo, to_vptr(kill_oleo))
#ifdef FUNC_ARG_STRINGS
"MSpreadsheet modified.  Quit without saving? (yes or no) ",
#endif

DEFUN_3 ("suspend-oleo", FDsuspend_oleo, to_vptr(suspend_oleo))

DEFUN ("show-menu", FDshow_main_menu, FAshow_main_menu, to_vptr(show_main_menu))
#ifdef FUNC_ARG_STRINGS
"+#0",
"p",
#endif


DEFUN_3("test-curses-suspension", FDtest_curses_suspension, to_vptr(test_curses_suspension))

/* Spreadsheet Cursor Motion Commands and Cell Mark */

  /* By convention, motion and scrolling commands take a magic number
   * as their first argument.  The number tells which direction to move.
   * 0 == up, 1 == down, etc.
   */

DEFUN ("up-cell", FDup_cell, FAup_cell, to_vptr(shift_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#0",
"p",
#endif

DEFUN ("down-cell", FDdown_cell, FAdown_cell, to_vptr(shift_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#1",
"p",
#endif

DEFUN ("right-cell", FDright_cell, FAright_cell, to_vptr(shift_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#2",
"p",
#endif

DEFUN ("left-cell", FDleft_cell, FAleft_cell, to_vptr(shift_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#3",
"p",
#endif
#ifndef fnord
#define fnord	0
#endif

DEFUN ("upright-cell", FDupright_cell, FAupright_cell, to_vptr(shift_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#4",
"p",
#endif

DEFUN ("upleft-cell", FDupleft_cell, FAupleft_cell, to_vptr(shift_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#5",
"p",
#endif

DEFUN ("downright-cell", FDdownright_cell, FAdownright_cell, to_vptr(shift_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#6",
"p",
#endif

DEFUN ("downleft-cell", FDdownleft_cell, FAdownleft_cell, to_vptr(shift_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#7",
"p",
#endif

DEFUN ("goto-cell", FDgoto_cell, FAgoto_cell, to_vptr(goto_cell))
#ifdef FUNC_ARG_STRINGS
"RGoto cell? ",
#endif

DEFUN ("goto-region", FDgoto_region, FAgoto_region, to_vptr(goto_region))
#ifdef FUNC_ARG_STRINGS
"RGoto region? ",
#endif

DEFUN_3 ("upper-left", FDupper_left, to_vptr(upper_left))

DEFUN_3 ("lower-left", FDlower_left, to_vptr(lower_left))

DEFUN_3 ("upper-right", FDupper_right, to_vptr(upper_right))

DEFUN_3 ("lower-right", FDlower_right, to_vptr(lower_right))

DEFUN ("exchange-point-and-mark", FDexchange_point_and_mark,
       FAexchange_point_and_mark, to_vptr(exchange_point_and_mark))
#ifdef FUNC_ARG_STRINGS
"p?",
#endif

DEFUN ("mark-cell", FDmark_cell_cmd, FAmark_cell_cmd, to_vptr(mark_cell_cmd))
#ifdef FUNC_ARG_STRINGS
"p?",
#endif

DEFUN_3 ("clear-mark", FDclear_mark, unmark_cmd)

DEFUN ("save-mark-to-cell", FDsave_mark_to_cell,
	FAsave_mark_to_cell, 
	to_vptr(save_mark_to_cell))
#ifdef FUNC_ARG_STRINGS
"RSave mark to cell? ",
#endif

DEFUN ("save-point-to-cell", FDsave_point_to_cell,
	FAsave_point_to_cell, 
	to_vptr(save_point_to_cell))
#ifdef FUNC_ARG_STRINGS
"RSave point to cell? ",
#endif

DEFUN ("scroll-up", FDscroll_up, FAscroll_up, to_vptr(scroll_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#0",
"p",
#endif

DEFUN ("scroll-down", FDscroll_down, FAscroll_down, to_vptr(scroll_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#1",
"p",
#endif

DEFUN ("scroll-right", FDscroll_right, FAscroll_right, to_vptr(scroll_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#2",
"p",
#endif

DEFUN ("scroll-left", FDscroll_left, FAscroll_left, to_vptr(scroll_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#3",
"p",
#endif

DEFUN ("scroll-upright", FDscroll_upright, FAscroll_upright,
       to_vptr(scroll_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#4",
"p",
#endif
#ifdef DOC_STRINGS
"Scroll the current window upright ARG row/cols; or a full screen if no ARG.",
#endif

DEFUN ("scroll-upleft", FDscroll_upleft, FAscroll_upleft, to_vptr(scroll_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#5",
"p",
#endif
#ifdef DOC_STRINGS
"Scroll the current window upleft ARG row/cols; or full screen if no ARG.",
#endif

DEFUN ("scroll-downright", FDscroll_downright, FAscroll_downright,
       to_vptr(scroll_cell_cursor)) 
#ifdef FUNC_ARG_STRINGS
"+#6",
"p",
#endif
#ifdef DOC_STRINGS
"Scroll the current window downright ARG row/cols; or a full screen w/no ARG.",
#endif

DEFUN ("scroll-downleft", FDscroll_downleft, FAscroll_downleft,
       to_vptr(scroll_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#7",
"p",
#endif
#ifdef DOC_STRINGS
"Scroll the current window downleft ARG row/cols; or a full screen if no ARG.",
#endif

DEFUN ("scan-up", FDscan_up, FAscan_up, to_vptr(scan_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#0",
"p",
#endif
#ifdef DOC_STRINGS
"Move up to the next border between filled and empty cells.",
"With an argument, moves to the Nth border.",
#endif

DEFUN ("scan-down", FDscan_down, FAscan_down, to_vptr(scan_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#1",
"p",
#endif
#ifdef DOC_STRINGS
"Move down to the next border between filled and empty cells.",
"With an argument, moves to the Nth border.",
#endif

DEFUN ("scan-right", FDscan_right, FAscan_right, to_vptr(scan_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#2",
"p",
#endif
#ifdef DOC_STRINGS
"Move right to the next border between filled and empty cells.",
"With an argument, moves to the Nth border.",
#endif

DEFUN ("scan-left", FDscan_left, FAscan_left, to_vptr(scan_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#3",
"p",
#endif
#ifdef DOC_STRINGS
"Move left to the next border between filled and empty cells.",
"With an argument, moves to the Nth border.",
#endif

DEFUN ("beginning-of-row",
       FDbeginning_of_row, FAbeginning_of_row, to_vptr(beginning_of_row))
#ifdef FUNC_ARG_STRINGS
"p",
#endif
#ifdef DOC_STRINGS
"Move point to beginning of the current row.",
"With a prefix argument, move forward ARG - 1 rows first.",
#endif

DEFUN ("beginning-of-col",
       FDbeginning_of_col, FAbeginning_of_col, to_vptr(beginning_of_col))
#ifdef FUNC_ARG_STRINGS
"p",
#endif
#ifdef DOC_STRINGS
"Move point to beginning of the current col.",
"With a prefix argument, move forward ARG - 1 cols first.",
#endif

DEFUN ("end-of-row", FDend_of_row, FAend_of_row, to_vptr(end_of_row))
#ifdef FUNC_ARG_STRINGS
"p",
#endif
#ifdef DOC_STRINGS
"Move point to end of the current row.",
"With a prefix argument, move forward ARG - 1 rows first.",
#endif

DEFUN ("end-of-col", FDend_of_col, FAend_of_col, to_vptr(end_of_col))
#ifdef FUNC_ARG_STRINGS
"p",
#endif
#ifdef DOC_STRINGS
"Move point to end of the current col.",
"With a prefix argument, move forward ARG - 1 cols first.",
#endif

DEFUN_3 ("recenter-window", FDrecenter_window, to_vptr(recenter_window))
#ifdef DOC_STRINGS
"Center the cell cursor in the current window.",
#endif

DEFUN_3 ("redraw-screen", FDredraw_screen, to_vptr(redraw_screen))
#ifdef DOC_STRINGS
"Center the cell cursor in the current window and redisplay the screen.",
#endif

/* Commands for Editing from the Sheet */

DEFUN ("edit-cell", FDedit_cell, FAedit_cell, to_vptr(edit_cell))
#ifdef FUNC_ARG_STRINGS
"$Set %c to? ", 
#endif
#ifdef DOC_STRINGS
"Assign FORMULA to the current cell.",
#endif

DEFUN ("set-cell", FDset_cell, FAset_cell, to_vptr(edit_cell))
#ifdef FUNC_ARG_STRINGS
"$'Set %c to? ", 
#endif
#ifdef DOC_STRINGS
"Assign FORMULA to the current cell.",
#endif

DEFUN ("goto-edit-cell",
       FDgoto_edit_cell, FAgoto_edit_cell, to_vptr(goto_edit_cell))
#ifdef FUNC_ARG_STRINGS
"l",
#endif
#ifdef DOC_STRINGS
"Begin to edit the current cell's formula.",
"The last character used to invoke this command is reexecuted as a",
"formula editing command.",
#endif

DEFUN ("goto-set-cell",
       FDgoto_set_cell, FAgoto_set_cell, to_vptr(goto_set_cell))
#ifdef FUNC_ARG_STRINGS
"l",
#endif
#ifdef DOC_STRINGS
"Begin to edit the current cell, ignoring its content if any.",
"The last character used to invoke this command is reexecuted as a",
"formula editing command.",
#endif

DEFUN ("set-region-formula",
       FDset_region_formula, FAset_region_formula, to_vptr(set_region_formula))
#ifdef FUNC_ARG_STRINGS
"rSet region? ",
"$'Set region %0 to? ",
#endif
#ifdef DOC_STRINGS
"Change the definition of all cells in RANGE to FORMULA.",
#endif

/* Commands for Moving Cursor and Editing in Input Area */

DEFUN ("toggle-overwrite", FDtoggle_overwrite, FAtoggle_overwrite, to_vptr(toggle_overwrite))
#ifdef FUNC_ARG_STRINGS
"p?",
"p",
#endif
#ifdef DOC_STRINGS
"Toggle between insert and overwrite mode.",
"This command only applies when an argument is being edited.",
"",
"With a positive (non-0) prefix argument, turn overwrite mode on.",
"With a negative or 0 prefix argument, turn overwrite mode off.",
"With no prefix argument, toggle overwrite mode.",
#endif

DEFUN_3 ("beginning-of-line", FDbeginning_of_line, to_vptr(beginning_of_line))
#ifdef DOC_STRINGS
"Move point to beginning of input line.",
#endif

DEFUN_3 ("end-of-line", FDend_of_line, to_vptr(end_of_line))
#ifdef DOC_STRINGS
"Move point to end of input line.",
#endif

DEFUN ("backward-char", FDbackward_char, FAbackward_char, to_vptr(backward_char))
#ifdef FUNC_ARG_STRINGS
"p",
#endif
#ifdef DOC_STRINGS
"Move point left ARG characters (right if ARG negative).",
"On attempt to pass beginning or end of buffer, report an error.",
#endif

DEFUN ("backward-word", FDbackward_word, FAbackward_word, to_vptr(backward_word))
#ifdef FUNC_ARG_STRINGS
"p",
#endif
#ifdef DOC_STRINGS
"Move backward in the input area until encountering the end of a word.",
"With argument, do this that many times.",
#endif


DEFUN ("backward-delete-char", FDbackward_delete_char, FAbackward_delete_char, to_vptr(backward_delete_char))
#ifdef FUNC_ARG_STRINGS
"p",
#endif
#ifdef DOC_STRINGS
"Delete ARG characters backward.",
#endif

DEFUN ("backward-delete-word", FDbackward_delete_word, FAbackward_delete_word, to_vptr(backward_delete_word))
#ifdef FUNC_ARG_STRINGS
"p",
#endif
#ifdef DOC_STRINGS
"Kill characters backward until encountering the end of a word.",
"With prefix argument, do this that many times.",
#endif

DEFUN_3 ("delete-to-start", FDdelete_to_start, to_vptr(delete_to_start))
#ifdef DOC_STRINGS
"Delete the input line, upto the cursor position.",
#endif

DEFUN ("forward-char", FDforward_char, FAforward_char, to_vptr(forward_char))
#ifdef FUNC_ARG_STRINGS
"p",
#endif
#ifdef DOC_STRINGS
"Move the input area cursor right ARG characters (left if ARG negative).",
"On reaching end of buffer, report an error.",
#endif

DEFUN ("forward-word", FDforward_word, FAforward_word, to_vptr(forward_word))
#ifdef FUNC_ARG_STRINGS
"p",
#endif
#ifdef DOC_STRINGS
"Move the input area cursor forward ARG words (backward if ARG is negative).",
#endif

DEFUN ("delete-char", FDdelete_char, FAdelete_char, to_vptr(delete_char))
#ifdef FUNC_ARG_STRINGS
"p",
#endif
#ifdef DOC_STRINGS
"Delete ARG following characters in the input area.",
"Deletes backwards with negative arg.",
#endif

DEFUN ("delete-word", FDdelete_word, FAdelete_word, to_vptr(delete_word))
#ifdef FUNC_ARG_STRINGS
"p",
#endif
#ifdef DOC_STRINGS
"Kill characters forward until encountering the end of a word.",
"With argument, do this that many times.",
#endif

DEFUN_3 ("kill-line", FDkill_line, to_vptr(kill_line))
#ifdef DOC_STRINGS
"Delete the rest of the input line, beginning at the cursor position.",
#endif

/* Commands for Moving Between Cells from Input Area */

DEFUN ("auto-move-up", FDauto_move_up, FAauto_move_up,
       set_auto_direction)
#ifdef FUNC_ARG_STRINGS
"#0",
#endif
#ifdef DOC_STRINGS
"Set the auto-move direction to `up'.",
"This is the direction that will be used by the next-edit command.",
#endif

DEFUN ("auto-move-down", FDauto_move_down, FAauto_move_down, to_vptr(set_auto_direction))
#ifdef FUNC_ARG_STRINGS
"#1",
#endif
#ifdef DOC_STRINGS
"Set the auto-move direction to `down'.",
"This is the direction that will be used by the next-edit command.",
#endif

DEFUN ("auto-move-left", FDauto_move_left, FAauto_move_left,
       to_vptr(set_auto_direction))
#ifdef FUNC_ARG_STRINGS
"#3",
#endif
#ifdef DOC_STRINGS
"Set the auto-move direction to `left'.",
"This is the direction that will be used by the next-edit command.",
#endif


DEFUN ("auto-move-right", FDauto_move_right, FAauto_move_right,
       to_vptr(set_auto_direction))
#ifdef FUNC_ARG_STRINGS
"#2",
#endif
#ifdef DOC_STRINGS
"Set the auto-move direction to `right'.",
"This is the direction that will be used by the next-edit command.",
#endif

DEFUN ("auto-move-up-left", FDauto_move_up_left, FAauto_move_up_left,
       to_vptr(set_auto_direction))
#ifdef FUNC_ARG_STRINGS
"#5",
#endif
#ifdef DOC_STRINGS
"Set the auto-move direction to `up left'.",
"This is the direction that will be used by the next-edit command.",
#endif

DEFUN ("auto-move-up-right", FDauto_move_up_right, FAauto_move_up_right,
       to_vptr(set_auto_direction))
#ifdef FUNC_ARG_STRINGS
"#4",
#endif
#ifdef DOC_STRINGS
"Set the auto-move direction to `up right'.",
"This is the direction that will be used by the next-edit command.",
#endif

DEFUN ("auto-move-down-left", FDauto_move_down_left, FAauto_move_down_left,
       to_vptr(set_auto_direction))
#ifdef FUNC_ARG_STRINGS
"#7",
#endif
#ifdef DOC_STRINGS
"Set the auto-move direction to `down left'.",
"This is the direction that will be used by the next-edit command.",
#endif

DEFUN ("auto-move-down-right", FDauto_move_down_right, FAauto_move_down_right,
       to_vptr(set_auto_direction))
#ifdef FUNC_ARG_STRINGS
"#6",
#endif
#ifdef DOC_STRINGS
"Set the auto-move direction to `down right'.",
"This is the direction that will be used by the next-edit command.",
#endif

DEFUN ("auto-move-no-motion", FDauto_move_no_motion, FAauto_move_no_motion,
       to_vptr(set_auto_direction))
#ifdef FUNC_ARG_STRINGS
"#8",
#endif
#ifdef DOC_STRINGS
"Set the auto-move direction to `no motion'.",
"This is the direction that will be used by the next-edit command.",
#endif

DEFUN_3 ("auto-move", FDauto_move, to_vptr(auto_move))
#ifdef DOC_STRINGS
"Move one cell in the automatic motion direction.",
"The automatic motion direction is set using one of the commands: ",
"	[auto-move-up]  auto-move-up",
"	[auto-move-down]  auto-move-down",
"	[auto-move-left]  auto-move-left",
"	[auto-move-right]  auto-move-right",
"	[auto-move-up-left]  auto-move-up-left",
"	[auto-move-up-right]  auto-move-up-right",
"	[auto-move-down-left]  auto-move-down-left",
"	[auto-move-down-right]  auto-move-down-right",
#endif

DEFUN_3 ("auto-next-set", FDauto_next_set, to_vptr(auto_next_set))
#ifdef DOC_STRINGS
"Scan in the opposite direction of automatic motion, then move one cell.",
"The direction of the one-cell motion is down for left or right",
"automatic motion, and to the right otherwise.  See also, `auto-move'.",
#endif

DEFUN ("next-edit", FDnext_edit, FAnext_edit, to_vptr(run_string_as_macro))
#ifdef FUNC_ARG_STRINGS
"={exit-minibuffer}{auto-move}{edit-cell}",
#endif
#ifdef DOC_STRINGS
"Equivalent to: {exit-minibuffer}{auto-move}{edit-cell}",
"This command is used to terminate an edit-cell command, move to another",
"cell, and begin a second edit-cell command.  See also `auto-move'.",
#endif

DEFUN ("next-edit-set", FDnext_edit_set, FAnext_edit_set, to_vptr(run_string_as_macro))
#ifdef FUNC_ARG_STRINGS
"={exit-minibuffer}{auto-next-set}{edit-cell}",
#endif
#ifdef DOC_STRINGS
"Equivalent to: {exit-minibuffer}{auto-next-set}{edit-cell}",
"This command is used to terminate an edit-cell command, move to another",
"cell, and begin a second edit-cell command.  See also `auto-next-set'.",
#endif

/* Moving, Copying and Deleting Cells */

DEFUN ("copy-region", FDcopy_region, FAcopy_region, to_vptr(copy_region))
#ifdef FUNC_ARG_STRINGS
"rCopy region? ",
"RCopy %0 to? ",
#endif
#ifdef DOC_STRINGS
"Copy the contents of SOURCE-RANGE to DEST-RANGE.",
"DEST-RANGE must be either a single cell (the upper-left corner of the",
"destination), or a range containing the same number of cells as the",
"source range.",
#endif
  
DEFUN("copy-this-cell-formula", FDcopy_this_cell_formula, FAcopy_this_cell_formula, copy_this_cell_formula)
#ifdef DOC_STRINGS
"Copy the formula in the current row to the yank buffer",
#endif

DEFUN("paste-this-cell-formula", FDpaste_this_cell_formula, FApaste_this_cell_formula, paste_this_cell_formula)
#ifdef DOC_STRINGS
"Paste the formula in the current row to the yank buffer",
#endif


DEFUN ("copy-row", FDcopy_row, FAcopy_row, copy_row)
#ifdef FUNC_ARG_STRINGS
"nSource row? ",
#endif
#ifdef DOC_STRINGS
"Copy the contents of SOURCE-ROW to current row.",
#endif
	
DEFUN ("copy-values-in-region",
       FDcopy_values_region, FAcopy_values_region, to_vptr(copy_values_region))
#ifdef FUNC_ARG_STRINGS
"rCopy values in region? ",
"RCopy values in %0 to:",
#endif
#ifdef DOC_STRINGS
"Copy the values of the cells in SOURCE-RANGE to DEST-RANGE.",
"DEST-RANGE must be either a single cell (the upper-left corner of the",
"destination), or a range containing the same number of cells as the",
"source range.",
#endif
    
DEFUN ("move-region",
       FDmove_region, FAmove_region, to_vptr(move_region))
#ifdef FUNC_ARG_STRINGS
"rMove region? ",
"RMove region %0 to? ",
#endif
#ifdef DOC_STRINGS
"Move the contents of SOURCE-RANGE to DEST-RANGE.",
"DEST-RANGE must be either a single cell (the upper-left corner of the",
"destination), or a range containing the same number of rows and columns",
"as the source range.",
"",
"This command makes adjustments to all formulas in the range as well as to",
"formulas and variable bindings that refer to the range.",
#endif
    
DEFUN ("move-marked-region",
       FDmove_marked_region, FAmove_marked_region, to_vptr(move_region))
#ifdef FUNC_ARG_STRINGS
"@Move region? ",
"RMove region %0 to? ",
#endif
#ifdef DOC_STRINGS
"Move the contents of SOURCE-RANGE to DEST-RANGE.",
"DEST-RANGE must be either a single cell (the upper-left corner of the",
"destination), or a range containing the same number of rows and columns",
"as the source range.",
"",
"This command makes adjustments to all formulas in the range as well as to",
"formulas and variable bindings that refer to the range.",
"",
"This command should be used in macros only when the mark is known to have",
"been set.",
#endif
    
DEFUN ("insert-row", FDinsert_row, FAinsert_row, to_vptr(insert_row))
#ifdef FUNC_ARG_STRINGS
"+p",
#endif
#ifdef DOC_STRINGS
"Insert ARG empty rows.",
#endif

DEFUN ("insert-col", FDinsert_col, FAinsert_col, to_vptr(insert_col))
#ifdef FUNC_ARG_STRINGS
"+p",
#endif
#ifdef DOC_STRINGS
"Insert ARG empty cols.",
#endif

#if 0 /* From Oleo-patches-B, no assignment yet */
DEFUN ("insert-copy-row", FDinsert_copy_row, FAinsert_copy_row,
       insert_copy_row)
#ifdef FUNC_ARG_STRINGS
"+p",
#endif
#ifdef DOC_STRINGS
"Insert ARG copies of the current row, below the current row.",
#endif
#endif /* 0 */

#if 0 /* From Oleo-Patches-B, no assignment yet */
DEFUN ("insert-copy-col", FDinsert_copy_col, FAinsert_copy_col,
       insert_copy_col)
#ifdef FUNC_ARG_STRINGS
"+p",
#endif
#ifdef DOC_STRINGS
"Insert ARG copies of the current column, to the right of the current column.",
#endif
#endif /* 0 */

DEFUN ("delete-row", FDdelete_row, FAdelete_row, to_vptr(delete_row))
#ifdef FUNC_ARG_STRINGS
"+p",
#endif
#ifdef DOC_STRINGS
"Delete ARG rows.",
#endif

DEFUN ("delete-col", FDdelete_col, FAdelete_col, to_vptr(delete_col))
#ifdef FUNC_ARG_STRINGS
"+p",
#endif
#ifdef DOC_STRINGS
"Delete ARG cols.",
#endif

DEFUN_3 ("delete-cell", FDdelete_cell, to_vptr(kill_cell_cmd))
#ifdef DOC_STRINGS
"Erase the formula, font and format of the current cell.",
#endif

DEFUN ("delete-region", FDdelete_region, FAdelete_region, to_vptr(delete_region))
#ifdef FUNC_ARG_STRINGS
"rDelete region? ",
#endif
#ifdef DOC_STRINGS
"Erase the formulas, fonts and formats of the cells in RANGE.",
#endif

DEFUN ("clear-spreadsheet",
       FDclear_spreadsheet, FAclear_spreadsheet, to_vptr(kill_all_cmd))
#ifdef FUNC_ARG_STRINGS
"MSpreadsheet modified;  clear anyway? (yes or no) ",
#endif
#ifdef DOC_STRINGS
"Erase all cells, reset all column widths and row heights.",
#endif

/* Sorting Commands */

DEFUN ("sort-region", FDsort_region_cmd, FAsort_region_cmd,
        to_vptr(sort_region_cmd))
#ifdef FUNC_ARG_STRINGS
"SSort region? ",
#endif
#ifdef DOC_STRINGS
"Sort a row or column of cells.",
#endif

/* Commands For Setting Cell Attributes. */

DEFUN_5 ("set-region-height", FDset_region_height, FAset_region_height,
	 DFset_region_height, to_vptr(set_region_height))
#ifdef FUNC_ARG_STRINGS
"rSet height for region? ",
"sSet height for %0 to? ",
#endif
#ifdef FUNC_INIT_CODE
0,
"{insert-cell-attr %0 height}",
#endif
#ifdef DOC_STRINGS
"Set the height for rows in RANGE to HEIGHT.",
"HEIGHT may be a non-negative integer or `default', in which case the value",
"set with [set-default-height] is used.",
"",
"On the screen, a cell of height N is as tall as N lines of text in the",
"default cell font.",
#endif

DEFUN_5 ("set-region-width", FDset_region_width, FAset_region_width,
	 DFset_region_width, to_vptr(set_region_width))
#ifdef FUNC_ARG_STRINGS
"rSet width for region? ",
"sSet width for %0 to? ",
#endif
#ifdef FUNC_INIT_CODE
0,
"{insert-cell-attr %0 width}",
#endif
#ifdef DOC_STRINGS
"Set the width of columns in RANGE to WIDTH.",
"WIDTH may be a non-negative integer or `default', in which case the value",
"set with [set-default-width] is used.",
"",
"On the screen, a cell of width N is as wide as N repetitions of the character",
"`M' in the default cell font.",
#endif

DEFUN ("set-region-protection", FDset_region_protection,
       FAset_region_protection, to_vptr(set_region_protection))
#ifdef FUNC_ARG_STRINGS
"rSet protection for region? ",
"[dpu][D]efault  [P]rotect  [U]nprotect? ",
#endif
#ifdef DOC_STRINGS
"Set the protection for cells in a REGION to PROT.",
"PROT may be `d' for default, `p' - protected, or `u' - unprotected.",
#endif

DEFUN ("set-region-alignment", FDset_region_alignment,
       FAset_region_alignment, to_vptr(set_region_alignment))
#ifdef FUNC_ARG_STRINGS
"rSet alignment for region? ",
"[dlcr][D]efault  [L]eft  [C]enter  [R]ight? ",
#endif
#ifdef DOC_STRINGS
"Set the alignment for cells in a REGION to JST.",
"JST may be `d' for default, `l' - left, `r' - right, or `c' - center.",
#endif

DEFUN_5 ("set-region-format", FDset_region_format, FAset_region_format,
	 DFset_region_format, to_vptr(set_region_format))
#ifdef FUNC_ARG_STRINGS
"rSet format for region? ",
"FSet format of %0 to? ",
#endif
#ifdef FUNC_INIT_CODE
0,
"{insert-cell-attr %0 format}",
#endif
#ifdef DOC_STRINGS
"Set the format for cells in REGION to FORMAT.",
"Oleo can display numbers in these formats:",
"",
"integer		The number is rounded to an integer before being displayed.",
"",
"float		The number is displayed in decimal notation, using",
"		whatever precision is needed (or fits).",
"",
"hidden		The number is not displayed.  This has no other effect on",
"		the cell (e.g. it is recalculated normally).",
"",
"",
"graph		The number is displayed as a small graph.  If the number is",
"		close to zero, it will be displayed as '0', If it is positive,",
"		a row of '+' is displayed.  If it is negative, a row of",
"		'-' is displayed.",
"",
"user-1 through	These are the sixteen user-defined formats.  ",
"user-16		c.f. set-user-format.",
"",
"The following formats require a precision.  The precision may be a number",
"from zero to 14, in which case that number of digits will always be displayed",
"after the decimal point (zero-padding or rounding the number as needed), or",
"the precision may be 'float', in which case oleo will use as many digits as",
"necessary.  As examples:",
"",
"	dollar.2			display like ``$1,769.00''",
"	general.float			    \"    \"    ``1769.00032''	",
"",
"general		This uses either normal or scientific notation, depending on",
"		the magnitude of the number and the width of the column.",
"",
"dollar		Positive values are preceded by '$', (so 3 is displayed as",
"		'$3').  Negative values are parenthesized (so -3 is displayed",
"		as '($3)'), and all large values have a ',' every three",
"		digits (so 1000 is displayed as '$1,000'.",
"",
"comma		Positive numbers are not preceded by '$', but this is",
"		otherwise identical to 'dollar'.",
"",
"percent		The value is multiplied by 100, and is displayed with a",
"		trailing '%'.  Thus .01 displays as '1%', while 1 displays as",
"		'100%'.",
"",
"fixed		The number is displayed in normal, everyday notation, using",
"		the precision specified.  'fixed.float' is the same as",
"		'float'.  'fixed.0' is the same as 'integer'.",
"",
"exponent	The number is displayed in scientific notation.",
"",
#endif

DEFUN_5 ("set-cell-height", FDset_cell_height, FAset_cell_height,
	 DFset_cell_height, to_vptr(set_region_height))
#ifdef FUNC_ARG_STRINGS
".'",
"sSet height for %0 to? ",
#endif
#ifdef FUNC_INIT_CODE
0,
"{insert-cell-attr %0 height}",
#endif
#ifdef DOC_STRINGS
"Set the height for ROW to HEIGHT.",
"HEIGHT may be a non-negative integer or `default', in which case the value",
"set with [set-default-height] is used.",
"",
"On the screen, a cell of height N is as tall as N lines of text in the",
"default cell font.",
#endif

DEFUN_5 ("set-cell-width", FDset_cell_width, FAset_cell_width,
	 DFset_cell_width, to_vptr(set_region_width))
#ifdef FUNC_ARG_STRINGS
".'",
"sSet width for %0 to? ",
#endif
#ifdef FUNC_INIT_CODE
0,
"{insert-cell-attr %0 width}",
#endif
#ifdef DOC_STRINGS
"Set the width for COL to WIDTH.",
"WIDTH may be a non-negative integer or `default', in which case the value",
"set with [set-default-width] is used.",
"",
"On the screen, a cell of width N is as wide as N repetitions of `M' in the",
"default cell font.",
#endif

DEFUN ("set-cell-protection",
       FDset_cell_protection, FAset_cell_protection, to_vptr(set_region_protection))
#ifdef FUNC_ARG_STRINGS
".'",
"[dpu][D]efault  [P]rotect  [U]nprotect? ",
#endif
#ifdef DOC_STRINGS
"Set the protection for CELL to PROT.",
"PROT may be `d' for default, `p' - protected, or `u' - unprotected.",
#endif

DEFUN ("set-cell-alignment",
       FDset_cell_alignment, FAset_cell_alignment, to_vptr(set_region_alignment))
#ifdef FUNC_ARG_STRINGS
".'",
"[dlcr][D]efault  [L]eft  [C]enter  [R]ight? ",
#endif
#ifdef DOC_STRINGS
"Set the alignment for CELL to JST.",
"JST may be `d' for default, `l' - left, `r' - right, or `c' - center.",
#endif

DEFUN_5 ("set-cell-format", FDset_cell_format, FAset_cell_format,
	 DFset_cell_format, to_vptr(set_region_format))
#ifdef FUNC_ARG_STRINGS
".'",
"FSet format of %0 to? ",
#endif
#ifdef FUNC_INIT_CODE
0,
"{insert-cell-attr %0 format}",
#endif
#ifdef DOC_STRINGS
"Set the format for numeric display for CELL to FORMAT.",
"For a description of valid formats, see set-region-format.",
#endif

DEFUN_5 ("define-user-format",
	 FDdefine_usr_fmt, FAdefine_usr_fmt, DFdefine_usr_fmt, to_vptr(define_usr_fmt))
#ifdef FUNC_ARG_STRINGS
"N[1,16]Define format [1-16]? ",
"wPositive header? ",
"wNegative header? ",
"wPositive trailer? ",
"wNegative trailer? ",
"wRepresentation of zero? ",
"wComma? ",
"wDecimal point? ",
"wPrecision [0-14 or `float']? ",
"wScale-factor? ",
#endif
#ifdef FUNC_INIT_CODE
0,
"{insert-user-format-part %0 1}",
"{insert-user-format-part %0 2}",
"{insert-user-format-part %0 3}",
"{insert-user-format-part %0 4}",
"{insert-user-format-part %0 5}",
"{insert-user-format-part %0 6}",
"{insert-user-format-part %0 7}",
"{insert-user-format-part %0 8}",
"{insert-user-format-part %0 9}",
#endif
#ifdef DOC_STRINGS
"Set the parameters of a user-defined numeric format.",
"",
"There are sixteen user-defined numeric formats (maned `user-1' ... `user-16').",
"Each format specifies a syntax for printing numbers.  They may be used ",
"in conjunction with the [set-cell-format] and [set-region-format] commands.",
"",
"Each format has these parameters (using the 'dollar' format for an example).",
"",
"Parameter Name		Example:	How it is used",
"",
"Positive header		$		before positive numbers.",
"Negative header		($		before negative numbers.",
"Positive trailer			after positive numbers.",
"Negative trailer	)		after negative numbers.",
"Zero			$0.00		if the number is 0.",
"Comma			,		between each three digits (1,000,000).",
"Decimal			.		as a decimal point.",
"Precision		2		The number of digits to print after the",
"					decimal point. This may be an integer",
"					in the range [0-14] or `float'.",
"					`float' means use however many digits",
"					are needed, or however many will fit,",
"					whichever is less.",
"Scale			1		What to multiply the value by before",
"					printing. Most often this will be one,",
"					but it might be 100 if you're printing",
"					percentages, or .000001 if you're",
"					printing in megabucks.  (Beware of",
"					overflow!)",
"",
"Do not use digits for the headers, trailers, the comma, or the decimal point",
"symbol.  Using digits will confuse the internal routines and produce",
"incomprehensible results.   (This is a bug that will be fixed.)",
#endif

/* Commands For Setting Cell Defaults */

DEFUN ("set-default-height",
       FDset_def_height, FAset_def_height, to_vptr(set_def_height))
#ifdef FUNC_ARG_STRINGS
"sDefault height? ",
#endif
#ifdef DOC_STRINGS
"Set the default height of rows.",
"See set-region-height for more information.",
#endif

DEFUN ("set-default-width",
       FDset_def_width, FAset_def_width, to_vptr(set_def_width))
#ifdef FUNC_ARG_STRINGS
"sDefault width? ",
#endif
#ifdef DOC_STRINGS
"Set the default height of cols.",
"See set-region-width for more information.",
#endif

DEFUN ("set-user-scales",
       FDset_user_scales, FAset_user_scales, to_vptr(set_user_scales))
#ifdef FUNC_ARG_STRINGS
"dScale row heights by? ",
"dScale column widths by? ",
#endif
#ifdef DOC_STRINGS
"Scale HEIGHT and WIDTH by the given floating point amounts.",
#endif

DEFUN ("set-default-protection", FDset_def_protection,
       FAset_def_protection, to_vptr(set_def_protection))
#ifdef FUNC_ARG_STRINGS
"[pu][U]nprotected  or  [P]rotected",
#endif
#ifdef DOC_STRINGS
"Set the default protection of cells.",
"A protection may be specified as `p' for protected, or `u' -- unprotected.",
#endif

DEFUN ("set-default-alignment",
       FDset_def_alignment, FAset_def_alignment, to_vptr(set_def_alignment))
#ifdef FUNC_ARG_STRINGS
"[dlcrDLCR][L]eft  [C]enter  [R]ight  [D]efault",
#endif
#ifdef DOC_STRINGS
"Set the default alignment of cells.",
"An alignment may be specified as `l' for left, `c' -- center, `r' -- right",
"or `d' -- default.",
#endif

DEFUN ("set-default-format",
       FDset_def_format, FAset_def_format, to_vptr(set_def_format))
#ifdef FUNC_ARG_STRINGS
"FDefault format? ",
#endif
#ifdef DOC_STRINGS
"Set the default format for numeric display to FORMAT.",
"See set-region-format for an explanation of formats.",
#endif

#if 0
DEFUN ("set-default-font",
       FDset_default_font, FAset_default_font, set_default_font)
#ifdef FUNC_ARG_STRINGS
"wSet default font to? ",
"dPoint size? (e.g. 1.3) ",
#endif
#ifdef DOC_STRINGS
"Set the default font to NAME at SCALE",
"NAME should be a name defined with [define-font].",
"SCALE should be a floating point number, which will be interpreted",
"as a ratio to the default point size.  For example, if SCALE is 1.33",
"and the default point size is 12 then this cell will be drawn with",
"a point size of 16.",
"",
"Under X, this scaling behavior depends on having a wide variety of fonts",
"installed, and wildcarding point-sizes in x11 font names passed to ",
"define-font.",
#endif
#endif

/* Commands for Inserting Cell Details Into Input Area  */

DEFUN_3 ("insert-cell-expression",
         FDinsert_cell_expression, to_vptr(insert_cell_expression))
#ifdef DOC_STRINGS
"Insert the current cell's formula in the input area.",
#endif

DEFUN ("insert-other-cell-expression",
         FDinsert_other_cell_expression, FAinsert_other_cell_expression,
         to_vptr(insert_other_cell_expression))
#ifdef FUNC_ARG_STRINGS
"RInsert expression from? ",
#endif
#ifdef DOC_STRINGS
"Insert another cell's formula in the input area.  If the other cell",
"is a range, the value in the top left cell, the NW corner is used.",
#endif

DEFUN_3 ("insert-cell-value", FDinsert_cell_value, to_vptr(insert_cell_value))
#ifdef DOC_STRINGS
"Insert the current cell's value (not its formula) in the input area.",
#endif

DEFUN ("insert-other-cell-value",
       FDinsert_other_cell_value, FAinsert_other_cell_value,
       to_vptr(insert_other_cell_value))
#ifdef FUNC_ARG_STRINGS
"RInsert value from? ",
#endif
#ifdef DOC_STRINGS
"Insert another cell's value (not its formula) in the input area.",
"If the other cell is a range, the value in the top left cell",
"the NW corner is used.",
#endif

DEFUN_3 ("insert-rel-ref", FDinsert_rel_ref, insert_rel_ref)
#ifdef DOC_STRINGS
"Insert a relative reference in the input area.",
"This command can only be executed when a cell formula is being edited.",
"The inserted reference is to the current cell, and is relative to the",
"cell being defined.",
#endif

DEFUN ("insert-abs-ref", FDinsert_abs_ref,
  FAinsert_abs_ref, insert_abs_ref)
#ifdef FUNC_ARG_STRINGS
"#0",
#endif
#ifdef DOC_STRINGS
"Insert an absolute reference to the current cell or marked region",
"in the input area.",
#endif

DEFUN ("insert-abs-cell-ref", FDinsert_abs_cell_ref,
  FAinsert_abs_cell_ref, insert_abs_ref)
#ifdef FUNC_ARG_STRINGS
"#1",
#endif
#ifdef DOC_STRINGS
"Insert an absolute reference to the current cell in the input area.",
#endif

DEFUN ("insert-cell-attr",
       FDinsert_cell_attr, FAinsert_cell_attr, insert_cell_attr)
#ifdef FUNC_ARG_STRINGS
"RInsert attribute of cell? ",
"SInsert %0 attribute? ", 
#endif
#ifdef DOC_STRINGS
"(Internal use -- probably not useful as a command.)",
"For the lr,lc cell in RNG, insert the name of its named ATTRIBUTE.",
"Supported ATTRIBUTEs are `width', `height', `font', `font-scale' and",
"`format'.",
#endif

DEFUN_3 ("insert-current-filename",
	 FDinsert_current_filename, insert_current_filename)
#ifdef DOC_STRINGS
"Insert the name of the current file (if any) in the input area.",
#endif

DEFUN_3 ("insert-context-word", FDinsert_context_word, insert_context_word)
#ifdef DOC_STRINGS
"(internal use)",
#endif

DEFUN ("insert-user-format-part",
       FDinsert_usr_fmt_part, FAinsert_usr_fmt_part, insert_usr_fmt_part)
#ifdef FUNC_ARG_STRINGS
"nInsert part of user format [1-16]? ",
"nInsert which part of user format %0 [1-9]? ",
#endif
#ifdef DOC_STRINGS
"(Internal use -- probably not useful as a command.)",
"For user format FMT, insert the Nth field.",
"The fields of a user format are numbered this way:",
"  1 - positive header",
"  2 - negative header",
"  3 - positive trailer",
"  4 - negative trailer",
"  5 - representation of zero",
"  6 - comma (as in 1,000,000.00)",
"  7 - decimal point",
"  8 - precision", 
"  9 - scale factor",
"",
"For more information, see documentation for the command define-user-format.",
#endif

/* Keyboard Macro and Variable Commands */

DEFUN_3 ("start-entering-macro", FDstart_entering_macro, start_entering_macro)
#ifdef DOC_STRINGS
"Record subsequent keyboard input, defining a keyboard macro.",
"The commands are recorded even as they are executed.",
"Use [stop-entering-macro] to finish recording and make the macro available.",
"Use [store-last-macro] to record the macro in a spreadsheet cell.",
#if 0
Should support this:
Non-nil arg (prefix arg) means append to last macro defined;
This begins by re-executing that macro as if you typed it again.
#endif
#endif

DEFUN_3 ("stop-entering-macro", FDstop_entering_macro, stop_entering_macro)
#ifdef DOC_STRINGS
"Finish defining a keyboard macro.",
"The definition was started by [start-entering-macro]",
"The macro is now available for use via [call-last-kbd-macro]",
"or it can be given a name with [store-last-macro] and then invoked",
"by passing its cell address to [exec].",
#if 0
"With numeric arg, repeat macro now that many times,",
"counting the definition just completed as the first repetition.",
#endif
#endif

DEFUN ("call-last-kbd-macro", FDcall_last_kbd_macro,
       FAcall_last_kbd_macro, call_last_kbd_macro)
#ifdef FUNC_ARG_STRINGS
"p",
#endif
#ifdef DOC_STRINGS
"Call the last keyboard macro that you defined with [start-entering-macro].",
"To make a macro permanent so you can call it even after",
"defining others, use [store-last-macro].",
#endif

DEFUN_3 ("end-macro", FDend_macro, end_macro)
#ifdef DOC_STRINGS
"This function is for internal use only.",
"Oleo executes this command as part of the process of executing a kbd  macro.",
#endif

DEFUN ("store-last-macro",
       FDstore_last_macro, FAstore_last_macro, store_last_macro) 
#ifdef FUNC_ARG_STRINGS
"rStore at? ",
#endif
#ifdef DOC_STRINGS
"Saves the most recently defined keyboard macro in a spreadsheet cell.",
"The sole argument is the cell address at which to store the macro.",
"Thereafter, that cell address is a valid argument to [exec].",
#endif

DEFUN ("run-string-as-macro",
       FDrun_string_as_macro, FArun_string_as_macro, run_string_as_macro)
#ifdef FUNC_ARG_STRINGS
"sRun macro string: ",
#endif
#ifdef DOC_STRINGS
"Execute STRING as a keyboard macro.",
#endif

DEFUN ("name-macro-string",
       FDname_macro_string, FAname_macro_string, name_macro_string)
#ifdef FUNC_ARG_STRINGS
"SName for macro? ",
"sMacro %0? ",
#endif
#ifdef DOC_STRINGS
"Assign NAME to MACRO.",
"The name functions like any interactive command name.",
"When invoked, the command executes the macro string, using the prefix",
"as a repeat count.",
#endif

DEFUN ("set-var", FDset_var, FAset_var, set_var)
#ifdef FUNC_ARG_STRINGS
"@Region to define? ",
"SVariable name for %0? ",
#endif
#ifdef DOC_STRINGS
"Change the binding of a user-defined variable.",
"Variables can be bound to cells or ranges of cells, and then referenced",
"in formulas wherever a cell or range reference is allowed.",
"If no new binding is given, then the variable becomes unbound.",
#endif

DEFUN ("unset-var", FDunset_var, FAunset_var, unset_var)
#ifdef FUNC_ARG_STRINGS
"SUnset variable name? ",
#endif
#ifdef DOC_STRINGS
"Delete the binding of a user-defined variable.",
"If no such binding exists, an error is issued.",
#endif

/* Commands Affecting Macro Flow */

DEFUN_3 ("exit-minibuffer", FDexit_minibuffer, exit_minibuffer)
#ifdef DOC_STRINGS
"Terminate this minibuffer argument.",
"If no valid argument has been given, an error is reported.",
#endif

DEFUN_3 ("break",  FDbreak, 0)
#ifdef DOC_STRINGS
"Abort the current complex command or incomplete keysequence.",
#endif

/* Commands for Running Other Commands */

DEFUN ("exec",
       FDexecute_command, FAexecute_command, execute_command)
#ifdef FUNC_ARG_STRINGS
"CM-x ",
#endif
#ifdef DOC_STRINGS
"Read command name, then read its arguments and call it.",
#endif

DEFUN_3 ("universal-argument", FDuniversal_argument, 0)
#ifdef DOC_STRINGS
"Begin a numeric argument for the following command.",
"Digits or minus sign following this command make up the numeric argument.",
"If no digits or minus sign follow, this by itself provides 4 as argument.",
"Used more than once, this command multiplies the argument by 4 each time.",
#endif

/* Mouse Commands */

DEFUN_3 ("mouse-goto", FDmouse_goto, mouse_goto_cmd)
DEFUN_3 ("mouse-mark", FDmouse_mark, mouse_mark_cmd)
DEFUN_3 ("mouse-mark-and-goto",
	 FDmouse_mark_and_goto_cmd, mouse_mark_and_goto_cmd)

/* Import and Export Commands of Various Kinds */

DEFUN_5 ("save-dsv", FDwsave_dsv, FAwsave_dsv, DFsave_dsv, save_dsv)
#ifdef FUNC_ARG_STRINGS
"fwSave spreadsheet in dsv? ",
"=%0",
#endif
#ifdef FUNC_INIT_CODE
"{insert-current-filename}",
0,
#endif
#ifdef DOC_STRINGS
"Save the spreadsheet to FILENAME in dsv format.",
#endif
	
DEFUN_5 ("save-spreadsheet", FDwrite_cmd, FAwrite_cmd, DFwrite_cmd, write_cmd)
#ifdef FUNC_ARG_STRINGS
"fwSave spreadsheet? ",
"=%0",
#endif
#ifdef FUNC_INIT_CODE
"{insert-current-filename}",
0,
#endif
#ifdef DOC_STRINGS
"Save the spreadsheet to FILENAME.",
#endif

DEFUN ("read-variables", FDread_variables, FAread_variables, read_variables)
#ifdef FUNC_ARG_STRINGS
"frRead variables from file? ",
#endif
#ifdef DOC_STRINGS
"Read cell values from a FILE.",
"The syntax for the contents of FILE is <var>=<value>, e.g.:",
"	w2_exemptions=2",	
"For each assignment, if a variable with the given name is bound to a cell,",
"the value is assigned to that cell.  See also, write-variables.",
#endif

DEFUN ("read-commands", FDread_cmds_cmd, FAread_cmds_cmd, read_cmds_cmd)
#ifdef FUNC_ARG_STRINGS
"frRead commands from file? ",
#endif
#ifdef DOC_STRINGS
"Read Oleo commands from FILE.",
"The syntax for a command is a command name followed by a whitespace",
"separated list arguments.  For example: ",
"	bind-key main next-row ^n",
"Blank lines and lines that begin with `#' are ignored.",
"Lines may be continued by preceding the final newline with an",
"odd number of `\\' characters.  One backslash and the newline will",
"be discarded.",
#endif

DEFUN("shell", FDrun_shell, FArun_shell, run_shell)
#ifdef FUNC_ARG_STRINGS
"sRun shell command file? ",
#endif
#ifdef DOC_STRINGS
"Run Neoleo commands from an external command printed to stdout.",
"The syntax for the output is as for `read-commands', except that",
"comments and line continuations have not been implemented.",	
#endif


DEFUN ("toggle-load-hooks",
       FDtoggle_load_hooks, FAtoggle_load_hooks, toggle_load_hooks)
#ifdef FUNC_ARG_STRINGS
"p?",
#endif
#ifdef DOC_STRINGS
"Change whether load-hooks are run when spreadsheets are loaded.",
"When active, the find-alternate-spreadsheet command looks for a variable",
"called `load_hooks' and executes the macro at that address.",
"With a positive prefix argument, turns load hooks on.  With a negative",
"argument, turns them off.  With no argument, acts as a toggle.",
#endif

DEFUN_5 ("find-alternate-spreadsheet",
	 FDread_cmd, FAread_cmd, DFread_cmd, read_cmd)
#ifdef FUNC_ARG_STRINGS
"frFind alternate spreadsheet? ",
"MSpreadsheet modified;  kill anyway? (yes or no) ",
"=%0",
#endif
#ifdef FUNC_INIT_CODE
"{insert-current-filename}",
0,
0,
#endif
#ifdef DOC_STRINGS
"Replace the currently loaded spreadsheet with the contents of FILE.",
#endif

DEFUN ("merge-spreadsheet",
       FDread_merge_cmd, FAread_merge_cmd, read_merge_cmd)
#ifdef FUNC_ARG_STRINGS
"frMerge spreadsheet? ",
"MSpreadsheet modified; merge anyway? (yes or no) ",
#endif
#ifdef DOC_STRINGS
"Combine the spreadsheet in FILE with the current spreadsheet.",
"This reads a new spreadsheet without clearing all cells first.",
#endif
    
DEFUN ("write-region-to-file",
       FDwrite_reg_cmd, FAwrite_reg_cmd, write_reg_cmd)
#ifdef FUNC_ARG_STRINGS
"rWrite region? ",
"fwWrite region to file? ",
#endif
#ifdef DOC_STRINGS
"Write the cells in RANGE to FILE using the current file format.",
"This command also writes all variables that with bindings entirely",
"inside of RANGE.",
#endif



/* Commands that Trigger Calculation */

DEFUN ("recalculate", FDrecalculate, FArecalculate, recalculate)
#ifdef FUNC_ARG_STRINGS
"p?",
#endif
#ifdef DOC_STRINGS
"Recompute any cells that need it.  With a prefix arg, recompute all cells.",
#endif

DEFUN_3 ("recompile_spreadsheet",
	 FDrecompile_spreadsheet, recompile_spreadsheet)
#ifdef DOC_STRINGS
"Recompile all cell formulas.",
#endif

/* Window Commands */

DEFUN ("open-window", FDopen_window, FAopen_window, open_window)
#ifdef FUNC_ARG_STRINGS
"sSplit command? ",
#endif
#ifdef DOC_STRINGS
"Split the window according to SPLIT.",
"The syntax of split is an orientation (`h' or `v') followed by",
"either a number of rows (cols) or a percentage of rows (cols).",
"For example: ",
"	h10",
"	v33%",
#endif

DEFUN_3 ("split-window-horizontally", FDhsplit_window, hsplit_window)
#ifdef DOC_STRINGS
"Divide the current window evenly into two windows side by side.",
#endif

DEFUN_3 ("split-window-vertically", FDvsplit_window, vsplit_window)
#ifdef DOC_STRINGS
"Divide the current window evenly into two stacked windows.",
#endif
    
DEFUN_3 ("delete-window", FDdelete_window, delete_window)
#ifdef DOC_STRINGS
"Remove the current window from the display.",
#endif

DEFUN_3 ("delete-other-windows", FDdelete_other_windows, delete_other_windows)
#ifdef DOC_STRINGS
"Remove all but the current window from the display.",
#endif

DEFUN ("close-window", FDclose_window, FAclose_window, close_window)
#ifdef FUNC_ARG_STRINGS
"NClose window number? ",
#endif
#ifdef DOC_STRINGS
"Remove the window ARG from the display.",
#endif
    
DEFUN ("goto-window", FDgoto_window, FAgoto_window, goto_window)
#ifdef FUNC_ARG_STRINGS
"NGoto window number? ",
#endif
#ifdef DOC_STRINGS
"Make the ARGth window the current window.",
#endif
    
DEFUN_3 ("goto-minibuffer", FDgoto_minibuffer, goto_minibuffer)
#ifdef DOC_STRINGS
"Select the input area as the current window.",
"An error is given if the input area is not active.",
#endif

DEFUN_3 ("other-window", FDother_window, other_window)
#ifdef FUNC_ARG_STRINGS
"+p",
#endif
#ifdef DOC_STRINGS
"Select the ARGth different window.",
#endif



DEFUN ("set-option", FDset_option, FAset_option, set_options)
#ifdef FUNC_ARG_STRINGS
"sSet option? ",
#endif
#ifdef DOC_STRINGS
"Change the value of a user-controlled option.",
"",
"Currently Supported Options:",
"",
"(no)a0		In a0 mode, Oleo uses cell-addresses like `A1'.",
"		In noa0 mode, Oleo uses `R1C1' style addresses.",
"",
"(no)auto	In auto mode, cells whose values may have changed are",
"		automatically recalculated.  In noauto mode, cells are only",
"		recalculated when the recalculate command is used.",
"",
"(no)background	In background mode, automatic recalculation is done while",
"(no)bkgrnd	the spreadsheet waits for you to type a key (and stops while",
"		the key is being handled).  In nobackground mode, Oleo",
"		performs all its recalculation before listening for",
"		keystrokes.",
"",
"(no)backup	In backup mode, whenever the spreadsheet writes out a file,",
"		if the file already exists, a backup copy is made (similarly",
"		to Emacs).",
"",
"(no)bkup_copy	In nobackup mode bkup_copy is ignored.  In bkup_copy mode,",
"		backup files are made by copying the original file, instead",
"		of renaming it.",
"",
"ticks (number)	This value controls how often, in seconds, time-dependent",
"		formulas are reevaluated.",
"",
"print (number)	This is the width of the page for the print command.",
"		The default value is the width of the screen.",
"",
"file (format-name)  This sets format Oleo will use when reading and writing",
"		spreadsheet files.  Valid format names are:",
"		 oleo		- The default (and supported) format.",
"		 sylk		- A partial implementation of a std. format.",
"		 sc		- partial SC compatibility.  This is buggy.",
"		 list		- An ascii (lines of fields) format.",
"				  This is not a complete file format",
"				  but may be useful for importing/exporting",
"				  data.",
"",
"list ch	Use the character CH to separate the cell values when using ",
"		the `list' file format.",
"",
"status (n)	This option controls which line on the screen is used for",
"		displaying the status of the current cell.  The number may be",
"		positive, (counting down from the top), negative (counting up",
"		from the bottom, or zero (disabling the status line).",
"",
"input (n)	This option controls which line on the screen is used for",
"		reading lines of text.  The number may be positive (counting",
"		down from the top), or negative (counting up from the",
"		bottom), but not zero.",
"",
"The following options affect the currently active window:",
"",
"(no)page	In page mode, whenever a particular cell is",
"		displayed in the window, it will always be in the same",
"		location on the screen.  In page mode, the recenter command",
"		acts just like the redraw-screen command.",
"",
"(no)pageh	These options turns on or off page mode only in",
"(no)pagev	the horizontal or vertical axis.",
"",
"(no)lockh	These options prevent the cell cursor in the current window",
"(no)lockv	from moving in the horizontal or vertical direction, but only",
"		when the cell cursor is moved in a window that this one is",
"		linked.",
"",
"",
"(no)edges	In edges mode oleo displays Row and Column numbers at the top",
"		and left edges of the window.  Perhaps edges should be split",
"		into edgeh and edgev?",
"",
"(no)standout	In standout mode, the edges (see (no)edges) are drawn in",
"		standout mode (reverse video, usually).",
#endif

DEFUN ("show-variable", FDshow_variable, FAshow_variable, show_var)
#ifdef FUNC_ARG_STRINGS
"SShow Variable? ",
#endif
#ifdef DOC_STRINGS
"Report the current binding of a user-defined variable.",
"Variables can be defined using the [set-variable].",
#endif
      

DEFUN ("write-keys", FDwrite_keys_cmd, FAwrite_keys_cmd, write_keys_cmd)
#ifdef FUNC_ARG_STRINGS
"fwWrite bindings to file? ",
#endif
#ifdef DOC_STRINGS
"Write a list of commands that recreate the current keybindings to FILE.",
"This command is of limited utility and will probably go away or be",
"changed considerably.  It is a way to generate a list of bindings, though.",
#endif

DEFUN ("write-variables",
       FDwrite_variables, FAwrite_variables, write_variables)
#ifdef FUNC_ARG_STRINGS
"fwWrite variables to file? ", 
#endif
#ifdef DOC_STRINGS
"Use variable names to construct a shell script to FILE.",
"This command looks for all variables that are bound to a single",
"cell (rather than to a range).  It writes a shell script that assigns",
"the value of that cell to a shell variable.  For example, if the",
"variable `w2_exemptions' is bound to cell R2C3, and that cell contains",
"the value `2', then the output of write-variables will include a line:",
"	w2_exemptions=2",
"",
"This command is of limited utility, but was implemented as part of a crude",
"system of forms generation.",
#endif

/* Keymap Commands */

DEFUN ("create-keymap", FDcreate_keymap, FAcreate_keymap, create_keymap)
#ifdef FUNC_ARG_STRINGS
"SCreate Keymap? ",
"S'Parent map? ",
#endif
#ifdef DOC_STRINGS
"Create a new keymap given its NAME and PARENT.",
"",
"A keymap is a mapping from characters to commands and keymap.  As you type",
"characters to Oleo, keymaps are used to decide how to interpret your",
"keystrokes.  Emacs users are familiar with this concept, but should note",
"that Oleo keymaps do not behave quite like Emacs keymaps.",
"",
"Every keymap has a (unique) name.  That name is used, for example, as an",
"argument to the bind-key command.",
"",
"Every keymap has a parent map that is searched for bindings of keys that",
"are otherwise undefined.  In other words, every keymap inherits bindings",
"from some other keymap.  The keymap `universal' is at the root of the ",
"inheritance tree.",
#endif

DEFUN ("set-map-prompt", FDset_map_prompt, FAset_map_prompt, set_map_prompt)
#ifdef FUNC_ARG_STRINGS
"KSet prompt for keymap? ",
"sPrompt for %1? ", 
#endif
#ifdef DOC_STRINGS
"Set the prompt for KEYMAP to STRING.",
"When KEYMAP is the current keymap, the prompt is displayed in the input",
"area.",
#endif

DEFUN ("bind-key", FDbind_key, FAbind_key, bind_key)
#ifdef FUNC_ARG_STRINGS
"KKeymap? ",
"SCommand? ",
"cCharacter? ",
#endif
#ifdef DOC_STRINGS
"In KEYMAP, make COMMAND the binding of KEY.",
"COMMAND may be the name of an interactive function, the address of a macro,",
"or the name of another keymap.",
#endif

DEFUN ("unbind-key", FDunbind_key, FAunbind_key, unbind_key)
#ifdef FUNC_ARG_STRINGS
"KKeymap? ",
"cCharacter? ",
#endif
#ifdef DOC_STRINGS
"In KEYMAP, eliminate the binding of KEY.",
#endif

DEFUN ("bind-set", FDbind_set, FAbind_set, bind_or_unbind_set)
#ifdef FUNC_ARG_STRINGS
"KKeymap? ",
"SCommand? ",
"sCharacters? ",
#endif
#ifdef DOC_STRINGS
"In KEYMAP, make COMMAND the binding of all chars in KEYSET.",
"COMMAND may be the name of an interactive function, the address of a macro,",
"or the name of another keymap.  Keyset should be two characters, separated",
"by a `-', for example:",
"	bind-set main begin-edit a-z",
#endif

DEFUN ("unbind-set", FDunbind_set, FAunbind_set, unbind_set)
#ifdef FUNC_ARG_STRINGS
"KKeymap? ",
"sCharacters? ",
#endif
#ifdef DOC_STRINGS
"In KEYMAP, unbind all chars in KEYSET.",
"Keyset should be two characters, separated",
"by a `-', for example:",
"	unbind-set main begin-edit 0-9",
#endif

DEFUN ("bind-all-keys", FDbind_all_keys, FAbind_all_keys, bind_all_keys)
#ifdef FUNC_ARG_STRINGS
"KKeymap? ",
"SCommand? ",
#endif
#ifdef DOC_STRINGS
"In KEYMAP, bind every character to COMMAND.",
"COMMAND may be the name of an interactive function, the address of a macro,",
"or the name of another keymap.",
#endif

DEFUN ("with-keymap", FAwith_keymap, FDwith_keymap, with_keymap)
#ifdef FUNC_ARG_STRINGS
"SWith which keymap? ",
"m%0",
#endif
#ifdef DOC_STRINGS
"Create a user context in which KEYMAP is the top level keymap.",
"Commands will be read in the context until an exit-minibuffer or",
"break command is executed.",
#endif

DEFUN ("one-command-with-keymap", FAone_cmd_with_keymap,
       FDone_cmd_with_keymap, one_cmd_with_keymap)
#ifdef FUNC_ARG_STRINGS
"SWith which keymap? ",
"m'%0",
#endif
#ifdef DOC_STRINGS
"Create a user context in which KEYMAP is the top level keymap.",
"Commands will be read in the context until an exit-minibuffer or",
"break command is executed.",
#endif

/* Quite Low Level Commands */

DEFUN ("self-insert-command",
       FDself_insert_command, FAself_insert_command, self_insert_command)
#ifdef FUNC_ARG_STRINGS
"l",
"p",
#endif
#ifdef DOC_STRINGS
"Insert the last-typed character in the input area.",
"Prefix arg is a repeat-count.",
#endif

DEFUN ("self-map-command",
       FDself_map_command, FAself_map_command, self_map_command)
#ifdef FUNC_ARG_STRINGS
"l",
#endif
#ifdef DOC_STRINGS
"Map the last-typed character in the keysequence argument being edited.",
"This is used by the help system to prompt for the argument to describe-key and",
"describe-key-briefly.",
#endif

DEFUN ("exit-self-inserting",
       FDexit_self_inserting, FAexit_self_inserting, exit_self_inserting)
#ifdef FUNC_ARG_STRINGS
"l",
#endif
#ifdef DOC_STRINGS
"Insert the name of the last character typed and exit the minibuffer.",
"This is used to implement commands which prompt the user for a character.",
#endif

DEFUN ("pushback-keystroke",
       FApushback_keystroke, FDpushback_keystroke, pushback_keystroke)
#ifdef FUNC_ARG_STRINGS
"c'(any key continues) ",
#endif
#ifdef DOC_STRINGS
"(Internal use -- probably not useful as a command.)",
"Push the last keystroke back onto the input stream.",
#endif

DEFUN ("display-msg",
       FDdisplay_msg, FAdisplay_msg, display_msg)
#ifdef FUNC_ARG_STRINGS
"sMessage? ",
"c'%0",
#endif
#ifdef DOC_STRINGS
"(For internal use only)",
#endif

DEFUN ("error-msg",
       FDerror_msg, FAerror_msg, display_msg)
#ifdef FUNC_ARG_STRINGS
"sMessage? ",
"c!%0",
#endif
#ifdef DOC_STRINGS
"Display an error message for the user.",
"Macro processing will come to a halt.  The message vanishes as soon as the",
"user types the next character of input or after a brief timeout.",
#endif




DEFUN ("bload", FDbload, FAbload, bload)
#ifdef FUNC_ARG_STRINGS
"frLoad neo-basic? ",
#endif
#ifdef DOC_STRINGS
"Load neoleo basic FILENAME.",
#endif
