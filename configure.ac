AC_PREREQ([2.69])
AC_INIT(timezoned, 0.1, mr.zoltan.gyarmati@gmail.com)
AC_CONFIG_SRCDIR([src/timezoned.c])
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
PKG_CHECK_MODULES(GPSD, [libgps >= 2.91],  have_gpsd="yes", have_gpsd="no")
if test "x$have_gpsd" = "xyes"; then
    AC_DEFINE(HAVE_LIBGPS, 1, gpsd libgps client support)
else
    AC_MSG_ERROR([No libgps found!])
fi


# look for shapelib (no pkg-config... )
AC_CHECK_LIB([shp], [SHPOpen],havelibshp=yes,havelibshp=no)
AC_CHECK_HEADER([shapefil.h],havelibshp=yes,havelibshp=no)

if test "$havelibshp" = "yes"; then
    AC_DEFINE(HAVE_LIBSHP, 1, shapefile support)
    AC_SUBST(SHP_LIBS,-lshp)
else
    AC_MSG_ERROR([No libshp found!])
fi


# check whether the user wants to build with
AC_ARG_ENABLE([systemd], " --enable-systemd: Enable to let timezoned to use systemd to set the timezone and start at boot",
              use_systemd=yes, use_systemd=no)

if test "x$use_systemd" = "xyes"; then
    AC_DEFINE(USE_SYSTEMD, 1, Use systemd for autostart and system timezone settings)
    PKG_CHECK_MODULES(DBUS, [dbus-1 >= 1.3.1])
    AC_SUBST(DBUS_CFLAGS)
    AC_SUBST(DBUS_LIBS)
    AC_MSG_NOTICE(["Systemd support enabled, also enables dbus linking"])
else
    AC_MSG_WARN([Systemd support not enabled, building without!])
fi


#check for doc generating tools
AC_CHECK_PROGS([A2X], [a2x])
if test -z "$A2X";then 
    AC_MSG_WARN([asciidoc a2x not found - continuing without building man pages])
fi
AM_CONDITIONAL([HAVE_A2X], [test -n "$A2X"])

AC_CONFIG_FILES([Makefile
                 man/Makefile
                 data/Makefile
                 src/Makefile])

AC_OUTPUT
