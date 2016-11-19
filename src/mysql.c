/*
 *  $Id: mysql.c,v 1.10 2000/07/22 06:13:16 danny Exp $
 *
 *  This file is part of Oleo, the GNU spreadsheet.
 *
 *  Copyright © 1999-2000 by the Free Software Foundation, Inc.
 *  Written by Danny Backx <danny@gnu.org>.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

static char rcsid[] = "$Id: mysql.c,v 1.10 2000/07/22 06:13:16 danny Exp $";

#ifdef	HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include "global.h"
#include "oleosql.h"
#include "cmd.h"
#include "io-term.h"
#include "mysql.h"

#ifdef	HAVE_LIBMYSQLCLIENT

#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>

/*
 * Define some MySQL access functions for Oleo
 */

#include "funcdef.h"
#include "sysdef.h"

#include "global.h"
#include "cell.h"
#include "eval.h"
#include "errors.h"

/*
 * Declarations necessary for defining an Oleo function
 */
struct value
  {
    int type;
    union vals x;
  };

#define Float   x.c_d
#define String  x.c_s
#define Int     x.c_l
#define Value   x.c_i
#define Rng     x.c_r

/*
 * Forward declarations
 */
static void do_mysql_query(struct value *);
static void do_mysql_read(struct value *);
static void do_mysql_write(struct value *);

/*
 * Define Oleo functions
 */
struct function mysql_functions[] = {
	{ C_FN1, X_A1, "S", do_mysql_query, "mysql_query" },
	{ C_FN1, X_A1, "S", do_mysql_read, "mysql_read" },
	{ C_FN1, X_A1, "S", do_mysql_write, "mysql_write" },
	{ 0, 0, "", 0, 0 }
};

int init_mysql_function_count(void) 
{
        return sizeof(mysql_functions) / sizeof(struct function) - 1;
}

/*
 * This function should just retrieve a single value from the dbms.
 */
static void
do_mysql_query(struct value *p)
{
	char		*sql = p[0].String;
	MYSQL		db;
	MYSQL_RES	*res;
	MYSQL_ROW	row;
	MYSQL_FIELD	*field;
	int		r, i, j, nrows;
	char		*result;
	double		d;

	AllocateDatabaseGlobal();

	if (Global->DatabaseGlobal == NULL) {
		io_error_msg("Need to initialize database");
		return;
	}
	if (Global->DatabaseGlobal->name == 0 || Global->DatabaseGlobal->user == 0) {
		io_error_msg("Database Access requires db name and user name");
		return;
	}

	if (mysql_connect(&db, Global->DatabaseGlobal->host, Global->DatabaseGlobal->user, "")
			== NULL) {
		io_error_msg("MySQL error '%s'\n", mysql_error(&db));
		return;		/* FIX ME */
	}

	r = mysql_select_db(&db, Global->DatabaseGlobal->name);

	if (r != 0) {
		io_error_msg("MySQL error '%s'\n", mysql_error(&db));
		return;		/* FIX ME */
	}

	r = mysql_query(&db, sql);

	if (r != 0) {
		io_error_msg("MySQL error '%s'\n", mysql_error(&db));
		return;		/* FIX ME */
	}

	res = mysql_store_result(&db);

	if (mysql_num_fields(res) != 1) {
		/* There's more than one, or less than one, result. */
		io_error_msg("MySQLQuery: number of results (%d) should be one\n", nrows);
		return;		/* FIX ME */
	}

	field = mysql_fetch_field(res);

	nrows = mysql_num_rows(res);
	if (nrows != 1) {
		/* There's more than one, or less than one, result. */
		io_error_msg("MySQLQuery: number of results (%d) should be one\n", nrows);
		return;		/* FIX ME */
	}

	row = mysql_fetch_row(res);

	switch (field->type) {
	case FIELD_TYPE_CHAR:
	case FIELD_TYPE_NULL:
	case FIELD_TYPE_TIMESTAMP:
	case FIELD_TYPE_DATE:
	case FIELD_TYPE_TIME:
	case FIELD_TYPE_DATETIME:
	case FIELD_TYPE_VAR_STRING:
	case FIELD_TYPE_STRING:
		p->type = TYP_STR;
		p->String = strdup(row[0]);
		break;
	case FIELD_TYPE_DECIMAL:
	case FIELD_TYPE_FLOAT:
	case FIELD_TYPE_DOUBLE:
	case FIELD_TYPE_LONGLONG:
	case FIELD_TYPE_INT24:
	default:
		p->type = TYP_FLT;
		d = atof(row[0]);
		p->Float = d;
		break;
	case FIELD_TYPE_SHORT:
	case FIELD_TYPE_LONG:
		p->type = TYP_INT;
		sscanf(row[0], "%d", &i);
		p->Int = i;
		break;
	case FIELD_TYPE_TINY_BLOB:
	case FIELD_TYPE_MEDIUM_BLOB:
	case FIELD_TYPE_LONG_BLOB:
	case FIELD_TYPE_BLOB:
		break;
	}

	mysql_free_result(res);

	mysql_close(&db);

	return;
}

static void
do_mysql_read(struct value *p)
{
	/*
	 * This doesn't do a thing.
	 *
	 * A database query is only triggered on request from the user.
	 */
}

