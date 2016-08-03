/*! @file configuration.c
 *  author: Zoltan Gyarmati <mr.zoltan.gyarmati@gmail.com>
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
    return retval;
}

