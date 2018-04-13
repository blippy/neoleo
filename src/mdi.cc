/*
 *  $Id: mdi.c,v 1.10 2001/02/13 23:38:06 danny Exp $
 *
 *  This file is part of Oleo, the GNU spreadsheet.
 *
 *  Copyright © 1999, 2000, 2001 by the Free Software Foundation, Inc.
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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "io-abstract.h"
#include "io-term.h"
#include "global.h"

static int	maxglobals = 0, nglobals = 0;
static struct OleoGlobal	*globals = 0;

#define	NGLOBALS_INC	10

void MdiInitializeXXX(void)
{
	globals = (struct OleoGlobal *) calloc(NGLOBALS_INC, sizeof(struct OleoGlobal));
	maxglobals = NGLOBALS_INC;

	Global = &globals[0];
	globals[0].valid = 1;
	nglobals++;

}
void MdiInitialize(void)
{
	globals = new struct OleoGlobal;
	Global = globals;
	//maxglobals = NGLOBALS_INC;

	//Global = &globals[0];
	//globals[0].valid = 1;
	//nglobals++;

}


