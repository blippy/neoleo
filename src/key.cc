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

#include "funcs.h"
#include "key.h"
#include "cmd.h"
#include "io-abstract.h"
#include "io-generic.h"
#include "io-utils.h"
#include "io-term.h"
#include "utils.h"
#include "xcept.h"

using RPTR = rng*;

struct keymap **the_maps;
char **map_names;
char **map_prompts;
int num_maps;
struct cmd_func **the_funcs;
int num_funcs;



	static void 
do_bind_key (struct keymap *m, int key, int vector, int code)
{
	ASSERT_UNCALLED();
	m->keys[key].vector = (short)vector;
	m->keys[key].code = (short)code;
}


	void 
bind_key (char * keymap, char * function, int ch)
{
	ASSERT_UNCALLED();
	struct cmd_func * tmpfunc;
	int map = map_id (keymap);
	int vec;
	int code;
	struct rng rng;
	struct cmd_func * cmd;

	if (map < 0)
	{
		io_error_msg ("bind_key: bad keymap.");
		return;
	}

	if (!find_func (&vec, &cmd, function))
	{
		code = (cmd - the_funcs[vec]);
		goto fini;
	}

	for (code = 0; code < num_maps; code++)
	{
		if (!strcmp (function, map_names[code]))
		{
			vec = -1;
			goto fini;
		}
	}
	if (get_abs_rng (&function, &rng))
	{
		io_error_msg ("Unknown command '%s'", function);
		return;
	}

	for (code = 0; code < n_bound_macros; code++)
	{
		if (!bcmp (&bound_macros[code], &rng, sizeof (struct rng)))
			goto fini;
	}

	function = range_name (&rng);
	if (!bound_macro_vec)
	{
		bound_macros = (RPTR) ck_malloc (sizeof (struct rng));
		n_bound_macros = 1;
		tmpfunc = (struct cmd_func *)ck_malloc (2 * sizeof (struct cmd_func));
		bzero (tmpfunc + 1, sizeof (*tmpfunc));
		bound_macro_vec = add_usr_cmds (tmpfunc);
	}
	else
	{
		n_bound_macros++;
		bound_macros = (RPTR) ck_realloc (bound_macros,
				n_bound_macros * sizeof (struct rng));
		the_funcs[bound_macro_vec]
			= (cmd_func*)ck_realloc (the_funcs[bound_macro_vec],
					(1 + n_bound_macros) * sizeof (struct cmd_func));
		tmpfunc = &the_funcs[bound_macro_vec][n_bound_macros - 1];
		bzero (tmpfunc + 1, sizeof (*tmpfunc));
	}
	vec = bound_macro_vec;
	bound_macros[n_bound_macros - 1] = rng;
	tmpfunc->func_args = (char **)ck_malloc (2 * sizeof (char *));
	tmpfunc->func_args[0] = mk_sprintf ("#%d", n_bound_macros - 1);
	tmpfunc->func_args[1] = 0;
	tmpfunc->init_code = 0;
	tmpfunc->func_name = ck_savestr (function);
	tmpfunc->func_func = to_vptr(bound_macro);
fini:
	do_bind_key (the_maps[map], ch, vec, code);
}

	void
unbind_key (char * keymap, int ch)
{
	ASSERT_UNCALLED();
	int map = map_id (keymap);
	struct keymap *m;
	m = the_maps[map];
	m->keys[ch].vector = -1;
	m->keys[ch].code = -1;
}

	void
bind_or_unbind_set (char * keymap, char * command, char * keyset)
{
	ASSERT_UNCALLED();
	int first;
	int last;
	first = string_to_char (&keyset);
	if (first < 0)
	{
		io_error_msg ("Invalid character set.");
		return;
	}
	while (isspace (*keyset))
		++keyset;
	if (*keyset == '-')
	{
		++keyset;
		last = string_to_char (&keyset);
		if (last < 0)
		{
			io_error_msg ("Unterminated character set.");
			return;
		}
	}
	else if (*keyset)
	{
		io_error_msg ("Extra characters in keyset: `%s'.",
				keyset);
		return;
	}
	else
		last = first;

	while (first <= last)
	{
		if (command != "0")
			bind_key (keymap, command, first);
		else
			unbind_key (keymap, first);
		++first;
	}
}

void unbind_set (char *keymap, char *keyset)
{
	ASSERT_UNCALLED();
	char *command = const_cast<char*>("0");
	bind_or_unbind_set (keymap, command, keyset);
}

	void 
