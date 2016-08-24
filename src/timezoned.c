/*! @file timezoned.c
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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <math.h>
#include <assert.h>


#include <gps.h>

#include "configuration.h"
#include "logging.h"
#include "tz_system_utils.h"
#include "tz_finder_location.h"

#include <config.h>

static int running = 0;
static char *conf_file_name = PACKAGE_NAME".ini";
static char *pid_file = "/var/lock/"PACKAGE_NAME;
static int pid_fd = -1;
static char *app_name = PACKAGE_NAME;


Configuration *config = NULL;

/**
 * \brief   Callback function for handling signals.
 * \param	sig identifier of signal
 */
void handle_signal(int sig)
{
    if(sig == SIGINT) {
     //   fprintf(log_stream, "Debug: stopping daemon ...\n");
        /* Unlock and close lockfile */
        if(pid_fd != -1) {
            lockf(pid_fd, F_ULOCK, 0);
            close(pid_fd);
        }
        /* Try to delete lockfile */
        if(pid_file != NULL) {
            unlink(pid_file);
        }
        running = 0;
        /* Reset signal handling to default behavior */
        signal(SIGINT, SIG_DFL);
    }
}


/**
 * \brief This function will daemonize this app
 */
static void daemonize()
{
    pid_t pid = 0;
    int fd;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if(pid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Success: Let the parent terminate */
    if(pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* On success: The child process becomes session leader */
    if(setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    /* Ignore signal sent from child to parent process */
    signal(SIGCHLD, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if(pid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Success: Let the parent terminate */
    if(pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");

    /* Close all open file descriptors */
    for(fd = sysconf(_SC_OPEN_MAX); fd > 0; fd--)
    {
        close(fd);
    }

    /* Reopen stdin (fd = 0), stdout (fd = 1), stderr (fd = 2) */
    stdin = fopen("/dev/null", "r");
    stdout = fopen("/dev/null", "w+");
    stderr = fopen("/dev/null", "w+");

    /* Try to write PID of daemon to lockfile */
    if(pid_file!= NULL)
    {
        char str[256];
        pid_fd = open(pid_file, O_RDWR|O_CREAT, 0640);
        if(pid_fd < 0)
        {
            /* Can't open lockfile */
            exit(EXIT_FAILURE);
        }
        if(lockf(pid_fd, F_TLOCK, 0) < 0)
        {
            /* Can't lock file */
            exit(EXIT_FAILURE);
        }
        /* Get current PID */
        sprintf(str, "%d\n", getpid());
        /* Write PID to lockfile */
        write(pid_fd, str, strlen(str));
    }
}

/**
 * \brief Print help for this application
 */
void print_help(void)
{
    printf("\nUsage: %s [OPTIONS]\n\n", app_name);
    printf("Options:\n");
    printf("   -h --help                 Print this help\n");
    printf("   -c --configfile filename  Read configuration from the file\n");
    printf("   -d --daemon               Daemonize this application\n");
    printf("\n");
}


void
process_coordinates(const float lat, const float lon,
                    const char *shp_path, const char *dbf_path)
{
    assert(config);
    char *tz_name = tz_get_name_by_coordinates(lon, lat,shp_path, dbf_path);
    if (!system_timezone_is_valid(tz_name)){
        ERROR("Timezone %s is invalid on this system, not setting it",tz_name);
        return;
    }
    INFO("The timezone got from coordinates: %s\n",tz_name);

#ifdef USE_SYSTEMD
    system_set_tz_dbus(tz_name);
#else
    system_set_tz_symlink(tz_name);
#endif

    system_execute_action(config->action_command, tz_name);
}


// Main function
int main(int argc, char *argv[])
{
    static struct option long_options[] = {
        {"configfile", required_argument, 0, 'c'},
        {"help", no_argument, 0, 'h'},
        {"daemon", no_argument, 0, 'd'},
        {NULL, 0, 0, 0}
    };
    int value, option_index = 0, ret;
    int start_daemonized = 0;

    app_name = argv[0];

    /* Try to process all command line arguments */
    while( (value = getopt_long(argc, argv, "c:dh", long_options, &option_index)) != -1) {
        switch(value) {
            case 'c':
                conf_file_name = strdup(optarg);
                break;
            case 'd':
                start_daemonized = 1;
                break;
            case 'h':
                print_help();
                return EXIT_SUCCESS;
            case '?':
                print_help();
                return EXIT_FAILURE;
            default:
                break;
        }
    }

    /* When daemonizing is requested at command line. */
    if(start_daemonized == 1) {
        daemonize();
    }
    signal(SIGINT, handle_signal);
    config = init_config(conf_file_name);
    if (config == NULL){
        fprintf(stderr, "CONFIG ERROR, EXITING!\n");
        exit(EXIT_FAILURE);
    }
    ret = log_init(config->loglevel, config->logtarget, config->logfile,
              config->logfacility, 0);
    INFO("Timezoned started, pid: %d", getpid());

    // changed from signal handler
    running = 1;

    // Never ending loop to check GPSd and process results if any
    while(running == 1) {
        struct gps_data_t gps_data;
        int rc;
        int i = 0;
        INFO("connecting to GPSd");

        if ((rc = gps_open("localhost", "2947", &gps_data)) == -1) {
            ERROR("Failed to connect GPSd, code: %d, reason: %s\n", rc, gps_errstr(rc));
            sleep(1); //we gotta try each second...
            continue;
        }
        gps_stream(&gps_data, WATCH_ENABLE | WATCH_JSON, NULL);
        while (i++ < 12){
            if (gps_waiting (&gps_data, 2000000)) {
                /* read data */
                if ((rc = gps_read(&gps_data)) == -1) {
                    WARNING("Error occured reading gps data. code: %d, reason: %s\n", rc, gps_errstr(rc));
                    continue;
                }
                /* Display data from the GPS receiver. */
                if ((gps_data.status == STATUS_FIX) &&
                    (gps_data.fix.mode == MODE_2D || gps_data.fix.mode == MODE_3D) &&
                    !isnan(gps_data.fix.latitude) &&
                    !isnan(gps_data.fix.longitude)) {
                        INFO("Got GPSd coordinates, latitude: %f, longitude: %f",
                             gps_data.fix.latitude, gps_data.fix.longitude);
                        process_coordinates(gps_data.fix.latitude, gps_data.fix.longitude,
                                            config->shp_path, config->dbf_path);
                        break;
                }
                else {
                    DEBUG("GPS read result invalid, retrying...");
                } //we continue to the next trial to read
            }
        }
        gps_close(&gps_data);
        sleep(config->check_interval);
    }
    INFO("bye");

    return EXIT_SUCCESS;
}
