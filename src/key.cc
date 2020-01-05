/*
 * Copyright (c) 1993, 2001 Free Software Foundation, Inc.
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
#include <ctype.h>
#include <string.h>

#include "key.h"
#include "cmd.h"
#include "io-abstract.h"
#include "io-generic.h"
#include "io-utils.h"
#include "io-term.h"
#include "utils.h"

using RPTR = rng*;

struct keymap **the_maps;
char **map_names;
char **map_prompts;
int num_maps;
struct cmd_func **the_funcs;
int num_funcs;




	int 
map_idn (const char *name, int n)
{
	int x;
	for (x = 0; x < num_maps; ++x)
		if (!strincmp (name, map_names[x], n))
			return x;
	return -1;
}

	int
map_id(const char *name)
{
	return map_idn(name, strlen(name));
}
