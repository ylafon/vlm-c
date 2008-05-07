/**
 * $Id: grib.h,v 1.6 2008/05/07 16:44:33 ylafon Exp $
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

/**
 * init global wind structures in the context based on a grib file
 * containing only UGRD/VGRD entries in matched pairs.
 * time offest is set to compile time default. (GRIB_TIME_OFFSET)
 * it corresponds by default to 35 to 40mn time delay, from VLM average.
 * unit is seconds.
 */
void init_grib();

/**
 * init global wind structures in the context based on a grib file
 * containing only UGRD/VGRD entries in matched pairs.
 * time offest is given as a parameter (see above)
 */
void init_grib_offset PARAM1(long);
/**
 * set the grib offset (see above) without having to re-read the grib
 */
void set_grib_offset PARAM1(long);
/**
 * get the current grib offset (see above)
 */
long get_grib_offset ();

#endif /* _GRIB_H_ */
