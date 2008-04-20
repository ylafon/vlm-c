/**
 * $Id: lines.c,v 1.4 2008/04/20 21:16:49 ylafon Exp $
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
#include "ortho.h"

/**
 * Strict check will et intersection only between 0 and 1 (inclusive)
 * However, to ensure that no rounding issues happens, we can widen this
 * and extend by a 1%o factor, in practise, if we take into account that
 * the boat is not a single pixel, it makes sense :)
 */
#ifdef SAFE_LINE_CHECK
#  define MAX_LIMIT 1.001
#  define MIN_LIMIT -0.001
#else
#  define MAX_LIMIT 1.0
#  define MIN_LIMIT 0.0
# endif /* SAFE_LINE_CHECK */

/**
 * The shore line is composed of an array of "coast zones" consisting of
 * an int, the number of segments to be checked, and the segments, with
 * longitude and latitude coordinates of the two points of each segment
 * in radians, this is a global variable 
 */
extern coast_zone shoreline[3601][1800];

/**
 * input: longitude/latitude of old position -> new position
 * longitude/latitude of wpa->wpb or coast segment
 * return a double is intersects, with inter_longitude/latitude filled
 * -1 otherwise */

double intersects(longitude, latitude, new_longitude, new_latitude,
		  seg_a_longitude, seg_a_latitude, seg_b_longitude,
		  seg_b_latitude, inter_longitude, inter_latitude)
     double longitude, latitude;
     double new_longitude, new_latitude;
     double seg_a_longitude, seg_a_latitude;
     double seg_b_longitude, seg_b_latitude;
     double *inter_longitude, *inter_latitude;
{
  double x, y, x1, x2, t, t_seg,d;
  if (longitude <0) {
    longitude += TWO_PI;
  }
  if (new_longitude <0) {
    new_longitude += TWO_PI;
  }
#ifdef DEBUG
  printf("Checking intersection between %.4fx%4f->%.4fx%.4f and %.4fx%4f->%.4fx%.4f",
	 longitude, latitude, new_longitude, new_latitude,
	 seg_a_longitude, seg_a_latitude, seg_b_longitude,seg_b_latitude);
#endif /* DEBUG */
  x = (longitude - seg_a_longitude);
  x1 = (new_longitude - longitude);
  x2 = (seg_b_longitude - seg_a_longitude);
  y = (latitude - seg_a_latitude);
  
  /* 
   *  FIXME we must check when we are reaching the 0/360 line 
   *  hence the longitude-relater intermediate variables.
   */
  
  d = ((seg_b_latitude - seg_a_latitude)*x1 - x2 * (new_latitude - latitude));
  t = (x2*y - (seg_b_latitude - seg_a_latitude)*x) / d;
  t_seg = (x1*y - (new_latitude - latitude)*x) / d;
  
  if ((t >= MIN_LIMIT && t <=MAX_LIMIT) && (t_seg>=MIN_LIMIT && t_seg <=MAX_LIMIT)) {
    *inter_longitude = longitude + t*(new_longitude - longitude);
    *inter_latitude = latitude + t*(new_latitude - latitude);
#ifdef DEBUG
    printf(" -> YES\n");
#endif /* DEBUG */
    return t;
  }
  return -1;
}

/**
 * input: longitude/latitude of boat's old and new position
 * return a double is intersects, with inter_longitude/latitude filled
 * -1 otherwise 
 */
