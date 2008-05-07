/**
 * $Id: grib.h,v 1.5 2008/05/07 11:20:22 ylafon Exp $
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

#ifndef _GRIB_H_
#define _GRIB_H_

#include <math.h>

#include "defs.h"
#include "types.h"

void init_grib();

/* init structures from the grib file, a time offset (in seconds) is applied
   positive time offset means that the structure will be drifted in the 
   future, so the boats will be late.
*/
void init_grib_offset PARAM1(long);
void set_grib_offset PARAM1(long);
long get_grib_offset ();

#endif /* _GRIB_H_ */
