/**
 * $Id: vlm.c,v 1.1 2008/05/19 21:36:14 ylafon Exp $
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
#include "defs.h"
#include "types.h"
#include "loxo.h"
#include "vmg.h"
#include "ortho.h"

void set_vlm_pilot_mode(boat *aboat, int vlm_mode) {

   switch (vlm_mode) {
   case 1:
     aboat->set_heading_func=&set_heading_loxo;
     break;
   case 2:
     aboat->set_heading_func=&set_heading_wind_angle;
     break;
   case 3:
     aboat->set_heading_func=&set_heading_ortho;
     break;
   case 4:
     aboat->set_heading_func=&set_heading_bvmg;
     break;
   default:
     /* add a warning? fail? exit? */
     ;
   }
}
   
     
