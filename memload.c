#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>

struct CONF {
   int start; /* Interarrival rate for each transaction per second */
   int increment; /* Number of transactions to exectue */
   int interval; /* Number of transactions to exectue */
   int debug; /* Debug mode */
};

void print_usage(char *basename) {
   fprintf(stderr, "%s: [-s <start size >] [-i <increment size >]\n", basename);
   fprintf(stderr, "   [-n <sleep interval>] [-d] [-h]\n");
   fprintf(stderr, "   start size: in megabytes (default 100)\n");
   fprintf(stderr, "   increment size: in megabytes (default 0)\n");
   fprintf(stderr, "   sleep interval: in seconds (default 60)\n");
   fprintf(stderr, "   -d: debug mode, shows values for each step\n");
   fprintf(stderr, "   -h: print usage\n");
}

void parse_args(int argc, char **argv, struct CONF *confp) {
   int opt;
   confp->start = 100; /* Megabytes */
   confp->increment = 0; /* Megabytes */
   confp->interval = 60; /* Seconds */
   confp->debug = 0;

   while ( (opt = getopt(argc, argv, "hds:i:n:")) != -1 ) {
      switch (opt) {
      case 'h':
         print_usage(argv[0]);
         exit(0);
         break;
      case 's':
         confp->start = atof(optarg);
         break;
      case 'i':
         confp->increment = atoi(optarg);
         break;
      case 'n':
         confp->interval = atoi(optarg);
         break;
      case 'd':
         confp->debug = 1;
         break;
      default:
         print_usage(argv[0]);
         exit(1);
      }
   }
}

void run_load(struct CONF *confp) {
   void *mem = (char *) malloc(confp->start * 1024 * 1024);

   memset(mem, 42, confp->start * 1024 * 1024);
   if ( confp->debug ) {
      fprintf(stdout, "Starting malloc = %d MB\n", confp->start);
      fprintf(stdout, "Sleeping %d seconds\n", confp->interval);
   }

   sleep(confp->interval);

   for ( ; ; ) {
      if ( confp->debug ) {
         fprintf(stdout, "Allocating %d more MB\n", confp->increment);
      }
      mem = malloc(confp->increment * 1024 * 1024);
      memset(mem, 42, confp->increment * 1024 * 1024);

      if ( confp->debug ) {
         fprintf(stdout, "Sleeping %d seconds\n", confp->interval);
      }

      sleep(confp->interval);

   }
}

int main(int argc, char **argv) {
   struct CONF conf;
   parse_args(argc, argv, &conf);

   run_load(&conf);
}
