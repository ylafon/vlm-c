/**
 * $Id: engine.c,v 1.25 2010-12-13 20:09:44 ylafon Exp $
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
#include <pthread.h>

#include <mysql.h>

#include <defs.h>
#include <types.h>
#include <gshhs.h>
#include <grib.h>
#include <polar.h>
#include <shmem.h>
#include <useshmem.h>
#include <context.h>
#include <winds.h>

#include "enginetypes.h"
#include "timings.h"
#include "races.h"
#include "boats.h"
#include "utils.h"

#define MAX_THREADS 64

/**
 * Some global variables...
 */
db_access *db_config;
engine_race *all_races;
vlmc_context *global_vlmc_context;
int nb_boats, nb_races, nb_threads;
time_t now;

th_context *worker_ctxs;
pthread_t *workers;
pthread_mutex_t nb_boats_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_db_config(db_access *config) {
  char default_host[] = "localhost";
  char default_username[] = "vlmdev";
  char default_passwd[] = "vlmdev";
  char default_dbname[]   = "vlmdev";
  
  config->host = malloc(sizeof(char)*(1+strlen(default_host)));
  strcpy(config->host, default_host);
  config->username = malloc(sizeof(char)*(1+strlen(default_username)));
  strcpy(config->username, default_username);
  config->passwd = malloc(sizeof(char)*(1+strlen(default_passwd)));
  strcpy(config->passwd, default_passwd);
  config->dbname = malloc(sizeof(char)*(1+strlen(default_dbname)));
  strcpy(config->dbname, default_dbname);
  config->port = 0;
}


void init_shmem_vlmc_structures() {
  int polar_shmid;

  polar_shmid = get_polar_shmid(1);
  if (polar_shmid < 0 ) { /* safeline */
    init_polar();
    create_and_fill_polar_shm();
  } else {
    shm_lock_sem_construct_polar(1);  
  }
}

void *th_init_coastline(void *arg) {
  double th_step0, th_step1;
  th_step0 = get_microtime();
  init_coastline();
  th_step1 = get_microtime();
  printf("Init coastline: %lf\n", th_step1 - th_step0);
  pthread_exit(NULL);
}

/**
 * We generate an interim grib in one shot, to avoid
 * tampering with a possible weather update
 */
void init_weather(vlmc_context *ctx, time_t when) {
  winds *fake_grib;
  winds_prev *prevs;

  shm_lock_sem_construct_grib_context(ctx, 1);
  fake_grib = generate_interim_grib(when);
  shm_unlock_sem_destroy_grib_context(ctx, 1);
  prevs = &ctx->windtable;
  prevs->time_offset = 0;
  prevs->nb_prevs = 1;
  prevs->wind = malloc(sizeof(winds *));
  prevs->wind[0] = fake_grib;
}

