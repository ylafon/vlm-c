/**
 * $Id: loxo.h,v 1.1 2008/04/20 12:30:37 ylafon Exp $
 *
 * (c) 2008 by Yves Lafon
 *      See COPYING file for copying and redistribution conditions.
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; version 2 of the License.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 * Contact: <yves@raubacapeu.net>
 */

#ifndef _LOXO_H_
#define _LOXO_H_

#include <math.h>

#include "defs.h"
#include "types.h"

void move_boat_loxo           PARAM1(boat *);
void estimate_boat_loxo       PARAM5(boat *, int, double, double *, double *);
int  estimate_boat_loxo_coast PARAM5(boat *, int, double, double *, double *);
void set_heading_loxo         PARAM1(boat *);
void set_heading_direct       PARAM2(boat *, double);

#endif /* _LOXO_H_ */
