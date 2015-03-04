/**
 * $Id: boats.c,v 1.9 2011-03-29 21:36:30 ylafon Exp $
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
#include <polar.h>
#include <shmem.h>
#include <useshmem.h>
#include <vlm.h>
#include <boat.h>
#include <winds.h>
#include <lines.h>
#include <waypoint.h>
#include <ortho.h>
#include <vmg.h>
#include <lines.h>
#include <loxo.h>

#include "enginedefs.h"
#include "enginetypes.h"
#include "timings.h"
#include "races.h"
#include "utils.h"

extern race *all_races;
extern boat *all_boats;
extern vlmc_context *global_vlmc_context;
extern int nb_boats, nb_races;
extern time_t now;

/**
 * lock a boat for some time (defined by the race definition)
 */
void lock_boat(engine_boat *cur_boat) {
  boat *b;
  race *r;

  b = cur_boat->vlm_boat;
  r = b->in_race;
  b->release_time = now + r->coastpenalty;
  cur_boat->status |= BOAT_STATUS_CHG_RLT;
}

/**
 * after a coast crossing, -> mooring
 */
void set_boat_stopped(engine_boat *cur_boat) {
  boat *b;

  b = cur_boat->vlm_boat;
  if (b->pim != PILOTMODE_WINDANGLE) {
    b->pim = PILOTMODE_WINDANGLE;
    cur_boat->status |= BOAT_STATUS_CHG_PIM;
  }
  b->wangle = 0.0;
  cur_boat->status |= BOAT_STATUS_CHG_PIP;
}

int record_arrival(engine_boat *cur_boat, double duration) {
  time_t race_time;
  char cmd[1024];
  boat *b;
  th_context *ctx;
  MYSQL_RES *result;
  MYSQL_ROW row;
  MYSQL *db;
  race *r;
  time_t deptime, old_race_time;

  // FIXME TODO $fullUsersObj->users->logUserEvent("Boat arrived in race ".$fullRacesObj->races->idraces);
  race_time = duration;
  ctx = cur_boat->context;
  db = &ctx->db;
  b = cur_boat->vlm_boat;
  r = b->in_race;
  if (r->racetype & RACE_TYPE_RECORD) {
    // we get the previous arrival time and compare..
    deptime = b->departure_time;
    old_race_time = 0;
    sprintf(cmd, "SELECT duration FROM races_results WHERE idusers=%d AND idraces=%d", b->num, r->idraces);
    mysql_query(db, cmd);
    result = mysql_use_result(db);
    row = mysql_fetch_row(result);
    if (row) {
      old_race_time = sscanf(row[0], "%ld", &old_race_time);
    } 
    // not present, not finished or less than previous -> replace (insert)
    if (old_race_time <=0 || old_race_time > race_time) {
      thl_printf(cur_boat, "\t\tBoat arrived...\n");
      sprintf(cmd, "REPLACE INTO races_results (idraces,idusers,position,deptime,duration,loch,longitude,latitude) VALUES (%d,%d,%d,%ld,%ld,%lf,%lf,%lf)",
	      r->idraces, b->num, BOAT_STATUS_ARR, deptime, 
	      race_time, b->loch, b->longitude, b->latitude);
      mysql_query(db, cmd);
      return mysql_affected_rows(db);
    }
    thl_printf(cur_boat, "Boat arrived, but (%ld) is not better (%ld)\n",
	       race_time, old_race_time);
  } else {
    thl_printf(cur_boat, "\t\tBoat arrived...\n");
    deptime = r->deptime;
    sprintf(cmd, "INSERT INTO races_results (idraces,idusers,position,deptime,duration,loch,longitude,latitude) VALUES (%d,%d,%d,%lu,%lu,%lf,%lf,%lf)",
	    r->idraces, b->num, BOAT_STATUS_ARR, deptime, 
	    race_time, b->loch, b->longitude, b->latitude);
    mysql_query(db, cmd);
    return mysql_affected_rows(db);
  }
  // if nothing happened, return 0 rows updated
  return 0;
}

