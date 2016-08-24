/*! @file gettz.c
 *  author: Zoltan Gyarmati <mr.zoltan.gyarmati@gmail.com>
 *
 *
 *                   The MIT License (MIT)
 *
 * Copyright (c) 2016 Zoltan Gyarmati <mr.zoltan.gyarmati@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of 
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */



#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <stdarg.h>

#include "tz_finder_location.h"
#include "tz_system_utils.h"

#include "config.h"

#ifndef PACKAGE_STRING
#define PACKAGE_STRING "gettz"
#endif
#ifndef PACKAGE_BUGREPORT
#define PACKAGE_BUGREPORT "mr.zoltan.gyarmati@gmail.com"
#endif

// can be increased to add verbosity
int verbose = 0;
// 0 or 1, when 1, we print only the timezone
int quiet = 0;
//can be overriden from the command line
static char *conf_file_name = "timezoned.ini";

#ifndef DEFAULT_SHP_FILE
#define DEFAULT_SHP_FILE "/usr/share/timezoned/tz_world/tz_world.shp"
#endif

#ifndef DEFAULT_DBF_FILE
#define DEFAULT_DBF_FILE "/usr/share/timezoned/tz_world/tz_world.dbf"
#endif

void
print_version()
{
    fputs(PACKAGE_STRING "/gettz\n", stderr);
    fputs("Version: " PACKAGE_VERSION "\n", stderr);
    fputs("Please report bugs to: " PACKAGE_BUGREPORT"\n", stderr);
}


void
print_help()
{
    fputs("Usage:\n\n",stderr);
    fputs("gettz <options> longitude latitude\n\n",stderr);
    fputs("where longitude and latitude are the geographical coordinates in decimal format\n",stderr);
    fputs("Options:\n",stderr);
    fputs("\t-c  --configfile filename  Read configuration from the file\n",stderr);
    fputs("\t-h, --help           Print this help\n",stderr);
    fputs("\t-V, --version        Print version info\n",stderr);
    fputs("\t-s, --set-timezone   Set the system timezone to the requested value\n",stderr);
    fputs("\t-v, --verbose        Verbose output, multiple for more verbose\n",stderr);
    fputs("\t-q, --quiet          Print only the timezone (or nothing), overrides -v\n",stderr);
}


void
verbosemsg(int level, const char *formatstring,...)
{
    if (level > verbose || quiet){
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
    fputc('\n', stderr);

}

int
main (int argc, char * argv[])
{
    //parse command line args
    int  set_system_tz = 0;
    char ch;
    static struct option long_options[] = {
        {"configfile", required_argument, 0, 'c'},
        {"help",    no_argument,       0,  'h' },
        {"version", no_argument,       0,  'V' },
        {"set-timezone",     no_argument,       0,  's' },
        {"verbose", no_argument,       0,  'v' },
        {"quiet", no_argument,         0,  'q' },
        {0,         0,                 0,  0 }};

    while ((ch = getopt_long(argc, argv, "c:hvsVq", long_options, NULL)) != -1){
        switch (ch){
            case 'c':
                conf_file_name = strdup(optarg);
                break;
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
            case 'q':
                quiet = 1;
                break;
        }
    }


    if ((argc-optind) != 2){
        errormsg("Need 2 coordinate parameters!\n");
        print_help();
        exit(EXIT_FAILURE);
    }
    char *endptr;
    double longitude = strtof(argv[optind],&endptr);
    if (argv[optind++] == endptr) {
        errormsg("can't convert longitude value <%s>, exiting...\n",argv[--optind]);
        exit(EXIT_FAILURE);
    }
    double latitude = strtof(argv[optind], &endptr);
    if (argv[optind++] == endptr) {
        errormsg("can't convert latitude value <%s>, exiting...\n",argv[--optind]);
        exit(EXIT_FAILURE);
    }
    //get shp and dbf file paths
    char *shp_path = getenv("GETTZ_SHP_FILE");
    char *dbf_path = getenv("GETTZ_DBF_FILE");

    if (shp_path == NULL){
        shp_path = DEFAULT_SHP_FILE;
    }
    if (dbf_path == NULL){
        dbf_path = DEFAULT_DBF_FILE;
    }

    //get to the business
    verbosemsg(1,"Coordinates: longitude %f, latitude %f\n", longitude, latitude);
    verbosemsg(1,"Map files: %s, %s\n",shp_path, dbf_path);
    char * tz_name = tz_get_name_by_coordinates(longitude, latitude,
                                                shp_path, dbf_path);
    if (tz_name == NULL){
        errormsg("Failed to look up timezone for the given coordinates");
        return EXIT_FAILURE;
    }

    if (!quiet){
        printf("Timezone: %s\n", tz_name);
    }
    else {
        printf("%s\n", tz_name);
    }
    if(set_system_tz){
#ifdef USE_SYSTEMD
        system_set_tz_dbus(tz_name);
#else
        system_set_tz_symlink(tz_name);
#endif
    }
    return EXIT_SUCCESS;
}
