/*
 * $Id: oleofile.c,v 1.26 2001/02/13 23:38:06 danny Exp $
 *
 * Copyright © 1990-2000, 2001 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include "funcdef.h"
#include <stdio.h>
#include <ctype.h>
#include "sysdef.h"
#include "io-generic.h"
#include "io-abstract.h"
#include "io-utils.h"
#include "io-term.h"
#include "font.h"
#include "global.h"
#include "cell.h"
#include "line.h"
#include "sylk.h"
#include "lists.h"
#include "ref.h"
#include "regions.h"
#include "window.h"
#include "info.h"
#include "cmd.h"

#include "graph.h"

#include "oleosql.h"

/* These functions read and write OLEO style files. */

void
oleo_read_file (fp, ismerge)
     FILE *fp;
     int ismerge;
{
  char *ptr;
  CELLREF crow = 0, ccol = 0, czrow = 0, czcol = 0;
  int lineno;
  char cbuf[1024];
  char expbuf[1024];
  char *vname, *vval;
  int vlen = 0;
  int cprot;
  char *cexp, *cval;
  CELL *cp;
  struct rng rng;
  int fmt = 0, prc = 0;
  int jst = 0;
  struct font_memo * fnt = 0;
  struct font_memo ** fnt_map = 0;
  int fnt_map_size = 0;
  int fnt_map_alloc = 0;
  int font_spec_in_format = 1;	/* Reset if we discover this is a v1.1 file. */
  
  long mx_row = MAX_ROW, mx_col = MAX_COL;
  int old_a0;
  int next_a0;

  Global->return_from_error = 1;

  old_a0 = Global->a0;
  next_a0 = old_a0;
  Global->a0 = 0;
  lineno = 0;
  if (!ismerge)
    clear_spreadsheet ();
  while (fgets (cbuf, sizeof (cbuf), fp))
    {
      lineno++;

#if 0
      if (lineno % 50 == 0)
	io_info_msg ("Line %d", lineno);
#endif
      if ((ptr = (char *)index (cbuf, '\n')))
	*ptr = '\0';

      ptr = cbuf;
      switch (*ptr)
	{
	case '#':		/* comment line -- ignored */
	  break;
	case '%':		/* Font or pixel size data. */
	  ptr++;
	  switch (*ptr) {
	    case 'F':		/* %F font-name */
	      if (fnt_map_size == fnt_map_alloc)
		{
		  fnt_map_alloc = (fnt_map_alloc + 1) * 2;
		  fnt_map =
		    ((struct font_memo **)
		     ck_remalloc
		     (fnt_map, fnt_map_alloc * sizeof (struct font_memo *)));
		}
	      fnt_map[fnt_map_size++] = parsed_matching_font (ptr + 1);
	      break;
	    case 'f':		/* %f range font-name */
	      {
		struct rng rng;
		/* This field only occurs in files written by 1.1
		 * oleo.  It's presense indicates that when parsing
		 * format fields, we should *not* reset cell fonts to 0.
		 */
		font_spec_in_format = 0;
		++ptr;
		while (isspace (*ptr))
		  ++ptr;
		if (!parse_cell_or_range (&ptr, &rng))
		  goto bad_field;
		while (isspace (*ptr))
		  ++ptr;
		{
		  struct font_memo * fm = parsed_matching_font (ptr);
		  set_region_font (&rng, fm->names->oleo_name, fm->scale);
		}
		break;
	      }
	    default:		/* % with something invalid */
	      goto bad_field;
	    }
	  break;
	case 'F':		/* Format field */
	  vlen = 0;
	  ptr++;
	  fnt = 0;	/* The font must be explicitly overriden for a cell. */
	  while (*ptr)
	    {
	      if (*ptr != ';')
		goto bad_field;
	      ptr++;
	      switch (*ptr++) {
		  int clo, chi, cwid;
		case 'C':	/* Column from rows 1 to 255 */
		  czcol = astol (&ptr);
		  vlen = 2;
		  break;

		case 'D':	/* Default format */
		  switch (*ptr++)
		    {
		    case 'G':
		      default_fmt = FMT_GEN;
		      break;
		    case 'E':
		      default_fmt = FMT_EXP;
		      break;
		    case 'F':
		      default_fmt = FMT_FXT;
		      break;
		    case '$':
		      default_fmt = FMT_DOL;
		      break;
		    case '*':	/* * format implemented as +- format */
		      default_fmt = FMT_GPH;
		      break;
		    case ',':	/* JF */
		      default_fmt = FMT_CMA;
		      break;
		    case 'U':
		      default_fmt = FMT_USR;
		      break;
		    case '%':
		      default_fmt = FMT_PCT;
		      break;
		    case 'H':
		      default_fmt = FMT_HID;
		      break;
		    case 'd':	/* Date */
			default_fmt = FMT_DATE;
			break;
		      /* End of JF */
		    default:
		      io_error_msg ("Line %d: format %c not supported", lineno, ptr[-1]);
		      break;
		    }
		  if (*ptr == 'F')
		    {
		      prc = default_prc = FLOAT_PRECISION;
		      ptr++;
		    }
		  else
		    default_prc = prc = astol (&ptr);

		  switch (*ptr++)
		    {
		    case 'C':
		      default_jst = JST_CNT;
		      break;
		    case 'L':
		      default_jst = JST_LFT;
		      break;
		    case 'R':
		      default_jst = JST_RGT;
		      break;
		    case 'G':	/* General format not supported */
		    default:
		      io_error_msg ("Line %d: Alignment %c not supported", lineno, ptr[-1]);
		      break;
		    }
		  default_width = astol (&ptr);
		  break;

		case 'f': /* Font specification */
		  {
		    int id;
		    id = astol(&ptr);
		    if (id < 0 || id >= fnt_map_size)
		      {
			io_error_msg ("Line %d: Undefined font (%d)\n",
				      lineno, id);
			break;
		      }
		    fnt = fnt_map[id];
		    break;
		  }

		case 'F':
		  switch (*ptr++)
		    {
		    case 'D':
		      fmt = FMT_DEF;
		      break;
		    case 'G':
		      fmt = FMT_GEN;
		      break;
		    case 'E':
		      fmt = FMT_EXP;
		      break;
		    case 'F':
		      fmt = FMT_FXT;
		      break;
		    case '$':
		      fmt = FMT_DOL;
		      break;
		    case '*':	/* JF implemented as +- format */
		      fmt = FMT_GPH;
		      break;
		    case ',':	/* JF */
		      fmt = FMT_CMA;
		      break;
		    case 'U':
		      fmt = FMT_USR;
		      break;
		    case '%':
		      fmt = FMT_PCT;
		      break;
		    case 'H':
		      fmt = FMT_HID;
		      break;	/* END of JF */
		    case 'd':
			fmt = FMT_DATE;
			break;
		    case 'C':
		    default:
		      io_error_msg ("Line %d: format %c not supported", lineno, ptr[-1]);
		      fmt = FMT_DEF;
		      break;
		    }
		  if (*ptr == 'F') {
		      prc = FLOAT_PRECISION;
		      ptr++;
		  } else {
		    prc = astol(&ptr);
		  }
		  switch (*ptr++)
		    {
		    case 'C':
		      jst = JST_CNT;
		      break;
		    case 'L':
		      jst = JST_LFT;
		      break;
		    case 'R':
		      jst = JST_RGT;
		      break;
		    case 'D':
		      jst = JST_DEF;
		      break;
		    default:
		      io_error_msg ("Line %d: Alignment %c not supported", lineno, ptr[-1]);
		      jst = JST_DEF;
		      break;
		    }
		  vlen = 1;
		  break;
		case 'R':	/* Row from cols 1 to 63 */
		  czrow = astol (&ptr);
		  vlen = 4;
		  break;

		case 'W':	/* Width of clo to chi is cwid */
		  clo = astol (&ptr);
		  chi = astol (&ptr);
		  cwid = astol (&ptr) + 1;
		  for (; clo <= chi; clo++) {
			set_width (clo, cwid);
		    }
		  break;

		case 'H':	/* JF: extension */
		  clo = astol (&ptr);
		  chi = astol (&ptr);
		  cwid = astol (&ptr) + 1;
		  for (; clo <= chi; clo++)
		    set_height (clo, cwid);
		  break;
		case 'c':
		  ccol = astol (&ptr);
		  break;
		case 'r':
		  crow = astol (&ptr);
		  break;

		default:
		  goto bad_field;
		}
	    }
	  switch (vlen)
	    {
	    case 1:
	      cp = find_or_make_cell (crow, ccol);
	      SET_FORMAT (cp, fmt);
		  SET_PRECISION(cp, prc);
	      SET_JST (cp, jst);
	      if (font_spec_in_format)
		cp->cell_font = fnt;
	      break;
	    case 2:
	      rng.lr = MIN_ROW;
	      rng.lc = czcol;
	      rng.hr = mx_row;
	      rng.hc = czcol;
	      make_cells_in_range (&rng);
	      while ((cp = next_cell_in_range ()))
		{
		  SET_FORMAT (cp, fmt);
		  SET_PRECISION(cp, prc);
		  SET_JST (cp, jst);
		  if (font_spec_in_format)
		    cp->cell_font = fnt;
		}
	      break;
	    case 4:
	      rng.lr = czrow;
	      rng.lc = MIN_COL;
	      rng.hr = czrow;
	      rng.hc = mx_col;
	      make_cells_in_range (&rng);
	      while ((cp = next_cell_in_range ()))
		{
		  SET_FORMAT (cp, fmt);
		  SET_JST (cp, jst);
		  if (font_spec_in_format)
		    cp->cell_font = fnt;
		}
	      break;
	    default:
	      break;
	    }
	  break;

	case 'B':		/* Boundry field, ignored */
	  ptr++;
	  while (*ptr)
	    {
	      if (*ptr != ';')
		goto bad_field;
	      ptr++;
	      switch (*ptr++)
		{
		case 'c':
		  mx_col = astol (&ptr);
		  if (mx_col > MAX_COL)
		    {
		      io_error_msg ("Boundry column %lu too large!", mx_col);
		      mx_col = MAX_COL;
		    }
		  break;
		case 'r':
		  mx_row = astol (&ptr);
		  if (mx_row > MAX_ROW)
		    {
		      io_error_msg ("Boundry row %lu too large!", mx_row);
		      mx_row = MAX_ROW;
		    }
		  break;
		default:
		  goto bad_field;
		}
	    }
	  break;

	case 'N':		/* A Name field */
	  if (ptr[1] != 'N')
	    goto bad_field;
	  ptr += 2;
	  vname = 0;
	  vval = 0;
	  while (*ptr)
	    {
	      if (*ptr != ';')
		goto bad_field;
	      *ptr++ = '\0';
	      switch (*ptr++)
		{
		case 'N':	/* Name is */
		  vname = ptr;
		  while (*ptr && *ptr != ';')
		    ptr++;
		  vlen = ptr - vname;
		  break;
		case 'E':	/* Expression is */
		  vval = ptr;
		  while (*ptr && *ptr != ';')
		    ptr++;
		  break;
		default:
		  --ptr;
		  goto bad_field;
		}
	    }
	  if (!vname || !vval)
	    goto bad_field;
	  *ptr = '\0';
	  ptr = old_new_var_value (vname, vlen, vval);
	  if (ptr)
	    io_error_msg ("Line %d: Couldn't set %.*s to %s: %s", lineno, vlen, vname, vval, ptr);
	  break;

	case 'C':		/* A Cell entry */
	  cprot = 0;
	  cval = 0;
	  cexp = 0;
	  cval = 0;
	  ptr++;
	  while (*ptr)
	    {
	      int quotes;

	      if (*ptr != ';')
		goto bad_field;
	      *ptr++ = '\0';
	      switch (*ptr++)
		{
		case 'c':
		  ccol = astol (&ptr);
		  break;
		case 'r':
		  crow = astol (&ptr);
		  break;
		case 'R':
		  czrow = astol (&ptr);
		  break;
		case 'C':
		  czcol = astol (&ptr);
		  break;
		case 'P':	/* This cell is Protected */
		  cprot++;
		  break;
		case 'K':	/* This cell's Konstant value */
		  cval = ptr;
		  quotes = 0;
		  while (*ptr && (*ptr != ';' || quotes > 0))
		    if (*ptr++ == '"')
		      quotes = !quotes;
		  break;
		case 'E':	/* This cell's Expression */
		  cexp = ptr;
		  quotes = 0;
		  while (*ptr && (*ptr != ';' || quotes > 0))
		    if (*ptr++ == '"')
		      quotes = !quotes;

		  break;
		case 'G':
		  strcpy (expbuf, cval);
		  break;
		case 'D':
		  strcpy (expbuf, cexp);
		  break;
		case 'S':
		  cexp = expbuf;
		  break;
		default:
		  --ptr;
		  goto bad_field;
		}
	    }
	  *ptr = '\0';
	  if (cexp && cval && strcmp (cexp, cval))
	    {
	      ptr = read_new_value (crow, ccol, cexp, cval);
	      if (ptr)
		{
		  io_error_msg ("Line %d: %d,%d: Read '%s' %s", lineno, crow, ccol, cexp, ptr);
		  break;
		}
	    }
	  else if (cval)
	    {
	      ptr = read_new_value (crow, ccol, 0, cval);
	      if (ptr)
		{
		  io_error_msg ("Line %d: %d,%d: Val '%s' %s", lineno, crow, ccol, cexp, ptr);
		  break;
		}
	    }
	  else if (cexp)
	    {
	      ptr = read_new_value (crow, ccol, cexp, 0);
	      if (ptr)
		{
		  io_error_msg ("Line %d: %d,%d: Exp '%s' %s", lineno, crow, ccol, cexp, ptr);
		  break;
		}
	    }
	  if (cprot)
	    SET_LCK (find_or_make_cell (crow, ccol), LCK_LCK);
	  if (ismerge)
	    push_cell (crow, ccol);
	  /* ... */
	  break;
	case 'E':	/* End of input ?? */
	  break;
	case 'W':
	  io_read_window_config (ptr + 2);
	  break;
	case 'U':
	  /* JF extension:  read user-defined formats */
	  read_mp_usr_fmt (ptr + 1);
	  break;
	  /* JF extension: read uset-settable options */
	case 'O':
	  Global->a0 = next_a0;
	  read_mp_options (ptr + 2);
	  next_a0 = Global->a0;
	  Global->a0 = 0;
	  break;
	case 'G':	/* Graph data */
/*	  fprintf(stderr, "Graph input line '%s'\n", cbuf);	*/
	  switch (*(ptr+1)) {
	  case 'T':	/* Graph Title */
	    graph_set_title(cbuf+2);
	    break;
	  case 'D':	/* Axis title : GDxtitle */
	    graph_set_axis_title(cbuf[2], cbuf+3);
	    break;
	  case 't':	/* Graph data title : Gtxtitle */
	    graph_set_data_title(cbuf[2] - '0', cbuf+3);
	    break;
	  case 'a':	/* Automatic axis setting : Gax0 or Gax1 */
	    graph_set_axis_auto(cbuf[2] - '0', cbuf[3] == '1');
	    break;
	  case 'o':	/* Whether to draw line to offscreen data points */
	    graph_set_linetooffscreen(cbuf[2] == '1');
	    break;
	  case 'r':	/* Axis range : GrxlVALUE , l = 0 for low, 1 for high */
	    if (cbuf[3] == '0')
		graph_set_axis_lo('x' + cbuf[2] - '0', &cbuf[4]);
	    else if (cbuf[3] == '1')
		graph_set_axis_hi('x' + cbuf[2] - '0', &cbuf[4]);
	    break;
	  case 'L':	/* Axis logness GLx0 or GLx1 */
	    graph_set_logness(cbuf[2], 1, cbuf[3] == '1');
	    break;
	  case '0': case '1': case '2': case '3':
	  case '4': case '5': case '6': case '7':
	  case '8': case '9':
            {
	      int	i, a, b, c, d;
	      struct rng r;
	      sscanf(cbuf, "G%d,%d,%d,%d,%d", &i, &a, &b, &c, &d);
	      r.lc = a;
	      r.lr = b;
	      r.hc = c;
	      r.hr = d;
	      graph_set_data(i, &r);
	    }
	    break;
	  case 'm':	/* Tick marks Gmxts, x = 0 or 1, t = 0 .. 4 (the tick type),
			 * s is the format string, if any.
			 */
	    {
		int	axis, tp;

		axis = cbuf[2] - '0';
		tp = cbuf[3] - '0';

		graph_set_axis_ticks(axis, tp, strdup(&cbuf[4]));
	    }
		break;
	  default:
	    fprintf(stderr, "Graph: invalid line '%s'\n", cbuf);
	  }
	  break;
	case 'D':	/* Database Access */
		ptr++;
		switch (*ptr) {
		case 'u':
			DatabaseSetUser(ptr+1);
			break;
		case 'h':
			DatabaseSetHost(ptr+1);
			break;
		case 'n':
			DatabaseSetName(ptr+1);
			break;
		default:
			io_error_msg("Line %d - unknown code %s\n", lineno, cbuf);
		}
		break;
	default:
	bad_field:
	  Global->a0 = old_a0;
	  if (!ismerge)
	    clear_spreadsheet ();
	  io_recenter_all_win ();
	  io_error_msg ("Line %d: Unknown OLEO line \"%s\"", lineno, cbuf);
	  Global->return_from_error = 0;
	  return;
	}	/* End of switch */
    }
  if (!feof (fp)) {
	if (!ismerge)
		clear_spreadsheet ();
	io_recenter_all_win ();
	io_error_msg ("read-file: read-error near line %d.", lineno);
	Global->return_from_error = 0;
	return;
    }
  Global->a0 = next_a0;
  io_recenter_all_win ();

  Global->return_from_error = 0;
}

