AM_CFLAGS = -Wall -std=gnu99 $(GPSD_CFLAGS) @DBUS_CFLAGS@
AM_LDFLAGS = -lm @DBUS_LIBS@

bin_PROGRAMS = timezoned gettz
timezoned_SOURCES = timezoned.c logging.c configuration.c configuration.h \
					iniparser/dictionary.c iniparser/iniparser.c \
					configuration.h  logging.h  tz_finder_location.h \
					tz_system_utils.c  \
					tz_system_utils.h iniparser/dictionary.h  iniparser/iniparser.h

timezoned_LDADD = $(SHP_LIBS)  $(GPSD_LIBS)

gettz_SOURCES = gettz.c tz_finder_location.c configuration.c \
				iniparser/dictionary.c iniparser/iniparser.c \
				configuration.h  logging.h  tz_finder_location.h \
				tz_system_utils.h iniparser/dictionary.h  iniparser/iniparser.h

gettz_LDADD = $(SHP_LIBS)

