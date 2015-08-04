#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <time.h>

enum WORKLOAD_ID {
   int_load_id,
   float_load_id,
   str_load_id
};

struct WORKLOAD {
   enum WORKLOAD_ID id;
   char *name;
   void (*load)();
};

void int_load() {
   int a;
   for ( a = 0; a < 1000000; a++ );
}

void float_load() {
   double a = 7.0;
   double b = 123.456;
   double result = a / b;
}

void str_load() {
   char a[80] = "abcdefghijklmnopqrstuvwxyz";
   char b[80] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   strcat(a, b);
}

struct WORKLOAD workload_list[] = {
   { int_load_id, "int", int_load },
   { float_load_id, "float", float_load },
   { str_load_id, "str", str_load }
};

int workload_list_len = sizeof(workload_list) / sizeof(struct WORKLOAD); 
 
struct WORKLOAD *find_workload_by_name(char *str) {
   int i;
   for ( i = 0; i < workload_list_len; i++ ) {
      if ( strcmp(workload_list[i].name, str) == 0 )
         return workload_list + i;
   }
 
   /* Didn't find it, so return NULL */
   return NULL;
}

struct CONF {
   struct WORKLOAD *workload; /* Type of workload */
   double lambda; /* Interarrival rate for each transaction per second */
   int count; /* Number of transactions to exectue */
   long int seed; /* Seed for random number generator */
   int debug; /* Debug mode */
};

void print_usage(char *basename) {
   fprintf(stderr, "%s: [-w <workload type>] [-l <arrival rate>]\n", basename);
   fprintf(stderr, "   [-c <count>] [-s <seed int>] [-d] [-h]\n");
   fprintf(stderr, "   workload types: int (default), float, str\n");
   fprintf(stderr, "   arrival rate: in transactions per second (default 1)\n");
   fprintf(stderr, "   count: number of transactions (default 20)\n");
   fprintf(stderr, "   seed int: int used for seed (default current time)\n");
   fprintf(stderr, "   -d: debug mode, shows values for each transaction\n");
   fprintf(stderr, "   -h: print usage\n");
}

void parse_args(int argc, char **argv, struct CONF *confp) {
   int opt;
   /* Default to first in the list */
   confp->workload = workload_list;
   confp->lambda = 1; /* Default to 1 arrival per second */
   confp->count = 20;
   confp->seed = time(NULL);
   confp->debug = 0;

   while ( (opt = getopt(argc, argv, "hdw:l:c:s:")) != -1 ) {
      switch (opt) {
      case 'h':
         print_usage(argv[0]);
         exit(0);
         break;
      case 'w':
         confp->workload = find_workload_by_name(optarg);
         if ( confp->workload == NULL ) {
            fprintf(stderr, "Error: Unknown workload '%s'\n", optarg);
            exit(1);
         }
         break;
      case 'l':
         confp->lambda = atof(optarg);
         break;
      case 'c':
         confp->count = atoi(optarg);
         break;
      case 's':
         confp->seed = atoi(optarg);
         break;
      case 'd':
         confp->debug = 1;
         break;
      default:
         print_usage(argv[0]);
         exit(1);
      }
   }

   srand48(confp->seed);
}

struct timespec compute_interarrival_time(double lambda, int debug) {
   struct timespec ret;
   double x = - 1 / lambda * log(1.0 - drand48());
   if ( debug ) 
      fprintf(stdout, "x = %12.9f, ", x);
   ret.tv_sec = (time_t) x;
   ret.tv_nsec = (x - (double)ret.tv_sec) * 1000000000;
   if ( debug ) {
      fprintf(stdout, "tv_sec = %06d, tv_nsec = %09d ... ", 
              ret.tv_sec, ret.tv_nsec);
      fflush(stdout);
   }
   return ret;
}

void add_timespec(struct timespec *dest, struct timespec *src) {
   dest->tv_sec += src->tv_sec;
   dest->tv_nsec += src->tv_nsec;
   if ( dest->tv_nsec > 1000000000 ) {
      dest->tv_sec += 1;
      dest->tv_nsec -= 1000000000;
   }  
}

void sub_timespec(struct timespec *dest, struct timespec *src) {
   dest->tv_sec -= src->tv_sec;
   dest->tv_nsec -= src->tv_nsec;
   if ( dest->tv_nsec < 0 ) {
      dest->tv_sec -= 1;
      dest->tv_nsec += 1000000000;
   }
}

void run_load(struct CONF *confp) {
   int i;
   struct timespec arrival_time;
   struct timespec total_service_time;
   struct timespec t_start, t_end;
   struct timespec run_start, run_end;

   total_service_time.tv_sec = 0;
   total_service_time.tv_nsec = 0;

   clock_gettime(CLOCK_MONOTONIC_RAW, &run_start);
   for ( i = 0; i < confp->count; i++ ) {
      /* Which iteration */
      if ( confp->debug )
         fprintf(stdout, "%08d ", i);

      /* Determine next arrival */
      if ( confp->lambda > 0 ) {
         arrival_time = compute_interarrival_time(confp->lambda, confp->debug);

         /* Wait */
         nanosleep(&arrival_time, NULL);
      }

      /* Do the work */
      clock_gettime(CLOCK_MONOTONIC_RAW, &t_start);
      confp->workload->load();
      clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
      sub_timespec(&t_end, &t_start);
      if ( confp->debug )
         fprintf(stdout, " %d.%09d\n", t_end.tv_sec, t_end.tv_nsec);
      add_timespec(&total_service_time, &t_end);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &run_end);
   sub_timespec(&run_end, &run_start);

   fprintf(stdout, "===== RESULTS =====\n");
   fprintf(stdout, "Total service time: %d.%09d(s)\n", 
           total_service_time.tv_sec, total_service_time.tv_nsec);
   fprintf(stdout, "Transactions: %06d\n", confp->count);
   double avg_service_time = ((double) total_service_time.tv_sec +
                              (double) total_service_time.tv_nsec / 
                              1000000000.0 ) / (double) confp->count;
   fprintf(stdout, "Avg ST(s) = %12.9f\n", avg_service_time);
   fprintf(stdout, "Total response time: %d.%09d(s)\n",
           run_end.tv_sec, run_end.tv_nsec);
   double transaction_rate = (double) confp->count / 
                             ((double) run_end.tv_sec +
                              (double) run_end.tv_nsec /
                              1000000000.0 );
   fprintf(stdout, "Requested transaction rate (per sec): %12.9f\n",
           confp->lambda);
   fprintf(stdout, "Transactions Rate (per sec): %12.9f\n", transaction_rate);
}

int main(int argc, char **argv) {
   struct CONF conf;
   parse_args(argc, argv, &conf);

   fprintf(stdout, "workload = %d, lambda = %f\n", conf.workload->id, 
           conf.lambda);

   run_load(&conf);
}
