/*
 * $Id: ir.h,v 1.4 2000/08/10 21:02:50 danny Exp $
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
#ifndef INTRECTANGLEH
#define INTRECTANGLEH

#include "proto.h"
/*  t. lord	Thu Feb 13 00:44:35 1992	*/

#define xx_IRllx(R)  ((R)->x)
#define xx_IRlly(R)  ((R)->y)
#define xx_IRurx(R)  ((R)->x + (R)->w - 1)
#define xx_IRury(R)  ((R)->y + (R)->h - 1)

typedef struct xx_sIntRectangle * xx_IntRectangle;
struct xx_sIntRectangle
{
  int x, y;
  unsigned int w, h;
};

#define xx_IRinit(R,X,Y,W,H)   (void)(((R)->x = (X)), ((R)->y = (Y)), \
				      ((R)->w = (W)), ((R)->h = (H)))
/* extern int xx_IRintersects (xx_IntRectangle, xx_IntRectangle); */
#define xx_IRintersects(R1,R2) \
  (xx_IRllx(R1) <= xx_IRurx(R2) &&\
   xx_IRurx(R1) >= xx_IRllx(R2) &&\
   xx_IRlly(R1) <= xx_IRury(R2) &&\
   xx_IRury(R1) >= xx_IRlly(R2))



#ifdef PRETTY_PRINT_DECLS
extern void xx_pp_intrectangle (xx_IntRectangle, FILE *);
#endif

#define xx_IRx(R) ((R)->x)
#define xx_IRy(R) ((R)->y)
#define xx_IRw(R) ((R)->w)
#define xx_IRh(R) ((R)->h)
#define xx_IRxl(R) xx_IRx(R)
#define xx_IRyl(R) xx_IRy(R)
#define xx_IRxh(R) (xx_IRx(R) + xx_IRw(R) - 1)
#define xx_IRyh(R) (xx_IRy(R) + xx_IRh(R) - 1)

#ifdef PRETTY_PRINT

void
xx_pp_intrectangle (r, stream)
     xx_IntRectangle r;
     FILE * stream;
{
  fprintf (stream, "(xx_IntRectangle x %d, y %d, w %d, h %d)",
	   r->x, r->y, r->w, r->h);
}
#endif

extern int xx_IRencloses (xx_IntRectangle r1, xx_IntRectangle r2);
extern int xx_IRencloses_width (xx_IntRectangle r1, xx_IntRectangle r2);
extern int xx_IRequiv (xx_IntRectangle r1, xx_IntRectangle r2);
extern void xx_IRbound (xx_IntRectangle r1, xx_IntRectangle r2);
extern int xx_IRarea (xx_IntRectangle r);
extern int xx_IRhits_point (xx_IntRectangle rect, int x, int y);
extern void xx_IRclip (xx_IntRectangle r1, xx_IntRectangle r2);
extern int xx_IRsubtract (xx_IntRectangle outv, xx_IntRectangle a, xx_IntRectangle b);

#endif
