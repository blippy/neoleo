#include "sort.h"
void
sort_region (void)
{
  srdiff = 1 + sort_rng.hr - sort_rng.lr;
  erdiff = 1 + sort_ele.hr - sort_ele.lr;

  scdiff = 1 + sort_rng.hc - sort_rng.lc;
  ecdiff = 1 + sort_ele.hc - sort_ele.lc;

  if (srdiff != erdiff && srdiff % erdiff != 0)
    {
      io_error_msg ("Rows %u:%u and %u:%u don't fit", sort_rng.lr, sort_rng.hr, sort_ele.lr, sort_ele.hr);
      return;
    }
  if (scdiff != ecdiff && scdiff % ecdiff != 0)
    {
      io_error_msg ("Cols %u:%u and %u:%u don't fit", sort_rng.lc, sort_rng.hc, sort_ele.lc, sort_ele.hc);
      return;
    }
  if (scdiff != ecdiff && srdiff != erdiff)
    {
      io_error_msg ("Can't sort this region!");
      return;
    }
  Global->modified = 1;
  if (scdiff != ecdiff)
    {
      erdiff = 0;
      sort (scdiff / ecdiff, cmp_cells, swp_cells, rot_cells);
    }
  else
    {
      ecdiff = 0;
      sort (srdiff / erdiff, cmp_cells, swp_cells, rot_cells);
    }
}



void
swp_cells (int n1, int n2)
{
  int rn, cn;
  CELLREF r1, r2, c1, c2;

#ifdef TEST
  if (debug & 04)
    io_error_msg ("Swap %u<-->%u", n1, n2);
#endif
  for (rn = sort_ele.lr; rn <= sort_ele.hr; rn++)
    for (cn = sort_ele.lc; cn <= sort_ele.hc; cn++)
      {
	r1 = sort_rng.lr + (n1 * erdiff) + rn;
	r2 = sort_rng.lr + (n2 * erdiff) + rn;
	c1 = sort_rng.lc + (n1 * ecdiff) + cn;
	c2 = sort_rng.lc + (n2 * ecdiff) + cn;
#ifdef TEST
	if (debug & 04)
	  io_error_msg ("Save  r%uc%u", r1, c1);
#endif
	move_cell (r1, c1, NON_ROW, NON_COL);
#ifdef TEST
	if (debug & 04)
	  io_error_msg ("Copy r%uc%u --> r%uc%u", r2, c2, r1, c1);
#endif
	move_cell (r2, c2, r1, c1);

#ifdef TEST
	if (debug & 04)
	  io_error_msg ("Restore r%uc%u", r2, c2);
#endif
	move_cell (NON_ROW, NON_COL, r2, c2);

	/* push_cell(r1,c1);
			push_cell(r2,c2); */
      }
}
int
cmp_cells (int n1, int n2)
{
  CELL *c1, *c2;
  int t1, t2;
  union vals v1, v2;
  CELLREF row1, row2, col1, col2;
  int keyn;
  int cmpval;

  if (n1 == n2)
    return 0;

  for (keyn = 0; keyn < sort_keys_num; keyn++)
    {
      row1 = sort_rng.lr + (n1 * erdiff) + sort_keys[keyn].row;
      col1 = sort_rng.lc + (n1 * ecdiff) + sort_keys[keyn].col;
      row2 = sort_rng.lr + (n2 * erdiff) + sort_keys[keyn].row;
      col2 = sort_rng.lc + (n2 * ecdiff) + sort_keys[keyn].col;
#ifdef TEST
      if (debug & 04)
	io_error_msg ("Cmp %u %u  r%uc%u <-%u-> r%uc%u", n1, n2, row1, col1, sort_keys[keyn].mult, row2, col2);
#endif
      c1 = find_cell (row1, col1);
      c2 = find_cell (row2, col2);
      if (!c1 && !c2)
	continue;

      if (c1)
	{
	  t1 = GET_TYP (c1);
	  v1 = c1->get_c_z();
	}
      else
	t1 = 0;
      if (c2)
	{
	  t2 = GET_TYP (c2);
	  v2 = c2->get_c_z();
	}
      else
	t2 = 0;

      if (t1 == TYP_ERR || t1 == TYP_BOL)
	{
	  t1 = TYP_STR;
	  v1.c_s = print_cell (c1);
	}
      if (t2 == TYP_ERR || t2 == TYP_BOL)
	{
	  t2 = TYP_STR;
	  v2.c_s = print_cell (c2);
	}
      if (t1 != t2)
	{
	  if (t1 == 0)
	    {
	      if (t2 == TYP_STR)
		{
		  t1 = TYP_STR;
		  v1.c_s = "";
		}
	      else if (t2 == TYP_INT)
		{
		  t1 = TYP_INT;
		  v1.c_l = 0;
		}
	      else
		{
		  t1 = TYP_FLT;
		  v1.c_n = 0.0;
		}
	    }
	  else if (t2 == 0)
	    {
	      if (t1 == TYP_STR)
		{
		  t2 = TYP_STR;
		  v2.c_s = "";
		}
	      else if (t1 == TYP_INT)
		{
		  t2 = TYP_INT;
		  v2.c_l = 0;
		}
	      else
		{
		  t2 = TYP_FLT;
		  v2.c_n = 0.0;
		}
	    }
	  else if (t1 == TYP_STR)
	    {
	      t2 = TYP_STR;
	      v2.c_s = print_cell (c2);
	    }
	  else if (t2 == TYP_STR)
	    {
	      t1 = TYP_STR;
	      v1.c_s = print_cell (c1);
	      /* If we get here, one is INT, and the other
			   is FLT  Make them both FLT */
	    }
	  else if (t1 == TYP_INT)
	    {
	      t1 = TYP_FLT;
	      v1.c_n = (num_t) v1.c_l;
	    }
	  else
	    {
	      t2 = TYP_FLT;
	      v2.c_n = (num_t) v2.c_l;
	    }
	}
      if (t1 == TYP_STR)
	cmpval = strcmp (v1.c_s, v2.c_s);
      else if (t1 == TYP_FLT)
	cmpval = (v1.c_n < v2.c_n) ? -1 : ((v1.c_n > v2.c_n) ? 1 : 0);
      else if (t1 == TYP_INT)
	cmpval = (v1.c_l < v2.c_l) ? -1 : ((v1.c_l > v2.c_l) ? 1 : 0);
      else
	cmpval = 0;
      if (cmpval)
	return cmpval * sort_keys[keyn].mult;
    }

  return 0;
}

