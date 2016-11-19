#ifndef UTILSH
#define UTILSH

/*
 * $Id: utils.h,v 1.4 2000/08/10 21:02:51 danny Exp $
 *
 * Copyright © 1992, 1993 Free Software Foundation, Inc.
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
#include <stdio.h>
#include "funcdef.h"
#include "global.h"

extern char *argv_name;
extern int __make_backups;
extern int __backup_by_copying;

extern VOIDSTAR ck_malloc (size_t);
extern VOIDSTAR ck_calloc (size_t);
extern VOIDSTAR ck_realloc (void *,size_t);
extern char * ck_savestr (char *);
extern char * ck_savestrn (char *, int);
extern void ck_free (VOIDSTAR);
#define ck_remalloc(OLD, SIZE) \
  ((OLD) ? ck_realloc ((OLD), (SIZE)) : ck_malloc (SIZE))

extern void get_usr_stats (int, char **);
extern void set_usr_stats (int, char **);

extern char *char_to_string (int);
extern int string_to_char (char **);
extern FILE *xopen_with_backup (const char *,const char *);
extern int xclose (FILE *);
extern char *err_msg (void);
extern char *mk_sprintf (char *, ...);

extern void init_mem (void);
extern void init_eval (void);
extern void init_refs (void);
extern void init_cells (void);
extern VOIDSTAR init_stack (void);

extern VOIDSTAR pop_stack (VOIDSTAR);
extern void push_stack (VOIDSTAR, VOIDSTAR);
extern void flush_stack (VOIDSTAR);

#endif
