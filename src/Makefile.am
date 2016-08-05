AM_CFLAGS = -Wall -std=gnu99
AM_LDFLAGS = -lm

bin_PROGRAMS = timezoned gettz
timezoned_SOURCES = timezoned.c logging.c configuration.c configuration.h \
					iniparser/dictionary.c iniparser/iniparser.c

gettz_SOURCES = gettz.c tz_finder_location.c configuration.c \
				iniparser/dictionary.c iniparser/iniparser.c
gettz_LDADD = -lshp

