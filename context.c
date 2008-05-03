/**
 * $Id: context.c,v 1.1 2008/04/28 15:37:31 ylafon Exp $
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
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "types.h"

/**
 * tell if we are in VLM compatibility mode
 * returns a boolean, true if in vlm mode
 * false otherwise
 */
extern vlmc_context global_vlmc_context;

void context_set(structelem, fname)
     char **structelem;
     char *fname;
{
  char *c;
  int len;
  if (*structelem) {
    free(*structelem);
    *structelem = NULL;
  }
  if (fname) {
    len = strlen(fname);
    c = calloc(len+1, sizeof(char));
    if (c) {
      strncpy(c, fname, len);
      *structelem = c;
    } else {
      printf("FATAL: while allocating memory for context initialization\n");
    }
  }
}

void set_grib_filename(fname)
     char *fname;
{
  context_set(&global_vlmc_context.grib_filename, fname);
}

void set_gshhs_filename(fname)
     char *fname;
{
  context_set(&global_vlmc_context.gshhs_filename, fname);
}
void set_polar_definition_filename(fname)
     char *fname;
{
  context_set(&global_vlmc_context.polar_definition_filename, fname);
}

void init_context()
{
  global_vlmc_context.windtable.wind     = NULL;
  global_vlmc_context.windtable.nb_prevs = 0;
  memset(global_vlmc_context.shoreline, 0, 3601*1800*sizeof(coast_zone));
}

void init_context_default() {
  set_grib_filename("latest.grb");
  set_gshhs_filename("gshhs.b");
  set_polar_definition_filename(NULL);
  init_context();
}