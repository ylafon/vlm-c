# $Id: vlmc-unittest.py,v 1.1 2008/04/25 11:58:41 ylafon Exp $
#
# (c) 2008 by paparazzia adapted from unittest.c by ylafon
#      See COPYING file for copying and redistribution conditions.
#
#      This program is free software you can redistribute it and/or modify
#      it under the terms of the GNU General Public License as published by
#      the Free Software Foundation version 2 of the License.
#
#      This program is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#      GNU General Public License for more details.
#
# Contact: <paparazzia@gmail.com>

from vlmc import *
from math import radians, degrees
import time

lat1  = radians(10)
long1 = radians(10)

lat2  = radians(11.1)
long2 = radians(9)

lat3  = radians(30)
long3 = radians(-10)

lata  = radians(10)
longa = radians(9)

latb  = radians(11.1)
longb = radians(10)

print "Unit test: distance from Point 1 to Line A-B"
print "Point1: lat %.3f, long %.3f" % (degrees(lat1), degrees(long1))
print "Line A: lat %.3f, long %.3f" % (degrees(lata), degrees(longa))
print "Line B: lat %.3f, long %.3f" % (degrees(latb), degrees(longb))

print "Distance Point1 -> Point A: %.3f" % (ortho_distance(lat1, long1, lata, longa))
print "Distance Point1 -> Point B: %.3f" % (ortho_distance(lat1, long1, latb, longb))
print "Distance Point1 -> Line A-B: %.3f" % (distance_to_line(lat1, long1, lata, longa, latb, longb))

print "Unit test: distance from Point 2 to Line A-B"
print "Point2: lat %.3f, long %.3f" % (degrees(lat2), degrees(long2))
print "Line A: lat %.3f, long %.3f" % (degrees(lata), degrees(longa))
print "Line B: lat %.3f, long %.3f" % (degrees(latb), degrees(longb))

print "Distance Point2 -> Point A: %.3f" % ortho_distance(lat2, long2, lata, longa)
print "Distance Point2 -> Point B: %.3f" % ortho_distance(lat2, long2, latb, longb)
print "Distance Point2 -> Line A-B: %.3f" % distance_to_line(lat2, long2, lata, longa, latb, longb)

print "Unit test: distance from Point 3 to Line A-B"
print "Point3: lat %.3f, long %.3f" % ( degrees(lat3), degrees(long3))
print "Line A: lat %.3f, long %.3f" % ( degrees(lata), degrees(longa))
print "Line B: lat %.3f, long %.3f" % ( degrees(latb), degrees(longb))

print "Distance Point3 -> Point A: %.3f" % ortho_distance(lat3, long3, lata, longa)
print "Distance Point3 -> Point B: %.3f" % ortho_distance(lat3, long3, latb, longb)
print "Distance Point3 -> Line A-B: %.3f" % distance_to_line(lat3, long3, lata, longa, latb, longb)

print "\nWind test"
init_grib()

# commented out for now, but it seems to work
# init_coastline()

current_time = long(time.time())
print current_time
lat_boat     = radians(39.812)
long_boat    = radians(8.43)

wind_boat = wind_info()

for i in range(0, 4) :
    print "Date: %s" % time.ctime(current_time)
    # each 15mn
    get_wind_info_latlong_UV(lat_boat, long_boat, current_time, wind_boat)
    print "UV   Wind at  lat: %.2f long: %.2f, speed %.1f angle %.1f" % ( degrees(lat_boat), degrees(long_boat), wind_boat.speed, degrees(wind_boat.angle) )
    get_wind_info_latlong_TWSA(lat_boat, long_boat, current_time, wind_boat)
    print "TWSA Wind at  lat: %.2f long: %.2f, speed %.1f angle %.1f" % ( degrees(lat_boat), degrees(long_boat), wind_boat.speed, degrees(wind_boat.angle) )
    current_time += (15 * 60)
