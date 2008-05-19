/**
 * $Id: loxo.c,v 1.4 2008/05/19 21:23:24 ylafon Exp $
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
#include <stdio.h>

#include "defs.h"
#include "types.h"
#include "loxo.h"
#include "polar.h"
#include "lines.h"
#include "winds.h"

/* vac_duration in seconds */
void move_boat_loxo(boat *aboat) {
  double speed;
  double latitude, t_lat;
  double vac_l, d;
  double longitude;
  int vac_duration;
  wind_info *wind;

  vac_duration = aboat->in_race->vac_duration;
  /* compute the heading based on the function used */
  aboat->set_heading_func(aboat);
  wind = &aboat->wind;

  speed = find_speed(aboat, wind->speed, wind->angle - aboat->heading);
    
  vac_l = speed*(vac_duration/3600.0);
    
  d = degToRad(vac_l/60.0);
  latitude = aboat->latitude + d*cos(aboat->heading);
    
  t_lat = (latitude + aboat->latitude) / 2;
  longitude = aboat->longitude + (d*sin(aboat->heading))/cos(t_lat);
    
  d = check_coast(aboat->longitude, aboat->latitude, longitude, latitude,
		  &t_lat, &speed);
  /* FIXME, usr SAFE_LINE_CHECK , move things from lines.c */
  if (d>=0.0 && d<=1.0) {
    /* move right before the coast */
    vac_l = vac_l * 0.99 * d;
    d = degToRad(vac_l/60.0);
    latitude = aboat->latitude + d*cos(aboat->heading);
	
    t_lat = (latitude + aboat->latitude) / 2;
    longitude = aboat->longitude + (d*sin(aboat->heading))/cos(t_lat);
    aboat->landed = 1;
  }
  if (longitude > PI) {
    longitude -= TWO_PI;
  } else if (longitude < -PI ) {
    longitude += TWO_PI;
  }
  aboat->longitude = longitude;
  aboat->latitude  = latitude;
  aboat->loch += vac_l;
  aboat->last_vac_time += vac_duration;
}

/* used to estimate where the boat will go based on a specific heading
   returns longitude and latitude (double) */
void estimate_boat_loxo(boat *aboat, int vac_duration, double heading, 
			double *new_latitude, double *new_longitude) {
  double speed;
  double latitude, t_lat;
  double vac_l;
  wind_info *wind;

  /* BE CAREFUL, you must compute the wind before calling this function */
  wind = &aboat->wind;
  speed = find_speed(aboat, wind->speed, wind->angle - heading);

  vac_l = degToRad(speed*vac_duration/(3600.0*60.0));
  latitude = aboat->latitude + vac_l*cos(heading);
    
  t_lat = (latitude + aboat->latitude) / 2;
  *new_longitude = aboat->longitude + (vac_l*sin(heading))/cos(t_lat);
  if (*new_longitude > PI) {
    *new_longitude -= TWO_PI;
  } else if (*new_longitude < -PI ) {
    *new_longitude += TWO_PI;
  }
  *new_latitude  = latitude;
}

/* 
   used to estimate where the boat will go based on a specific heading
   returns longitude and latitude (double) and check for collision
   function returns 0 if the boat landed, 1 otherwise 
*/
int estimate_boat_loxo_coast(boat *aboat, int vac_duration, double heading, 
			     double *new_latitude, double *new_longitude) {
  double speed;
  double latitude, t_lat;
  double vac_l;
  wind_info *wind;

  /* BE CAREFUL, you must compute the wind before calling this function */
  wind = &aboat->wind;
  speed = find_speed(aboat, wind->speed, wind->angle - heading);

  vac_l = degToRad(speed*vac_duration/(3600.0*60.0));
  latitude = aboat->latitude + vac_l*cos(heading);
    
  t_lat = (latitude + aboat->latitude) / 2;
  *new_longitude = aboat->longitude + (vac_l*sin(heading))/cos(t_lat);
  *new_latitude  = latitude;
  /* we reuse old variable to save space */
  vac_l = check_coast(aboat->longitude, aboat->latitude,
		      *new_longitude, *new_latitude,
		      &t_lat, &speed);
  return ((vac_l<0.0) || (vac_l>1.0));
}

void set_heading_loxo(boat *aboat) {
  get_wind_info(aboat, &aboat->wind);
  /* do nothing, as heading is already set */
}

void set_heading_direct(boat *aboat, double heading) {
  aboat->heading = heading;
}

/* set heading according to wind angle */
void set_heading_wind_angle(boat *aboat) {
  double angle;

  get_wind_info(aboat, &aboat->wind);
  angle = fmod(aboat->wind.angle + aboat->wp_heading, TWO_PI);
  if (angle < 0) {
    angle += TWO_PI;
  }
  set_heading_direct(aboat, angle);
}
