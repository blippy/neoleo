#pragma once
/*
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

/* This file repeatedly includes the contents of defun.h which contains
 * doc strings and FUNC_ARGS structures for all of the interactive
 * built-ins.
 * 
 * It uses ugly preprocessor magic to build various tables.
 */


struct cmd_func** get_cmd_funcs();
void rewrite_defuns();
