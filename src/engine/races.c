/**
 * $Id: races.c,v 1.25 2011-03-26 21:53:31 ylafon Exp $
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

#include <mysql.h>

#include <defs.h>
#include <types.h>
#include <polar.h>
#include <shmem.h>
#include <useshmem.h>
#include <vlm.h>
#include <boat.h>

#include "enginetypes.h"
#include "timings.h"
#include "races.h"


extern engine_race *all_races;
extern vlmc_context *global_vlmc_context;
extern int nb_boats, nb_races;
extern time_t now;

void get_race_pilototo(MYSQL *db, engine_race *erace) {
  race *r;
  pilot_task *tasks;
  char cmd[1024];
  int nb_tasks, pim, t;
  double lat, lon, heading;
  MYSQL_RES *result;
  MYSQL_ROW row;
  
  r = erace->vlm_race;
  sprintf(cmd, "SELECT a.idusers,taskid,a.pilotmode,a.pilotparameter FROM auto_pilot AS a,users AS u WHERE u.idusers=a.idusers AND u.engaged=%d AND status='pending' AND a.time<%ld ORDER BY a.idusers,taskid", r->idraces, now);
  printf("DEBUG: [%s]\n", cmd);

  mysql_query(db, cmd);
  result = mysql_store_result(db);

  nb_tasks = mysql_num_rows(result);

  if (nb_tasks) {
    tasks = calloc(nb_tasks, sizeof(pilot_task));
    erace->tasks = tasks;
    while ((row = mysql_fetch_row(result))) {
      sscanf(row[0], "%d", &tasks->boatid);
      sscanf(row[1], "%d", &tasks->taskid);
      sscanf(row[2], "%d", &pim);
      switch(pim) {
      case 1:
      case 2:
	t = sscanf(row[3],"%lf", &heading);
	tasks->pip = degToRad(heading);
	if (!t || (t == EOF)) {
	  nb_tasks--;
	  continue; // invalid entry, will be overridden
	}
	break;
      case 3:
      case 4:
      case 5:
	if (strchr(row[3],'@')) {
	  tasks->mode = 1;
	  sscanf(row[3],"%lf,%lf@%lf", &lat, &lon, &heading);
	  tasks->wp_heading = degToRad(fmod(heading, 360.0));
	} else {
	  sscanf(row[3],"%lf,%lf", &lat, &lon);
	}
	tasks->wp_lat = degToRad(lat);
	tasks->wp_lon = degToRad(fmod(lon, 360.0));
	break;
      default:
	nb_tasks--;
	continue;
      }
      tasks++;
    }
    // if all were invalid, free the allocated memory
    if (!nb_tasks) {
      free(erace->tasks);
    }
  }
  erace->nb_pilot_tasks = nb_tasks;
  mysql_free_result(result);
}


void get_race_waypoints(MYSQL *db, engine_race *erace) {
  race *cur_race;
  char cmd[1024];
  MYSQL_RES *result;
  MYSQL_ROW row;
  int nb_waypoints;
  waypoint *w;
  int wpformat, wporder, laisser_au;
  double latitude1, longitude1, latitude2, longitude2;

  cur_race = erace->vlm_race;
  sprintf(cmd, "SELECT RW.wporder,RW.wpformat,RW.laisser_au,WP.latitude1, WP.longitude1,WP.latitude2,WP.longitude2 FROM races_waypoints RW,waypoints WP WHERE idraces=%d AND RW.idwaypoint=WP.idwaypoint ORDER BY wporder DESC", cur_race->idraces);
  printf("DEBUG: [%s]\n", cmd);
  mysql_query(db, cmd);
  result = mysql_use_result(db);
  row = mysql_fetch_row(result);
  if (row) {
    sscanf(row[0], "%d", &nb_waypoints);
    cur_race->waypoint_list = malloc(nb_waypoints*sizeof(waypoint));
    cur_race->nb_waypoints = nb_waypoints;
    w = &cur_race->waypoint_list[nb_waypoints-1];
    do {
      sscanf(row[0],"%d", &wporder);
      sscanf(row[1],"%d", &wpformat);
      sscanf(row[2],"%d", &laisser_au);
      sscanf(row[3],"%lf", &latitude1);
      sscanf(row[4],"%lf", &longitude1);
      sscanf(row[5],"%lf", &latitude2);
      sscanf(row[6],"%lf", &longitude2);
      VLM_init_waypoint(w, wpformat, wporder, latitude1, longitude1,
			latitude2, longitude2, laisser_au, WPLL);
      w--;
    } while ((row = mysql_fetch_row(result)));
  } else {
    // ERROR?
    cur_race->waypoint_list = NULL;
    cur_race->nb_waypoints = 0;
  }
  mysql_free_result(result);
}

void get_boats_in_race(MYSQL *db, engine_race *erace) {
  char cmd[1024], *c;
  MYSQL_RES *result;
  MYSQL_ROW row;
  boat *b;
  engine_boat *eb;
  race *cur_race;
  int n_boats;
  int idrace;
  double t_lat, t_lon, t_heading;

  cur_race = erace->vlm_race;
  sprintf(cmd, "SELECT US.idusers AS idusers,boattype,boatheading,pilotmode,pilotparameter,engaged,nextwaypoint,userdeptime,lastupdate,US.loch AS loch,targetlat,targetlong,targetandhdg,releasetime,`time`,lat,`long` FROM races_ranking RR,users US, positions AS p, (SELECT MAX(`time`) AS t,`idusers` AS id FROM positions WHERE race=%d GROUP BY idusers) AS pos WHERE id=US.idusers AND RR.idusers=US.idusers AND p.idusers=US.idusers AND `time`=t AND p.race=%d AND RR.idraces=%d AND US.engaged=%d ORDER BY idusers ASC", cur_race->idraces, cur_race->idraces, cur_race->idraces, cur_race->idraces);
  printf("DEBUG: [%s]\n", cmd);

  mysql_query(db, cmd);
  result = mysql_store_result(db);

  n_boats = mysql_num_rows(result);
  b = calloc(n_boats, sizeof(boat));
  eb = calloc(n_boats, sizeof(engine_boat));
  cur_race->boat_list = b;
  cur_race->nb_boats = n_boats;
  erace->boats = eb;

  while ((row = mysql_fetch_row(result))) {
    eb->vlm_boat = b;
    sscanf(row[0], "%d", &b->num);
    c = strchr(row[1], '_');
    if (c) {
      c++; 
      associate_polar_boat(b, c);
    } else {
      associate_polar_boat(b, row[1]);
    }
    sscanf(row[2], "%lf", &t_heading);
    b->heading = degToRad(fmod(t_heading, 360.0));
    sscanf(row[3], "%d", &b->pim);
    sscanf(row[4], "%lf", &b->wangle);
    sscanf(row[5], "%d", &idrace);
    if (idrace == cur_race->idraces)  {
      b->in_race = cur_race;
    }
    sscanf(row[6], "%d", &b->nwp);
    sscanf(row[7], "%ld", (long *)&b->departure_time);
    if (row[8]) {
      sscanf(row[8], "%ld", (long *)&b->last_update);
    }
    sscanf(row[9], "%lf", &b->loch);
    sscanf(row[10], "%lf", &t_lat);
    b->real_wp_lat = degToRad(t_lat);
    sscanf(row[11], "%lf", &t_lon);
    b->real_wp_lon = degToRad(fmod(t_lon, 360.0));
    sscanf(row[12], "%lf", &t_heading);
    b->wp_heading = degToRad(fmod(t_heading, 360.0));
    sscanf(row[13], "%ld", (long *)&b->release_time);
    // now the position
    sscanf(row[14], "%ld", (long *)&b->last_vac_time);
    sscanf(row[15], "%lf", &t_lat);
    b->latitude = degToRad(t_lat/1000.0);
    sscanf(row[16], "%lf", &t_lon);
    b->longitude = degToRad(fmod(t_lon/1000.0, 360.0));
    eb++;
    b++;
  }
  mysql_free_result(result);
}

int get_all_races(MYSQL *db) {
  struct tm calltime;
  char *selector, cmd[1024];
  char sel0[] = " IN (1,2,5,10)";
  char sel5[] = " IN (1,5)";
  char sel2[] = " IN (1,2)";
  char sel1[] = "=1";
  MYSQL_RES *result;
  MYSQL_ROW row;
  int num_fields, i;
  int raceid;
  time_t arrival_dur;
  unsigned int num_races;
  engine_race *erace;
  race *r;

  gmtime_r(&now, &calltime);
  switch (calltime.tm_min % 10) {
  case 0:
    selector=sel0;
    break;
  case 5:
    selector=sel5;
    break;
  case 2:
  case 4:
  case 6:
  case 8:
    selector=sel2;
    break;
  default:
    selector=sel1;
  }

  sprintf(cmd, "SELECT idraces,deptime,closetime,racetype,firstpcttime,coastpenalty,bobegin,boend FROM races WHERE started=1 AND vacfreq%s ORDER BY vacfreq ASC, deptime DESC", selector);
  printf("DEBUG: [%s]\n", cmd);

  mysql_query(db, cmd);
  result = mysql_store_result(db);

  num_fields = mysql_num_fields(result);
  printf("num_fields: %d\n", num_fields);
  num_races = mysql_num_rows(result);
  printf("Got %d\n", num_races);
  all_races = calloc(num_races, sizeof(engine_race));
  erace = all_races;

  while ((row = mysql_fetch_row(result))) {
    r = calloc(1, sizeof(race));
    erace->vlm_race = r;
    /* FIXME check conversion using sscanf return code */
    sscanf(row[0],"%d", &r->idraces);
    sscanf(row[1],"%ld", (long *)&r->deptime);
    sscanf(row[2],"%ld", (long *)&r->closetime);
    sscanf(row[3],"%d", &r->racetype);
    sscanf(row[4],"%d", &r->firstpcttime);
    sscanf(row[5],"%d", &r->coastpenalty);
    sscanf(row[6],"%ld", (long *)&r->bobegin);
    sscanf(row[7],"%ld", (long *)&r->boend);
    r->started = 1;
    printf("*** INITIALIZING RACE %10d ***\n", r->idraces);
    get_race_waypoints(db, erace);
    get_boats_in_race(db, erace);
    get_race_pilototo(db, erace);
    erace++;
  }
  mysql_free_result(result);

  sprintf(cmd, "SELECT r.idraces,min(duration) FROM races_results AS rr,races AS r WHERE r.idraces=rr.idraces AND r.started=1 AND rr.position=1 AND vacfreq%s GROUP BY r.idraces ORDER BY r.vacfreq ASC, r.deptime DESC", selector);
  printf("DEBUG: [%s]\n", cmd);

  mysql_query(db, cmd);
  result = mysql_store_result(db);
  /* and now associate the idraces with the computed race limit time */
  // we keep the same ordering, so we can optimize the loop that way...
  i=0; erace=all_races;
  while ((row = mysql_fetch_row(result))) {
    r = erace->vlm_race;
    sscanf(row[0], "%d", &raceid);
    sscanf(row[1], "%ld", &arrival_dur);
    while(i<num_races) {
      if (raceid == r->idraces) {
	r->maxarrivaltime = ((r->racetype & RACE_TYPE_RECORD) ? 
			     r->closetime :
			     r->deptime) 
	  + arrival_dur*(1.0+r->firstpcttime/100);
	break;
      }
      i++;
      erace++;
    }
  } 
  mysql_free_result(result);
  
  /* now check if a race has to be closed or not */
  for (i=0, erace=all_races; i<num_races; i++, erace++) {
    check_race(erace);
  }
  return num_races;
}

