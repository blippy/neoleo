/*
 * Copyright (c) 1992, 1993, 1999 Free Software Foundation, Inc.
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

#include <string.h>

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <sstream>

#undef NULL

#include "global.h"
#include "cell.h"
#include "io-utils.h"
#include "io-edit.h"
#include "io-abstract.h"
#include "format.h"
#include "sheet.h"
#include "regions.h"
#include "spans.h"
#include "utils.h"

#define the_do_prompt		the_cmd_arg.do_prompt
#define the_is_set		the_cmd_arg.is_set


	int
check_editting_mode (void)
{
	return 0;
}













#undef the_text
#undef the_cursor
#undef the_do_prompt
