/**
 * $Id: grib.c,v 1.1 2008/04/20 12:30:37 ylafon Exp $
 *
 * (c) 2008 by Yves Lafon
 *
 * Parts of this code are taken from wgrib-c v1.8.0.12o (5-07) by Wesley Ebisuzaki
 * and adapted to fit our data structures.
 * See http://www.cpc.ncep.noaa.gov/products/wesley/wgrib.html
 *
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
#include <stdlib.h>

#include "defs.h"
#include "types.h"
#include "wgrib/grib.h"
#include "wgrib/bms.h"
#include "wgrib/bds.h"
#include "wgrib/gds.h"
#include "wgrib/pds4.h"

extern winds_prev windtable;

#define MSEEK 4096
#define BUFF_ALLOC0  1048576

void init_grib() {
  /* by default we are looking at the "latest.grib" file, easy to download the file
     then create a symlink. It can be changed easily anyway, as it would be more
     flexible */
  struct tm     gribtime_tm;
  time_t        gribtime;
  FILE          *gribfile;
  unsigned char *buffer, *msg;
  unsigned char *pds, *gds, *bms, *bds, *pointer;
  long          len_grib, pos, buffer_size;
  int           nx, ny;
  long          nxny;
  int           count, wpos;
  int           scan, mode;
  int           i,x,y;
  winds         **w;
  winds         *winds_t;
  float         *array;
  double        temp;

  /* to make the compiler happy */
  winds_t = NULL;
  gribtime = 0;
  /**
   * first we read the file, find the number or records, alloc memory for structures
   * then reread the file to fill them
   */
  gribfile = fopen("latest.grb", "r");
  if (gribfile == NULL) {
    printf("FATAL: unable to open \"latest.grib\"\n");
    return;
  }

  if ((buffer = (unsigned char *) calloc(BUFF_ALLOC0, sizeof(char))) == NULL) {
    printf("FATAL: not enough memory while allocating buffer to read grib files\n");
    return;
  }
  buffer_size = BUFF_ALLOC0;
  pos = 0;
  count = 0;
  do {
    msg = seek_grib(gribfile, &pos, &len_grib, buffer, MSEEK);
    if (msg) {
      count++;
      pos += len_grib;
    }
  } while (msg);
#ifdef DEBUG
  printf("Found %d records, %d wind previsions\n", count, count>>1);
#endif /* DEBUG */
  if (count & 1) {
    printf("Error reading GRIB file, found odd number or records\n");
    return;
  }
  wpos = 0;

  w = calloc (count/2, sizeof(winds *));

  /* now reread the file, and dump stuff one by one */
  pos = 0;
  for (i=0; i<count; i++) {
    msg = seek_grib(gribfile, &pos, &len_grib, buffer, MSEEK);
    /* ensure buffer is enough (should be ok for two records by default */
    if (len_grib + msg - buffer > buffer_size) {
      buffer_size = len_grib + msg - buffer + 1000;
      buffer = (unsigned char *) realloc((void *) buffer, buffer_size);
      if (buffer == NULL) {
	printf("INIT GRIB ran out of memory\n");
	return;
      }
    }
    if (read_grib(gribfile, pos, len_grib, buffer) == 0) {
      printf("INIT GRIB error, could not read to end of record %d\n",i*2);
      break;
    }
    msg = buffer;
    pds = msg+8;
    pointer = pds + PDS_LEN(pds);
    if (PDS_HAS_GDS(pds)) {
      gds = pointer;
      pointer += GDS_LEN(gds);
    } else {
      gds = NULL;
    }
    if (PDS_HAS_BMS(pds)) {
      bms = pointer;
      pointer += BMS_LEN(bms);
    } else {
      bms = NULL;
    }
    bds = pointer;
    pointer += BDS_LEN(bds);
    if (pointer-msg+4 != len_grib) {
      printf("INIT GRIB: Len of grib message is inconsistent.\n");
      break;
    }
    
    if (pointer[0] != 0x37 || pointer[1] != 0x37 ||
	pointer[2] != 0x37 || pointer[3] != 0x37) {
      printf("INIT GRIB: missing end section\n");
      break;
    }
    if (gds == NULL) {
      /* we check only full grids */
      printf("INIT GRIB: wind grid invalid\n");
      break;
    }
    GDS_grid(gds, bds, &nx, &ny, &nxny);
#ifdef DEBUG
    printf("GRID: %d by %d\n", nx, ny);
#endif /* DEBUG */
    if (GDS_LatLon(gds)) {
      scan = GDS_LatLon_scan(gds);
      mode = GDS_LatLon_mode(gds);
    }

    if ((array = (float *) calloc(nxny, sizeof(float))) == NULL) {
      printf("Unable to alloc float array\n");
      break;
    }

    temp = int_power(10.0, - PDS_DecimalScale(pds));
    BDS_unpack(array, bds, BMS_bitmap(bms), BDS_NumBits(bds), nxny,
	       temp*BDS_RefValue(bds),temp*int_power(2.0, BDS_BinScale(bds)));
    
    /* ok, now we have the GDS, let's work on it */
#ifdef DEBUG
    for (x=0; x<10; x++) {
      printf("%.3f ", array[x]);
    }
    printf("\n");
#endif /* DEBUG */
    /**
     * we have two passes, one for U, one for V, we alloc only the first time
     * as well as the prevision time 
     * FIXME: should we _verify_ that the time is the same ? 
     */
    if (!i) {
      gribtime_tm.tm_year   = PDS_Year4(pds) - 1900;
      gribtime_tm.tm_mon    = PDS_Month(pds) - 1; /* As January is 0 and not 1 */
      gribtime_tm.tm_mday   = PDS_Day(pds); 
      gribtime_tm.tm_hour   = PDS_Hour(pds);
      gribtime_tm.tm_min    = 0;
      gribtime_tm.tm_sec    = 0;
      gribtime_tm.tm_isdst  = 0;
      gribtime_tm.tm_gmtoff = 0;
      gribtime = timegm(&gribtime_tm);
    }
    if (i&1) {
      for (y=0; y<ny; y++) {
	for (x=0; x<nx; x++) {
	  winds_t->wind_v[x][360 - y] = (double) array[y*nx+x];
	}
      }
      winds_t = NULL;
    } else {
      winds_t = calloc(1, sizeof(winds));
      winds_t->prevision_time = gribtime + (3*1800*i); /* 1800 as i is *2  FIXME do it
							from GRIB forecast info */
#ifdef DEBUG
      printf("Prev time: %ld, %s\n", winds_t->prevision_time, 
	     ctime(&winds_t->prevision_time));
#endif /* DEBUG */
     w[i/2] = winds_t;
      for (y=0; y<ny; y++) {	
	for (x=0; x<nx; x++) {
	  winds_t->wind_u[x][360 - y] = (double) array[y*nx+x];
	}
      }
    }
    free(array);
    pos += len_grib;
  }
  if (i < count) {
    /* we got an error, dealloc stuff now */
  } else {
    windtable.nb_prevs = count/2; 
    windtable.wind     = w;
  }
}
