/**
 * $Id: gshhs.c,v 1.3 2008/05/04 08:39:45 ylafon Exp $
 *
 * (c) 2008 by Yves Lafon
 *
 * Parts of code Copyright (c) 1996-2007 by P. Wessel and W. H. F. Smith
 * Released under GPLv2 License.
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
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "defs.h"
#include "types.h"
#include "dist_gshhs.h"

#define MAX_DETAILS 3 /* 1 land, 2 lake, 3 island_in_lake, 4 pond_in_island_in_lake */

extern vlmc_context global_vlmc_context;
int segnum[3601][1800]; /* this one goes here as otherwise the structure is not allocated */

void init_coastline() {
  FILE *coastfile;
  struct GSHHS h;
  struct POINT p;
  int nb_read, level, greenwich,i,k;
  int x,y, prev_x, prev_y;
  double longitude, latitude, prev_longitude, prev_latitude;
  coast_seg *segment;
  int flip;
  double min_longitude, max_longitude;
  min_longitude = 100000.0;
  max_longitude = -100000.0;

  /* to make the compiler happy */
  prev_y =0;
  prev_longitude = prev_latitude = 0.0;

  memset(segnum, 0, 3601*1800*sizeof(int));
    
  coastfile = fopen(global_vlmc_context.gshhs_filename, "r");
  if (coastfile == NULL) {
    printf("Fatal error trying to read %s\n", global_vlmc_context.gshhs_filename);
    exit(2);
  }
  /* we read the file twice: once for finding the number of segments per
     0.1 deg square, then alloc the structures, and another time to fill
     the structure. this is to avoid building linked lists that are slower
     to traverse during the processing of the boats */

  nb_read = fread ((void *)&h, (size_t)sizeof (struct GSHHS), 
		   (size_t)1, coastfile);
  flip = (((h.flag >> 8) & 0xff) != GSHHS_DATA_VERSION);

  while (nb_read == 1) {
    if (flip) {
      h.id = swabi4 ((unsigned int)h.id);
      h.n  = swabi4 ((unsigned int)h.n);
      h.west  = swabi4 ((unsigned int)h.west);
      h.east  = swabi4 ((unsigned int)h.east);
      h.south = swabi4 ((unsigned int)h.south);
      h.north = swabi4 ((unsigned int)h.north);
      h.area  = swabi4 ((unsigned int)h.area);
      h.flag  = swabi4 ((unsigned int)h.flag);
    }
    level = h.flag & 0xff;
    greenwich = (h.flag >>16) & 0xff;
    if (level > MAX_DETAILS) { /* keep only land ?, not lake, island in lake, 
				  pond in island in lake => take everything now */
      for (i=0; i<h.n; i++) {
	if (fread ((void *)&p, (size_t)sizeof(struct POINT), 
		   (size_t)1, coastfile) != 1) {
	  printf ("Fatal error reading Error reading file %s\n",
		  global_vlmc_context.gshhs_filename);
	  exit(2);
	}
      }
    } else {
      prev_x = -1;
      for (i=0; i<h.n; i++) {
	if (fread ((void *)&p, (size_t)sizeof(struct POINT), 
		   (size_t)1, coastfile) != 1) {
	  printf ("Fatal error reading Error reading file %s\n",
		  global_vlmc_context.gshhs_filename);
	  exit(2);
	}
	if (flip) {
	  p.x = swabi4 ((unsigned int)p.x);
	  p.y = swabi4 ((unsigned int)p.y);
	}
	x = floor((double)p.x * GSHHS_SCL * 10.0);
	y = floor((double)p.y * GSHHS_SCL * 10.0)+900;
	assert((x>=0 && x<=3600) && (y>=0 && y< 1800));
	if (prev_x == -1) {
	  prev_x = x;
	  prev_y = y;
	  continue;
	}
	if (prev_x == x) {
	  if (prev_y == y) {
	    segnum[x][y]++;
	  } else {
	    segnum[x][y]++;
	    segnum[x][prev_y]++;
	  } 
	} else {
	  if (prev_y == y) {
	    segnum[x][y]++;
	    segnum[prev_x][y]++;
	  } else {
	    segnum[x][y]++;
	    segnum[x][prev_y]++;
	    segnum[prev_x][y]++;
	    segnum[prev_x][prev_y]++;
	  }
	}
	prev_x = x;
	prev_y = y;
      }
    }
    nb_read = fread((void *)&h, (size_t)sizeof (struct GSHHS), (size_t)1, 
		    coastfile);
  }
  fclose (coastfile);

  /* now allocate the structures */
  for (x=0; x<=3600; x++) {
    for (y=0; y<1800; y++) {
      global_vlmc_context.shoreline[x][y].nb_segments = segnum[x][y];
      global_vlmc_context.shoreline[x][y].seg_array = calloc(segnum[x][y], sizeof(struct coast_seg_str));
    }
  }

  coastfile = fopen(global_vlmc_context.gshhs_filename, "r");
  if (coastfile == NULL) {
    printf("Fatal error trying to read %s\n", global_vlmc_context.gshhs_filename);
    exit(2);
  }
    
  nb_read = fread ((void *)&h, (size_t)sizeof (struct GSHHS), 
		   (size_t)1, coastfile);
    
  while (nb_read == 1) {
    if (flip) {
      h.id = swabi4 ((unsigned int)h.id);
      h.n  = swabi4 ((unsigned int)h.n);
      h.west  = swabi4 ((unsigned int)h.west);
      h.east  = swabi4 ((unsigned int)h.east);
      h.south = swabi4 ((unsigned int)h.south);
      h.north = swabi4 ((unsigned int)h.north);
      h.area  = swabi4 ((unsigned int)h.area);
      h.flag  = swabi4 ((unsigned int)h.flag);
    }
    level = h.flag & 0xff;
    greenwich = (h.flag >>16) & 0xff;
    if (level > MAX_DETAILS) { /* keep only land ?, not lake, island in lake, 
				  pond in island in lake => take everything now */
      for (i=0; i<h.n; i++) {
	if (fread ((void *)&p, (size_t)sizeof(struct POINT), 
		   (size_t)1, coastfile) != 1) {
	  printf ("Fatal error reading Error reading file %s\n",
		  global_vlmc_context.gshhs_filename);
	  exit(2);
	}
      }
    } else {
      prev_x = -1;
      for (i=0; i<h.n; i++) {
	if (fread ((void *)&p, (size_t)sizeof(struct POINT), 
		   (size_t)1, coastfile) != 1) {
	  printf ("Fatal error reading Error reading file %s\n",
		  global_vlmc_context.gshhs_filename);
	  exit(2);
	}
	if (flip) {
	  p.x = swabi4 ((unsigned int)p.x);
	  p.y = swabi4 ((unsigned int)p.y);
	}
	x = floor((double)p.x * GSHHS_SCL * 10.0);
	y = floor((double)p.y * GSHHS_SCL * 10.0)+900;
	longitude = degToRad((double)p.x * GSHHS_SCL);
	latitude = degToRad((double)p.y * GSHHS_SCL);
	assert((x>=0 && x<=3600) && (y>=0 && y< 1800));
	if (prev_x == -1) {
	  prev_x = x;
	  prev_y = y;
	  prev_longitude = longitude;
	  prev_latitude = latitude;
	  continue;
	}

#define _add_segment(a,b) k = --segnum[a][b];		\
	segment = &global_vlmc_context.shoreline[a][b].seg_array[k];	\
	segment->longitude_a = prev_longitude;		\
	segment->longitude_b = longitude;		\
	segment->latitude_a = prev_latitude;		\
	segment->latitude_b = latitude


	if (prev_x == x) {
	  if (prev_y == y) {
	    _add_segment(x,y);
	  } else {
	    _add_segment(x,y);
	    _add_segment(x, prev_y);
	  } 
	} else {
	  if (prev_y == y) {
	    _add_segment(x,y);
	    _add_segment(prev_x,y);
	  } else {
	    _add_segment(x,y);
	    _add_segment(x,prev_y);
	    _add_segment(prev_x,y);
	    _add_segment(prev_x,prev_y);
	  }
	}
	prev_x = x;
	prev_y = y;
	prev_longitude = longitude;
	prev_latitude = latitude;
      }
    }
    nb_read = fread((void *)&h, (size_t)sizeof (struct GSHHS), (size_t)1, 
		    coastfile);
  }
  fclose (coastfile);
}
