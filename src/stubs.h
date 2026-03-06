/*
 * $Id: stubs.h,v 1.3 2001/04/16 23:50:07 pw Exp $
 *
 * Copyright © 1993, 2000 Free Software Foundation, Inc.
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

/*
 * This file contains the only list of all the stub functions that
 * are needed for the command set.  The list is maintained this way
 * to make it harder to make mistakes in building the table of stub
 * functions.  Preprocessor magic is used in stub.c to process this list.
 */

STUB1("f", stub_f, (FILE *), fp, FILE *)
STUB1("k", stub_k, &, key, struct key_sequence *)
STUB1("s", stub_s, (char *), string, char *)
STUB1("i", stub_i, (int), integer, int)
STUB1("d", stub_d, (float), floating, float)
STUB1("r", stub_r, &, range, struct rng *)

STUB2("dd", stub_dd, (double), floating, double, (double), floating, double)
STUB2("fs", stub_fs, (FILE *), fp, FILE *, (char *), string, char *)
STUB2("ii", stub_ii, (int), integer, int, (int), integer, int)
STUB2("is", stub_is, (int),integer, int, (char *),string, char *)
STUB2("ir", stub_ir, (int), integer, int, &, range, struct rng *)
STUB2("rr", stub_rr, &, range, struct rng *, &, range, struct rng *)
STUB2("rf", stub_rf, &, range, struct rng *, (FILE *), fp, FILE *)
STUB2("ri", stub_ri, &, range, struct rng *, (int), integer, int)
STUB2("rs", stub_rs, &, range, struct rng *, (char *), string, char *)
STUB2("sd", stub_sd, (char *), string, char *, (double), floating, double)
STUB2("sf", stub_sf, (char *), string, char *, (FILE *), fp, FILE *)
STUB2("si", stub_si, (char *), string, char *, (int), integer, int)
STUB2("sk", stub_sk, (char *), string, char *, &, key, struct key_sequence *)
STUB2("ss", stub_ss, (char *), string, char *, (char *), string, char *)

STUB3 ("iii", stub_iii, (int), integer, int, (int), integer, int, (int), integer, int)
STUB3 ("iri", stub_iri, (int), integer, int, &, range, struct rng *, (int), integer, int)
STUB3 ("rsd", stub_rsd,
       &, range, struct rng *, (char *), string, char *, (double), floating, double)
STUB3 ("ssi", stub_ssi, (char *), string, char *, (char *), string, char *, (int), integer, int)
STUB3 ("sss", stub_sss, (char *), string, char *, (char *), string, char *, (char *), string, char *)

STUB4 ("irii", stub_irii, (int), integer, int, &, range, struct rng *, (int), integer, int, (int), integer, int)

STUB5 ("siisi", stub_siisi, (char *), string, char *, (int), integer, int, (int), integer, int, (char *), string, char *, (int), integer, int)
