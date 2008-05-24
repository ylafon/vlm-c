/**
 * $Id: vlm.c,v 1.4 2008/05/24 15:12:19 ylafon Exp $
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

/**
 * using VLM's PIM definition, set the heading function of the boat
 * accordingly 
 * @param aboat, a pointer to a boat struct
 * @param vlm_mode, an int, the numeric value of the PIM
 */
void set_vlm_pilot_mode(boat *aboat, int vlm_mode) {

   switch (vlm_mode) {
   case 1:
     /* needs heading filled in aboat->wp_heading */
     aboat->set_heading_func=&set_heading_constant;
     break;
   case 2:
     /* needs heading filled in aboat->wp_heading */
     aboat->set_heading_func=&set_heading_wind_angle;
     break;
   case 3:
     /* needs WP filled in aboat->wp_lat/long */
     aboat->set_heading_func=&set_heading_ortho;
     break;
   case 4:
     /* needs WP filled in aboat->wp_lat/long */
     aboat->set_heading_func=&set_heading_bvmg;
     break;
   default:
     /* add a warning? fail? exit? */
     ;
   }
}
