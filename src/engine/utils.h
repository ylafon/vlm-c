/**
 * $Id: utils.h,v 1.1 2010-12-09 10:22:32 ylafon Exp $
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

#ifndef _VLMC_ENGINE_UTILS_H_
#define _VLMC_ENGINE_UTILS_H_

#include <defs.h>
#include <types.h>
#include <mysql.h>
#include <stdarg.h>

#include "enginetypes.h"

int    thl_printf           PARAM3(engine_boat *, const char *f, ...);
int    print_boat_status    PARAM1(engine_boat *);

#endif /* _VLMC_ENGINE_UTILS_H_ */
