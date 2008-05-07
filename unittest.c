/**
 * $Id: unittest.c,v 1.7 2008/05/07 20:20:31 ylafon Exp $
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

#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

#include "defs.h"
#include "types.h"
#include "ortho.h"
#include "lines.h"
#include "winds.h"
#include "grib.h"
#include "context.h"

vlmc_context global_vlmc_context;

int main (argc, argv) 
    int argc;
    char **argv;
{
  double lat1,long1, lat2, long2, lat3, long3;
  double lata, longa, latb, longb;
  
  double lat_boat, long_boat;
  time_t current_time, previ_time;
  int i;
  wind_info wind_boat;
 
  init_context_default();

  lat1  = degToRad(10);
  long1 = degToRad(10);
  
  lat2  = degToRad(11.1);
  long2 = degToRad(9);

  lat3  = degToRad(30);
  long3 = degToRad(-10);

  lata  = degToRad(10);
  longa = degToRad(9);

  latb  = degToRad(11.1);
  longb = degToRad(10);

  printf("Unit test: distance from Point 1 to Line A-B\n");
  printf("Point1: lat %.3f, long %.3f\n", radToDeg(lat1), radToDeg(long1));
  printf("Line A: lat %.3f, long %.3f\n", radToDeg(lata), radToDeg(longa));
  printf("Line B: lat %.3f, long %.3f\n", radToDeg(latb), radToDeg(longb));

  printf("Distance Point1 -> Point A: %.3f\n", ortho_distance(lat1, long1,
							      lata, longa));
  printf("Distance Point1 -> Point B: %.3f\n", ortho_distance(lat1, long1,
							      latb, longb));
  printf("Distance Point1 -> Line A-B: %.3f\n", 
	 distance_to_line(lat1, long1, lata, longa, latb, longb));

  printf("\nUnit test: distance from Point 2 to Line A-B\n");
  printf("Point2: lat %.3f, long %.3f\n", radToDeg(lat2), radToDeg(long2));
  printf("Line A: lat %.3f, long %.3f\n", radToDeg(lata), radToDeg(longa));
  printf("Line B: lat %.3f, long %.3f\n", radToDeg(latb), radToDeg(longb));

  printf("Distance Point2 -> Point A: %.3f\n", ortho_distance(lat2, long2,
							      lata, longa));
  printf("Distance Point2 -> Point B: %.3f\n", ortho_distance(lat2, long2,
							      latb, longb));
  printf("Distance Point2 -> Line A-B: %.3f\n", 
	 distance_to_line(lat2, long2, lata, longa, latb, longb));

  printf("\nUnit test: distance from Point 3 to Line A-B\n");
  printf("Point3: lat %.3f, long %.3f\n", radToDeg(lat3), radToDeg(long3));
  printf("Line A: lat %.3f, long %.3f\n", radToDeg(lata), radToDeg(longa));
  printf("Line B: lat %.3f, long %.3f\n", radToDeg(latb), radToDeg(longb));

  printf("Distance Point3 -> Point A: %.3f\n", ortho_distance(lat3, long3,
							      lata, longa));
  printf("Distance Point3 -> Point B: %.3f\n", ortho_distance(lat3, long3,
							      latb, longb));
  printf("Distance Point3 -> Line A-B: %.3f\n", 
	 distance_to_line(lat3, long3, lata, longa, latb, longb));

  printf("\nWind test\n");
  init_grib();
  purge_gribs();
  previ_time = get_max_prevision_time();
  printf("Max prevision time: %s\n", ctime(&previ_time));
  time(&current_time);
  lat_boat     = degToRad(39.812);
  long_boat    = degToRad(8.43);
  for (i=0; i<4; i++) {
    printf("Date: %s", ctime(&current_time));
    /* each 15mn */
    get_wind_info_latlong_UV(lat_boat, long_boat, current_time, &wind_boat);
    printf("UV   Wind at  lat: %.2f long: %.2f, speed %.1f angle %.1f\n",
	   radToDeg(lat_boat), radToDeg(long_boat),
	   wind_boat.speed, radToDeg(wind_boat.angle));
    get_wind_info_latlong_TWSA(lat_boat, long_boat, current_time, &wind_boat);
    printf("TWSA Wind at  lat: %.2f long: %.2f, speed %.1f angle %.1f\n",
	   radToDeg(lat_boat), radToDeg(long_boat),
	   wind_boat.speed, radToDeg(wind_boat.angle));
    current_time += (15 * 60);
  }
  return 0;
}
