/**
 * $Id: vlm.c,v 1.15 2008/12/14 18:46:06 ylafon Exp $
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
#include "defs.h"
#include "types.h"
#include "loxo.h"
#include "lines.h"
#include "vmg.h"
#include "ortho.h"
#include "winds.h"

/**
 * using VLM's PIM definition, set the heading function of the boat
 * accordingly 
 * @param aboat, a pointer to a boat struct
 * @param vlm_mode, an int, the numeric value of the PIM
 */
void set_vlm_pilot_mode(boat *aboat, int vlm_mode) {

   switch (vlm_mode) {
   case 1:
     /* needs heading filled in aboat->wp_heading */
     aboat->set_heading_func=&set_heading_constant;
     break;
   case 2:
     /* needs heading filled in aboat->wp_heading */
     aboat->set_heading_func=&set_heading_wind_angle;
     break;
   case 3:
     /* needs WP filled in aboat->wp_lat/long */
     aboat->set_heading_func=&set_heading_ortho;
     break;
   case 4:
     /* needs WP filled in aboat->wp_lat/long */
     aboat->set_heading_func=&set_heading_bvmg;
     break;
   default:
     /* add a warning? fail? exit? */
     ;
   }
}

/**
 * This function uses the default interpolation function as defined in the
 * compilation options
 * @param latitude, a double, in milli-degree.
 * @param longitude, a double, in milli-degree.
 * @param vac_time, a time_t, in seconds since 00:00:00 January 1, 1970
 * @param wind, a pointer to a wind_info structure
 * @return the pointer to the wind_info structure above
 * NOTE: the wind_info structure is filled with
 * * speed, a double, in kts
 * * angle, a double, in degrees between 0.0 and 359.9999..
 */
 wind_info *VLM_get_wind_info_latlong_deg(double latitude, double longitude,
					 time_t vac_time, wind_info *wind) {
  get_wind_info_latlong(degToRad(latitude), degToRad(longitude),
			vac_time, wind);
  /* VLM needs wind in the opposite direction */
  wind->angle = fmod((radToDeg(wind->angle)+180.0), 360.0);
  return wind;
}

/**
 * This function uses the U/V/time trilinear interpolation
 * @param latitude, a double, in degree.
 * @param longitude, a double, in degree.
 * @param vac_time, a time_t, in seconds since 00:00:00 January 1, 1970
 * @param wind, a pointer to a wind_info structure
 * @return the pointer to the wind_info structure above
 * NOTE: the wind_info structure is filled with
 * * speed, a double, in kts
 * * angle, a double, in degrees between 0.0 and 359.9999..
 */
wind_info *VLM_get_wind_info_latlong_deg_UV(double latitude, double longitude, 
					    time_t vac_time, wind_info *wind) {
  get_wind_info_latlong_UV(degToRad(latitude), degToRad(longitude),
			   vac_time, wind);
  wind->angle = fmod((radToDeg(wind->angle)+180.0), 360.0);
  return wind;
}

/**
 * This function uses the True Wind Speed & Angle interpolation function
 * (polar/time tri-linear interpolation)
 * @param latitude, a double, in degree.
 * @param longitude, a double, in degree.
 * @param vac_time, a time_t, in seconds since 00:00:00 January 1, 1970
 * @param wind, a pointer to a wind_info structure
 * @return the pointer to the wind_info structure above
 * NOTE: the wind_info structure is filled with
 * * speed, a double, in kts
 * * angle, a double, in degrees between 0.0 and 359.9999..
 */
wind_info *VLM_get_wind_info_latlong_deg_TWSA(double latitude, double longitude,
					      time_t vac_time,
					      wind_info *wind) {
  get_wind_info_latlong_TWSA(degToRad(latitude), degToRad(longitude),
			     vac_time, wind);
  wind->angle = fmod((radToDeg(wind->angle)+180.0), 360.0);
  return wind;
}

/**
 * This function uses the default interpolation function as defined in the
 * compilation options
 * @param latitude, a double, in milli-degree.
 * @param longitude, a double, in milli-degree.
 * @param vac_time, a time_t, in seconds since 00:00:00 January 1, 1970
 * @param wind, a pointer to a wind_info structure
 * @return the pointer to the wind_info structure above
 * NOTE: the wind_info structure is filled with
 * * speed, a double, in kts
 * * angle, a double, in degrees between 0.0 and 359.9999..
 */
