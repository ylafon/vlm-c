/**
 * $Id: windserver.c,v 1.1 2008/07/30 12:16:42 ylafon Exp $
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
#include <sys/shm.h>

#include "defs.h"
#include "types.h"
#include "winds.h"
#include "grib.h"
#include "context.h"
#include "shmem.h"

vlmc_context *global_vlmc_context;

int main(int argc, char **argv) {
  int shmid;
  void *segmaddr;

  global_vlmc_context = calloc(1, sizeof(vlmc_context));
  init_context_default(global_vlmc_context);

  if (argc > 1) {
    set_grib_filename(global_vlmc_context, argv[1]);
  }
  
  init_grib();

  /* FIXME used semaphore */

  shmid = get_grib_shmid();
  if (shmid == -1) {
    /* not there, we create it */
    shmid = create_grib_shmid(&global_vlmc_context->windtable);
  }

  segmaddr = get_grib_shmem(shmid, 0);
  copy_grib_array_to_shmem(&global_vlmc_context->windtable, segmaddr);
  shmdt(segmaddr);
  
  /* FIXME release semaphore */
  
  return 0;
}