void
rot_cells (int n1, int n2)
{
  int rn, cn;
  int nn;
  CELLREF r1, r2, c1, c2;

  if (n1 + 1 == n2 || n2 + 1 == n1)
    {
      swp_cells (n1, n2);
      return;
    }
#ifdef TEST
  if (debug & 04)
    io_error_msg ("Rot cells %u -- %u", n1, n2);
#endif
  for (rn = sort_ele.lr; rn <= sort_ele.hr; rn++)
    for (cn = sort_ele.lc; cn <= sort_ele.hc; cn++)
      {

	/* store a copy of cell # n2 */
	r2 = sort_rng.lr + (n2 * erdiff) + rn;
	c2 = sort_rng.lc + (n2 * ecdiff) + cn;
	move_cell (r2, c2, NON_ROW, NON_COL);

#ifdef TEST
	if (debug & 04)
	  io_error_msg ("Save r%uc%u", r2, c2);
#endif
	/* Copy each cell from n1 to n2-1 up one */
	for (nn = n2; nn > n1; --nn)
	  {
	    r2 = sort_rng.lr + (nn * erdiff) + rn;
	    c2 = sort_rng.lc + (nn * ecdiff) + cn;

	    r1 = sort_rng.lr + ((nn - 1) * erdiff) + rn;
	    c1 = sort_rng.lc + ((nn - 1) * ecdiff) + cn;

	    move_cell (r1, c1, r2, c2);
#ifdef TEST
	    if (debug & 04)
	      io_error_msg ("Copy r%uc%u --> r%uc%u", r1, c1, r2, c2);
#endif
	    /* push_cell(r2,c2); */
	  }

	r1 = sort_rng.lr + (nn * erdiff) + rn;
	c1 = sort_rng.lc + (nn * ecdiff) + cn;
#ifdef TEST
	if (debug & 04)
	  io_error_msg ("Restore r%uc%u", r1, c1);
#endif
	move_cell (NON_ROW, NON_COL, r1, c1);

	/* push_cell(r1,c1); */
      }
}

/* End of functions for sort_region() */

/* A front end to sorting. */

void
sort_region_cmd (char *ptr)
{
  struct rng tmp_rng;

  if (get_abs_rng (&ptr, &sort_rng))
    {
      io_error_msg ("Can't find a range to sort in %s", ptr);
      return;
    }

  cur_row = sort_rng.lr;
  cur_col = sort_rng.lc;

  while (*ptr == ' ')
    ptr++;
  if (!*ptr)
    {
      sort_ele.lr = 0;
      sort_ele.lc = 0;
      sort_ele.hr = 0;
      sort_ele.hc = 0;
    }
  else if (!parse_cell_or_range (&ptr, &sort_ele))
    {
      io_error_msg ("Can't parse elements in %s", ptr);
      return;
    }
  else
    {
      sort_ele.lr -= sort_rng.lr;
      sort_ele.lc -= sort_rng.lc;
      sort_ele.hr -= sort_rng.lr;
      sort_ele.hc -= sort_rng.lc;
    }

  sort_keys_num = 0;
  while (*ptr == ' ')
    ptr++;
  for (; *ptr;)
    {
      if (sort_keys_num == sort_keys_alloc)
	{
	  sort_keys_alloc++;
	  if (sort_keys_alloc > 1)
	    sort_keys = (cmp *) ck_realloc (sort_keys, sort_keys_alloc * sizeof (struct cmp));
	  else
	    sort_keys = (cmp *)ck_malloc (sizeof (struct cmp));
	}
      sort_keys[sort_keys_num].mult = 1;
      if (*ptr == '+')
	ptr++;
      else if (*ptr == '-')
	{
	  sort_keys[sort_keys_num].mult = -1;
	  ptr++;
	}
      if (!*ptr)
	{
	  sort_keys[sort_keys_num].row = 0;
	  sort_keys[sort_keys_num].col = 0;
	  sort_keys_num++;
	  break;
	}
      if (!parse_cell_or_range (&ptr, &tmp_rng) || tmp_rng.lr != tmp_rng.hr || tmp_rng.lc != tmp_rng.hc)
	{
	  io_error_msg ("Can't parse key #%d in %s", sort_keys_num + 1, ptr);
	  sort_keys_num = -1;
	  return;
	}
      sort_keys[sort_keys_num].row = tmp_rng.lr - sort_rng.lr;
      sort_keys[sort_keys_num].col = tmp_rng.lc - sort_rng.lc;
      sort_keys_num++;

      while (*ptr == ' ')
	ptr++;
    }
  if (sort_keys_num == 0)
    {
      if (sort_keys_alloc == 0)
	{
	  sort_keys_alloc++;
	  sort_keys = (cmp *)ck_malloc (sizeof (struct cmp));
	}
      sort_keys[0].mult = 1;
      sort_keys[0].row = 0;
      sort_keys[0].col = 0;
      sort_keys_num++;
    }
  sort_region ();
  io_repaint ();
}