wind_info *VLM_get_wind_info_latlong_millideg(double latitude, double longitude,
					      time_t vac_time, 
					      wind_info *wind) {
  get_wind_info_latlong(degToRad(latitude/1000.0), degToRad(longitude/1000.0),
			vac_time, wind);
  wind->angle = fmod((radToDeg(wind->angle)+180.0), 360.0);
  return wind;
}

/**
 * This function uses the U/V/time trilinear interpolation
 * @param latitude, a double, in milli-degree.
 * @param longitude, a double, in milli-degree.
 * @param vac_time, a time_t, in seconds since 00:00:00 January 1, 1970
 * @param wind, a pointer to a wind_info structure
 * @return the pointer to the wind_info structure above
 * NOTE: the wind_info structure is filled with
 * * speed, a double, in kts
 * * angle, a double, in degrees between 0.0 and 359.9999..
 */
wind_info *VLM_get_wind_info_latlong_millideg_UV(double latitude, 
						 double longitude, 
						 time_t vac_time,
						 wind_info *wind) {
  get_wind_info_latlong_UV(degToRad(latitude/1000.0), 
			   degToRad(longitude/1000.0),
			   vac_time, wind);
  wind->angle = fmod((radToDeg(wind->angle)+180.0), 360.0);
  return wind;
}

/**
 * This function uses the True Wind Speed & Angle interpolation function
 * (polar/time tri-linear interpolation)
 * @param latitude, a double, in milli-degree.
 * @param longitude, a double, in milli-degree.
 * @param vac_time, a time_t, in seconds since 00:00:00 January 1, 1970
 * @param wind, a pointer to a wind_info structure
 * @return the pointer to the wind_info structure above
 * NOTE: the wind_info structure is filled with
 * * speed, a double, in kts
 * * angle, a double, in degrees between 0.0 and 359.9999..
 */
wind_info *VLM_get_wind_info_latlong_millideg_TWSA(double latitude,
						   double longitude,
						   time_t vac_time,
						   wind_info *wind) {
  get_wind_info_latlong_TWSA(degToRad(latitude/1000.0), 
			     degToRad(longitude/1000.0),
			     vac_time, wind);
  wind->angle = fmod((radToDeg(wind->angle)+180.0), 360.0);
  return wind;
}

/**
 * Compute the orthodromic distance between two points, A & B
 * @param latitude_a, a <code>double</code>, in <em>milli-degrees</em>
 * @param longitude_a, a <code>double</code>, in <em>milli-degrees</em>
 * @param latitude_b, a <code>double</code>, in <em>milli-degrees</em>
 * @param longitude_b, a <code>double</code>, in <em>milli-degrees</em>
 * @return a double, the distance, a <code>double</code> in nautic miles.
 * If the parameters are incorrect, -1.0 is returned.
 */
double VLM_ortho_distance(double latitude_a, double longitude_a, 
			  double latitude_b, double longitude_b) {
  /* sanity check */
  latitude_a  = latitude_a / 1000.0;
  longitude_a = fmod((longitude_a / 1000.0), 360.0);
  latitude_b  = latitude_b / 1000.0;
  longitude_b = fmod((longitude_b / 1000.0), 360.0);

  /* if something goes wrong, return -1 */
  if (latitude_a < -90.0 || latitude_a > 90.0 ||
      latitude_b < -90.0 || latitude_b > 90.0) {
    return -1.0;
  }

  return ortho_distance(degToRad(latitude_a), degToRad(longitude_a),
			degToRad(latitude_b), degToRad(longitude_b));
}

/**
 * Compute the orthodromic distance between a point and a line defined
 * by two points, A & B
 * This is done in cartesian coordinates to find the intersection point
 * which is a _bad_ approximation for long distances. Then ortho is used
 * to get the real distance.
 * @param latitude, a <code>double</code>, in <em>milli-degrees</em>
 * @param longitude, a <code>double</code>, in <em>milli-degrees</em>
 * @param latitude_a, a <code>double</code>, in <em>milli-degrees</em>
 * @param longitude_a, a <code>double</code>, in <em>milli-degrees</em>
 * @param latitude_b, a <code>double</code>, in <em>milli-degrees</em>
 * @param longitude_b, a <code>double</code>, in <em>milli-degrees</em>
 * @return a double, the distance, a <code>double</code> in nautic miles.
 * If the parameters are incorrect, -1.0 is returned.
 */
