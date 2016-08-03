/*! @file configuration.h
 *  author: Zoltan Gyarmati <mr.zoltan.gyarmati@gmail.com>
 */
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

typedef struct
{
    const char* appname;
    const char* logtarget;
    const char* logfile;
    const char* logfacility;
    const char* loglevel;
    int check_interval;
    const char* gpsd_address;
    int gpsd_port;

} Configuration;


Configuration *init_config(const char*filepath);

#endif
