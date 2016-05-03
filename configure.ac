AC_PREREQ([2.69])
AC_INIT(timezoned, 0.1, mr.zoltan.gyarmati@gmail.com)
AC_CONFIG_SRCDIR([src/log.h])
AM_INIT_AUTOMAKE([subdir-objects])
AC_PROG_RANLIB
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday memset strchr strdup strerror strtol])


# Look for libgps
havelgps="yes"
AC_CHECK_LIB([gps], [gps_open],,havelgps=no)
AC_CHECK_HEADER([gps.h],,havelgps=no)

if test "$havelgps" = "yes"; then
    AC_DEFINE(HAVE_LIBGPS, 1, gpsd libgps client support)
    KSLIBS="$KSLIBS -lgps"
fi

#check for doc generating tools
AC_CHECK_PROGS([A2X], [a2x])
if test -z "$A2X";
   then AC_MSG_WARN([asciidoc a2x not found - continuing without building man pages])
fi
AM_CONDITIONAL([HAVE_A2X], [test -n "$A2X"])

AC_CONFIG_FILES([Makefile
                 man/Makefile
                 src/Makefile])

AC_OUTPUT
