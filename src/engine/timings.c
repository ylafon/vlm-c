/**
 * $Id: timings.c,v 1.2 2010-11-16 10:09:10 ylafon Exp $
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

#include <stdio.h>
#include <sys/time.h>

#include "timings.h"

/**
 * get current time in second, including subseconds as a double
 */
double get_microtime() {
  struct timeval tv;
  struct timezone tz;

  if(!gettimeofday(&tv, &tz)) {
    return ((0.000001 * tv.tv_usec) + tv.tv_sec);
  }
  /* if there is something wrong, we return -1.0 (see @time) */
  return -1.0;
}
