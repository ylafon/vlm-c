/**
 * $Id: move.h,v 1.3 2010/12/09 13:32:14 ylafon Exp $
 *
 * (c) 2008 by Yves Lafon
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

#ifndef _VLMC_MOVE_H_
#define _VLMC_MOVE_H_

#include <math.h>

#include "defs.h"
#include "types.h"

/**
 * move a boat by one vacation 
 * param: a pointer to a boat structure
 */
void move_boat PARAM1(boat *);

/**
 * move a boat by 'n' vacations
 * @param a pointer to a boat structure
 * @param an int, the number of vacations
 * @return an boolean, true if the boat landed, false if still on water
 */
int move_boat_n_vac PARAM2(boat *, int);

#endif /* _VLMC_MOVE_H_ */