/**
 * record waypoint crossing
 */
int record_waypoint_crossing(engine_boat *cur_boat, time_t xingtime,
			     int validity) {
  char cmd[512];
  boat *b;
  th_context *ctx;
  MYSQL *db;

  ctx = cur_boat->context;
  db = &ctx->db;
  b = cur_boat->vlm_boat;
  sprintf(cmd, "REPLACE INTO waypoint_crossing (idraces,idwaypoint,idusers,validity,time,userdeptime) VALUES (%d,%d,%d,%d,%ld,%ld)", 
	  b->in_race->idraces, b->nwp, b->num, validity, 
	  xingtime, b->departure_time);
  printf("DEBUG: [%s]\n", cmd);
  mysql_query(db, cmd);
  return mysql_affected_rows(db);
}

/**
 * Delete from the DB the valid wp crossing.
 * @return the number of valid wp crossing removed
 */
int clear_valid_waypoint_crossing(engine_boat *cur_boat) {
  char cmd[512];
  boat *b;
  th_context *ctx;
  MYSQL *db;

  ctx = cur_boat->context;
  db = &ctx->db;
  b = cur_boat->vlm_boat;
  sprintf(cmd, "DELETE FROM waypoint_crossing WHERE idusers=%d AND idraces=%d AND validity=1 AND idwaypoint=%d AND userdeptime=%ld", 
	  b->num, b->in_race->idraces, b->nwp, b->departure_time);
  printf("DEBUG: [%s]\n", cmd);
  mysql_query(db, cmd);
  return mysql_affected_rows(db);
}

/**
 * get from the DB the number of invalid crossing of this WP
 * @return the unmber of invalid wp crossing
 */
int check_invalid_waypoint_crossing(engine_boat *cur_boat) {
  char cmd[512];
  boat *b;
  th_context *ctx;
  MYSQL_RES *result;
  MYSQL_ROW row;
  MYSQL *db;
  int inv_count;

  inv_count = 0;
  ctx = cur_boat->context;
  db = &ctx->db;
  b = cur_boat->vlm_boat;
  sprintf(cmd, "SELECT count(*) AS nbinvalid FROM waypoint_crossing WHERE validity=0 AND idusers=%d AND idraces=%d AND idwaypoint=%d", 
	  b->num, b->in_race->idraces, b->nwp);
  printf("DEBUG: [%s]\n", cmd);
  mysql_query(db, cmd);
  result = mysql_use_result(db);
  row = mysql_fetch_row(result);
  if (row) {
    inv_count = sscanf(row[0], "%d", &inv_count);
  } 
  mysql_free_result(result);
  return inv_count;
}

/**
 * Delete from the DB the invalid wp crossings.
 * @return the unmber of invalid wp crossing removed
 */
int clear_invalid_waypoint_crossing(engine_boat *cur_boat) {
  char cmd[512];
  boat *b;
  th_context *ctx;
  MYSQL *db;

  ctx = cur_boat->context;
  db = &ctx->db;
  b = cur_boat->vlm_boat;
  sprintf(cmd, "DELETE FROM waypoint_crossing WHERE idusers=%d AND idraces=%d AND idwaypoint=%d", 
	  b->num, b->in_race->idraces, b->nwp);
  printf("DEBUG: [%s]\n", cmd);
  mysql_query(db, cmd);
  return mysql_affected_rows(db);
}

