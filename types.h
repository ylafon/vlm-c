/**
 * $Id: types.h,v 1.1 2008/04/20 12:30:37 ylafon Exp $
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

#ifndef _TYPES_H_
#define _TYPES_H_

#include <time.h>

#include "defs.h"

typedef struct coast_seg_str {
  double longitude_a;
  double latitude_a;
  double longitude_b;
  double latitude_b;
} coast_seg;

typedef struct coast_zone_str {
  int                   nb_segments;
  struct coast_seg_str *seg_array;
} coast_zone;

typedef struct waypoint_str {
  int    idwaypoint;
  double latitude1;
  double longitude1;
  double latitude2;
  double longitude2;
  char   *name;
  /* int laisser_au; */
} waypoint;

typedef struct race_str {
  int                 idraces;
  char                *racename;
  int                 started;
  time_t              deptime;
  time_t              closetime;
  long                startlong;
  long                startlat;
  int                 boattype;
  int                 racetype;
  time_t              firstpcttime;  /* first arrival ? */
  struct boat_str     *boat_list;
  int                 nb_boats;
  struct waypoint_str *waypoint_list;
  int                 nb_waypoints;
  int                 vac_duration;
} race;

typedef struct winds_str {
  time_t prevision_time;           /* set origin time */
  double  wind_u[WIND_GRID_LONG][WIND_GRID_LAT]; 
  double  wind_v[WIND_GRID_LONG][WIND_GRID_LAT];
  /* u = horizontal W->E , v = vertical N->S ? */
} winds;

typedef struct wind_prev_str {
    int nb_prevs;
    struct winds_str **wind;
} winds_prev;

typedef struct wind_info_str {
    double speed;
    double angle; /* in rad */
} wind_info;

/* is type part of boat, or taken from the race ? */
typedef struct boat_str {
  int         racing;        /* has the boat started or not ? bool */
  int         landed;
  int         num ;          /* boat number                        */
  char        *name;         /* boat name                          */
  double      latitude;      /* latitude  in rad                   */
  double      longitude;     /* longitude in rad                   */
  double      wp_latitude;   /* latitude  in rad                   */
  double      wp_longitude;  /* longitude in rad                   */
  double      wp_heading;    /* for fixed angle/wind_angle in rad  */
  double      wp_distance;   /* distance to the WP in nm           */
  double      heading;       /* actual heading in rad              */
  double      loch;          /* loch                               */
  struct race_str *in_race;
  time_t      last_vac_time; /* time of last move                  */
  struct wind_info_str wind; /* the computed wind                  */
  void   (*set_heading_func)();
} boat;


#endif /* _TYPES_H_ */
