/*
 *  $Id: gsl.c,v 1.1 2000/07/08 15:21:24 danny Exp $
 *
 *  This file is part of Oleo, the GNU spreadsheet.
 *
 *  Copyright © 2000 by the Free Software Foundation, Inc.
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

/*
 * This file defines some GSL access functions for Oleo
 */

static char rcsid[] = "$Id: gsl.c,v 1.1 2000/07/08 15:21:24 danny Exp $";

#ifdef	HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include "funcdef.h"
#include "sysdef.h"

#include "global.h"
#include "cell.h"
#include "eval.h"
#include "errors.h"
#include "ref.h"

/*
 * Declarations necessary for defining an Oleo function
 */

/*
#define Float   x.c_d
#define String  x.c_s
#define Int     x.c_l
#define Value   x.c_i
#define Rng     x.c_r
*/

#ifdef	HAVE_GSL
#include <gsl/gsl_math.h>
#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_poly.h>

typedef double (*GSL_F_A)(const double data[], size_t stride, size_t n);
typedef double (*GSL_F_AF)(const double data[], size_t stride, size_t n, double d);
typedef double (*GSL_F_AFF)(const double data[], size_t stride, size_t n, double d, double e);

/*
 * This internal function does the dirty work of grabbing an Oleo range
 * and putting the numbers into an array of doubles.
 * Then it calls the GSL function in its second argument, and returns the result.
 */
static double
oleo_gsl_call_array(struct value *p, GSL_F_A f)
{
	double		*data, r;
	int		n;
	struct rng	rngx = p->Rng;
	CELL		*cp;

	n = 0;
	make_cells_in_range(&rngx);
        while ((cp = next_cell_in_range()))
		if (GET_TYP(cp) == TYP_FLT)
			n++;

	data = (double *)calloc(n, sizeof(double));
	n = 0;
	make_cells_in_range(&rngx);
        while ((cp = next_cell_in_range())) {
		if (GET_TYP(cp) == TYP_FLT) {
			data[n] = cp->cell_flt;
			n++;
		}
	}

	r = (*f)(data, 1, n);

	free(data);

	return r;
}

static double
oleo_gsl_call_array_double(struct value *p, GSL_F_AF f)
{
	double		*data, r,
			d = (p+1)->Float;
	int		n;
	struct rng	rngx = p->Rng;
	CELL		*cp;

	n = 0;
	make_cells_in_range(&rngx);
        while ((cp = next_cell_in_range()))
		if (GET_TYP(cp) == TYP_FLT)
			n++;

	data = (double *)calloc(n, sizeof(double));
	n = 0;
	make_cells_in_range(&rngx);
        while ((cp = next_cell_in_range())) {
		if (GET_TYP(cp) == TYP_FLT) {
			data[n] = cp->cell_flt;
			n++;
		}
	}

	r = (*f)(data, 1, n, d);

	free(data);

	return r;
}

static double
oleo_gsl_call_array_double_double(struct value *p, GSL_F_AFF f)
{
	double		*data, r,
			d = (p+1)->Float,
			e = (p+2)->Float;
	int		n;
	struct rng	rngx = p->Rng;
	CELL		*cp;

	n = 0;
	make_cells_in_range(&rngx);
        while ((cp = next_cell_in_range()))
		if (GET_TYP(cp) == TYP_FLT)
			n++;

	data = (double *)calloc(n, sizeof(double));
	n = 0;
	make_cells_in_range(&rngx);
        while ((cp = next_cell_in_range())) {
		if (GET_TYP(cp) == TYP_FLT) {
			data[n] = cp->cell_flt;
			n++;
		}
	}

	r = (*f)(data, 1, n, d, e);

	free(data);

	return r;
}

/*
 * Statistical functions
 */
static void
oleo_gsl_stats_variance(struct value *p)
{
	GSL_F_A		f = gsl_stats_variance;
	p->Float = oleo_gsl_call_array(p, f);
	p->type = TYP_FLT;
}

