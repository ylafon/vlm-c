/**
 * $Id: grib.c,v 1.11 2008/05/07 09:37:20 ylafon Exp $
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
#include "grib.h"
#include "wgrib/grib.h"
#include "wgrib/bms.h"
#include "wgrib/bds.h"
#include "wgrib/gds.h"
#include "wgrib/pds4.h"

extern vlmc_context global_vlmc_context;

/* local definition */
winds **read_gribs PARAM1(int *);

#define MSEEK 4096
#define BUFF_ALLOC0  1048576

void init_grib() {
  init_grib_offset(GRIB_TIME_OFFSET);
}

void set_grib_offset(time_offset) 
     long time_offset;
{
  global_vlmc_context.windtable.time_offset = time_offset;
}

long get_grib_offset() 
{
  return global_vlmc_context.windtable.time_offset;
}

void init_grib_offset(time_offset)
     long time_offset;
{
  winds **w, **oldw;
  int nb_prevs, oldcount, i;

  w = read_gribs(&nb_prevs);
  /* no error, cleanup old data */
  printf("NB prevs: %d\n", nb_prevs);
  if (w) {
    oldcount = global_vlmc_context.windtable.nb_prevs;
    oldw = global_vlmc_context.windtable.wind;
    global_vlmc_context.windtable.nb_prevs    = nb_prevs; 
    global_vlmc_context.windtable.wind        = w;
    global_vlmc_context.windtable.time_offset = time_offset;
    if (oldw != NULL) {
      for (i=0; i<oldcount; i++) {
	free(oldw[i]);
      }
      free(oldw);
    }
  }
}

winds **read_gribs(nb_prevs) 
  int *nb_prevs;
{
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
  int           in_error, gribtype;

  /* to make the compiler happy */
  winds_t = NULL;
  array = NULL;
  gribtime = 0;
  in_error = 0;
  /**
   * first we read the file, find the number or records, alloc memory for structures
   * then reread the file to fill them
   */
  if (!global_vlmc_context.grib_filename) {
    printf("FATAL: global_vlmc_context not initialized\n");
    return NULL;
  }
  
  gribfile = fopen(global_vlmc_context.grib_filename, "r");
  if (gribfile == NULL) {
    printf("FATAL: unable to open \"%s\"\n", global_vlmc_context.grib_filename);
    return NULL;
  }

  if ((buffer = (unsigned char *) calloc(BUFF_ALLOC0, sizeof(char))) == NULL) {
    printf("FATAL: not enough memory while allocating buffer to read grib files\n");
    return NULL;
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
    return NULL;
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
	return NULL;
      }
    }
    if (read_grib(gribfile, pos, len_grib, buffer) == 0) {
      printf("INIT GRIB error, could not read to end of record %d\n",i*2);
      in_error = 1;
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
      in_error = 1;
      break;
    }
    
    if (pointer[0] != 0x37 || pointer[1] != 0x37 ||
	pointer[2] != 0x37 || pointer[3] != 0x37) {
      printf("INIT GRIB: missing end section\n");
      in_error = 1;
      break;
    }
    if (gds == NULL) {
      /* we check only full grids */
      printf("INIT GRIB: wind grid invalid\n");
      in_error = 1;
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
      in_error = 1;
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
    gribtype = PDS_PARAM(pds); /* 33 is U, 34 is V */
    gribtime_tm.tm_year   = PDS_Year4(pds) - 1900;
    gribtime_tm.tm_mon    = PDS_Month(pds) - 1; /* As January is 0 and not 1 */
    gribtime_tm.tm_mday   = PDS_Day(pds); 
    gribtime_tm.tm_hour   = PDS_Hour(pds);
    gribtime_tm.tm_min    = 0;
    gribtime_tm.tm_sec    = 0;
    gribtime_tm.tm_isdst  = 0;
#ifndef __CYGWIN
    gribtime_tm.tm_gmtoff = 0;
#endif /* __CYGWIN */
    gribtime = timegm(&gribtime_tm);
#ifdef DEBUG
    printf("Time: %ld", gribtime);
#endif /* DEBUG */
    if (PDS_ForecastTimeUnit(pds) == HOUR) {
      gribtime += PDS_P2(pds) * 3600;
    } else {
      printf("Unknown forecat time unit %d, contact maintainer\n", 
	     PDS_ForecastTimeUnit(pds));
      in_error = 1;
      break;
    }
#ifdef DEBUG
    printf(" -> %ld\n", gribtime);
    printf("Time Unit: %d\n", PDS_ForecastTimeUnit(pds));
    printf("Time P1: %d\n", PDS_P1(pds));
    printf("Time P2: %d\n", PDS_P2(pds));
    printf("Time Range: %d\n", PDS_TimeRange(pds));
#endif /* DEBUG */
    /* get or create a new winds structure */
    if (w[i/2] == NULL) {
      winds_t = calloc(1, sizeof(winds));
      winds_t->prevision_time = gribtime;
      w[i/2] = winds_t;
#ifdef DEBUG
      printf("Prev time: %ld, %s\n", winds_t->prevision_time, 
	     ctime(&winds_t->prevision_time));
#endif /* DEBUG */
    } else {
      winds_t = w[i/2];
    }
    /* fill depending on grid type */
    if (gribtype == 34) { /* VGRD */
#ifdef GRIB_RESOLUTION_1
      for (y=0; y<ny; y+=2) {
	for (x=0; x<nx; x+=2) {
	  winds_t->wind_v[x/2][180 - y/2] = (double) array[y*nx+x];
	}
      }
#else
      for (y=0; y<ny; y++) {
	for (x=0; x<nx; x++) {
	  winds_t->wind_v[x][360 - y] = (double) array[y*nx+x];
	}
      }
#endif /* GRIB_RESOLUTION_1 */
      winds_t = NULL;
    } else if (gribtype == 33) { /* UGRD */
#ifdef GRIB_RESOLUTION_1
      for (y=0; y<ny; y+=2) {	
	for (x=0; x<nx; x+=2) {
	  winds_t->wind_u[x/2][180 - y/2] = (double) array[y*nx+x];
	}
      }
#else /* should we check we match GRIB_REOLUTION_0_5 ? */
      for (y=0; y<ny; y++) {	
	for (x=0; x<nx; x++) {
	  winds_t->wind_u[x][360 - y] = (double) array[y*nx+x];
	}
      }
#endif /* GRIB_RESOLUTION_1 */
    } else {
      in_error = 1;
      break;
    }
    free(array);
    array = NULL;
    pos += len_grib;
  }
  /* free structures */
  free(buffer);
  fclose(gribfile);
  /* in case of error, do some cleanup */
  if ((i < count) || in_error) {
    if (array) {
      free(array);
    }
    count = i;
    for (i=0; i<count; i++) {
      free(w[i]);
    }
    free(w);
    return NULL;
  } else {
    *nb_prevs = count/2;
    return w;
  }
}