static void
do_mysql_write(struct value *p)
{
	/*
	 * This doesn't do a thing.
	 *
	 * A database query is only triggered on request from the user.
	 */
}

/*
 * This function reads a block of data from an SQL statement
 * into the spreadsheet.
 */
void
MySQLRead(void)
{
	MYSQL		db;
	MYSQL_RES	*res;
	MYSQL_ROW	row;
	MYSQL_FIELD	*field;
	int		r, i, j, nrows;
	enum enum_field_types	*types_list;

	char *sql = "select * from koers";
	int in_row = curow, in_col = cucol;

	if (mysql_connect(&db, "localhost", "danny", "") == NULL) {
		fprintf(stderr, "MySQL error '%s'\n", mysql_error(&db));
		return;
	}

	r = mysql_select_db(&db, "test");

	if (r != 0) {
		fprintf(stderr, "MySQL error '%s'\n", mysql_error(&db));
		return;
	}

	r = mysql_query(&db, sql);

	if (r != 0) {
		fprintf(stderr, "MySQL error '%s'\n", mysql_error(&db));
		return;
	}

	res = mysql_store_result(&db);

	types_list = malloc(mysql_num_fields(res));

	for (j=0; j<mysql_num_fields(res); j++) {
		field = mysql_fetch_field(res);

		(void) quote_new_value(in_row, j+in_col, field->name);
		types_list[j] = field->type;
	}

	nrows = mysql_num_rows(res);
	for (i=0; i<mysql_num_rows(res); i++) {
		row = mysql_fetch_row(res);
		for (j=0; j<mysql_num_fields(res); j++) {
			switch (types_list[j]) {
			case FIELD_TYPE_CHAR:
			case FIELD_TYPE_NULL:
			case FIELD_TYPE_TIMESTAMP:
			case FIELD_TYPE_DATE:
			case FIELD_TYPE_TIME:
			case FIELD_TYPE_DATETIME:
			case FIELD_TYPE_VAR_STRING:
			case FIELD_TYPE_STRING:
#ifdef	VERBOSE
				fprintf(stderr, "Cell[%d,%d] = '%s'\n", i+1+in_row, j+in_col, row[j]);
#endif
				(void)quote_new_value(i+1+in_row, j+in_col, row[j]);
				break;
			case FIELD_TYPE_DECIMAL:
			case FIELD_TYPE_SHORT:
			case FIELD_TYPE_LONG:
			case FIELD_TYPE_FLOAT:
			case FIELD_TYPE_DOUBLE:
			case FIELD_TYPE_LONGLONG:
			case FIELD_TYPE_INT24:
			default:
#ifdef	VERBOSE
				fprintf(stderr, "Cell[%d,%d] = %s\n", i+1+in_row, j+in_col, row[j]);
#endif
				(void)new_value(i+1+in_row, j+in_col, row[j]);
				break;
			case FIELD_TYPE_TINY_BLOB:
			case FIELD_TYPE_MEDIUM_BLOB:
			case FIELD_TYPE_LONG_BLOB:
			case FIELD_TYPE_BLOB:
				break;
			}
		}
	}

	mysql_free_result(res);

	mysql_close(&db);
	free(types_list);

	Global->modified = 1;
	recalculate(1);

	return;
}
#endif	/* HAVE_LIBMYSQLCLIENT */

/*
 * The functions below need to exist even if we don't have MySQL
 */
void AllocateDatabaseGlobal(void)
{
	if (Global->DatabaseGlobal == NULL) {
		Global->DatabaseGlobal = (struct DatabaseGlobalType *)
			malloc(sizeof(struct DatabaseGlobalType));
		memset(Global->DatabaseGlobal, 0, sizeof(struct DatabaseGlobalType));
	}
}

void DatabaseSetName(const char *name)
{
	if (Global->DatabaseGlobal->name)
		free(Global->DatabaseGlobal->name);
	Global->DatabaseGlobal->name = strdup(name);
}

void DatabaseSetHost(const char *host)
{
	if (Global->DatabaseGlobal->host)
		free(Global->DatabaseGlobal->host);
	Global->DatabaseGlobal->host = strdup(host);
}

void DatabaseSetUser(const char *user)
{
	if (Global->DatabaseGlobal->user)
		free(Global->DatabaseGlobal->user);
	Global->DatabaseGlobal->user = strdup(user);
}

int
DatabaseInitialised(void)
{
	if (Global->DatabaseGlobal)
		return 1;
	return 0;
}

char *
DatabaseGetName(void)
{
	if (Global->DatabaseGlobal)
		return Global->DatabaseGlobal->name;
	return NULL;
}

char *
DatabaseGetHost(void)
{
	if (Global->DatabaseGlobal)
		return Global->DatabaseGlobal->host;
	return NULL;
}

char *
DatabaseGetUser(void)
{
	if (Global->DatabaseGlobal)
		return Global->DatabaseGlobal->user;
	return NULL;
}

#ifndef	HAVE_LIBMYSQLCLIENT

#include "eval.h"

/*
 * Define Oleo functions
 */
struct function mysql_functions[] = {
	{ 0, 0, "", 0, 0 }
};

int init_mysql_function_count(void) 
{
        return sizeof(mysql_functions) / sizeof(struct function) - 1;
}
#endif	/* HAVE_LIBMYSQLCLIENT */