/**
 * stop a race, and do some cleanup if needed
 *
 * NOTE that moving positions to histpos takes time,
 *      it should be better to do that in parallel at the end of the call
 */
void stop_race(MYSQL *db, engine_race *erace) {
  race *the_race;
  char cmd[1024];
  int id;

  the_race = erace->vlm_race;
  id = the_race->idraces;

  printf("=> CLOSING RACE %d\n", id);
  
  sprintf(cmd, "UPDATE races SET `started`=-1 WHERE idraces=%d", id);
  mysql_query(db, cmd);
  if (mysql_affected_rows(db) == (my_ulonglong) -1 ) {
    // FIXME ERROR
  }

  sprintf(cmd, "DELETE FROM races_ranking WHERE idraces=%d", id);
  mysql_query(db, cmd);
  if (mysql_affected_rows(db) == (my_ulonglong) -1 ) {
    // ERROR
  }

  /* we update the boats only if it is needed */
  if (the_race->nb_boats > 0) {
    if (the_race->maxarrivaltime && (the_race->maxarrivaltime < now)) {
      // set all users in this race as HTP
      time_t result_timestamp = MAX_DURATION - now;
      sprintf(cmd, "REPLACE INTO races_results (idraces,idusers,position,duration,latitude,longitude) SELECT %d,US.idusers,%d,%ld,p.lat,p.long FROM users US, positions AS p, (SELECT MAX(`time`) AS t,`idusers` AS id FROM positions WHERE race=%d GROUP BY idusers) AS pos WHERE id=US.idusers AND p.idusers=US.idusers AND p.time=t AND p.race=%d AND US.engaged=%d", 
	      id, BOAT_STATUS_HTP, result_timestamp, id, id, id);
      mysql_query(db, cmd);
      if (mysql_affected_rows(db) == (my_ulonglong) -1 ) {
	// ERROR
      } 
    }
    sprintf(cmd, "UPDATE users SET engaged=0 WHERE engaged=%d", 
	    the_race->idraces); 
    mysql_query(db, cmd);
    if (mysql_affected_rows(db) == (my_ulonglong) -1 ) {
      // ERROR
    }
  }

  sprintf(cmd, "INSERT INTO histpos SELECT * FROM positions WHERE race=%d",
	  the_race->idraces); 
  mysql_query(db, cmd);
  if (mysql_affected_rows(db) == (my_ulonglong) -1 ) {
    // ERROR
  }  

  sprintf(cmd, "DELETE FROM positions WHERE race=%d", the_race->idraces); 
  mysql_query(db, cmd);
  if (mysql_affected_rows(db) == (my_ulonglong) -1 ) {
    // ERROR
  }  
}

void check_race(engine_race *erace) {
  race *the_race;

  the_race = erace->vlm_race;
  if (the_race->nb_boats == 0 && the_race->closetime < now) {
    /* mark the race as closed, cleanup will happen later */
    the_race->started = 0;
    printf("=> CLOSING RACE %d\n", the_race->idraces);
    return;
  }
  if (the_race->maxarrivaltime && (the_race->maxarrivaltime < now)) {
    printf("\n   This race is finished : time exceeded the Winners's PCT time !\n");
    // set HTP
    the_race->started = 0;
  }
}
  
