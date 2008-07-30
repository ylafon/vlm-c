/**
 * $Id: shmem.h,v 1.6 2008/07/30 19:07:59 ylafon Exp $
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

#ifndef _SHMEM_H_
#define _SHMEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "defs.h"
#include "types.h"

/* The value is VLM SEM (a=1 z=26) encoded by blocks of 5 bits */
#define VLM_SEMAPHORE_KEY ((22<<25)|(12<<20)|(13<<15)|(19<<10)|(5<<5)|13)

/* The value is VLM KEY (a=1 z=26) encoded by blocks of 5 bits */
#define VLM_SEMAPHORE_MEM_KEY ((22<<25)|(12<<20)|(13<<15)|(11<<10)|(5<<5)|25)

/* The value is VLM GSH (a=1 z=26) encoded by blocks of 5 bits */
#define VLM_COAST_MEM_KEY ((22<<25)|(12<<20)|(13<<15)|(7<<10)|(19<<5)|19)

/* The value is VLM GRB (a=1 z=26) encoded by blocks of 5 bits */
#define VLM_GRIB_MEM_KEY ((22<<25)|(12<<20)|(13<<15)|(7<<10)|(18<<5)|2)

#define DEFAULT_NB_SHARED_GRIB_ENTRIES 61

int create_semaphore ();

/**
 * create the shared memory entry in order to store a grib array
 * @param windtable, a pointer to a <code>winds_prev</code> structure
 * used to get the number of entries in the grib. If this parameter is NULL
 * the default  DEFAULT_NB_SHARED_GRIB_ENTRIES is used
 * @return an int, the shmid of the segment
 */
int create_grib_shmid PARAM1(winds_prev *);
int get_grib_shmid ();
int get_semaphore_id ();

void *get_grib_shmem PARAM2(int, int);
/**
 * Copy the wind previsions (in windtable) in the shared memory segment
 * If the segment is too small, it may be resized 
 * @param shmid an int, the id of the shared memory segment
 * @param windtable a pointer to a <code>winds_prev</code> structure
 * @param memseg a generic (void *) pointer representing the segment
 * @return an int, the shmid. If it changed, the user must check that
 * the memseg pointer is still valid
 */
int copy_grib_array_to_shmem PARAM3(int, winds_prev *, void *);
void construct_grib_array_from_shmem PARAM2(winds_prev *, void *);
void allocate_grib_array_from_shmem PARAM2(winds_prev *, void *);

#endif /* _SHMEM_H_ */
