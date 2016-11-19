/*
 * Copyright © 1993, 1999 Free Software Foundation, Inc.
 * 
 * $Id: print.h,v 1.13 2000/08/10 21:02:51 danny Exp $
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

#ifndef PRINTH
#define PRINTH

extern void put_ps_string (char *str, FILE *fp);
extern void psprint_region (FILE * fp, struct rng * rng, float wid, float hgt, char * font);
extern void set_page_size_cmd (char * whole_str);
extern void print_region_cmd (struct rng *rng, FILE *fp);

extern int PrintGetNumPageSizes(void);
extern char * PrintGetPageName(int index);
extern void PrintSetPageSize(float wid, float ht);
extern void PrintSetPage(char *page);

extern void PrintInit(void);

struct page_size 
{
    char *name;
    float wid;
    float hgt;
};

struct PrintDriver {
	char	*name;
	void	(*job_header) (char *title, int npages, FILE *fp);
	void	(*job_trailer) (int npages, FILE *fp);

	void	(*page_header)(char *str, FILE *);
	void	(*page_footer)(char *str, FILE *);

	/*
	 * Print a field to the printer.
	 *	The field width is separately passed (in points).
	 *	Rightborder is a boolean which indicates whether to display
	 *		a border at the right of this field.
	 *	Xpoints, Xchars are the position on the line in points or characters
	 *		that the real string should start at.
	 */
	void	(*field)(char *str, int wid, int rightborder,
			int xpoints, int xchars, FILE *);
	void	(*borders)(/* ... , */ FILE *);
	/*
	 * Select a font by three parameters.
	 *	Size should be the (well known) point size.
	 */
	void	(*font)(char *family, char *slant, int size, FILE *);
	/*
	 * Start a new line, descend by HT points on the page.
	 */
	void	(*newline)(int ht, FILE *);
	/*
	 * PaperSize(wid, ht, fp)
	 *
	 * Paper width and height should be passed in as integers reflecting the
	 *	paper size in points (1/72 of an inch).
	 */
	void	(*paper_size)(int wid, int ht, FILE *);
	/*
	 * Set the encoding (the character set), usually ISOLatin1.
	 */
	void	(*set_encoding)(char *encoding);
	/*
	 * Set border width and height.
	 */
	void	(*set_border)(int width, int height, FILE *fp);
};

char *PrintGetType(int);
void PrintSetType(char *);

void PrintSetInterline(int);
#endif /* PRINTH */