static void
oleo_gsl_stats_variance_m(struct value *p)
{
	GSL_F_AF		f = gsl_stats_variance_m;
	p->Float = oleo_gsl_call_array_double(p, f);
	p->type = TYP_FLT;
}

static void
oleo_gsl_stats_variance_with_fixed_mean(struct value *p)
{
	GSL_F_AF		f = gsl_stats_variance_with_fixed_mean;
	p->Float = oleo_gsl_call_array_double(p, f);
	p->type = TYP_FLT;
}

static void
oleo_gsl_stats_mean(struct value *p)
{
	GSL_F_A		f = gsl_stats_mean;
	p->Float = oleo_gsl_call_array(p, f);
	p->type = TYP_FLT;
}

static void
oleo_gsl_stats_sd(struct value *p)
{
	GSL_F_A		f = gsl_stats_sd;
	p->Float = oleo_gsl_call_array(p, f);
	p->type = TYP_FLT;
}

static void
oleo_gsl_stats_sd_m(struct value *p)
{
	GSL_F_AF		f = gsl_stats_sd_m;
	p->Float = oleo_gsl_call_array_double(p, f);
	p->type = TYP_FLT;
}

static void
oleo_gsl_stats_sd_with_fixed_mean(struct value *p)
{
	GSL_F_AF		f = gsl_stats_sd_with_fixed_mean;
	p->Float = oleo_gsl_call_array_double(p, f);
	p->type = TYP_FLT;
}

static void
oleo_gsl_stats_absdev(struct value *p)
{
	GSL_F_A		f = gsl_stats_absdev;
	p->Float = oleo_gsl_call_array(p, f);
	p->type = TYP_FLT;
}

static void
oleo_gsl_stats_absdev_m(struct value *p)
{
	GSL_F_AF		f = gsl_stats_absdev_m;
	p->Float = oleo_gsl_call_array_double(p, f);
	p->type = TYP_FLT;
}

static void
oleo_gsl_stats_skew(struct value *p)
{
	GSL_F_A		f = gsl_stats_skew;
	p->Float = oleo_gsl_call_array(p, f);
	p->type = TYP_FLT;
}

static void
oleo_gsl_stats_skew_m_sd(struct value *p)
{
	GSL_F_AFF		f = gsl_stats_skew_m_sd;
	p->Float = oleo_gsl_call_array_double_double(p, f);
	p->type = TYP_FLT;
}

static void
oleo_gsl_stats_kurtosis(struct value *p)
{
	GSL_F_A		f = gsl_stats_kurtosis;
	p->Float = oleo_gsl_call_array(p, f);
	p->type = TYP_FLT;
}

static void
oleo_gsl_stats_kurtosis_m_sd(struct value *p)
{
	GSL_F_AFF		f = gsl_stats_kurtosis_m_sd;
	p->Float = oleo_gsl_call_array_double_double(p, f);
	p->type = TYP_FLT;
}

static void
oleo_gsl_stats_lag1_autocorrelation(struct value *p)
{
	GSL_F_A		f = gsl_stats_lag1_autocorrelation;
	p->Float = oleo_gsl_call_array(p, f);
	p->type = TYP_FLT;
}

static void
oleo_gsl_stats_median_from_sorted_data(struct value *p)
{
	GSL_F_A		f = gsl_stats_median_from_sorted_data;
	p->Float = oleo_gsl_call_array(p, f);
	p->type = TYP_FLT;
}

static void
oleo_gsl_stats_quantile_from_sorted_data(struct value *p)
{
	GSL_F_AF		f = gsl_stats_quantile_from_sorted_data;
	p->Float = oleo_gsl_call_array_double(p, f);
	p->type = TYP_FLT;
}

/*
 * Polynomial functions
 */
static void
oleo_gsl_poly_solve_quadratic(struct value *p)
{
	double	a = (p+0)->Float,
		b = (p+1)->Float,
		c = (p+2)->Float,
		d = 0.0,
		e = 0.0;
	struct rng	rg = (p+3)->Rng;
	char	*r, s;
	union vals	v;

	gsl_poly_solve_quadratic(a, b, c, &d, &e);

	/* lr, lc, hr, hc */
	v.c_d = d;
	r = set_new_value(rg.lr, rg.lc, TYP_FLT, &v);
	v.c_d = e;
	r = set_new_value(rg.lr, rg.lc, TYP_FLT, &v);
}

