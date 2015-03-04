/**
 * $Id: races.h,v 1.7 2010-11-27 15:31:33 ylafon Exp $
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

#ifndef _VLMC_ENGINE_RACES_H_
#define _VLMC_ENGINE_RACES_H_

#include <defs.h>
#include <types.h>
#include <mysql.h>

#include "enginetypes.h"

void   get_race_pilototo  PARAM2(MYSQL *, engine_race *);
void   get_race_waypoints PARAM2(MYSQL *, engine_race *);
void   get_boats_in_race  PARAM2(MYSQL *, engine_race *);
int    get_all_races      PARAM1(MYSQL *);
void   stop_race          PARAM2(MYSQL *, engine_race *);
void   check_race         PARAM1(engine_race *);


#endif /* _VLMC_ENGINE_RACES_H_ */