static char *
oleo_fmt_to_str (int f1, int p1)
{
  static char p_buf[40];

  p_buf[1] = '\0';
  switch (f1)
    {
    case FMT_DEF:
      p_buf[0] = 'D';
      break;
    case FMT_HID:
      p_buf[0] = 'H';
      break;
    case FMT_GPH:
      p_buf[0] = '*';
      break;
    default:
      if (p1 == FLOAT_PRECISION)
	{
	  p_buf[1] = 'F';
	  p_buf[2] = '\0';
	}
      else
	sprintf (&p_buf[1], "%d", p1);

      switch (f1)
	{
	case FMT_USR:
	  p_buf[0] = 'U';
	  break;
	case FMT_GEN:
	  p_buf[0] = 'G';
	  break;
	case FMT_DOL:
	  p_buf[0] = '$';
	  break;
	case FMT_PCT:
	  p_buf[0] = '%';
	  break;
	case FMT_FXT:
	  p_buf[0] = 'F';
	  break;
	case FMT_CMA:
	  p_buf[0] = ',';
	  break;
	case FMT_EXP:
	  p_buf[0] = 'E';
	  break;
	case FMT_DATE:
	  p_buf[0] = 'd';
	  break;
	default:
	  p_buf[0] = '?';
#if 0
	  fprintf(stderr, "OleoWrite: format %d not supported\n", f1);
#endif
	  break;
	}
      break;
    }
  return p_buf;
}