void check_waypoint_crossing(engine_boat *cur_boat) {
  double lat_check, lon_check;
  double lat_pre_check, lon_pre_check;
  double lat_xing, lon_xing, xing_ratio;
  boat *b;
  race *r;
  int wpidx, wp_xed;
  int is_arrived, prev_wp_invalidated;
  int waypoint_crossed, invalid_crossing;
  waypoint *pwp, *wp;
  double prev_time, duration;
  time_t xing_time;

  b = cur_boat->vlm_boat;
  r = b->in_race;
  wpidx = b->nwp;
  // get the current and past waypoints
  wp = &r->waypoint_list[wpidx-1];
  if (wpidx > 1) {
    pwp = &r->waypoint_list[wpidx-2];
  } else {
    pwp = NULL;
  }
  // get the relevant segment
  lat_pre_check = cur_boat->original_lat;
  lon_pre_check = cur_boat->original_lon;
  lat_check = b->latitude;
  lon_check = b->longitude;
  prev_time = b->last_vac_time;
  xing_time = 0;

  prev_wp_invalidated = 0;
  if (pwp && (pwp->type & WP_CROSS_ONCE)) {
    thl_printf(cur_boat, ", checking for previous WP crossing... ");
    thl_printf(cur_boat, "\n\t\t* WP   : %f, %f <---> %f, %f (%d)", 
	       radToDeg(pwp->latitude1), radToDeg(pwp->longitude1),
	       radToDeg(pwp->latitude2), radToDeg(pwp->longitude2),
	       pwp->type);
    wp_xed = VLM_check_WP(lat_pre_check, lon_pre_check,
			  lat_check, lon_check, pwp,
			  &lat_xing, &lon_xing, &xing_ratio);
    switch (wp_xed) {
    case -1:
      if (pwp->type & (WP_CROSS_CLOCKWISE|WP_CROSS_ANTI_CLOCKWISE)) {
	thl_printf(cur_boat, "\n\t\t* INVALID (anti)Clockwise Crossing of previous WP -> INVALIDATING");
	b->nwp -= 1;
	cur_boat->status |= BOAT_STATUS_CHG_NWP;
	clear_valid_waypoint_crossing(cur_boat);
	prev_wp_invalidated = 1;
      }
      break;
    case 1:
      thl_printf(cur_boat, "\n\t\t* INVALID Crossing of previous WP -> INVALIDATING");
      b->nwp -= 1;
      cur_boat->status |= BOAT_STATUS_CHG_NWP;
      clear_valid_waypoint_crossing(cur_boat);
      prev_wp_invalidated = 1;
      break;
    case 0:
    default:
      // normal case, do nothing
      break;
    }
  }

  if (!prev_wp_invalidated) {
    if (r->racetype & RACE_TYPE_RECORD) {
      duration = difftime(b->last_vac_time, b->departure_time);
    } else {
      duration = difftime(b->last_vac_time, r->deptime);
    } 
    do {
      thl_printf(cur_boat, "\n\tNext Waypoint is %d", b->nwp);
      thl_printf(cur_boat, ", checking for WP crossing... ");
      thl_printf(cur_boat, "\n\t\t* WP   : %lf, %lf <---> %lf, %lf", 
		 radToDeg(wp->latitude1), radToDeg(wp->longitude1),
		 radToDeg(wp->latitude2), radToDeg(wp->longitude2));
      thl_printf(cur_boat, "\n\t\t* BOAT : %f, %f <---> %f, %f", 
		 radToDeg(lat_pre_check), radToDeg(lon_pre_check),
		 radToDeg(lat_check), radToDeg(lon_check));
      
      waypoint_crossed = 0;
      invalid_crossing = 0;
      wp_xed = VLM_check_WP(lat_pre_check, lon_pre_check,
			    lat_check, lon_check, pwp,
			    &lat_xing, &lon_xing, &xing_ratio);   
      switch (wp_xed) {
      case -1:
	waypoint_crossed=1;
	invalid_crossing=1;
	break;
      case  1:
	waypoint_crossed=1;
	break;
      case 0:
      default:
	// all false
	break;
      }
      if (invalid_crossing) {
	thl_printf(cur_boat, "\n\t\t*** Yes INVALID ***\n");
	record_waypoint_crossing(cur_boat, now, INVALID_CROSSING); 
	// exit the do-while loop now
	break;
      }
      if (waypoint_crossed) {
	thl_printf(cur_boat, "\n\t\t*** Yes (vlm-c) ***\n");
	if (wp->type & (WP_CROSS_CLOCKWISE|WP_CROSS_ANTI_CLOCKWISE)) {
	  if (check_invalid_waypoint_crossing(cur_boat)) {
	    thl_printf(cur_boat, "\n\t\t*** INVALID CROSSING ***\n");
	    clear_invalid_waypoint_crossing(cur_boat);
	    break;
	  }
	}
	// remove the beginning of the segment up to the intersection 
	// point to test further WP crossing
	lat_pre_check = lat_xing;
	lon_pre_check = lon_xing;
	thl_printf(cur_boat, "\t==>Boat %d crossed waypoint %d",
		   b->num, b->nwp);
	
	// compute crossing time, and record it
	// now check the duration, it depends on the race type
	duration = duration + xing_ratio*((double)now - prev_time);
	prev_time = prev_time + xing_ratio*((double)now - prev_time);
	xing_time = prev_time;
	record_waypoint_crossing(cur_boat, xing_time, VALID_CROSSING);

	// waypoint recorded, now check if it is the last one
	is_arrived = (b->nwp == r->nb_waypoints);
	if (is_arrived) {
	  cur_boat->is_arrived = is_arrived;
	  thl_printf(cur_boat, "\t==>Course =%d\n\t==>encounterCoordinates = %lf,%lf\n",
		     r->idraces, radToDeg(lat_xing), radToDeg(lon_xing));
	  record_arrival(cur_boat, duration);
	} else {
	  b->nwp++;
	  cur_boat->status |= BOAT_STATUS_CHG_NWP;
	}
      } else {
	thl_printf(cur_boat, ":\t *** NO ***");
      }
    } while (!is_arrived && waypoint_crossed);
  }

}

