/**
 * $Id: gshhs.h,v 1.3 2008/07/31 19:59:42 ylafon Exp $
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

#ifndef _GSHHS_H_
#define _GSHHS_H_

#include "defs.h"
#include "types.h"

void init_coastline();
/**
 * min latitude, min longitude, max latitude, max longitude 
 * all in radians, in min latitude > max latitude, it selects the whole
 * world
 */
void init_partial_coastline PARAM4(double, double, double, double);

/* free the allocated arrays of coast_seg, then set everything to 0 */
void free_gshhs();

#endif /* _GSHHS_H_ */
