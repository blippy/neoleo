/*
 *  $Id: userpref.h,v 1.4 2000/08/10 21:02:51 danny Exp $
 *
 *  This file is part of Oleo, the GNU spreadsheet.
 *
 *  Copyright © 1999 by the Free Software Foundation, Inc.
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

#ifndef	_USERPREF_H_
#define	_USERPREF_H_

struct UserPreferences {
	int	a0,
		auto_recalc,
		backup,
		backup_copy,
		ticks,
		print;
	char	*file_type,
		*bgcolor;
	int	run_load_hooks;
	char	*encoding;
};

extern struct UserPreferences UserPreferences;
#endif