/**
 * check coast crossing
 */
void check_coast_crossing(engine_boat *cur_boat, 
			  double new_lat, double new_lon) {
  double time_ratio, coast_check, coast_lat, coast_lon;
  boat *b;

  b = cur_boat->vlm_boat;
  thl_printf(cur_boat, "\n\tChecking for coast crossing (vlm-c)\n \tUsing: %s\n ", 
	 global_vlmc_context->grib_filename);

  coast_check = check_coast(b->latitude, b->longitude, new_lat, new_lon,
			    &coast_lat, &coast_lon);
  if (coast_check > -1) {
    time_ratio = coast_check;
    b->landed = 1;
    // coast check is between 0->360, transform in -180/+180
    if (coast_lon > M_PI) {
      coast_lon -= TWO_PI;
    }

    // FIXME TODO $fullUsersObj->users->logUserEvent("Coast has been crossed at " . $encounter_lat/1000 . ", " . $encounter_long/1000);

    thl_printf(cur_boat, "\t*** YES player %d CROSSED (vlmc), ", b->num);
    thl_printf(cur_boat, "\n\t\tCoast has been crossed : \n\t\t\t");
    thl_printf(cur_boat, "BOAT : %lf,%lf <----> %lf,%lf", 
	   radToDeg(b->latitude), radToDeg(b->longitude), 
	   radToDeg(new_lat), radToDeg(new_lon));
    thl_printf(cur_boat, "\n\t\t\tEncounterCoordinates %lf, %lf", radToDeg(coast_lat), 
	   radToDeg(coast_lon));
    thl_printf(cur_boat, "\n\nGoogleMap http://maps.google.fr/maps?f=q&hl=fr&geocode=&q=%lf,%lf&ie=UTF8&spn=0.0191,0.082998&t=p&z=11&iwloc=addr. \n\n", 
	   radToDeg(coast_lat), radToDeg(coast_lon));
    thl_printf(cur_boat, "VLMMAP http://virtual-loup-de-mer.org/mercator.img.php?idraces=%d",
	   b->in_race->idraces);
    thl_printf(cur_boat, "&lat=%lf&long=%lf&maparea=18&tracs=on&age=2&list=%d",
	   radToDeg(b->latitude), radToDeg(b->longitude), b->num);
    thl_printf(cur_boat, "&x=1000&y=600&proj=mercator&text=right&seg1=%lf,%lf:%lf,%lf",
	   radToDeg(coast_lat), radToDeg(coast_lon), 
	   radToDeg(coast_lat), radToDeg(coast_lon));
    thl_printf(cur_boat, "&seg2=%lf,%lf:%lf,%lf\n\n", 
	   radToDeg(b->latitude), radToDeg(b->longitude),
	   radToDeg(new_lat), radToDeg(new_lon));
    // correct the distance
    time_ratio *= 0.9;
    new_lat = b->latitude + time_ratio*(new_lat - b->latitude);
    new_lon = b->longitude + time_ratio*(new_lon - b->longitude);

    while (check_coast(b->latitude, b->longitude, new_lat, new_lon,
		       &coast_lat, &coast_lon) > -1) {
      thl_printf(cur_boat, "*** Safety engaged for new point, moving back again\n");
      new_lat = b->latitude + 0.9* (new_lat - b->latitude);
      new_lon = b->longitude + 0.9* (new_lon - b->longitude);
      time_ratio *= 0.9;
    }
    thl_printf(cur_boat, "\nVLMMAP corrected position http://virtual-loup-de-mer.org/mercator.img.php?idraces=%d", b->in_race->idraces);
    thl_printf(cur_boat, "&lat=%lf&long=%lf&maparea=18&tracks=on&age=1",
	   radToDeg(b->latitude), radToDeg(b->longitude));
    thl_printf(cur_boat, "&list=%d&x=1000&y=600&proj=mercator&text=right", b->num);
    thl_printf(cur_boat, "&seg2=%lf,%lf:%lf,%lf\n\n", 
	   radToDeg(b->latitude), radToDeg(b->longitude),
	   radToDeg(new_lat), radToDeg(new_lon));   
    // we adjust the time ratio (helps finding the exact time of the coast crossing */
    cur_boat->vac_ratio = time_ratio;
  }
  // now move effectively the boat
  b->latitude = new_lat;
  b->longitude = new_lon;
}
/**
 * Move the boat
 * Note that we have the heading, wind information, we now need
 * to find speed and time length, move the boat, check for coast and
 * WPs
 */
