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

static char rcsid[] = "$Id: mdi.c,v 1.10 2001/02/13 23:38:06 danny Exp $";

#ifdef	HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "io-term.h"
#include "mysql.h"
#include "global.h"

#ifdef	HAVE_MOTIF
#include "io-motif.h"
#endif

#ifndef	False
#define	False	0
#endif
#ifndef	True
#define	True	1
#endif

static int	maxglobals = 0, nglobals = 0;
static struct OleoGlobal	*globals = 0;

#define	NGLOBALS_INC	10

/*
 * Hack.
 * This file only works well with Motif/LessTif anyway
 */
#ifndef	HAVE_MOTIF
void MotifGlobalInitialize()
{
}

void MessageAppend()
{
}
#endif

void MdiError(int offset1, int offset2, void *ptr)
{
#if 0
	fprintf(stderr,
		"MdiSelectGlobal: no match for offset %d,%d ptr %p\n",
		offset1, offset2, ptr);
#endif
	MessageAppend(True,
		"MdiSelectGlobal: no match for offset %d,%d ptr %p\n",
		offset1, offset2, ptr);
}

static void AllocateSubStructures(void)
{
	PlotInit();
	AllocateDatabaseGlobal();
	MotifGlobalInitialize();
	InitializeGlobals();
}

void
MdiInitialize(void)
{
	globals = (struct OleoGlobal *)
		calloc(NGLOBALS_INC, sizeof(struct OleoGlobal));
	maxglobals = NGLOBALS_INC;

	Global = &globals[0];
	globals[0].valid = 1;
	nglobals++;

	AllocateSubStructures();
}

/*
 * MdiOpen()
 *
 * Create an additional Global structure
 */
void
MdiOpen()
{
	int	i, j, found = 0;

	for (i=0; i<maxglobals; i++)
		if (globals[i].valid == 0) {
			found = 1;
			break;
		}
	/* If none free, allocate more entries */
	if (! found) {
		i = maxglobals;
		maxglobals += NGLOBALS_INC;
		globals = realloc(globals, maxglobals * sizeof(struct OleoGlobal));
		for (j=i; j<maxglobals; j++)
			globals[j].valid = 0;
	}

	/* Grab it */
	globals[i].valid = 1;
	nglobals++;

	/* Start using it */
	Global = &globals[i];

	/* Allocate the sub-structures */
	AllocateSubStructures();
}

/*
 * Close the current Global structure
 */
void
MdiClose()
{
	MessageAppend(False, "Closing file '%s'", Global->FileName);

	/* Indicate file closed */
	free(Global->FileName);
	Global->FileName = 0;

	/* Release this global entry */
	Global->valid = 0;
	nglobals--;

	/*
	 * We've close the last open window/file, therefore the
	 *	application must end.
	 */
	if (nglobals <= 0) {
#ifdef	WITH_DMALLOC
		dmalloc_shutdown();
#endif
		exit(0);
	}
}

void
MdiSelectGlobal(int offset1, int offset2, void *ptr)
{
	int	i;
	char	**p, **q;

	for (i=0; i < maxglobals; i++)
		if (globals[i].valid) {
			p = (char **) (((char *)&globals[i]) + offset1);
			q = (char **) (*p + offset2);
			if (*q == ptr) {
				Global = &globals[i];
#if 0
				fprintf(stderr, "MdiSelectGlobal: match for %d,%d,%p -> %p\n",
					offset1, offset2, ptr, Global);
#endif
				return;
			}
		}

	MdiError(offset1, offset2, ptr);
}

int
MdiHasFile(void)
{
	return	Global->FileName != NULL;
}

void
MdiQuit(void)
{
}