double VLM_distance_to_line(double latitude, double longitude, 
			    double latitude_a, double longitude_a, 
			    double latitude_b, double longitude_b) {
  /* sanity check */
  latitude    = latitude / 1000.0;
  longitude   = fmod((longitude / 1000.0), 360.0);
  latitude_a  = latitude_a / 1000.0;
  longitude_a = fmod((longitude_a / 1000.0), 360.0);
  latitude_b  = latitude_b / 1000.0;
  longitude_b = fmod((longitude_b / 1000.0), 360.0);

  /* if something goes wrong, return -1 */
  if (latitude   < -90.0 || latitude   > 90.0 ||
      latitude_a < -90.0 || latitude_a > 90.0 ||
      latitude_b < -90.0 || latitude_b > 90.0) {
    return -1.0;
  }

  return distance_to_line(degToRad(latitude)  , degToRad(longitude),
			  degToRad(latitude_a), degToRad(longitude_a),
			  degToRad(latitude_b), degToRad(longitude_b));
}

/**
 * Compute the coordinate of a point computed form an origin, using a 
 * loxodromic course, with a specified heading and distance.
 * it is clipped at +/-80 degrees.
 * @param latitude, a <code>double</code>, in <em>milli-degrees</em>
 * @param longitude, a <code>double</code>, in <em>milli-degrees</em>
 * @param distance, a <code>double</code>, in <em>milli-degrees</em>
 * @param heading, a <code>double</code>, in <em>degrees</em>
 * @param target_lat, a pointer to a <code>double</code>, the resulting
 *                    latitude in <em>milli-degrees</em>
 * @param target_long, a pointer to a <code>double</code>, the resulting
 *                    longitude in <em>milli-degrees</em>
 */
void VLM_get_loxo_coord_from_dist_angle(double latitude, double longitude,
					double distance, double heading,
					double *target_lat, 
					double *target_long) {
  double new_lat, new_long, ratio;
  /* first, sanitize everything */
  latitude = degToRad(latitude/1000.0);
  longitude = fmod(degToRad(longitude/1000.0), TWO_PI);
  heading = degToRad(heading);
  
  get_loxo_coord_from_dist_angle(latitude, longitude, distance, heading,
				 &new_lat, &new_long);
  if (fabs(new_lat) > degToRad(80.0)) {
    ratio = (degToRad(80.0)-fabs(latitude)) / (fabs(new_lat)-fabs(latitude));
    distance *= ratio;
    get_loxo_coord_from_dist_angle(latitude, longitude, distance, heading,
				   &new_lat, &new_long);
  }
  if (new_long > PI) {
    new_long -= TWO_PI;
  } else if (new_long < -PI) {
    new_long += TWO_PI;
  }
  *target_lat = 1000.0 * radToDeg(new_lat);
  *target_long = 1000.0 * radToDeg(new_long);
}

/**
 * Compute the loxodromic distance and heading from one point to another.
 * @param latitude, a <code>double</code>, in <em>milli-degrees</em>
 * @param longitude, a <code>double</code>, in <em>milli-degrees</em>
 * @param target_lat, a <code>double</code>, in <em>milli-degrees</em>
 * @param target_long, a <code>double</code>, in <em>milli-degrees</em>
 * @param distance, a pointer to a <code>double</code>, the resulting
 *                  distance in <em>nautic miles</em>
 * @param heading, a pointer to a <code>double</code>, the resulting
 *                 heading in <em>degrees</em>
 */
void VLM_loxo_distance_angle(double latitude, double longitude, 
			     double target_lat, double target_long,
			     double *distance, double *heading) {
  
  double new_heading;
  /* first, sanitize everything */
  latitude    = degToRad(latitude/1000.0);
  longitude   = fmod(degToRad(longitude/1000.0), TWO_PI);
  target_lat  = degToRad(target_lat/1000.0);
  target_long = fmod(degToRad(target_long/1000.0), TWO_PI);
  
  loxo_distance_angle(latitude, longitude, target_lat, target_long,
		      distance, &new_heading);
  *heading = radToDeg(fmod(new_heading+TWO_PI, TWO_PI));
}

