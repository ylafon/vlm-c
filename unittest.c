/**
 * $Id: unittest.c,v 1.1 2008/04/23 13:45:39 ylafon Exp $
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

coast_zone shoreline[3601][1800];
winds_prev windtable;

int main (argc, argv) 
    int argc;
    char **argv;
{
  double lat1,long1, lat2, long2, lat3, long3;
  double lata, longa, latb, longb;
  
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

  return 0;
}
