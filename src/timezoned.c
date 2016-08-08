/*
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

#include <gps.h>

#include "configuration.h"
#include "logging.h"
#include "tz_system_utils.h"

static int running = 0;
static int delay = 1;
static char *conf_file_name = "timezoned.ini";
static char *pid_file = "/var/lock/timezoned";
static int pid_fd = -1;
static char *app_name = NULL;

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

/* Main function */
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
    Configuration *config = init_config(conf_file_name);
    if (config == NULL){
        fprintf(stderr, "CONFIG ERROR, EXITING!\n");
        exit(EXIT_FAILURE);
    }
    ret = log_init(config->loglevel, config->logtarget, config->logfile,
              config->logfacility, 0);
    INFO("Timezoned started, pid: %d", getpid());

    /* This global variable can be changed in function handling signal */
    running = 1;
    system_execute_action("echo %s > /tmp/timezone", "Europe/Budapest");

    /* Never ending loop of server */
    while(running == 1) {
        struct gps_data_t gps_data;
        int rc;
        int i = 0;
        INFO("connecting GPSd");

        if ((rc = gps_open("localhost", "2947", &gps_data)) == -1) {
            ERROR("code: %d, reason: %s\n", rc, gps_errstr(rc));
            sleep(1); //we gotta try each second...
            continue;
        }
        gps_stream(&gps_data, WATCH_ENABLE | WATCH_JSON, NULL);
        while (i++ < 12){
            if (gps_waiting (&gps_data, 2000000)) {
                /* read data */
                if ((rc = gps_read(&gps_data)) == -1) {
                    WARNING("error occured reading gps data. code: %d, reason: %s\n", rc, gps_errstr(rc));
                    continue;
                }
                /* Display data from the GPS receiver. */
                if ((gps_data.status == STATUS_FIX) &&
                    (gps_data.fix.mode == MODE_2D || gps_data.fix.mode == MODE_3D) &&
                    !isnan(gps_data.fix.latitude) &&
                    !isnan(gps_data.fix.longitude)) {
                        INFO("latitude: %f, longitude: %f", gps_data.fix.latitude, gps_data.fix.longitude);
                        break;
                }
                else {
                    DEBUG("GPS read result invalid, retrying...");
                } //we continue to the next trial to read
            }
        }
        sleep(config->check_interval);
    }
    INFO("bye");

    return EXIT_SUCCESS;
}



#if 0
int main(int argc, char* argv[])
{
 
    int i, nEntities, quality;
    SHPHandle   hSHP;
    DBFHandle   hDBF;
    hSHP = SHPOpen("../sandbox/world/tz_world.shp", "rb");
    SHPGetInfo(hSHP, &nEntities, NULL, NULL, NULL);
    printf("Number of entities: %d\n", nEntities);
    hDBF = DBFOpen( "../sandbox/world/tz_world.dbf", "rb" );
    if( hDBF == NULL ){
        printf( "DBFOpen(%s,\"r\") failed.\n","../world/tz_world.dbf" );
        exit( 2 );
    }
 
    for( i = 0; i < nEntities; i++ )
    {
        SHPObject *psShape;
        psShape = SHPReadObject( hSHP, i );
        if(psShape->nSHPType == SHPT_POLYGON)
        {
            if(pnpoly(psShape->nVertices, psShape->padfX, psShape->padfY,
                        atof(argv[1]),atof(argv[2]))){
                printf ("FOUND!!! TZID for shape: %d is: %s\n", i,
                        DBFReadStringAttribute(hDBF, i, 0));
            }
        }
        SHPDestroyObject( psShape );
    }
}

#endif

