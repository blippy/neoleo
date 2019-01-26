/*
 * $Id: init.c,v 1.10 2001/02/14 19:22:57 danny Exp $
 *
 * Copyright � 1993, 2000, 2001 Free Software Foundation, Inc.
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


#include <stdlib.h>

#include <string.h>

#include "init.h"
#include "global.h"
#include "io-term.h"
#include "utils.h"
#include "cmd.h"
/*
 * These commands are run before we are ready to do output.  They
 * should not do any kind of io or cause errors.  If they do, you
 * will experience core dumps.
 */
static char *init_cmds[] =
{
  "",
  "# The keymap tree.",
  "",
  "# The map `universal' already exists, and is always the keymap of last",
  "# resort.  It is almost always a bad idea to add new bindings there.",
  "",
  "# This is the generic top-level keymap.   It should contain bindings that",
  "# apply at the top level keymap regardless of what mode the editor is in.",
  "# All keymaps that are used at the top level should inherit from this.",
  "create-keymap generic-main universal",
  "",
  "# This is the keymap in force at the top level when no command arguments are",
  "# being edited.",
  "create-keymap main generic-main",
  "",
  "# The keymap after a M- or ESC prefix in main:",
  "create-keymap generic-meta universal",
  "create-keymap meta generic-meta",
  "",
  "# The keymap for ESC [, Arrow keys are bound in this map.",
  "create-keymap generic-ansi universal",
  "create-keymap ansi generic-ansi",
  "",
  "create-keymap generic-trolx universal",
  "create-keymap trolx generic-trolx",
  "",
  "# Bindings that define prefix keys.",
  "bind-key generic-main generic-meta ^[",
  "bind-key main meta ^[",
  "",
  "create-keymap mouse universal",
  "bind-key generic-main mouse ^\\\\",
  "bind-key mouse mouse-goto 0",
  "bind-key mouse mouse-mark 1",
  "bind-key mouse mouse-mark-and-goto 2",
  "",
  "",
  "bind-key generic-main generic-trolx ^x",
  "bind-key main trolx ^x",
  "",
  "bind-key generic-meta generic-ansi [",
  "bind-key meta ansi [",
  "",
  "# Misc generic commands",
  "",
  "bind-key universal break ^g",
  "bind-key universal redraw-screen ^l",
  
  "bind-key generic-main down-cell ^m",
  "bind-key generic-meta show-menu m",
  "bind-key generic-main kill-oleo ^q", // mcarter 28-Aug-2017 issue#29
  "bind-key generic-main universal-argument ^u",
  "bind-key generic-main suspend-oleo ^z",
  
  "bind-key generic-trolx kill-oleo ^c",
  "bind-key generic-trolx recalculate !",
  "bind-key generic-meta exec x",
  "",
  "# Prefix argument handling",
  "# This keymap is magicly invoked within command_loop in cmd.c.",
  "create-keymap prefix universal",
  "bind-set prefix universal-argument 0-9",
  "bind-key prefix universal-argument -",
  "",
  "#",
  "# Cursor motion",
  "#",
  "",
  "# For EMACS users",
  "# Notice that these motion commands are not bound genericly.  They don't",
  "# in general apply if the input area is active.",
  "",
  "bind-key main    up-cell  ^p",
  "bind-key main right-cell  ^f",
  "bind-key main  left-cell  ^b",
  "bind-key main  down-cell  ^n",
  "",

  "# Other functions for main",
  "",
  "bind-key main copy-this-cell-formula ^c", // mcarter 17-Aug-2017 issue#19
  "bind-key main save-spreadsheet ^s", // mcarter 20-Aug-2017 RFE#24
  "bind-key main paste-this-cell-formula ^v", // mcarter 17-Aug-2017 issue#19
  "",
  "# Alignment",
  "",
  "bind-key main set-cell-alignment-left ^l",
  "bind-key main set-cell-alignment-right ^r",
  "",

  "# Ansi motion.",
  "bind-key generic-ansi delete-cell 3", // RFE#20 delete key should delete a cell
  "bind-key generic-ansi scroll-up 5",
  "bind-key generic-ansi scroll-down 6",
  "bind-key generic-ansi up-cell A",
  "bind-key generic-ansi down-cell B",
  "bind-key generic-ansi right-cell C",
  "bind-key generic-ansi left-cell D",
  "bind-key generic-ansi beginning-of-row H",
  "",
  "create-keymap edit-ansi universal",
  "bind-key edit-ansi delete-char 3",
  "bind-key edit-ansi forward-char C",
  "bind-key edit-ansi backward-char D",
  "bind-key edit-ansi end-of-line F",
  "bind-key edit-ansi beginning-of-line H",
  "",
  "create-keymap meta-edit-ansi universal",
  "bind-key edit-ansi meta-edit-ansi ^[",
  "",
  "bind-key trolx goto-cell j",
  "bind-key meta goto-region j",
  "",
  "# Scrolling commands.",
  "bind-key meta scroll-up v",
  "bind-key trolx scroll-right >",
  "bind-key trolx scroll-left <",
  "",
  "# Other ways of moving the cell cursor.",
  "",
  "bind-key meta scan-up p",
  "bind-key meta scan-down n",
  "bind-key meta scan-left b",
  "bind-key meta scan-right f",
  "",
  "bind-key main beginning-of-row ^a",
  "bind-key meta beginning-of-col ^a",
  "",
  "# Macros",
  "bind-key generic-trolx start-entering-macro (",
  "bind-key generic-trolx stop-entering-macro )",
  "bind-key generic-trolx call-last-kbd-macro e",
  "",
  "# The keymap used when prompting for a key-sequence: ",
  "create-keymap read-keyseq universal",
  "bind-set read-keyseq self-map-command \\000-\\377",
  "",

  "",
  "# Variables",
  "bind-key trolx set-var :",
  "",
  "# Files",
  "",
  "bind-key trolx find-alternate-spreadsheet ^v",
  "bind-key trolx merge-spreadsheet i",
  "",
  "bind-key meta copy-region c",
  "bind-key main insert-row ^o",
  "bind-key meta insert-col o",
  "bind-key main delete-row ^k",
  "bind-key meta delete-col k",
  "",
  "# Changing cell formulas",
  "bind-key main set-cell =",
  "",
  "# Some characters start editing the current cell and then are reinterpreted",
  "# as editing commands:",
  "bind-set main goto-edit-cell a-z",
  "bind-set main goto-edit-cell A-Z",
  "bind-set main goto-edit-cell 0-9",
  "bind-key main goto-edit-cell \\",
  "bind-key main goto-edit-cell +",
  "bind-key main goto-edit-cell -",
  "bind-key main goto-edit-cell *",
  "bind-key main goto-edit-cell /",
  "bind-key main goto-edit-cell (",
  "bind-key main goto-edit-cell )",
  "bind-key main goto-edit-cell @",
  "bind-key main goto-edit-cell ^",
  "bind-key main goto-edit-cell %",
  "bind-key main goto-edit-cell >",
  "bind-key main goto-edit-cell <",
  "bind-key main goto-edit-cell !",
  "bind-key main goto-edit-cell ?",
  "bind-key main goto-edit-cell :",
  "bind-key main goto-edit-cell #",
  "bind-key main goto-edit-cell _",
  "bind-key main goto-edit-cell .",
  "bind-key main goto-edit-cell ,",
  "bind-key main goto-edit-cell ;",
  "bind-key main goto-edit-cell  \"",
  "bind-key meta set-region-formula =",
  "bind-key main delete-cell ^?",
  "bind-key meta delete-region ^?",
  "",
  "",
  "# Window commands",
  "",
  "bind-key meta recenter-window ^l",
  "bind-key trolx split-window-horizontally 5",
  "bind-key trolx split-window-vertically 2",
  "bind-key trolx delete-window 0",
  "bind-key trolx delete-other-windows 1",
  "bind-key meta goto-minibuffer ^",
  "bind-key trolx other-window o",
  "",
  "# generic frame bindings",
  "create-keymap generic-frame universal",
  "bind-set generic-frame self-insert-command 0-9",
  "bind-set generic-frame self-insert-command a-z",
  "bind-set generic-frame self-insert-command A-Z",
  "bind-set generic-frame self-insert-command .",
  "bind-set generic-frame self-insert-command -",
  "bind-set generic-frame self-insert-command _",
  "bind-key generic-frame backward-char D", // left arrow
  "bind-key generic-frame forward-char C", // right arrow
  "bind-key generic-frame backward-delete-char ^?", // Backspace key
  "bind-key generic-frame delete-char 3", // Delete key
  "bind-key generic-frame end-of-line F", // End key
  "bind-key generic-frame beginning-of-line H", // Home key
  "",
  "# Command argument editing",
  "",
  "create-keymap read-string generic-main",
  "create-keymap meta-read-string generic-meta",
  "create-keymap trolx-read-string generic-trolx",
  "",
  "bind-key read-string exit-minibuffer \\015", // return key
  "bind-key read-string meta-read-string ^[",
  "bind-set read-string self-insert-command \\ -~",
  "bind-set read-string self-insert-command �-�",
  "bind-key read-string exit-minibuffer ^m",
  "bind-key read-string beginning-of-line ^a",
  "bind-key read-string end-of-line ^e",
  "bind-key read-string backward-char ^b",
  "bind-key meta-read-string backward-word b",
  "bind-key read-string backward-delete-char ^?",
  "bind-key meta-read-string backward-delete-word ^?",
  "bind-key read-string forward-char ^f",
  "bind-key meta-read-string forward-word f",
  "bind-key read-string delete-char ^d",
  "bind-key meta-read-string delete-word d",
  "bind-key read-string kill-line ^k",
  "bind-key meta-read-string insert-cell-expression e",
  "bind-key meta-read-string insert-cell-value v",
  "bind-key meta-read-string insert-abs-ref a",
  "bind-key meta-read-string toggle-overwrite o",
  "",
  "bind-key read-string trolx-read-string ^x",
  "bind-key trolx-read-string other-window o",
  "",
  "# Reading various argument types specificly",
  "create-keymap read-symbol read-string",
  "create-keymap read-word read-string",
  "create-keymap read-range read-string",
  "create-keymap read-integer read-string",
  "create-keymap read-float read-string",
  "",
  "# The read-formula keymap",
  "create-keymap read-formula read-string",
  "create-keymap meta-read-formula meta-read-string",
  "bind-key read-formula meta-read-formula ^[",
  "bind-key meta-read-formula edit-ansi [",
  "",
  "# The read-filename keymap",
  "create-keymap read-filename read-string",
  "create-keymap meta-read-filename meta-read-string",
  "bind-key read-filename meta-read-filename ^[",
  "bind-key meta-read-filename edit-ansi [",
  "",

  "# auto-motion",
  "create-keymap set-auto-motion universal",
  "set-map-prompt set-auto-motion \\",
  "Set auto-motion direction [^v<>`'/\\ or SPC for none] ",
  "",
  "bind-key generic-trolx set-auto-motion m",
  "",
  "bind-key set-auto-motion auto-move-up ^",
  "bind-key set-auto-motion auto-move-down v",
  "bind-key set-auto-motion auto-move-down V",
  "bind-key set-auto-motion auto-move-left <",
  "bind-key set-auto-motion auto-move-left ,",
  "bind-key set-auto-motion auto-move-right >",
  "bind-key set-auto-motion auto-move-right .",
  "bind-key set-auto-motion auto-move-up-right '",
  "bind-key set-auto-motion auto-move-up-left `",
  "bind-key set-auto-motion auto-move-down-right \\\\   ",
  "bind-key set-auto-motion auto-move-down-left  /",
  "bind-key set-auto-motion auto-move-no-motion \\ ",
  "",
  "bind-key read-formula next-edit ^i",
  "bind-key read-formula next-edit-set ^j",
  "",
  "create-keymap read-most-chars universal",
  "",
  "bind-set read-most-chars exit-self-inserting \\000-\\377",
  "bind-key read-most-chars break ^G",
  "bind-key read-most-chars redraw-screen ^L",
  "",
  "create-keymap press-any universal",
  "set-map-prompt press-any \\",
  "Press any key to continue. ",
  "bind-set press-any exit-minibuffer \\000-\\377",
  "bind-key press-any break ^G",
  "bind-key press-any redraw-screen ^L",
  "",
  "create-keymap read-char read-most-chars",
  "bind-key read-char exit-self-inserting ^G",
  "",
  "",
  "create-keymap read-format read-string",
  "",
  "# These maps are used to manipulate cell attributes.",
  "# They reimplement the old format-cell, format-region and",
  "# set-default commands.",
  "",
  "create-keymap set-cell-attr 	universal",
  "create-keymap set-region-attr 	universal",
  "create-keymap set-default-attr 	universal",
  "",
  "set-map-prompt set-cell-attr \\",
  "Set cell [A]lignment [F]ormat f[O]nt [P]rotection [H]eight [W]idth",
  "set-map-prompt set-region-attr \\",
  "Set region [A]lignment [F]ormat f[O]nt [P]rotection [H]eight [W]idth",
  "set-map-prompt set-default-attr \\",
  "Set default [A]lignment [F]ormat f[O]nt [P]rotection [H]eight [W]idth",
  "",
  "bind-key meta set-default-attr d",
  "bind-key meta set-region-attr r",
  "bind-key meta set-cell-attr a",
  "",
  "bind-key set-cell-attr	set-cell-alignment	a",
  "bind-key set-cell-attr	set-cell-format		f",
  "bind-key set-cell-attr	set-cell-protection	p",
  "bind-key set-cell-attr	set-cell-height		h",
  "bind-key set-cell-attr	set-cell-width		w",
  "bind-key set-cell-attr	set-cell-alignment	A",
  "bind-key set-cell-attr	set-cell-format		F",
  "bind-key set-cell-attr	set-cell-protection	P",
  "bind-key set-cell-attr	set-cell-height		H",
  "bind-key set-cell-attr	set-cell-width		W",
  "",
  "bind-key set-region-attr	set-region-alignment		a",
  "bind-key set-region-attr	set-region-format		f",
  "bind-key set-region-attr	set-region-protection		p",
  "bind-key set-region-attr	set-region-height		h",
  "bind-key set-region-attr	set-region-width		w",
  "bind-key set-region-attr	set-region-alignment		A",
  "bind-key set-region-attr	set-region-format		F",
  "bind-key set-region-attr	set-region-protection		P",
  "bind-key set-region-attr	set-region-height		H",
  "bind-key set-region-attr	set-region-width		W",
  "",
  "bind-key set-default-attr	set-default-alignment		a",
  "bind-key set-default-attr	set-default-format		f",
  "bind-key set-default-attr	set-default-protection		p",
  "bind-key set-default-attr	set-default-height		h",
  "bind-key set-default-attr	set-default-width		w",
  "bind-key set-default-attr	set-default-alignment		A",
  "bind-key set-default-attr	set-default-format		F",
  "bind-key set-default-attr	set-default-protection		P",
  "bind-key set-default-attr	set-default-height		H",
  "bind-key set-default-attr	set-default-width		W",
  "",
  "",
  "# Verbose menu type keymaps use exit-minibuffer to move closer",
  "# to the root of the menu.",
  "",
  "create-keymap generic-menu universal",
  "bind-key generic-menu exit-minibuffer ^[",
  "bind-key generic-menu exit-minibuffer ^m",
  "bind-key generic-menu exit-minibuffer ^j",
  "",



  //"set-option a0",

  0

};

void 
run_init_cmds (void)
{
	char **p = init_cmds;
	while (*p)
	{
		char * cmd = strdup (*p++);
		int len = strlen (cmd);
		while (len && *p && (cmd [len - 1] == '\\')
				&& ((len == 1) || cmd[len - 2] != '\\'))
		{
			cmd [len - 1] = '\0';
			len += strlen (*p);
			cmd = (char*) ck_realloc (cmd, len + 1);
			strcat (cmd, *p);
			++p;
		}
		//puts(cmd);
		execute_command (cmd);
		free (cmd);
	}
}