void *th_boat_check(void *arg) {
  int offset, ir, ib, max_boats, nb;
  engine_race *erace;
  engine_boat *race_boats, *rb;
  th_context *ctx;
  race *r;
  MYSQL *db;

  nb = 0;
  offset = (long)arg;
  printf("Thread offset: %d\n", offset);
  /* now create a DB connection */
  ctx = &worker_ctxs[offset];
  db = &ctx->db;
  mysql_init(db);
  if (mysql_real_connect(db, db_config->host, db_config->username,
			 db_config->passwd, db_config->dbname,
			 db_config->port, NULL, 0) == NULL) {
    printf("Can't connect\n");
    exit(2);
  }
  // set auto commit off, we will commit later at the end
  if(mysql_autocommit(db, 0)) {
    printf("*** AUTOCOMMIT FAILED FOR THREAD %d\n", offset);
  }

  erace = all_races;
  for (ir=0; ir < nb_races; erace++,ir++) {
    r = erace->vlm_race;
    if (!r->started) {
      continue;
    }
    max_boats = r->nb_boats;
    race_boats = erace->boats;
    /* do the work */
    for (ib=offset; ib<max_boats; ib+=nb_threads) {
      rb = &race_boats[ib];
      rb->context = ctx;
      thl_printf(rb, "\n(%d:%d) Checking boat %d: (race %d) ", offset, nb, 
		 rb->vlm_boat->num, r->idraces);
      check_boat(erace, rb);
      print_boat_status(rb);
      // FIXME
      nb++;
    }
  }
  // now commit all the changes
  if (mysql_commit(db)) {
    printf("*** CRITICAL ERROR COMMIT FAILED FOR THREAD %d\n", offset);
  }
  mysql_close(db);
  if (ctx->buffer) {
    free(ctx->buffer);
    ctx->buffer = NULL;
  }
  pthread_mutex_lock(&nb_boats_mutex); 
  nb_boats += nb;
  pthread_mutex_unlock(&nb_boats_mutex);
  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  pthread_t tid;
  pthread_attr_t th_attr;
  void *status;
  engine_race *race_ptr;
  race *r;
  char *progname, *c;
  int i;
  double step1, step2, step3, step4, stepbis2, step5;
  MYSQL db;
  wind_info winfo;
  struct tm *t;

  nb_threads = 2;
  
  /* fugly option parsing... */
  progname = *argv;
  c = strrchr(progname, '/');
  if (c) {
    progname = ++c;
  }
  argv++;
  argc--;
  while (argc > 0) {
    if (**argv == '-') {
      if ((*(*argv+1) == 'n') && !*(*argv+2) && (argc > 1)) {
	c = *argv;
	argv++;
	argc--;
	if (!sscanf(*argv, "%d", &nb_threads)) {
	  printf("%s: illegal option value -- %s %s\n", progname, c, *argv);
	  exit(3);
	}
	if (nb_threads > MAX_THREADS) {
	  nb_threads = MAX_THREADS;
	} else if (nb_threads < 1) {
	  nb_threads = 1;
	}
      } else {
	printf("%s: illegal option -- %s\n", progname, *argv);
	exit(3);
      }	
    } else {
      // FIXME race number ?
      printf("%s: illegal option -- %s\n", progname, *argv);
      exit(3);
    }
    argv++;
    argc--;
  }
  
  worker_ctxs = calloc(nb_threads, sizeof(th_context));
  workers = calloc(nb_threads, sizeof(pthread_t));

  // get time and clamp to a round minute
  time(&now);
  t = gmtime(&now);
  t->tm_sec = 0;
  now = mktime(t);

  step1 = get_microtime();

  db_config = calloc(1, sizeof(db_access));
  global_vlmc_context = calloc(1, sizeof(vlmc_context));
  init_db_config(db_config);
  init_context_default(global_vlmc_context);

  step2 = get_microtime();
  /* in parallel, create races, and initialize coast */
  pthread_attr_init(&th_attr);
  pthread_attr_setdetachstate(&th_attr, PTHREAD_CREATE_JOINABLE);
  pthread_create(&tid, &th_attr, th_init_coastline, (void *)NULL);
  pthread_attr_destroy(&th_attr);

  init_shmem_vlmc_structures();
  init_weather(global_vlmc_context, now);
  stepbis2 = get_microtime();
  printf("SHMEM Polar+weather interpolation time: %lf\n",
	 stepbis2-step2);
  mysql_init(&db);
  if (mysql_real_connect(&db, db_config->host, db_config->username,
			 db_config->passwd, db_config->dbname,
			 db_config->port, NULL, 0) == NULL) {
    printf("Can't connect\n");
    exit(2);
  }
  nb_races = get_all_races(&db);  
  mysql_close(&db);

  step3 = get_microtime();
  get_wind_info_latlong_context(global_vlmc_context, degToRad(43.7),
				degToRad(7), now, &winfo);
  printf("*** WIND: %.4f kts %.2f\n", winfo.speed, radToDeg(winfo.angle));
  pthread_join(tid, &status);
  step4 = get_microtime();

  /* Now create all the worker threads and distribute the load */
  pthread_attr_init(&th_attr);
  pthread_attr_setdetachstate(&th_attr, PTHREAD_CREATE_JOINABLE);
  for (i=0; i<nb_threads; i++) {
    pthread_create(&workers[i], &th_attr, th_boat_check, (void *)((long)i));
  }
  pthread_attr_destroy(&th_attr);
  /* and wait for the end of the work */
  for (i=0; i<nb_threads; i++) {
    pthread_join(workers[i], &status);
  }
  pthread_mutex_destroy(&nb_boats_mutex);
  step5 = get_microtime();
  /* Now write donw all race informations */
  // FIXME

  printf("Checking... got %d races %d boats\n", nb_races, nb_boats);
  race_ptr = all_races;
  nb_boats = 0;
  for (i=0; i< nb_races; i++) {
    r = race_ptr->vlm_race;
    printf("Race id: %10d (boats %6d) (WPs %2d) (pending pilot tasks %3d)\n", 
	   r->idraces, r->nb_boats, r->nb_waypoints, race_ptr->nb_pilot_tasks);
    nb_boats += r->nb_boats;
    race_ptr++;
  }
  printf("Init phase %lf\n", step2-step1);
  printf("Races init phase %lf (%lf db) %d r (%lf r/s) %d b (%lf b/s)\n", 
	 step3-step2, step3-stepbis2, nb_races, nb_races/(step3-stepbis2),
	 nb_boats, nb_boats/(step3-stepbis2));
  printf("Race processing phase %lf, (%lf r/s) (%lf b/s)\n", step5-step4, 
	 nb_races/(step5-step4), nb_boats/(step5-step4));
  printf("Race check time %lf (%lf r/s) (%lf b/s)\n", (step3-stepbis2)+
	 (step5-step4), 
	 nb_races/((step3-stepbis2)+(step5-step4)), 
	 nb_boats/((step3-stepbis2)+(step5-step4)));
  printf("GSHHS Init phase %lf\n", step4-step2);
  printf("Total time %lf\n", step5-step1);

  return 0;
}
