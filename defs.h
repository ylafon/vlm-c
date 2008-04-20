/**
 * $Id: defs.h,v 1.1 2008/04/20 12:30:37 ylafon Exp $
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

#ifndef _DEFS_H_
#define _DEFS_H_

#include <math.h>

#if 0
#  define PI     M_PIl
#  define PI_2   M_PI_2l
#  define TWO_PI M_PIl * 2
#else
#  define PI     M_PI
#  define PI_2   M_PI_2
#  define TWO_PI M_PI * 2
#endif /* 0 */  


#define degToRad(angle) ((angle/180.0) * PI)
#define radToDeg(angle) ((angle*180.0) / PI)
#define msToKts(speed) (1.9438445*speed)
#define ktsToMs(speed) (0.51444444*speed)


#ifdef PARAM1
#  undef PARAM1
#endif /* PARAM1 */
#ifdef PARAM2
#  undef PARAM2
#endif /* PARAM2 */
#ifdef PARAM3
#  undef PARAM3
#endif /* PARAM3 */
#ifdef PARAM4
#  undef PARAM4
#endif /* PARAM4 */



#ifdef __STDC__
#  define PARAM1(a) (a)
#  define PARAM2(a,b) (a,b)
#  define PARAM3(a,b,c) (a,b,c)
#  define PARAM4(a,b,c,d) (a,b,c,d)
#  define PARAM5(a,b,c,d,e) (a,b,c,d,e)
#  define PARAM6(a,b,c,d,e,f) (a,b,c,d,e,f)
#  define PARAM7(a,b,c,d,e,f,g) (a,b,c,d,e,f,g)
#  define PARAM8(a,b,c,d,e,f,g,h) (a,b,c,d,e,f,g,h)
#  define PARAM9(a,b,c,d,e,f,g,h,i) (a,b,c,d,e,f,g,h,i)
#  define PARAM10(a,b,c,d,e,f,g,h,i,j) (a,b,c,d,e,f,g,h,i,j)
#else
#  define PARAM1(a) ()
#  define PARAM2(a,b) ()
#  define PARAM3(a,b,c) ()
#  define PARAM4(a,b,c,d) () 
#  define PARAM5(a,b,c,d,e) ()
#  define PARAM6(a,b,c,d,e,f) ()
#  define PARAM7(a,b,c,d,e,f,g) ()
#  define PARAM8(a,b,c,d,e,f,g,h) ()
#  define PARAM9(a,b,c,d,e,f,g,h,i) ()
#  define PARAM10(a,b,c,d,e,f,g,h,i,j) ()
#  define const
#endif /* __STDC__ */

#define BOAT_CLASS_40   0
#define BOAT_IMOCA_2007 1
#define MAX_POLAR       2


#define WIND_GRID_LONG 720
#define WIND_GRID_LAT 361

#endif /* _DEFS_H_ */