static void
oleo_gsl_poly_solve_quadratic1(struct value *p)
{
	double	a = p->Float,
		b = (p+1)->Float,
		c = (p+2)->Float,
		d, e;

	gsl_poly_solve_quadratic(a, b, c, &d, &e);

	p->Float = d;
}

static void
oleo_gsl_poly_solve_quadratic2(struct value *p)
{
	double	a = p->Float,
		b = (p+1)->Float,
		c = (p+2)->Float,
		d, e;

	gsl_poly_solve_quadratic(a, b, c, &d, &e);

	p->Float = e;
}
#endif

/*
 * Define Oleo functions
 *
 * This part must always be compiled and linked, the table gets rather empty
 * when building without GSL however.
 */
struct function gsl_functions[] = {
#ifdef	HAVE_GSL
/*
 * Statistics functions
 */
	{ C_FN1, X_A1, "R", oleo_gsl_stats_mean, "gsl_stats_mean" },
	{ C_FN1, X_A1, "R", oleo_gsl_stats_variance, "gsl_stats_variance" },
	{ C_FN1, X_A1, "R", oleo_gsl_stats_sd, "gsl_stats_sd" },
	{ C_FN1, X_A1, "R", oleo_gsl_stats_absdev, "gsl_stats_absdev" },
	{ C_FN1, X_A1, "R", oleo_gsl_stats_skew, "gsl_stats_skew" },
	{ C_FN1, X_A1, "R", oleo_gsl_stats_kurtosis, "gsl_stats_kurtosis" },
	{ C_FN1, X_A1, "R", oleo_gsl_stats_lag1_autocorrelation, "gsl_stats_lag1_autocorrelation" },
	{ C_FN1, X_A1, "R", oleo_gsl_stats_median_from_sorted_data, "gsl_stats_median_from_sorted_data" },
	{ C_FN2, X_A2, "RF", oleo_gsl_stats_quantile_from_sorted_data, "gsl_stats_quantile_from_sorted_data" },
	{ C_FN2, X_A2, "RF", oleo_gsl_stats_variance_m, "gsl_stats_variance_m" },
	{ C_FN2, X_A2, "RF", oleo_gsl_stats_sd_m, "gsl_stats_sd_m" },
	{ C_FN2, X_A2, "RF", oleo_gsl_stats_variance_with_fixed_mean, "gsl_stats_variance_with_fixed_mean" },
	{ C_FN2, X_A2, "RF", oleo_gsl_stats_sd_with_fixed_mean, "gsl_stats_sd_with_fixed_mean" },
	{ C_FN2, X_A2, "RF", oleo_gsl_stats_absdev_m, "gsl_stats_absdev_m" },
	{ C_FN3, X_A3, "RFF", oleo_gsl_stats_skew_m_sd, "gsl_stats_skew_m_sd" },
	{ C_FN3, X_A3, "RFF", oleo_gsl_stats_kurtosis_m_sd, "gsl_stats_kurtosis_m_sd" },

/*
 * Polynomial functions
 */
	{ C_FN3, X_A3, "FFF", oleo_gsl_poly_solve_quadratic1, "gsl_poly_solve_quadratic1" },
	{ C_FN3, X_A3, "FFF", oleo_gsl_poly_solve_quadratic2, "gsl_poly_solve_quadratic2" },
/*	{ C_FN4, X_A4, "FFFR", oleo_gsl_poly_solve_quadratic, "gsl_poly_solve_quadratic" }, */

	/* Add new GSL functions here, not outside the #if/#endif pair. */
#endif
	{ 0, 0, "", 0, 0 }
};

int init_gsl_function_count(void) 
{
        return sizeof(gsl_functions) / sizeof(struct function) - 1;
}