bind_all_keys (char * keymap, char * function)
{
	ASSERT_UNCALLED();
	struct cmd_func * tmpfunc;
	int map = map_id (keymap);
	int vec;
	int code;
	struct rng rng;
	struct cmd_func * cmd;

	if (map < 0)
	{
		io_error_msg ("bind_key: bad keymap.");
		return;
	}

	if (!find_func (&vec, &cmd, function))
	{
		code = (cmd - the_funcs[vec]);
		goto fini;
	}

	for (code = 0; code < num_maps; code++)
	{
		if (!strcmp (function, map_names[code]))
		{
			vec = -1;
			goto fini;
		}
	}
	if (get_abs_rng (&function, &rng))
	{
		io_error_msg ("Unknown command '%s'", function);
		return;
	}

	vec = bound_macro_vec;
	for (code = 0; code < n_bound_macros; code++)
	{
		if (!bcmp (&bound_macros[code], &rng, sizeof (struct rng)))
			goto fini;
	}

	function = range_name (&rng);
	if (!bound_macro_vec)
	{
		bound_macros = (RPTR) ck_malloc (sizeof (struct rng));
		n_bound_macros = 1;
		tmpfunc = (struct cmd_func *)ck_malloc (2 * sizeof (struct cmd_func));
		bzero (tmpfunc + 1, sizeof (*tmpfunc));
		bound_macro_vec = add_usr_cmds (tmpfunc);
	}
	else
	{
		n_bound_macros++;
		bound_macros = (RPTR) ck_realloc (bound_macros,
				n_bound_macros * sizeof (struct rng));
		the_funcs[bound_macro_vec]
			= (cmd_func*) ck_realloc (the_funcs[bound_macro_vec],
					(1 + n_bound_macros) * sizeof (struct cmd_func));
		tmpfunc = &the_funcs[bound_macro_vec][n_bound_macros - 1];
		bzero (tmpfunc + 1, sizeof (*tmpfunc));
	}
	bound_macros[n_bound_macros - 1] = rng;
	tmpfunc->func_args = (char **)ck_malloc (2 * sizeof (char *));
	tmpfunc->func_args[0] = mk_sprintf ("#%d", n_bound_macros - 1);
	tmpfunc->func_args[1] = 0;
	tmpfunc->func_name = ck_savestr (function);
	tmpfunc->func_func = to_vptr(bound_macro);
fini:
	{
		int ch;
		for (ch = 0; ch < OLEO_NUM_KEYS; ++ch)
			do_bind_key (the_maps[map], ch, vec, code);
	}
}


	void 
write_keys_cmd (FILE *fp)
{
	ASSERT_UNCALLED();
	struct keymap *map;
	int n;
	int key;
	int vec;
	int code;

	for (n = 0; n < num_maps; n++)
	{
		char *def;
		map = the_maps[n];
		def = 0;
		if (map && map->map_next)
		{
			for (key = 0; key < num_maps; key++)
				if (the_maps[key] == map->map_next)
				{
					def = map_names[key];
					break;
				}
		}
		if (def)
			fprintf (fp, "create-keymap %s %s\n", map_names[n], def);
		else
			fprintf (fp, "create-keymap %s\n", map_names[n]);
	}
	for (n = 0; n < num_maps; n++)
	{
		map = the_maps[n];
		for (key = 0; key < OLEO_NUM_KEYS; key++)
		{
			vec = map->keys[key].vector;
			code = map->keys[key].code;
			if (vec < 0 && code >= 0)
				fprintf (fp, "bind-key %s %s %s\n",
						map_names[n],
						map_names[code],
						char_to_string (key));
			else if (vec >= 0)
				fprintf (fp, "bind-key %s %s %s\n",
						map_names[n],
						the_funcs[vec][code].func_name,
						char_to_string (key));
		}
	}
}



	void 
clear_keymap (struct keymap *m)
{
	ASSERT_UNCALLED();
	int n;
	for (n = 0; n < OLEO_NUM_KEYS; n++)
	{
		m->keys[n].vector = -1;
		m->keys[n].code = -1;
	}
}

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

	void
create_keymap (const char * mapname, char * parentname)
{
	ASSERT_UNCALLED();
	int map = map_id (mapname);
	int parent = parentname ? map_id (parentname) : -1;

	if (map >= 0)
	{
		io_info_msg ("Map %s already exists.",  mapname);
		return;
	}
	if (parentname && (parent < 0))
	{
		io_error_msg ("Map %s does not exist.", parentname);
		return;
	}
	the_maps = (keymap**)ck_realloc (the_maps, (num_maps + 1) * sizeof (struct keymap *));
	the_maps[num_maps] = (keymap*) ck_malloc (sizeof (struct keymap));
	the_maps[num_maps]->id = num_maps;
	the_maps[num_maps]->map_next = ((parent >= 0)
			? the_maps[parent]
			: 0);
	{
		int c;
		for (c = 0; c < OLEO_NUM_KEYS; ++c)
		{
			the_maps[num_maps]->keys[c].vector = -1;
			the_maps[num_maps]->keys[c].code = -1;
		}
	}
	map_names = (char**) ck_realloc (map_names, (num_maps + 1) * sizeof (char *));
	map_prompts = (char**) ck_realloc (map_prompts, (num_maps + 1) * sizeof (char *));
	map_names[num_maps] = ck_savestr (mapname);
	map_prompts[num_maps] = 0;
	num_maps++;
}


	void
set_map_prompt (char * map, char * str)
{
	ASSERT_UNCALLED();
	int id = map_id (map);
	if (id < 0)
		io_error_msg ("No such keymap as %s.", map); /* No return. */
	if (map_prompts[id])
		free (map_prompts [id]);
	map_prompts[id] = ck_savestr (str);
}
