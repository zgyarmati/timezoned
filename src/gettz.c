#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <stdarg.h>

#include "tz_finder_location.h"

#ifndef PACKAGE_STRING
#define PACKAGE_STRING "gettz"
#endif
#ifndef PACKAGE_BUGREPORT
#define PACKAGE_BUGREPORT "mr.zoltan.gyarmati@gmail.com"
#endif

int verbose = 0;

void
print_version()
{
    fprintf(stderr, PACKAGE_STRING"\n");
    fprintf(stderr, "Please report bugs to: " PACKAGE_BUGREPORT"\n");
}


void
print_help()
{
    fprintf(stderr, "Usage:\n\n");
    fprintf(stderr, "gettz <options> longitude latitude\n\n");
    fprintf(stderr, "where longitude and latitude are the geographical coordinates in decimal format\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-h, --help           Print this help\n");
    fprintf(stderr, "\t-V, --version        Print version info\n");
    fprintf(stderr, "\t-s, --set-timezone   Set the system timezone to the requested value\n");
    fprintf(stderr, "\t-v, --verbose        Verbose output, multiple for more verbose\n");
}


void
verbosemsg(int level, const char *formatstring,...)
{
    if (level > verbose){
        return;
    }
    va_list args;
    va_start (args, formatstring);
    vprintf(formatstring, args);
    va_end(args);
}

void
errormsg(const char *formatstring,...)
{
    va_list args;
    va_start (args, formatstring);
    vfprintf(stderr, formatstring, args);
    va_end(args);
}

int
main (int argc, char * argv[])
{
    int  set_system_tz = 0;
    char ch;
    static struct option long_options[] = {
            {"help",    no_argument,       0,  'h' },
            {"version", no_argument,       0,  'V' },
            {"set-timezone",     no_argument,       0,  's' },
            {"verbose", no_argument,       0,  'v' },
            {0,         0,                 0,  0 }};

    while ((ch = getopt_long(argc, argv, "hvsV", long_options, NULL)) != -1){
        switch (ch){
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
                break;
            case 'V':
                print_version();
                exit(EXIT_SUCCESS);
                break;
            case 's':
                set_system_tz = 1;
                break;
            case 'v':
                verbose++;
                break;
        }
    }


    if ((argc-optind) != 2){
        fprintf(stderr, "Need 2 coordinate parameters!\n");
        print_help();
        exit(EXIT_FAILURE);
    }
    char *endptr;
    double longitude = strtof(argv[optind],&endptr);
    if (argv[optind++] == endptr) {
        fprintf(stderr,"can't convert longitude value <%s>, exiting...\n",argv[--optind]);
        exit(EXIT_FAILURE);
    }
    double latitude = strtof(argv[optind], &endptr);
    if (argv[optind++] == endptr) {
        fprintf(stderr,"can't convert latitude value <%s>, exiting...\n",argv[--optind]);
        exit(EXIT_FAILURE);
    }
    verbosemsg(1,"Coordinates: longitude %f, latitude %f\n", longitude, latitude);
    printf("Timezone: %s\n", tz_get_name_by_coordinates(longitude, latitude));
}
