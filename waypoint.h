/**
 * $Id: waypoint.h,v 1.1 2008/05/24 15:44:55 ylafon Exp $
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

#ifndef _WAYPOINT_H_
#define _WAYPOINT_H_

#include <time.h>

#include "defs.h"
#include "types.h"

int check_waypoint_crossed PARAM8(long, long, time_t, long, long, time_t, 
				  waypoint *, time_t *);

#endif /* _WAYPOINT_H_ */