static char
jst_to_chr (just)
     int just;
{
  switch (just)
    {
    case JST_DEF:
      return 'D';
    case JST_LFT:
      return 'L';
    case JST_RGT:
      return 'R';
    case JST_CNT:
      return 'C';
    default:
      return '?';
    }
}

static FILE *oleo_fp;
static struct rng *oleo_rng;

static void 
oleo_write_var (name, var)
     char *name;
     struct var *var;
{
  if (var->var_flags == VAR_UNDEF
	&& (!var->var_ref_fm || var->var_ref_fm->refs_used == 0))
    return;

  switch (var->var_flags) {
    case VAR_UNDEF:
      break;
    case VAR_CELL:
      if (var->v_rng.lr >= oleo_rng->lr && var->v_rng.lr <= oleo_rng->hr
		&& var->v_rng.lc >= oleo_rng->lc && var->v_rng.lc <= oleo_rng->hc)
	(void) fprintf(oleo_fp, "NN;N%s;E%s\n",
		var->var_name, cell_name(var->v_rng.lr, var->v_rng.lc));
      break;
    case VAR_RANGE:
      if (var->v_rng.lr < oleo_rng->lr || var->v_rng.hr > oleo_rng->hr
		|| var->v_rng.lc < oleo_rng->lc || var->v_rng.hc > oleo_rng->hc)
	break;

      (void) fprintf(oleo_fp, "NN;N%s;E%s\n", var->var_name, range_name (&(var->v_rng)));
      break;
#ifdef TEST
    default:
      panic ("Unknown var type %d", var->var_flags);
#endif
    }
}

