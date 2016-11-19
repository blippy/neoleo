/*
 * Copyright © 1999 Free Software Foundation, Inc.
 * 
 * $Id: afm.h,v 1.4 2000/08/10 21:02:49 danny Exp $
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

#ifndef _OLEO_AFM_H_
#define _OLEO_AFM_H_

void AfmSetFont(char *name, char *slant, int size);
void AfmSetEncoding(const char *);

int AfmStringWidth(char *s);
int AfmFontWidth(void);
int AfmFontHeight(void);
int AfmPitch(void);

#endif /* _OLEO_AFM_H_ */
