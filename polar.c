/**
 * $Id: polar.c,v 1.2 2008/04/24 10:51:41 ylafon Exp $
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

#include "defs.h"
#include "types.h"

void init_polar0();
void init_polar1();
void init_fill_blanks PARAM1(int);

/**
 * boat type / wind angle (deg) / speed (kts) 
 * current granularity is one degree, no interpolation,
 * rounding to nearest integer;
 * and linear interpolation for speed
 */
double polar[MAX_POLAR][181][61];

/**
 * finds current speed based on boat location,
 * wind_speed (in kts), wind_angle from boat's heading (in rad)
 * return boat speed (in kts)
 */
double find_speed(aboat, wind_speed, wind_angle)
     boat *aboat;
     double wind_speed;
     double wind_angle;
{
  int intangle;
  int intspeed;
  double valfloor, valceil;
#ifdef VLM_COMPAT
  /* in VLM compatibility mode, we interpolate only speed, not angle
     which is rounded to nearest integer */
  intangle = rint(radToDeg(fabs(fmod(wind_angle, TWO_PI))));
  if (intangle > 180) {
    intangle = 360 - intangle;
  }
  intspeed = floor(wind_speed);
  valfloor = polar[aboat->in_race->boattype][intangle][intspeed];
  valceil  = polar[aboat->in_race->boattype][intangle][intspeed+1];
#else
  /* higher reolution mode, where bilinear interpolation is performed
     (angle and speed) */
  double tvalfloor, tvalceil, tangle;
  int intangle_p1;
  tangle = radToDeg(fabs(fmod(wind_angle, TWO_PI)));
  if (tangle > 180.0) {
    tangle = 360.0 - tangle;
  }
  intangle = (int)floor(tangle);
  /* special case when we reach 180 */
  if (intangle == 180) {
    intangle_p1 = 179;
  } else {
    intangle_p1 = intangle+1;
  }
  intspeed  = floor(wind_speed);
  valfloor  = polar[aboat->in_race->boattype][intangle][intspeed];
  tvalfloor = polar[aboat->in_race->boattype][intangle_p1][intspeed];
  valfloor += (tvalfloor - valfloor)*(tangle - (double)intangle);

  valceil  = polar[aboat->in_race->boattype][intangle][intspeed+1];
  tvalceil = polar[aboat->in_race->boattype][intangle_p1][intspeed+1];
  valceil += (tvalceil - valceil)*(tangle - (double)intangle);
#endif /* VLM_COMPAT */
  /* linear interpolation for wind speed */
  return (valfloor + (valceil-valfloor)*(wind_speed-(double)intspeed));
}


void init_polar() {
  init_polar0();
  init_polar1();
}

/**
 * Polar files have one value per 5 degrees, and per 2 kts,
 * starting with 0/0.
 * This function fills all the remaining values using linear interpolation
 */
void init_fill_blanks(boat_type)
     int boat_type;
{
  int i,j,k;
  
  for (i=0; i<=60; i+=2) {
    for (j=1; j<180; j++) {
      if (!(j%5)) continue;
      k=j-(j%5);
      polar[0][j][i]=polar[0][k][i]+(polar[0][k+5][i]-polar[0][k][i])*((double)(j%5))/5.0;
    } 
  }
  for (i=1; i<60; i+=2) {
    for (j=0; j<=180; j++) {
      polar[0][j][i]=(polar[0][j][i-1]+polar[0][j][i+1])/2.0;
    }
  }
}

#include "polar_0.inc"
#include "polar_1.inc"
