/**
 * $Id: windserver.c,v 1.2 2008/07/30 15:00:59 ylafon Exp $
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
#include <sys/sem.h>

#include "defs.h"
#include "types.h"
#include "winds.h"
#include "grib.h"
#include "context.h"
#include "shmem.h"

vlmc_context *global_vlmc_context;

int main(int argc, char **argv) {
  int shmid, semid;
  void *segmaddr;
  struct sembuf sem_op[2];
  
  /* TODO add options like -merge -interpolate -replace */
  global_vlmc_context = calloc(1, sizeof(vlmc_context));
  init_context_default(global_vlmc_context);

  if (argc > 1) {
    set_grib_filename(global_vlmc_context, argv[1]);
  }

  /* first we read the grib before locking things */
  init_grib();

  semid = get_semaphore_id();
  if (semid == -1) {
    semid = create_semaphore();
    if (semid == -1) {
      fprintf(stderr, "Unable to create the semaphore\n");
      exit(1);
    }
  }
  sem_op[0].sem_num = 0;
  sem_op[0].sem_op  = 0;
  sem_op[0].sem_flg = SEM_UNDO;
  sem_op[1].sem_num = 0;
  sem_op[1].sem_op  = 1;
  sem_op[1].sem_flg = SEM_UNDO|IPC_NOWAIT;
  if (semop(semid, sem_op, 2) == -1) {
    fprintf(stderr, "Fail to lock the semaphore\n");
    exit(1);
  }
  
  shmid = get_grib_shmid();
  if (shmid == -1) {
    /* not there, we create it */
    shmid = create_grib_shmid(&global_vlmc_context->windtable);
    if (shmid == -1) {
      fprintf(stderr, "Fail to create the GRIB memory segment\n");
      exit(1);
    }
  }

  segmaddr = get_grib_shmem(shmid, 0);
  /* FIXME check the available size */
  copy_grib_array_to_shmem(&global_vlmc_context->windtable, segmaddr);
  shmdt(segmaddr);
  
  sem_op[0].sem_num = 0;
  sem_op[0].sem_op  = -1;
  sem_op[0].sem_flg = SEM_UNDO|IPC_NOWAIT;
  if (semop(semid, sem_op, 1) == -1) {
    fprintf(stderr, "Fail to unlock the semaphore\n");
    exit(1);
  }
  
  return 0;
}
