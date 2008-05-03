/**
 * $Id: winds.h,v 1.5 2008/05/03 21:10:53 ylafon Exp $
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

#ifndef _WINDS_H_
#define _WINDS_H_

#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "defs.h"
#include "types.h"

void get_wind_info PARAM2(boat *, wind_info *);
wind_info *get_wind_info_latlong PARAM4(double, double, time_t, wind_info *);
wind_info *get_wind_info_latlong_now PARAM3(double, double, wind_info *);

wind_info *get_wind_info_latlong_TWSA PARAM4(double, double, time_t, wind_info *);
wind_info *get_wind_info_latlong_UV PARAM4(double, double, time_t, wind_info *);

/* get the timestamp (in seconds , see time()) of the first grib entry
   technically, an observation, not a prevision */
time_t get_min_prevision_time();

/* get the timestamp (in seconds , see time()) of the last prevision */
time_t get_max_prevision_time();

#endif /* _WINDS_H_ */
