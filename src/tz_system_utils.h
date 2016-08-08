// sets the timezone via systemd's timedated
// dbus interface, if available
int system_set_tz_dbus(const char *tz);

// sets the system timezone by manually tinkering with the
// /etc/timezone > /usr/share/zoneinfo/ symlink
int system_set_tz_symlink(const char *tz);


// calls the command line specified with the cmd parameter
// if the cmd has the placeholder '%s', it will be substitued
// with the timezone name string tz
int system_execute_action(const char *cmd, const char* tz);
