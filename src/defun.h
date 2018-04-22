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

DEFUN ("scroll-upleft", FDscroll_upleft, FAscroll_upleft, to_vptr(scroll_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#5",
"p",
#endif

DEFUN ("scroll-downright", FDscroll_downright, FAscroll_downright, to_vptr(scroll_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#6",
"p",
#endif

DEFUN ("scroll-downleft", FDscroll_downleft, FAscroll_downleft,
       to_vptr(scroll_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#7",
"p",
#endif

DEFUN ("scan-up", FDscan_up, FAscan_up, to_vptr(scan_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#0",
"p",
#endif

DEFUN ("scan-down", FDscan_down, FAscan_down, to_vptr(scan_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#1",
"p",
#endif

DEFUN ("scan-right", FDscan_right, FAscan_right, to_vptr(scan_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#2",
"p",
#endif

DEFUN ("scan-left", FDscan_left, FAscan_left, to_vptr(scan_cell_cursor))
#ifdef FUNC_ARG_STRINGS
"+#3",
"p",
#endif

DEFUN ("beginning-of-row",
       FDbeginning_of_row, FAbeginning_of_row, to_vptr(beginning_of_row))
#ifdef FUNC_ARG_STRINGS
"p",
#endif

DEFUN ("beginning-of-col",
       FDbeginning_of_col, FAbeginning_of_col, to_vptr(beginning_of_col))
#ifdef FUNC_ARG_STRINGS
"p",
#endif

DEFUN ("end-of-row", FDend_of_row, FAend_of_row, to_vptr(end_of_row))
#ifdef FUNC_ARG_STRINGS
"p",
#endif

DEFUN ("end-of-col", FDend_of_col, FAend_of_col, to_vptr(end_of_col))
#ifdef FUNC_ARG_STRINGS
"p",
#endif

DEFUN_3 ("recenter-window", FDrecenter_window, to_vptr(recenter_window))

DEFUN_3 ("redraw-screen", FDredraw_screen, to_vptr(redraw_screen))

/* Commands for Editing from the Sheet */

DEFUN ("edit-cell", FDedit_cell, FAedit_cell, to_vptr(edit_cell))
#ifdef FUNC_ARG_STRINGS
"$Set %c to? ",
#endif

DEFUN ("set-cell", FDset_cell, FAset_cell, to_vptr(edit_cell))
#ifdef FUNC_ARG_STRINGS
"$'Set %c to? ",
#endif

DEFUN ("goto-edit-cell",
       FDgoto_edit_cell, FAgoto_edit_cell, to_vptr(goto_edit_cell))
#ifdef FUNC_ARG_STRINGS
"l",
#endif

DEFUN ("goto-set-cell",
       FDgoto_set_cell, FAgoto_set_cell, to_vptr(goto_set_cell))
#ifdef FUNC_ARG_STRINGS
"l",
#endif

DEFUN ("set-region-formula",
       FDset_region_formula, FAset_region_formula, to_vptr(set_region_formula))
#ifdef FUNC_ARG_STRINGS
"rSet region? ",
"$'Set region %0 to? ",
#endif

/* Commands for Moving Cursor and Editing in Input Area */

DEFUN ("toggle-overwrite", FDtoggle_overwrite, FAtoggle_overwrite, to_vptr(toggle_overwrite))
#ifdef FUNC_ARG_STRINGS
"p?",
"p",
#endif

DEFUN_3 ("beginning-of-line", FDbeginning_of_line, to_vptr(beginning_of_line))

DEFUN_3 ("end-of-line", FDend_of_line, to_vptr(end_of_line))

DEFUN ("backward-char", FDbackward_char, FAbackward_char, to_vptr(backward_char))
#ifdef FUNC_ARG_STRINGS
"p",
#endif

DEFUN ("backward-word", FDbackward_word, FAbackward_word, to_vptr(backward_word))
#ifdef FUNC_ARG_STRINGS
"p",
#endif


DEFUN ("backward-delete-char", FDbackward_delete_char, FAbackward_delete_char, to_vptr(backward_delete_char))
#ifdef FUNC_ARG_STRINGS
"p",
#endif

DEFUN ("backward-delete-word", FDbackward_delete_word, FAbackward_delete_word, to_vptr(backward_delete_word))
#ifdef FUNC_ARG_STRINGS
"p",
#endif

DEFUN_3 ("delete-to-start", FDdelete_to_start, to_vptr(delete_to_start))

DEFUN ("forward-char", FDforward_char, FAforward_char, to_vptr(forward_char))
#ifdef FUNC_ARG_STRINGS
"p",
#endif

DEFUN ("forward-word", FDforward_word, FAforward_word, to_vptr(forward_word))
#ifdef FUNC_ARG_STRINGS
"p",
#endif

DEFUN ("delete-char", FDdelete_char, FAdelete_char, to_vptr(delete_char))
#ifdef FUNC_ARG_STRINGS
"p",
#endif

DEFUN ("delete-word", FDdelete_word, FAdelete_word, to_vptr(delete_word))
#ifdef FUNC_ARG_STRINGS
"p",
#endif

DEFUN_3 ("kill-line", FDkill_line, to_vptr(kill_line))

/* Commands for Moving Between Cells from Input Area */

DEFUN ("auto-move-up", FDauto_move_up, FAauto_move_up,
       set_auto_direction)
#ifdef FUNC_ARG_STRINGS
"#0",
#endif

DEFUN ("auto-move-down", FDauto_move_down, FAauto_move_down, to_vptr(set_auto_direction))
#ifdef FUNC_ARG_STRINGS
"#1",
#endif

DEFUN ("auto-move-left", FDauto_move_left, FAauto_move_left,
       to_vptr(set_auto_direction))
#ifdef FUNC_ARG_STRINGS
"#3",
#endif


DEFUN ("auto-move-right", FDauto_move_right, FAauto_move_right,
       to_vptr(set_auto_direction))
#ifdef FUNC_ARG_STRINGS
"#2",
#endif

DEFUN ("auto-move-up-left", FDauto_move_up_left, FAauto_move_up_left,
       to_vptr(set_auto_direction))
#ifdef FUNC_ARG_STRINGS
"#5",
#endif

DEFUN ("auto-move-up-right", FDauto_move_up_right, FAauto_move_up_right,
       to_vptr(set_auto_direction))
#ifdef FUNC_ARG_STRINGS
"#4",
#endif

DEFUN ("auto-move-down-left", FDauto_move_down_left, FAauto_move_down_left,
       to_vptr(set_auto_direction))
#ifdef FUNC_ARG_STRINGS
"#7",
#endif

DEFUN ("auto-move-down-right", FDauto_move_down_right, FAauto_move_down_right,
       to_vptr(set_auto_direction))
#ifdef FUNC_ARG_STRINGS
"#6",
#endif

DEFUN ("auto-move-no-motion", FDauto_move_no_motion, FAauto_move_no_motion,
       to_vptr(set_auto_direction))
#ifdef FUNC_ARG_STRINGS
"#8",
#endif

DEFUN_3 ("auto-move", FDauto_move, to_vptr(auto_move))

DEFUN_3 ("auto-next-set", FDauto_next_set, to_vptr(auto_next_set))

DEFUN ("next-edit", FDnext_edit, FAnext_edit, to_vptr(run_string_as_macro))
#ifdef FUNC_ARG_STRINGS
"={exit-minibuffer}{auto-move}{edit-cell}",
#endif

DEFUN ("next-edit-set", FDnext_edit_set, FAnext_edit_set, to_vptr(run_string_as_macro))
#ifdef FUNC_ARG_STRINGS
"={exit-minibuffer}{auto-next-set}{edit-cell}",
#endif

/* Moving, Copying and Deleting Cells */

DEFUN ("copy-region", FDcopy_region, FAcopy_region, to_vptr(copy_region))
#ifdef FUNC_ARG_STRINGS
"rCopy region? ",
"RCopy %0 to? ",
#endif

DEFUN("copy-this-cell-formula", FDcopy_this_cell_formula, FAcopy_this_cell_formula, copy_this_cell_formula)

DEFUN("paste-this-cell-formula", FDpaste_this_cell_formula, FApaste_this_cell_formula, paste_this_cell_formula)


DEFUN ("copy-row", FDcopy_row, FAcopy_row, copy_row)
#ifdef FUNC_ARG_STRINGS
"nSource row? ",
#endif
	
DEFUN ("copy-values-in-region",
       FDcopy_values_region, FAcopy_values_region, to_vptr(copy_values_region))
#ifdef FUNC_ARG_STRINGS
"rCopy values in region? ",
"RCopy values in %0 to:",
#endif

DEFUN ("move-region",
       FDmove_region, FAmove_region, to_vptr(move_region))
#ifdef FUNC_ARG_STRINGS
"rMove region? ",
"RMove region %0 to? ",
#endif

DEFUN ("move-marked-region",
       FDmove_marked_region, FAmove_marked_region, to_vptr(move_region))
#ifdef FUNC_ARG_STRINGS
"@Move region? ",
"RMove region %0 to? ",
#endif

DEFUN ("insert-row", FDinsert_row, FAinsert_row, to_vptr(insert_row))
#ifdef FUNC_ARG_STRINGS
"+p",
#endif

DEFUN ("insert-col", FDinsert_col, FAinsert_col, to_vptr(insert_col))
#ifdef FUNC_ARG_STRINGS
"+p",
#endif



DEFUN ("delete-row", FDdelete_row, FAdelete_row, to_vptr(delete_row))
#ifdef FUNC_ARG_STRINGS
"+p",
#endif

DEFUN ("delete-col", FDdelete_col, FAdelete_col, to_vptr(delete_col))
#ifdef FUNC_ARG_STRINGS
"+p",
#endif

DEFUN_3 ("delete-cell", FDdelete_cell, to_vptr(kill_cell_cmd))

DEFUN ("delete-region", FDdelete_region, FAdelete_region, to_vptr(delete_region))
#ifdef FUNC_ARG_STRINGS
"rDelete region? ",
#endif

DEFUN ("clear-spreadsheet",
       FDclear_spreadsheet, FAclear_spreadsheet, to_vptr(kill_all_cmd))
#ifdef FUNC_ARG_STRINGS
"MSpreadsheet modified;  clear anyway? (yes or no) ",
#endif

/* Sorting Commands */

DEFUN ("sort-region", FDsort_region_cmd, FAsort_region_cmd,
        to_vptr(sort_region_cmd))
#ifdef FUNC_ARG_STRINGS
"SSort region? ",
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

DEFUN ("set-region-protection", FDset_region_protection,
       FAset_region_protection, to_vptr(set_region_protection))
#ifdef FUNC_ARG_STRINGS
"rSet protection for region? ",
"[dpu][D]efault  [P]rotect  [U]nprotect? ",
#endif

DEFUN ("set-region-alignment", FDset_region_alignment,
       FAset_region_alignment, to_vptr(set_region_alignment))
#ifdef FUNC_ARG_STRINGS
"rSet alignment for region? ",
"[dlcr][D]efault  [L]eft  [C]enter  [R]ight? ",
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

DEFUN ("set-cell-protection",
       FDset_cell_protection, FAset_cell_protection, to_vptr(set_region_protection))
#ifdef FUNC_ARG_STRINGS
".'",
"[dpu][D]efault  [P]rotect  [U]nprotect? ",
#endif

DEFUN ("set-cell-alignment",
       FDset_cell_alignment, FAset_cell_alignment, to_vptr(set_region_alignment))
#ifdef FUNC_ARG_STRINGS
".'",
"[dlcr][D]efault  [L]eft  [C]enter  [R]ight? ",
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

/* Commands For Setting Cell Defaults */

DEFUN ("set-default-height",
       FDset_def_height, FAset_def_height, to_vptr(set_def_height))
#ifdef FUNC_ARG_STRINGS
"sDefault height? ",
#endif

DEFUN ("set-default-width",
       FDset_def_width, FAset_def_width, to_vptr(set_def_width))
#ifdef FUNC_ARG_STRINGS
"sDefault width? ",
#endif

DEFUN ("set-user-scales",
       FDset_user_scales, FAset_user_scales, to_vptr(set_user_scales))
#ifdef FUNC_ARG_STRINGS
"dScale row heights by? ",
"dScale column widths by? ",
#endif

DEFUN ("set-default-protection", FDset_def_protection,
       FAset_def_protection, to_vptr(set_def_protection))
#ifdef FUNC_ARG_STRINGS
"[pu][U]nprotected  or  [P]rotected",
#endif

DEFUN ("set-default-alignment",
       FDset_def_alignment, FAset_def_alignment, to_vptr(set_def_alignment))
#ifdef FUNC_ARG_STRINGS
"[dlcrDLCR][L]eft  [C]enter  [R]ight  [D]efault",
#endif

DEFUN ("set-default-format",
       FDset_def_format, FAset_def_format, to_vptr(set_def_format))
#ifdef FUNC_ARG_STRINGS
"FDefault format? ",
#endif


/* Commands for Inserting Cell Details Into Input Area  */

DEFUN_3 ("insert-cell-expression",
         FDinsert_cell_expression, to_vptr(insert_cell_expression))

DEFUN ("insert-other-cell-expression",
         FDinsert_other_cell_expression, FAinsert_other_cell_expression,
         to_vptr(insert_other_cell_expression))
#ifdef FUNC_ARG_STRINGS
"RInsert expression from? ",
#endif

DEFUN_3 ("insert-cell-value", FDinsert_cell_value, to_vptr(insert_cell_value))

DEFUN ("insert-other-cell-value",
       FDinsert_other_cell_value, FAinsert_other_cell_value,
       to_vptr(insert_other_cell_value))
#ifdef FUNC_ARG_STRINGS
"RInsert value from? ",
#endif

DEFUN_3 ("insert-rel-ref", FDinsert_rel_ref, insert_rel_ref)

DEFUN ("insert-abs-ref", FDinsert_abs_ref,
  FAinsert_abs_ref, insert_abs_ref)
#ifdef FUNC_ARG_STRINGS
"#0",
#endif

DEFUN ("insert-abs-cell-ref", FDinsert_abs_cell_ref,
  FAinsert_abs_cell_ref, insert_abs_ref)
#ifdef FUNC_ARG_STRINGS
"#1",
#endif

DEFUN ("insert-cell-attr", FDinsert_cell_attr, FAinsert_cell_attr, insert_cell_attr)
#ifdef FUNC_ARG_STRINGS
"RInsert attribute of cell? ",
"SInsert %0 attribute? ",
#endif

DEFUN_3 ("insert-current-filename",
	 FDinsert_current_filename, insert_current_filename)

DEFUN_3 ("insert-context-word", FDinsert_context_word, insert_context_word)

DEFUN ("insert-user-format-part",
       FDinsert_usr_fmt_part, FAinsert_usr_fmt_part, insert_usr_fmt_part)
#ifdef FUNC_ARG_STRINGS
"nInsert part of user format [1-16]? ",
"nInsert which part of user format %0 [1-9]? ",
#endif

/* Keyboard Macro and Variable Commands */

DEFUN_3 ("start-entering-macro", FDstart_entering_macro, start_entering_macro)

DEFUN_3 ("stop-entering-macro", FDstop_entering_macro, stop_entering_macro)

DEFUN ("call-last-kbd-macro", FDcall_last_kbd_macro,
       FAcall_last_kbd_macro, call_last_kbd_macro)
#ifdef FUNC_ARG_STRINGS
"p",
#endif

DEFUN_3 ("end-macro", FDend_macro, end_macro)

DEFUN ("store-last-macro", FDstore_last_macro, FAstore_last_macro, store_last_macro)
#ifdef FUNC_ARG_STRINGS
"rStore at? ",
#endif

DEFUN ("run-string-as-macro",
       FDrun_string_as_macro, FArun_string_as_macro, run_string_as_macro)
#ifdef FUNC_ARG_STRINGS
"sRun macro string: ",
#endif

DEFUN ("name-macro-string",
       FDname_macro_string, FAname_macro_string, name_macro_string)
#ifdef FUNC_ARG_STRINGS
"SName for macro? ",
"sMacro %0? ",
#endif

DEFUN ("set-var", FDset_var, FAset_var, set_var)
#ifdef FUNC_ARG_STRINGS
"@Region to define? ",
"SVariable name for %0? ",
#endif

DEFUN ("unset-var", FDunset_var, FAunset_var, unset_var)
#ifdef FUNC_ARG_STRINGS
"SUnset variable name? ",
#endif

/* Commands Affecting Macro Flow */

DEFUN_3 ("exit-minibuffer", FDexit_minibuffer, exit_minibuffer)

DEFUN_3 ("break",  FDbreak, 0)

/* Commands for Running Other Commands */

DEFUN ("exec",
       FDexecute_command, FAexecute_command, execute_command)
#ifdef FUNC_ARG_STRINGS
"CM-x ",
#endif

DEFUN_3 ("universal-argument", FDuniversal_argument, 0)

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
	
DEFUN_5 ("save-spreadsheet", FDwrite_cmd, FAwrite_cmd, DFwrite_cmd, write_cmd)
#ifdef FUNC_ARG_STRINGS
"fwSave spreadsheet? ",
"=%0",
#endif
#ifdef FUNC_INIT_CODE
"{insert-current-filename}",
0,
#endif

DEFUN ("read-variables", FDread_variables, FAread_variables, read_variables)
#ifdef FUNC_ARG_STRINGS
"frRead variables from file? ",
#endif

DEFUN ("read-commands", FDread_cmds_cmd, FAread_cmds_cmd, read_cmds_cmd)
#ifdef FUNC_ARG_STRINGS
"frRead commands from file? ",
#endif

DEFUN("shell", FDrun_shell, FArun_shell, run_shell)
#ifdef FUNC_ARG_STRINGS
"sRun shell command file? ",
#endif


DEFUN ("toggle-load-hooks",
       FDtoggle_load_hooks, FAtoggle_load_hooks, toggle_load_hooks)
#ifdef FUNC_ARG_STRINGS
"p?",
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

DEFUN ("merge-spreadsheet",
       FDread_merge_cmd, FAread_merge_cmd, read_merge_cmd)
#ifdef FUNC_ARG_STRINGS
"frMerge spreadsheet? ",
"MSpreadsheet modified; merge anyway? (yes or no) ",
#endif

DEFUN ("write-region-to-file",
       FDwrite_reg_cmd, FAwrite_reg_cmd, write_reg_cmd)
#ifdef FUNC_ARG_STRINGS
"rWrite region? ",
"fwWrite region to file? ",
#endif



/* Commands that Trigger Calculation */

DEFUN ("recalculate", FDrecalculate, FArecalculate, recalculate)
#ifdef FUNC_ARG_STRINGS
"p?",
#endif

/* Window Commands */

DEFUN ("open-window", FDopen_window, FAopen_window, open_window)
#ifdef FUNC_ARG_STRINGS
"sSplit command? ",
#endif

DEFUN_3 ("split-window-horizontally", FDhsplit_window, hsplit_window)

DEFUN_3 ("split-window-vertically", FDvsplit_window, vsplit_window)

DEFUN_3 ("delete-window", FDdelete_window, delete_window)

DEFUN_3 ("delete-other-windows", FDdelete_other_windows, delete_other_windows)

DEFUN ("close-window", FDclose_window, FAclose_window, close_window)
#ifdef FUNC_ARG_STRINGS
"NClose window number? ",
#endif

DEFUN ("goto-window", FDgoto_window, FAgoto_window, goto_window)
#ifdef FUNC_ARG_STRINGS
"NGoto window number? ",
#endif

DEFUN_3 ("goto-minibuffer", FDgoto_minibuffer, goto_minibuffer)

DEFUN_3 ("other-window", FDother_window, other_window)
#ifdef FUNC_ARG_STRINGS
"+p",
#endif



DEFUN ("set-option", FDset_option, FAset_option, set_options)
#ifdef FUNC_ARG_STRINGS
"sSet option? ",
#endif

DEFUN ("show-variable", FDshow_variable, FAshow_variable, show_var)
#ifdef FUNC_ARG_STRINGS
"SShow Variable? ",
#endif

DEFUN ("write-keys", FDwrite_keys_cmd, FAwrite_keys_cmd, write_keys_cmd)
#ifdef FUNC_ARG_STRINGS
"fwWrite bindings to file? ",
#endif

DEFUN ("write-variables",
       FDwrite_variables, FAwrite_variables, write_variables)
#ifdef FUNC_ARG_STRINGS
"fwWrite variables to file? ",
#endif

/* Keymap Commands */

DEFUN ("create-keymap", FDcreate_keymap, FAcreate_keymap, create_keymap)
#ifdef FUNC_ARG_STRINGS
"SCreate Keymap? ",
"S'Parent map? ",
#endif

DEFUN ("set-map-prompt", FDset_map_prompt, FAset_map_prompt, set_map_prompt)
#ifdef FUNC_ARG_STRINGS
"KSet prompt for keymap? ",
"sPrompt for %1? ",
#endif

DEFUN ("bind-key", FDbind_key, FAbind_key, bind_key)
#ifdef FUNC_ARG_STRINGS
"KKeymap? ",
"SCommand? ",
"cCharacter? ",
#endif

DEFUN ("unbind-key", FDunbind_key, FAunbind_key, unbind_key)
#ifdef FUNC_ARG_STRINGS
"KKeymap? ",
"cCharacter? ",
#endif

DEFUN ("bind-set", FDbind_set, FAbind_set, bind_or_unbind_set)
#ifdef FUNC_ARG_STRINGS
"KKeymap? ",
"SCommand? ",
"sCharacters? ",
#endif

DEFUN ("unbind-set", FDunbind_set, FAunbind_set, unbind_set)
#ifdef FUNC_ARG_STRINGS
"KKeymap? ",
"sCharacters? ",
#endif

DEFUN ("bind-all-keys", FDbind_all_keys, FAbind_all_keys, bind_all_keys)
#ifdef FUNC_ARG_STRINGS
"KKeymap? ",
"SCommand? ",
#endif

DEFUN ("with-keymap", FAwith_keymap, FDwith_keymap, with_keymap)
#ifdef FUNC_ARG_STRINGS
"SWith which keymap? ",
"m%0",
#endif

DEFUN ("one-command-with-keymap", FAone_cmd_with_keymap,
       FDone_cmd_with_keymap, one_cmd_with_keymap)
#ifdef FUNC_ARG_STRINGS
"SWith which keymap? ",
"m'%0",
#endif

/* Quite Low Level Commands */

DEFUN ("self-insert-command",
       FDself_insert_command, FAself_insert_command, self_insert_command)
#ifdef FUNC_ARG_STRINGS
"l",
"p",
#endif

DEFUN ("self-map-command",
       FDself_map_command, FAself_map_command, self_map_command)
#ifdef FUNC_ARG_STRINGS
"l",
#endif

DEFUN ("exit-self-inserting",
       FDexit_self_inserting, FAexit_self_inserting, exit_self_inserting)
#ifdef FUNC_ARG_STRINGS
"l",
#endif

DEFUN ("pushback-keystroke",
       FApushback_keystroke, FDpushback_keystroke, pushback_keystroke)
#ifdef FUNC_ARG_STRINGS
"c'(any key continues) ",
#endif

DEFUN ("display-msg",
       FDdisplay_msg, FAdisplay_msg, display_msg)
#ifdef FUNC_ARG_STRINGS
"sMessage? ",
"c'%0",
#endif

DEFUN ("error-msg",
       FDerror_msg, FAerror_msg, display_msg)
#ifdef FUNC_ARG_STRINGS
"sMessage? ",
"c!%0",
#endif


DEFUN ("bload", FDbload, FAbload, bload)
#ifdef FUNC_ARG_STRINGS
"frLoad neo-basic? ",
#endif