static void
write_mp_windows (fp)
     FILE *fp;
{
  struct line line;

  line.alloc = 0;
  line.buf = 0;
  io_write_window_config (&line);
  fputs (line.buf, fp);
  free (line.buf);
}

void
oleo_write_file (fp, rng)
     FILE *fp;
     struct rng *rng;
{
  CELLREF r, c;
  CELL *cp;
  CELLREF crow = 0, ccol = 0;
  unsigned short w;
  /* struct var *var; */
  int old_a0, i, fnt_map_size = 0;
  char	*s;

  (void) fprintf (fp, "# This file was created by GNU Oleo\n");

  /* All versions of the oleo file format should have a 
   * version cookie on the second line.
   */
  (void) fprintf (fp, "# format 2.1 (requires Oleo 1.99.9 or higher)\n");

  /* If no range given, write the entire file */
  if (!rng)
    {
      int n;
      int fmts;
      char *data[9];

      rng = &all_rng;

      (void) fprintf (fp, "F;D%s%c%u\n",
		oleo_fmt_to_str (default_fmt, default_prc),
		jst_to_chr (default_jst),
		default_width);

      fmts = usr_set_fmts ();
      for (n = 0; n < 16; n++)
	{
	  if (fmts & (1 << n))
	    {
	      get_usr_stats (n, data);
	      fprintf (fp, "U;N%u;P%s;S%s", n + 1, data[7], data[8]);
	      if (data[0][0])
		fprintf (fp, ";HP%s", data[0]);
	      if (data[1][0])
		fprintf (fp, ";HN%s", data[1]);
	      if (data[2][0])
		fprintf (fp, ";TP%s", data[2]);
	      if (data[3][0])
		fprintf (fp, ";TN%s", data[3]);
	      if (data[4][0])
		fprintf (fp, ";Z%s", data[4]);
	      if (data[5][0])
		fprintf (fp, ";C%s", data[5]);
	      if (data[6])
		fprintf (fp, ";D%s", data[6]);
	      putc ('\n', fp);
	    }
	}
      write_mp_options (fp);
    }

