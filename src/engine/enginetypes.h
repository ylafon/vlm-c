/**
 * $Id: enginetypes.h,v 1.8 2011-03-26 21:53:31 ylafon Exp $
 *
 * (c) 2010 by Yves Lafon
 *
 *      See COPYING file for copying and redistribution conditions.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: <yves@raubacapeu.net>
 */

#ifndef _VLMC_ENGINE_ENGINETYPES_H_
#define _VLMC_ENGINE_ENGINETYPES_H_

#include <defs.h>
#include <types.h>

#include <mysql.h>

#include "enginedefs.h"

/**
 * General structure to access the DB
 */
typedef struct db_access_str {
  char *host;
  int  port;
  char *username;
  char *passwd;
  char *dbname;
} db_access;

/**
 * thread-local context
 */
typedef struct th_context_str {
  char *buffer; /* buffer used for printouts         */
  char *pb;     /* pointer within that buffer        */
  MYSQL db;    /* thread-local connection to the db */
} th_context;

/**
 * Pilototo statements parsed
 * To be used in cunjunction with engine race type.
 */
typedef struct pilot_task_str {
  int    taskid;
  time_t scheduled;
  int    boatid;
  int    pim;
  double pip;
  double wp_lat;
  double wp_lon;
  double wp_heading;
  int    mode;
  int    status;
} pilot_task;

/**
 * boat wrapper for the engine
 */
typedef struct engine_boat_str {
  boat *vlm_boat;
  double lat;
  double lon;
  double hdg;           /* computed heading         */
  double original_lat;  /* latitude prior any move  */
  double original_lon;  /* longitude prior any move */
  double vac_duration;  /* duration in hours        */
  double vac_ratio;
  int  nwp;
  int  status;
  int  is_arrived;      /* 0 if not arrived 1 if so */
  th_context *context;
} engine_boat;

/**
 * Race wrapper for the engine
 */
typedef struct engine_race_str {
  race        *vlm_race;
  engine_boat *boats;
  pilot_task  *tasks;
  int         nb_pilot_tasks;
  int         status;
} engine_race;

#endif /* _VLMC_ENGINE_ENGINETYPES_H_ */

