#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>

#include "tz_finder_location.h"

#define DEFAULT_CONFIGPATH "/etc/timezoned.ini"

#ifndef PACKAGE_STRING
#define PACKAGE_STRING "gettz"
#endif
#ifndef PACKAGE_BUGREPORT
#define PACKAGE_BUGREPORT "mr.zoltan.gyarmati@gmail.com"
#endif

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
    fprintf(stderr, "\t-h, --help Print this help\n");
    fprintf(stderr, "\t-v, --version Print version info\n");
    fprintf(stderr, "\t-c, --configfile </path/to/configfile.ini>, using custom config file (default is " DEFAULT_CONFIGPATH ")\n");
    fprintf(stderr, "\t-s, --set set the system timezone to the requested value\n");
}


int
main (int argc, char * argv[])
{
    char *configpath = DEFAULT_CONFIGPATH;
    int  set_system_tz = 0;
    char ch;
    static struct option long_options[] = {
            {"configfile",  required_argument, 0,  'c' },
            {"help",    no_argument,       0,  'h' },
            {"version", no_argument,       0,  'v' },
            {"set",     no_argument,       0,  's' },
            {0,         0,                 0,  0 }};

    while ((ch = getopt_long(argc, argv, "c:hvs", long_options, NULL)) != -1){
        switch (ch){
            case 'c':
                configpath = strdup(optarg); // or copy it if you want to
                break;
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
                break;
            case 'v':
                print_version();
                exit(EXIT_SUCCESS);
                break;
            case 's':
                set_system_tz = 1;
                break;
        }
    }

    printf ("CONFIGPATH: %s, %d\n", configpath, optind);

    if ((argc-optind) != 2){
        fprintf(stderr, "No coordinate parameters passed\n");
        print_help();
        exit(EXIT_FAILURE);
    }
    for (int i = optind; i < argc; i++)
            printf("%d argument is %s\n", i, argv[i]);
    double longitude = atof(argv[optind++]);
    double latitude = atof(argv[optind++]);
    printf("Coordinates: longitude %f, latitude %f\n", longitude, latitude);
    printf("Timezone: %s\n", tz_get_name_by_coordinates(longitude, latitude));

}