  old_a0 = Global->a0;
  Global->a0 = 0;

  find_widths (rng->lc, rng->hc);
  w = next_width (&c);
  while (w)
    {
      CELLREF cc, ccc;
      unsigned short ww;
      cc = c;
      do
	ww = next_width (&ccc);
      while (ccc == ++cc && ww == w);
      (void) fprintf (fp, "F;W%u %u %u\n", c, cc - 1, w - 1);
      c = ccc;
      w = ww;
    }

  find_heights (rng->lr, rng->hr);
  w = next_height (&r);
  while (w)
    {
      CELLREF rr, rrr;
      unsigned short ww;

      rr = r;
      do
	ww = next_height (&rrr);
      while (rrr == ++rr && ww == w);
      (void) fprintf (fp, "F;H%u %u %u\n", r, rr - 1, w - 1);
      r = rrr;
      w = ww;
    }

  oleo_fp = fp;
  oleo_rng = rng;
  for_all_vars (oleo_write_var);
  find_cells_in_range (rng);

  {
    struct font_memo * fm;
    for (fm = font_list; fm; fm = fm->next)
      fm->id_memo = -1;
  }
  while ((cp = next_row_col_in_range (&r, &c)))
    {
      char *ptr;
      int f1, j1;
      char p_buf[40];

      f1 = GET_FORMAT (cp);
      j1 = GET_JST (cp);
      if (f1 != FMT_DEF || j1 != JST_DEF || cp->cell_font)
	{
	  if (cp->cell_font)
	    {
	      if (cp->cell_font->id_memo < 0)
		{
		  cp->cell_font->id_memo = fnt_map_size++;
		  if (isnan(cp->cell_font->scale))
			fprintf (fp, "%%F%s,%s,%f\n",
			   cp->cell_font->names->x_name,
			   cp->cell_font->names->ps_name,
			   cp->cell_font->scale);
		  else
			fprintf (fp, "%%F%s,%s,%f\n",
			   cp->cell_font->names->x_name,
			   cp->cell_font->names->ps_name,
			   1.0);
		}
	    }
	  (void) fprintf (fp, "F;");
	  if (c != ccol)
	    {
	      (void) fprintf (fp, "c%u;", c);
	      ccol = c;
	    }
	  if (r != crow)
	    {
	      (void) fprintf (fp, "r%u;", r);
	      crow = r;
	    }
	  if (cp->cell_font)
	    (void) fprintf (fp, "f%d;", cp->cell_font->id_memo);
	    (void) fprintf (fp, "F%s%c\n",
			  oleo_fmt_to_str (f1, GET_PRECISION(cp)), jst_to_chr (j1));
	}

      if (!GET_TYP (cp) && !cp->cell_formula)
	continue;

      (void) fprintf (fp, "C;");
      if (c != ccol)
	{
	  (void) fprintf (fp, "c%u;", c);
	  ccol = c;
	}
      if (r != crow)
	{
	  (void) fprintf (fp, "r%u;", r);
	  crow = r;
	}

      if (cp->cell_formula)
	{
	  (void) fprintf (fp, "E%s", decomp(r, c, cp));
	  decomp_free();
	}

      switch (GET_TYP (cp))
	{
	case 0:
	  ptr = 0;
	  break;
	case TYP_STR:
	  ptr = 0;
	  if (cp->cell_formula)
	    putc (';', fp);
	  (void) fprintf (fp, "K\"%s\"", cp->cell_str);
	  break;
	case TYP_FLT:
	  ptr = flt_to_str (cp->cell_flt);
	  break;
	case TYP_INT:
	  sprintf (p_buf, "%ld", cp->cell_int);
	  ptr = p_buf;
	  break;
	case TYP_BOL:
	  ptr = bname[cp->cell_bol];
	  break;
	case TYP_ERR:
	  ptr = ename[cp->cell_err];
	  break;
	default:
	  ptr = 0;
#ifdef TEST
	  panic ("What cell type %d", GET_TYP (cp));
#endif
	}

      if (ptr)
	{
	  if (cp->cell_formula)
	    putc (';', fp);
	  (void) fprintf (fp, "K%s", ptr);
	}
      if (GET_LCK (cp) == LCK_LCK)
	(void) fprintf (fp, ";P");

      putc ('\n', fp);
    }

