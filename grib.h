/**
 * $Id: grib.h,v 1.3 2008/05/03 15:36:22 ylafon Exp $
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
void init_grib_offset PARAM1(long);

#endif /* _GRIB_H_ */