/**
 * Check if the waypoint is crossed.
 * We check if we are in the range where approximation is valid,
 * otherwise we compute a clipped part of the gate, then use the
 * approximation method of checking the gate crossing
 * order of parameters is:
 * start lat/long of boat
 * end lat/long of boat
 * start WP of Gate
 * end WP of Gate
 * result:
 * crossing lat/long
 * ratio from the start to end of boat
 * @param latitude, a <code>double</code>, in <em>milli-degrees</em>
 * @param longitude, a <code>double</code>, in <em>milli-degrees</em>
 * @param new_lat, a <code>double</code>, in <em>milli-degrees</em>
 * @param new_long, a <code>double</code>, in <em>milli-degrees</em> 
 * @param new_lat, a <code>double</code>, in <em>milli-degrees</em>
 * @param new_long, a <code>double</code>, in <em>milli-degrees</em> 
 * @param new_lat, a <code>double</code>, in <em>milli-degrees</em>
 * @param new_long, a <code>double</code>, in <em>milli-degrees</em> 
 * @param xing_lat, a pointer to a <code>double</code>, 
 *                  in <em>milli-degrees</em>
 * @param xing_long, a pointer to a <code>double</code>, 
 *                   in <em>milli-degrees</em> 
 * @param ratio, a pointer to  a <code>double</code>, the ratio of 
 *        the intersection, 0 (boat start) < ratio < 1 (boat end)
 * @return 1 if crossing occured, 0 otherwise
 */
int VLM_check_cross_WP(double latitude, double longitude, 
		       double new_lat, double new_long,
		       double wp0_lat, double wp0_long,
		       double wp1_lat, double wp1_long,
		       double *xing_lat, double *xing_long,
		       double *ratio) {

  double loxoheading, loxodist, c_ratio, r_lat, r_long;
  double t_dist, t1_lat, t1_long, t2_lat, t2_long;

  latitude  = degToRad(latitude/1000.0);
  longitude = fmod(degToRad(longitude/1000.0), TWO_PI);
  new_lat   = degToRad(new_lat/1000.0);
  new_long  = fmod(degToRad(new_long/1000.0), TWO_PI);

  wp0_lat  = degToRad(wp0_lat/1000.0);
  wp0_long = degToRad(wp0_long/1000.0);
  wp1_lat  = degToRad(wp1_lat/1000.0);
  wp1_long = degToRad(wp1_long/1000.0);
  
  loxo_distance_angle(wp0_lat, wp0_long, wp1_lat, wp1_long, 
		      &loxodist, &loxoheading);
  
  if (loxodist < 200.0) {
    c_ratio = intersects(latitude, longitude, new_lat, new_long,
			 wp0_lat, wp0_long, wp1_lat, wp1_long,
			 &r_lat, &r_long);
    if (c_ratio > -1.0) {
      *ratio     = c_ratio;
      *xing_lat  = 1000.0 * radToDeg(r_lat);
      *xing_long = 1000.0 * radToDeg(r_long);
      return 1;
    }
    return 0;
  }
  /* find two points around lat->new_lat in wp0->wp1 */
  c_ratio = (((new_lat+latitude)/2.0)-wp0_lat) / (wp1_lat - wp0_lat);
  if ((c_ratio < 0.0)||(c_ratio>1.0)) {
    /* ratio is negative, we are out of range, bail out */
    return 0;
  }
  t_dist = loxodist * c_ratio;
  get_loxo_coord_from_dist_angle(wp0_lat, wp0_long, 
				 t_dist - 10.0, loxoheading,
				 &t1_lat, &t1_long);
  get_loxo_coord_from_dist_angle(wp0_lat, wp0_long, 
				 t_dist + 10.0, loxoheading,
				 &t2_lat, &t2_long);  
  c_ratio = intersects(latitude, longitude, new_lat, new_long,
		       t1_lat, t1_long, t2_lat, t2_long,
		       &r_lat, &r_long);
  if (c_ratio > -1.0) {
    *ratio = c_ratio;
    *xing_lat  = 1000.0 * radToDeg(r_lat);
    *xing_long = 1000.0 * radToDeg(r_long);
    return 1;
  }
  return 0;
}