  if (rng == &all_rng)
    write_mp_windows (fp);

  /* Graphs */
  for (i=0; i<NUM_DATASETS; i++) {
	struct rng	r;
	int		a, b, c, d;

	r = graph_get_data(i);
	if (r.lc == 0 && r.lr == 0 && r.hc == 0 && r.hr == 0)
		continue;

	/* Write this thing */
	a = r.lc;
	b = r.lr;
	c = r.hc;
	d = r.hr;
	fprintf(fp, "G%d,%d,%d,%d,%d\n", i, a, b, c, d);
  }
  /* Graph title */
  s = graph_get_title();
  if (s && strlen(s)) {
    fprintf(fp, "GT%s\n", s);
  }

  s = graph_get_axis_title('x');
  if (s && strlen(s)) {
    fprintf(fp, "GDx%s\n", s);
  }

  s = graph_get_axis_title('y');
  if (s && strlen(s)) {
    fprintf(fp, "GDy%s\n", s);
  }

  for (i=0; i<NUM_DATASETS; i++) {
	if (graph_get_data_title(i))
		fprintf(fp, "Gt%c%s\n", i + '0', graph_get_data_title(i));
  }

  /* Axis range : GrxlVALUE , l = 0 for low, 1 for high */
  for (i=0; i< 2; i++) {
	double	d;
	d = graph_get_axis_lo(i);
	if (! isnan(d))
		fprintf(fp, "Gr%c0%f\n", '0' + i, d);
	d = graph_get_axis_hi(i);
	if (! isnan(d))
		fprintf(fp, "Gr%c1%f\n", '0' + i, d);
  }