void engine_move_boat(engine_boat *cur_boat) {
  boat *b;
  double speed;
  double vac_dist, vac_hours;
  double new_lat, new_lon;

  b = cur_boat->vlm_boat;
  // save previous location
  cur_boat->original_lat = b->latitude;
  cur_boat->original_lon = b->longitude;

  if (b->pim == PILOTMODE_WINDANGLE) {
    speed = find_speed(b, b->wind.speed, b->wangle);
  } else {
    speed = find_speed(b, b->wind.speed, b->wind.angle - b->heading);
  }
  vac_hours = difftime(now, b->last_vac_time)/3600.0;
  vac_dist = speed*vac_hours;
  // now move the boat in loxodromic mode
  raw_move_loxo(b->latitude, b->longitude,
		vac_dist, b->heading, &new_lat, &new_lon);
  thl_printf(cur_boat, "\n\tPosition update (WSpd=%.1lf, WHdg=%.1lf, Hours=%.4lf, ANG=%.2lf, Hdg=%lf, BS=%.3lf, Dist=%lfnm) ... done", b->wind.speed, radToDeg(b->wind.angle),
	 vac_hours, radToDeg(b->wind.angle - b->heading), radToDeg(b->heading),
	 speed, vac_dist);

  cur_boat->vac_duration = vac_hours;
  cur_boat->vac_ratio = 1.0;
  // now check the coast
  check_coast_crossing(cur_boat, new_lat, new_lon);
  check_waypoint_crossing(cur_boat);

  if (!cur_boat->is_arrived) {
    if (b->landed) { //not arrived and coast crossed!
      set_boat_stopped(cur_boat);
      lock_boat(cur_boat);
    }
    // FIXME write positions/loch/ranking
  }
  // FIXME check coast result + waypoint crossing
}
/**
 * based on the target lat/lon found in the db, we compute
 * the real targeted point, either the exact point from the 
 * db, or the closest point from a waypoint gate
 */
