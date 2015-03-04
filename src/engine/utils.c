/**
 * $Id: utils.c,v 1.1 2010-12-09 10:22:32 ylafon Exp $
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>

#include <mysql.h>

#include <defs.h>
#include <types.h>

#include "enginedefs.h"
#include "enginetypes.h"

/**
 * print information in a local buffer
 * @return the number of printed chars
 */
int thl_printf(engine_boat *cur_boat, const char *format, ...) {
  th_context *ctx;
  va_list argp;
  int nump;

  va_start(argp, format);
  ctx = cur_boat->context;
  if (!ctx->buffer) {
    ctx->buffer = calloc(TH_STATUS_BUFFER_SIZE, sizeof(char));
    ctx->pb = ctx->buffer;
  }
  nump = vsnprintf(ctx->pb, (TH_STATUS_BUFFER_SIZE-(ctx->pb-ctx->buffer)),
		   format, argp);
  ctx->pb +=nump;
  va_end(argp);
  return nump;
}

/**
 * Print boat status update information
 8 @return the number of printed chars
 */
int print_boat_status(engine_boat *cur_boat) {
  th_context *ctx;
  int num;

  ctx = cur_boat->context;
  if (ctx->buffer) {
    num = (ctx->pb-ctx->buffer);
    printf("%s", ctx->buffer);
    ctx->pb = ctx->buffer;
    return num;
  }
  return 0;
}
