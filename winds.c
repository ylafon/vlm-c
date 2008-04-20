/**
 * $Id: winds.c,v 1.1 2008/04/20 12:30:37 ylafon Exp $
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
#include <complex.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#include "defs.h"
#include "types.h"
#include "winds.h"

extern winds_prev windtable;


void get_wind_info(aboat, wind)
     boat      *aboat;
     wind_info *wind;
{
  time_t vac_time;
  
  vac_time = aboat->last_vac_time + aboat->in_race->vac_duration;
  get_wind_info_longlat(aboat->longitude, aboat->latitude, vac_time, wind);
}

wind_info *get_wind_info_longlat(longitude, latitude, vac_time, wind)
  double longitude;
  double latitude;
  time_t vac_time;
  wind_info *wind;
{
  winds *prev, *next;
  int i, t_long, t_lat;
  double u0prev, u0next, v0prev, v0next;
  double u1prev, u1next, v1prev, v1next;
  double u2prev, u2next, v2prev, v2next;
  double u3prev, u3next, v3prev, v3next;
  double u01next, u23next, u01prev, u23prev;
  double v01next, v23next, v01prev, v23prev;
  double uprev, unext, vprev, vnext;
  double u,v;
  double d_long, d_lat, t_ratio, angle;
  double complex c;
#ifdef DEBUG
  char buff[64];
#endif /* DEBUG */

  d_long = radToDeg(longitude); /* is there a +180 drift? see grib */
  if (d_long < 0) {
    d_long += 360;
  } else if (d_long >= 360) {
    d_long -= 360;
  }
  d_lat = radToDeg(latitude) + 90; /* is there a +90 drift? see grib*/
    
  prev = next = NULL;
  /* can't be zero, otherwise, we are in serious time drifting issues */
  for (i=1; i< windtable.nb_prevs; i++) {
    if (windtable.wind[i]->prevision_time > vac_time) {
      next = windtable.wind[i];
      prev = windtable.wind[i-1];
      break;
    }
  }
  /* none found the two are the last ones */
  if (!next) {
    prev = windtable.wind[i-1];
  } 
  t_long = (int)floor(d_long*2.0);
  t_lat = (int)floor(d_lat*2.0);
  u0prev = prev->wind_u[t_long][t_lat];
  v0prev = prev->wind_v[t_long][t_lat];
  u1prev = prev->wind_u[t_long][t_lat+1];
  v1prev = prev->wind_v[t_long][t_lat+1];
  u2prev = prev->wind_u[(t_long+1)%WIND_GRID_LONG][t_lat];
  v2prev = prev->wind_v[(t_long+1)%WIND_GRID_LONG][t_lat];
  u3prev = prev->wind_u[(t_long+1)%WIND_GRID_LONG][t_lat+1];
  v3prev = prev->wind_v[(t_long+1)%WIND_GRID_LONG][t_lat+1];    

  /*
    simple bilinear interpolation, we might factor the cos(lat) in
    the computation to tackle the shape of the pseudo square
    
    Doing interpolation on angle/speed might be better 
  */
    
  u01prev = u0prev + (u1prev - u0prev) * (d_lat - floor(d_lat));
  v01prev = v0prev + (v1prev - v0prev) * (d_lat - floor(d_lat));
  u23prev = u2prev + (u3prev - u2prev) * (d_lat - floor(d_lat));
  v23prev = v2prev + (v3prev - v2prev) * (d_lat - floor(d_lat));

  uprev = u01prev + (u23prev - u01prev) * (d_long - floor(d_long));
  vprev = v01prev + (v23prev - v01prev) * (d_long - floor(d_long));

  if (next) {
    u0next = next->wind_u[t_long][t_lat];
    v0next = next->wind_v[t_long][t_lat];
      
    u1next = next->wind_u[t_long][t_lat+1];
    v1next = next->wind_v[t_long][t_lat+1];
      
    u2next = next->wind_u[(t_long+1)%WIND_GRID_LONG][t_lat];
    v2next = next->wind_v[(t_long+1)%WIND_GRID_LONG][t_lat];
      
    u3next = next->wind_u[(t_long+1)%WIND_GRID_LONG][t_lat+1];
    v3next = next->wind_v[(t_long+1)%WIND_GRID_LONG][t_lat+1];
      
    /* simple bilinear interpolation, we might factor the cos(lat) in
       the computation to tackle the shape of the pseudo square */
      
    u01next = u0next + (u1next - u0next) * (d_lat - floor(d_lat));
    v01next = v0next + (v1next - v0next) * (d_lat - floor(d_lat));
    u23next = u2next + (u3next - u2next) * (d_lat - floor(d_lat));
    v23next = v2next + (v3next - v2next) * (d_lat - floor(d_lat));
      
    unext = u01next + (u23next - u01next) * (d_long - floor(d_long));
    vnext = v01next + (v23next - v01next) * (d_long - floor(d_long));
      
#ifdef DEBUG
    printf("vac_time %ld, prev prevision time %ld, next prevision time %ld\n", vac_time,
	   prev->prevision_time, next->prevision_time);
    ctime_r(&vac_time, buff);
    printf("vac_time %s", buff);
    ctime_r(&prev->prevision_time, buff);
    printf(" prev_time %s", buff);
    ctime_r(&next->prevision_time, buff);
    printf(" enxt_time %s\n", buff);
    printf("diff num %ld, diff_denom %ld\n", vac_time - prev->prevision_time,
	   next->prevision_time - prev->prevision_time);
#endif /* DEBUG */
    t_ratio = ((double)(vac_time - prev->prevision_time)) / 
      ((double)(next->prevision_time - prev->prevision_time));
      
    u = uprev + (unext - uprev) * t_ratio;
    v = vprev + (vnext - vprev) * t_ratio;
    /* check this from grib...
       WE -> NS V (west from east, north from south) and in m/s
       so U/V +/+ -> 270 to 360
              +/- -> 180 to 270
	      -/- -> 90 to 180
	      -/+ -> 0 to 90
	      going clockwise.
       1m/s -> 1.9438445 kts
    */

    c = - v - _Complex_I * u;
#ifdef DEBUG
    printf("time stamps: prev %ld, boat_time %ld", prev->prevision_time,
	   vac_time);
    printf(", next %ld, time ratio %.3f\n", next->prevision_time, t_ratio);
#endif /* DEBUG */
  } else {
    c = - vprev - _Complex_I * uprev;
  }
  angle = carg(c);
  if (angle < 0) {
    angle += TWO_PI;
  }
#ifdef DEBUG
  printf("U component %.3f, V component %.3f, speed %.3f, angle %.3f\n",
	 -cimag(c), -creal(c), msToKts(cabs(c)), radToDeg(angle));
#endif /* DEBUG */
  wind->speed = msToKts(cabs(c));
  wind->angle = angle;
  return wind;
}
