/**
 * $Id: ortho.c,v 1.1 2008/04/20 12:30:37 ylafon Exp $
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

#include <math.h>

#include "defs.h"
#include "types.h"
#include "winds.h"


/* could save on computing power if we do all computing for a boat at once,
   that way, we can save calls to sin or cos */

void set_heading_ortho_nowind(aboat)
     boat *aboat;
{
  double g,d, den;
  
  g = aboat->wp_longitude - aboat->longitude;
  if (fabs(g) < 0.0000001) { /* close enough to vertical, clamp to vertical*/
    den = aboat->wp_latitude - aboat->latitude;
    aboat->wp_distance = 60.0 * radToDeg(fabs(den));
    aboat->heading = (den>0) ? 0: PI;
    return;
  }
  if (g <= - PI) {
    g += TWO_PI;
  } else if (g > PI) {
    g -= TWO_PI;
  }
  d = acos(sin(aboat->wp_latitude)*sin(aboat->latitude) +
	   cos(aboat->wp_latitude)*cos(aboat->latitude) * cos(g));
  aboat->wp_distance = 60 * radToDeg(d);
  
  den = cos(aboat->latitude) * sin(d);
  if (g<0) {
    aboat->heading = TWO_PI - acos((sin(aboat->wp_latitude)-sin(aboat->latitude)*cos(d)) / den);
  } else {
    aboat->heading = acos((sin(aboat->wp_latitude)-sin(aboat->latitude)*cos(d)) / den);
  }
}

void set_heading_ortho(aboat)
     boat *aboat;
{
  get_wind_info(aboat, &aboat->wind);
  set_heading_ortho_nowind(aboat);
}

/* return distance in nautic miles */
double ortho_distance(longitude, latitude, wp_longitude, wp_latitude)
     double longitude;
     double latitude;
     double wp_longitude;
     double wp_latitude;
{
  double g,d;
  
  g = wp_longitude - longitude;
  if (fabs(g) < 0.0000001) { /* close enough to vertical, clamp to vertical*/
    d = wp_latitude - latitude;
    return (60.0 * radToDeg(fabs(d)));
  }
  if (g <= - PI) {
    g += TWO_PI;
  } else if (g > PI) {
    g -= TWO_PI;
  }
  d = acos(sin(wp_latitude)*sin(latitude) +
	   cos(wp_latitude)*cos(latitude) * cos(g));
  return 60.0 * radToDeg(d);
}