double check_coast(longitude, latitude, new_longitude, new_latitude, 
		   inter_longitude, inter_latitude) 
     double longitude, latitude;
     double new_longitude, new_latitude;
     double *inter_longitude, *inter_latitude;
{
  double min_val = 1000.0;
  double inter;
  int i_lat, i_long, i_new_lat, i_new_long;
  int i, nb_segments;
  coast_zone *c_zone;
  coast_seg *seg_array;
  double t_lat, t_long;
  double min_lat, min_long;
  
#define _check_intersection_with_array	 				 \
  nb_segments = c_zone->nb_segments;					 \
  seg_array = c_zone->seg_array;					 \
  for (i=0; i<nb_segments; i++) {					 \
    inter = intersects(longitude, latitude, new_longitude, new_latitude, \
		       seg_array->longitude_a, seg_array->latitude_a,	 \
		       seg_array->longitude_b, seg_array->latitude_b,	 \
		       &t_long, &t_lat);				 \
    seg_array++;							 \
    if (inter>=MIN_LIMIT && inter<=MAX_LIMIT) {				 \
      if (inter < min_val) {						 \
	min_val = inter;						 \
	min_long = t_long;						 \
	min_lat = t_lat;						 \
      }									 \
    }									 \
  }

  /* to keep the compiler happy */
  t_lat=t_long=min_lat=min_long=0.0;

  i_long     = floor(radToDeg(longitude)*10);
  i_lat      = floor(radToDeg(latitude)*10) + 900;
  i_new_long = floor(radToDeg(new_longitude)*10);
  i_new_lat  = floor(radToDeg(new_latitude)*10) + 900;
  
  if (i_long < 0) {
    i_long += 3600;
  } else if (i_long > 3600) {
    i_long -= 3600;
  }
  if (i_new_long < 0) {
    i_new_long += 3600;
  } else if (i_new_long > 3600) {
    i_new_long -= 3600;
  }
#if DEBUG
  printf("Checking segments: [%d][%d] -> %d\n", i_long, i_lat, 
	 shoreline[i_long][i_lat].nb_segments); 
#endif /* DEBUG */
  /* if we cross more than one 1/10 of degree, we must use two loops
     instead -> check based on vac time + max speed */
  if (i_long == i_new_long) {
    if (i_lat == i_new_lat) {
      c_zone=&shoreline[i_long][i_lat];
      _check_intersection_with_array;
    } else {
      c_zone=&shoreline[i_long][i_lat];
      _check_intersection_with_array;

      c_zone=&shoreline[i_long][i_new_lat];
      _check_intersection_with_array;
    }
  } else {
    if (i_lat == i_new_lat) {
      c_zone=&shoreline[i_long][i_lat];
      _check_intersection_with_array;

      c_zone=&shoreline[i_new_long][i_lat];
      _check_intersection_with_array;
    } else {
      c_zone=&shoreline[i_long][i_lat];
      _check_intersection_with_array;

      c_zone=&shoreline[i_long][i_new_lat];
      _check_intersection_with_array;

      c_zone=&shoreline[i_new_long][i_lat];
      _check_intersection_with_array;

      c_zone=&shoreline[i_new_long][i_new_lat];
      _check_intersection_with_array;
    }
  }
  if (min_val<=MAX_LIMIT) {
    *inter_longitude = min_long;
    *inter_latitude = min_lat;
    return min_val;
  }
  return -1;
}

/**
 * compute an approximative distance to a segment. Useful to estimate 
 * distance to a gate. It is at best an approximation, as the intersection
 * algorithm is not valid for long distances
 * Parameters: long/lat of point, then long and lat of A & B defining the
 * segment
 */
double distance_to_line(longitude, latitude, longitude_a, latitude_a,
			longitude_b, latitude_b)
     double longitude, latitude;
     double longitude_a, latitude_a;
     double longitude_b, latitude_b;
{
  double ortho_a, ortho_b, min_dist, ab_dist, t_dist;
  double longitude_x, latitude_x, intersect;

  ortho_a = ortho_distance(longitude, latitude, longitude_a, latitude_a);
  ortho_b = ortho_distance(longitude, latitude, longitude_b, latitude_b);
  ab_dist = ortho_distance(longitude_a, latitude_a, longitude_b, latitude_b);
  
  min_dist = fmin(ortho_a, ortho_b);
  /* we construct a line form the point, orthogonal to the segment, long of
     at least min_dist */
  longitude_x = longitude + (latitude_b - latitude_a) * min_dist / ab_dist;
  latitude_x = latitude + (longitude_a - longitude_b) * min_dist / ab_dist;
  
  intersect = intersects(longitude, latitude, longitude_x, latitude_x,
			 longitude_a, latitude_a, longitude_b, latitude_b,
			 &longitude_x, &latitude_x);
  if (intersect>=MIN_LIMIT && intersect<=MAX_LIMIT) { 
    t_dist = ortho_distance(longitude, latitude, longitude_x, latitude_x);
#ifdef DEBUG
    printf("Min dist: %.3f, found dist: %.3f\n", min_dist, t_dist);
#endif /* DEBUG */
    return fmin(min_dist, t_dist);
  }

  /* same as above, but opposite way (it could be factored in one, but this 
     reduces the odds of crossing the 0 line */
  longitude_x = longitude + (latitude_a - latitude_b) * min_dist / ab_dist;
  latitude_x = latitude + (longitude_b - longitude_a) * min_dist / ab_dist;
  
  intersect = intersects(longitude, latitude, longitude_x, latitude_x,
			 longitude_a, latitude_a, longitude_b, latitude_b,
			 &longitude_x, &latitude_x);
  if (intersect>=MIN_LIMIT && intersect<=MAX_LIMIT) { 
    t_dist = ortho_distance(longitude, latitude, longitude_x, latitude_x);
#ifdef DEBUG
    printf("Min dist: %.3f, found dist: %.3f\n", min_dist, t_dist);
#endif /* DEBUG */
    return fmin(min_dist, t_dist);
  }
  return min_dist;
}

