/**
 * $Id: boats.h,v 1.2 2011-03-26 21:53:31 ylafon Exp $
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

#ifndef _VLMC_ENGINE_BOATS_H_
#define _VLMC_ENGINE_BOATS_H_

#include <defs.h>
#include <types.h>
#include <mysql.h>
#include <stdarg.h>

#include "enginetypes.h"

// move to DB interaction ?
int record_arrival                  PARAM2(engine_boat *, double);
int clear_valid_waypoint_crossing   PARAM1(engine_boat *);
int check_invalid_waypoint_crossing PARAM1(engine_boat *);
int clear_invalid_waypoint_crossing PARAM1(engine_boat *);

void   set_boat_stopped     PARAM1(engine_boat *);
void   engine_move_boat     PARAM1(engine_boat *);
void   compute_target_wp    PARAM1(engine_boat *);
void   update_boat_heading  PARAM1(engine_boat *);
void   check_autopilot      PARAM2(engine_race *, engine_boat *);
void   check_boat           PARAM2(engine_race *, engine_boat *);

#endif /* _VLMC_ENGINE_BOATS_H_ */