void compute_target_wp(engine_boat *cur_boat) {
  boat *b;
  race *r;
  waypoint *w;
  int wpidx;
  b = cur_boat->vlm_boat;
  if ((fabs(b->real_wp_lat)<FLOAT_EQ_LIMIT) &&
      (fabs(b->real_wp_lon)<FLOAT_EQ_LIMIT) ) {
    wpidx = b->nwp; 
    r = b->in_race;
    w = &r->waypoint_list[wpidx-1]; // wp index starts at 1 and not 0
    // get only classification waypoints (no need to check boundaries
    // as the last WP is _always_ a classification WP).
    while (w->type & (WP_ICE_GATE_N|WP_ICE_GATE_S)) {
      w++;
      wpidx++;
    }
    best_way_to_waypoint(b, w);
  } else {
    b->wp_latitude = b->real_wp_lat;
    b->wp_longitude = b->real_wp_lon;
  }
}
    
void update_boat_heading(engine_boat *cur_boat) {
  boat *b;
  b = cur_boat->vlm_boat;

  switch (b->pim) {
  case PILOTMODE_HEADING:
    b->wangle = b->heading - b->wind.angle;
    break;
  case PILOTMODE_WINDANGLE:
    b->heading = b->wangle + b->wind.angle;
    cur_boat->status |= BOAT_STATUS_CHG_HDG;
    break;
  case PILOTMODE_ORTHODROMIC:
    compute_target_wp(cur_boat);
    set_heading_ortho_nowind(b);
    cur_boat->status |= BOAT_STATUS_CHG_HDG;
    break;
  case PILOTMODE_BESTVMG:
    compute_target_wp(cur_boat);
    set_heading_bvmg(b, VMGMODE);
    cur_boat->status |= BOAT_STATUS_CHG_HDG;
    break;
  case PILOTMODE_VBVMG:
    compute_target_wp(cur_boat);
    set_heading_vbvmg(b, VMGMODE);
    cur_boat->status |= BOAT_STATUS_CHG_HDG;
    break;
  }    
}

void check_autopilot(engine_race *erace, engine_boat *cur_boat) {
  // FIXME, use a thread context?
  int i, nb_tasks, boatid;
  pilot_task *task;
  boat *b;

  // TODO FIXME PILOTOTO OUTPUT
  // $logmsg = sprintf( "executing task %d, PIM=%d, PIP=%s... ** ", $row['taskid'], $PIM, $PIP); 
  // printf("** AUTO_PILOT : $logmsg"); 
  // $this->logUserEvent($logmsg);

  b = cur_boat->vlm_boat;
  boatid = b->num;
  nb_tasks = erace->nb_pilot_tasks;
  task = erace->tasks;

  for (i=0; i<nb_tasks; i++) {
    if (task->boatid >= boatid) {
      break;
    }
    task++;
  }
  if (i<nb_tasks) {
    // tasks are ordered on boat id, hence the test.
    // FIXME give the pinter in a thread-local pointer
    while (task->boatid == boatid) {
      if (b->pim != task->pim) {
	b->pim = task->pim;
	cur_boat->status |= BOAT_STATUS_CHG_PIM;
      }
      switch(task->pim) {
      case 1:
	if (fabs(fmod(b->heading - task->pip, TWO_PI)) > FLOAT_EQ_LIMIT) {
	  b->heading = task->pip;
	  cur_boat->status |= BOAT_STATUS_CHG_HDG;
	}
	break;
      case 2:
	if (fabs(fmod(b->wangle - task->pip, TWO_PI)) > FLOAT_EQ_LIMIT) {
	  b->wangle = task->pip;
	  cur_boat->status |= BOAT_STATUS_CHG_PIP;
	}
	break;
      case 3:
      case 4:
      case 5:
	if ((fabs(b->wp_latitude - task->wp_lat) > FLOAT_EQ_LIMIT) ||
	    (fabs(fmod(b->wp_longitude-task->wp_lon, TWO_PI))>FLOAT_EQ_LIMIT)||
	    (!task->mode && fabs(b->wp_heading+1.0) > FLOAT_EQ_LIMIT) ||
	    (task->mode && 
	     (fabs(fmod(b->wp_heading-task->wp_heading,TWO_PI))>FLOAT_EQ_LIMIT))
	    ) {
	  b->wp_latitude = task->wp_lat;
	  b->wp_longitude = task->wp_lon;
	  b->wp_heading = (task->mode) ? task->wp_heading : -1.0;
	  cur_boat->status |= BOAT_STATUS_CHG_WPT;
	  break;
	}
      default:
	thl_printf(cur_boat, "*** ALERT: UNKNOWN PIM %d\n", task->pim);
      }
      task++;
    }
  }
}

