#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include <dbus/dbus.h>

#include "logging.h"
#define TRIM_STRING(s) for (size_t i=0, j=0; s[j]=s[i]; j+=!isspace(s[i++]));
#define BUFSIZE 256

#define DBUS_BUS_NAME               "org.freedesktop.timedate1"
#define DBUS_OBJ_PATH               "/org/freedesktop/timedate1"
#define DBUS_INTERFACE_NAME         "org.freedesktop.timedate1"
#define DBUS_TZ_METHOD_NAME         "SetTimezone"


//forward declarations for local functions
DBusMessage*
dbus_call_timezone_method(DBusConnection *conn, const char* timezone);



// sets the timezone via systemd's timedated
// dbus interface
int
system_set_tz_dbus(const char *tz)
{
    int ret;
    DBusError err;
    DBusConnection* conn;
    // initialise the errors
    dbus_error_init(&err);

    // connect to the bus
    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if (dbus_error_is_set(&err)) { 
        ERROR("DBus connection Error (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (NULL == conn) {
        return 1;
    }

    DBusMessage * reply = dbus_call_timezone_method(conn, tz);
    if(reply != NULL) {

        DBusMessageIter MsgIter;
        dbus_message_iter_init(reply, &MsgIter);//msg is pointer to dbus message received

        if (DBUS_TYPE_STRING == dbus_message_iter_get_arg_type(&MsgIter)){
            char* str = NULL;
            dbus_message_iter_get_basic(&MsgIter, &str);
            WARNING("Received string: \n %s \n",str);
        }

        dbus_message_unref(reply);//unref reply
    }
    return 0;
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



DBusMessage*
dbus_call_timezone_method(DBusConnection *conn, const char* timezone)
{
    assert(conn != NULL);
    DBusMessage* methodcall = dbus_message_new_method_call(DBUS_BUS_NAME,
                                                           DBUS_OBJ_PATH,
                                                           DBUS_INTERFACE_NAME, 
                                                           DBUS_TZ_METHOD_NAME);
    if (methodcall == NULL)    {
        FATAL("Cannot allocate DBus message!\n");
    }
    int val = 1;
    if (!dbus_message_append_args(methodcall,
                                DBUS_TYPE_STRING, &timezone,
                                DBUS_TYPE_BOOLEAN, &val,
                                DBUS_TYPE_INVALID)) {
        FATAL("Ran out of memory while constructing args\n");
        exit(EXIT_FAILURE);
    }
    //Now do a sync call
    DBusPendingCall* pending;
    DBusMessage* reply;

    if (!dbus_connection_send_with_reply(conn, methodcall, &pending, -1))//Send and expect reply using pending call object
    {
        printf("failed to send message!\n");
    }
    dbus_connection_flush(conn);
    dbus_message_unref(methodcall);
    methodcall = NULL;

    dbus_pending_call_block(pending);//Now block on the pending call
    reply = dbus_pending_call_steal_reply(pending);//Get the reply message from the queuepplications must not close shared connections - see dbus_connection_close() docs. This is a bug in the application.
    dbus_pending_call_unref(pending);//Free pending call handle
    DEBUG("DBus method returned\n");
//    assert(reply != NULL);

    if(dbus_message_get_type(reply) ==  DBUS_MESSAGE_TYPE_ERROR)    {
        printf("Error : %s\n",dbus_message_get_error_name(reply));

        DBusMessageIter MsgIter;
        dbus_message_iter_init(reply, &MsgIter);


        if (DBUS_TYPE_STRING == dbus_message_iter_get_arg_type(&MsgIter)){
            char* str = NULL;
            dbus_message_iter_get_basic(&MsgIter, &str);//this function is used to read basic dbus types like int, string etc. 
            printf("ERROR ARG: %s\n",str);
        }

        dbus_message_unref(reply);
        reply = NULL;
    }

    return reply;
}


