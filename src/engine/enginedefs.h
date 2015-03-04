/**
 * $Id: enginedefs.h,v 1.7 2011-03-26 21:53:31 ylafon Exp $
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

#ifndef _VLMC_ENGINE_DEFS_H_
#define _VLMC_ENGINE_DEFS_H_

#include <defs.h>
#include <types.h>

#define VMGMODE 0
#define TH_STATUS_BUFFER_SIZE 8192

/* status change bitmask */

/* PIM changed */
#define BOAT_STATUS_CHG_PIM 1
/* Heading changed */
#define BOAT_STATUS_CHG_HDG (1<<1)
/* Pilot param (windangle mostly) changed */
#define BOAT_STATUS_CHG_PIP (1<<2)
/* waypoint changed */
#define BOAT_STATUS_CHG_WPT (1<<3)
/* boat moved to DNF */
#define BOAT_STATUS_CHG_DNF (1<<4)
/* departure time changed */
#define BOAT_STATUS_CHG_DPT (1<<5)
/* next race wayoint changed */
#define BOAT_STATUS_CHG_NWP (1<<6)
/* release time changed (after a coast penalty) */
#define BOAT_STATUS_CHG_RLT (1<<7)


#define INVALID_CROSSING 0
#define VALID_CROSSING 1

#define FLOAT_EQ_LIMIT 0.000001

/* taken form config-defines.php */
#define WPLL 2000

#define MAX_DURATION 315360000
#define MAX_STOPTIME (3*86400)

#define PILOTMODE_HEADING 1
#define PILOTMODE_WINDANGLE 2
#define PILOTMODE_ORTHODROMIC 3
#define PILOTMODE_BESTVMG 4 
#define PILOTMODE_VBVMG 5
#define PILOTMODE_BESTSPEED 6

#define BOAT_STATUS_ARR 1
#define BOAT_STATUS_HTP -1
#define BOAT_STATUS_DNF -2
#define BOAT_STATUS_ABD -3
#define BOAT_STATUS_HC -4

#define RACE_TYPE_CLASSIC 0
#define RACE_TYPE_RECORD 1
#define RACE_TYPE_OMORMB 2

#define RACE_ENDED -1
#define RACE_PENDING 0
#define RACE_STARTED 1

#endif /* _VLMC_ENGINE_DEFS_H_ */