void check_boat(engine_race *erace, engine_boat *cur_boat) {
  boat *b;
  race *r;
  
  check_autopilot(erace, cur_boat);
  b = cur_boat->vlm_boat;
  r = erace->vlm_race;

  if (((b->pim == PILOTMODE_WINDANGLE) && fabs(b->wangle < 1.0)) ||
      (b->release_time > now)) {
    thl_printf(cur_boat, "=== Player %d is mooring : ", b->num);
    if (b->departure_time == -1) {
      // not started check if line is closing or not
      if (r->closetime < now) {
	thl_printf(cur_boat, "========= Player %d set to DNF (Late Start) =======\n",b->num);
	cur_boat->status |= BOAT_STATUS_CHG_DNF;
      } else {
	thl_printf(cur_boat, "========= Player %d not started yet ========\n",b->num);
      }
    } else {
      if (b->last_update +  MAX_STOPTIME < now) {
	thl_printf (cur_boat, "========= Player %d set to DNF (Idling limit) =====\n",b->num);
	cur_boat->status |= BOAT_STATUS_CHG_DNF;
      } else {
	// ok mooring while in race
	// write ranking, position, loch
	//	$fullUsersObj->lastPositions->writePositions();
	//      $fullUsersObj->writeCurrentRanking(0, $now);
	//      $fullUsersObj->writeLoch($now);		
      }
    }
    thl_printf(cur_boat, "\t** DONE ** ");
    return;
  }
  if (b->departure_time == -1) {
    b->departure_time = now;
    cur_boat->status |= BOAT_STATUS_CHG_DPT;
  }
  thl_printf(cur_boat, "\n     Prev Hdg: %lf ", radToDeg(b->heading));
  get_wind_info_latlong(b->latitude, b->longitude, now, &b->wind);
  update_boat_heading(cur_boat);
  thl_printf(cur_boat, "PIM=%d/", b->pim);
  if (b->pim == PILOTMODE_WINDANGLE) {
    thl_printf(cur_boat, "PIP=%lf/", b->wangle);
  }
  thl_printf(cur_boat, "Heading=%lf", radToDeg(b->heading));
  if ((b->pim == PILOTMODE_ORTHODROMIC) || (b->pim == PILOTMODE_BESTVMG) ||
      (b->pim == PILOTMODE_VBVMG)) {
    thl_printf(cur_boat, ", Reaching position=%lf,%lf",radToDeg(b->wp_latitude), 
	   radToDeg(b->wp_longitude));
    if ((b->wp_latitude == b->real_wp_lat) && 
	(b->wp_longitude == b->real_wp_lon)) {
      thl_printf(cur_boat, " MyWP=(%lf,%lf) ",radToDeg(b->wp_latitude), 
	     radToDeg(b->wp_longitude));
      if (b->wp_heading<0) {
	thl_printf(cur_boat, " NO WPH ");
      } else {
	thl_printf(cur_boat, " @WPH=%lf", b->wp_heading);
      }
    } else {
      thl_printf(cur_boat, " bestWayToWP = (%lf,%lf)", radToDeg(b->wp_latitude), 
	     radToDeg(b->wp_longitude));
      // FIXME do we need to waste CPU cycles to display the distance ?
    }
  }
  engine_move_boat(cur_boat);
}
