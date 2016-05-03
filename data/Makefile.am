
SUBDIRS = . tz_world

edit = @sed \
       -e 's|@sbindir[@]|$(sbindir)|g' \
       -e 's|@sysconfdir[@]|$(sysconfdir)|g' \
       -e 's|@localstatedir[@]|$(localstatedir)|g' \
       -e 's|@libexecdir[@]|$(libexecdir)|g'

# systemd unit file
systemdsystemunitdir = $(SYSTEMD_UNIT_DIR)
systemdsystemunit_in_files = Timezoned.service.in

if HAVE_SYSTEMD
systemdsystemunit_DATA = Timezoned.service
Timezoned.service: Timezoned.service.in
	$(edit) $< >$@
endif

if HAVE_SYSTEMD
DISTCLEANFILES += $(systemdsystemunit_DATA)
endif


EXTRA_DIST = \
	$(systemdsystemunit_in_files)
