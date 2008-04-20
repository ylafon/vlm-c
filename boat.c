/**
 * $Id: boat.c,v 1.1 2008/04/20 12:30:37 ylafon Exp $
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

#include <string.h>
#include <stdlib.h>

#include "types.h"
#include "loxo.h"

boat *init_boat(aboat, num, name, latitude, longitude, heading) 
boat *aboat;
int num;
char *name;
double latitude;
double longitude;
double heading;
{
  aboat->num = num;
  aboat->name = (char *)calloc(strlen(name)+1, sizeof(char));
  strcpy(aboat->name, name);
  aboat->latitude = latitude;
  aboat->longitude = longitude;
  aboat->set_heading_func = &set_heading_loxo;
  /* FIXME fill this as well */
  aboat->loch = 0.0;
  aboat->heading = 0.0;
  aboat->wp_latitude = 0.0;
  aboat->wp_longitude = 0.0;
  aboat->wp_distance = 0.0;
  return aboat;
}

boat *set_wp(aboat, latitude, longitude, heading) 
boat *aboat;
double latitude;
double longitude;
double heading;
{
  aboat->wp_latitude = latitude;
  aboat->wp_longitude = longitude;
  aboat->wp_heading = heading;
  return aboat;
}
