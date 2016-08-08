#include <stdio.h>
#include <stdlib.h>
#include "logging.h"
#include <time.h>


#define TRIM_STRING(s) for (size_t i=0, j=0; s[j]=s[i]; j+=!isspace(s[i++]));
#define BUFSIZE 256

// sets the timezone via systemd's timedated
// dbus interface, if available
int
system_set_tz_dbus(const char *tz)
{


}

// sets the system timezone by manually tinkering with the
// /etc/timezone > /usr/share/zoneinfo/ symlink
int
system_set_tz_symlink(const char *tz)
{



}


// calls the command line specified with the cmd parameter
// if the cmd has the placeholder '%s', it will be substitued
// with the timezone name string tz
int
system_execute_action(const char *cmd, const char* tz)
{
    char stdoutbuffer[BUFSIZE];
    char buf[BUFSIZE];
    FILE *fp;
    clock_t begin, end;
    double time_spent;
    sprintf(buf,cmd, tz);

    INFO("Calling script as: <%s>", buf);
    begin = clock();
    //opening it, getting the stdout
    if ((fp = popen(buf, "r")) == NULL) {
        printf("Error opening pipe!\n");
        return -1;
    }
    // save the output into log
    while (fgets(stdoutbuffer, BUFSIZE, fp) != NULL) {
        TRIM_STRING(stdoutbuffer);
        INFO("ACTION OUTPUT: %s", stdoutbuffer);
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    INFO("Script call ready, ran for %f secs", time_spent);
    //close it
    if(pclose(fp))  {
        ERROR("Command not found or exited with error status");
        return -1;
    }
    return 0;

}
