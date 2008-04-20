/**
 * $Id: lines.h,v 1.1 2008/04/20 12:30:37 ylafon Exp $
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

#ifndef _LINES_H_
#define _LINES_H_

#include "defs.h"
#include "types.h"

double intersects PARAM10(double, double, double, double,
			  double, double, double, double,
			  double *, double *);

double check_coast PARAM6(double, double, double, double,
			  double *, double *);

#endif /* _LINES_H_ */