  /* Automatic axis setting : Gax0 or Gax1 */
  fprintf(fp, "Ga0%c\n", graph_get_axis_auto(0) ? '1' : '0');	/* X axis */
  fprintf(fp, "Ga1%c\n", graph_get_axis_auto(1) ? '1' : '0');	/* Y axis */

  /* Draw line to offscreen data points */
  fprintf(fp, "Go%c\n", graph_get_linetooffscreen() ? '1' : '0');

  /* Axis tick marks */
  fprintf(fp, "Gm0%c%s\n",
	'0' + graph_get_axis_ticktype(0),
	graph_get_axis_tickformat(0) ? graph_get_axis_tickformat(0) : "(null)");
  fprintf(fp, "Gm1%c%s\n",
	'0' + graph_get_axis_ticktype(1),
	graph_get_axis_tickformat(1) ? graph_get_axis_tickformat(1) : "(null)");

  /* Database stuff */
  if (DatabaseInitialised()) {
	fprintf(fp, "Dn%s\n", DatabaseGetName() ? DatabaseGetName() : "");
	fprintf(fp, "Dh%s\n", DatabaseGetHost() ? DatabaseGetHost() : "");
	fprintf(fp, "Du%s\n", DatabaseGetUser() ? DatabaseGetUser() : "");
  }

  /* End of writing */
  (void) fprintf (fp, "E\n");
  Global->a0 = old_a0;
}

int
oleo_set_options
  (set_opt, option)
     int set_opt;
     char *option;
{
  return -1;
}

void
oleo_show_options ()
{
  io_text_line ("File format: oleo.");
}


#if 0
This was used in releases 1.0 and 1.1 to write fonts.  
It is no longer used but is kept here for reference since 1.2 and later
versions should continue to understand the older file format for a while.

static int
oleo_write_fonts (rng, font, ignore)
     struct rng *rng;
     struct font_memo *font;
     void *ignore;
{
  struct rng r;
  char *rname;
  r = *rng;
  if (r.lr < oleo_rng->lr)
    r.lr = oleo_rng->lr;
  if (r.lc < oleo_rng->lc)
    r.lc = oleo_rng->lc;
  if (r.hr > oleo_rng->hr)
    r.hr = oleo_rng->hr;
  if (r.hc > oleo_rng->hc)
    r.hc = oleo_rng->hc;
  rname = range_name (&r);
  fprintf (oleo_fp, "%%f %s %s,%s,%f\n",
	   rname, font->name, font->psname, font->scale);
  return 1;
}


#endif

