# Timezoned

The timezoned package contains a daemon (timezoned) which connects to [GPSd](http://catb.org/gpsd/), regularly requests the current position of the system, and based on the GPS coordinates looks up the timezone assigned to the current location (using the [TZ shapefiles](http://efele.net/maps/tz/world/) created by Eric Muller) and sets the system's timezone accordingly. The goal is to always set the current timezone on ever-moving embededd systems (like digital signage systems on vehicles), so they can always display the correct local time. The package also contains a command line utility which accepts coordinates as command line arguments, looks up and prints the corresponding timezone name, and optionally sets the system's timezone to it.

## Building
Timezoned has 2 mandatory and 1 optional dependencies: [shapelib](http://shapelib.maptools.org/), [libgps](http://catb.org/gpsd/), and optionally (if the system uses systemd and systemd-timedated) [libdbus](https://www.freedesktop.org/wiki/Software/dbus/), and obviosly  running systemd-timedated as runtime dependency.

To check out the code and compile on a recent Debian based system:

```bash
# get dependencies:
apt-get install libdbus-1-dev libshp-dev libgps-dev
# get the source
git clone https://github.com/zgyarmati/timezoned.git
cd timezoned
# create configure and co. not needed when building from release tarball
# but needed when checked out from git
./autogen.sh
# to disable dbus (and in turn systemd), just don't add '-enable-systemd'
./configure  -enable-systemd --enable-silent-rules --prefix=/usr
make
# if you also want to install it:
sudo make install
#running gettz from the source dir:
GETTZ_SHP_FILE="data/tz_world/tz_world.shp" GETTZ_DBF_FILE="data/tz_world/tz_world.dbf" ./src/gettz 20.3821615  47.891469167
# ^ should yield Europe/Budapest
```

## Mandatory XKCD reference
![Time zone](https://imgs.xkcd.com/comics/supervillain_plan.png)

## Bookmarks

http://stackoverflow.com/questions/5584602/determine-timezone-from-latitude-longitude-without-using-web-services-like-geona

http://redwarrior.org/blog/?p=16

