/*! @file configuration.c
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


#include "configuration.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include "iniparser/iniparser.h"

#define INISECTION "timezoned:"


/*
 * parses the ini file, and returns a pointer to a
 * dynamically allocated instance of the
 * configuration struct with the config values
 * The caller supposed to free up the returned
 * object
 * If the return value is NULL, the parsing was unsuccesfull,
 * and the error supposed to be printed on stderr
 */
Configuration *
init_config(const char*filepath){
    Configuration *retval = malloc(sizeof(Configuration));
    dictionary  *ini ;
    ini = iniparser_load(filepath);
    if (ini==NULL) {
        fprintf(stderr, "cannot parse file: %s\n", filepath);
        return NULL;
    }
    iniparser_dump(ini, stderr);
    retval->appname = INISECTION;
    //logging
    retval->logtarget = iniparser_getstring(ini,INISECTION"logtarget","stdout");
    retval->logfile = iniparser_getstring(ini,INISECTION"logfile","timezoned.log");
    retval->logfacility = iniparser_getstring(ini,INISECTION"logfacility","local0");
    retval->loglevel = iniparser_getstring(ini,INISECTION"loglevel","fatal");
    //application
    retval->check_interval = iniparser_getint(ini,INISECTION"check_interval",10);
    //gpsd
    retval->gpsd_address = iniparser_getstring(ini,INISECTION"gpsdaddress","127.0.0.1");
    retval->gpsd_port = iniparser_getint(ini,INISECTION"gpsdport",2947);
    // tz_world shapefiles
    retval->shp_path = iniparser_getstring(ini,INISECTION"tz_world_shp_file","/usr/share/timezoned/world/tz_world.shp");
    retval->dbf_path = iniparser_getstring(ini,INISECTION"tz_world_dbf_file","/usr/share/timezoned/world/tz_world.dbf");
    //command to execute when timezone changed
    retval->action_command = iniparser_getstring(ini,INISECTION"action_command","echo %s");
    return retval;
}

