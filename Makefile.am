AUTOMAKE_OPTIONS = foreign
SUBDIRS = src man data
EXTRA_DIST = autogen.sh README.md timezoned.ini

dist-hook:
	if test -d "$(srcdir)/.git" && type git > /dev/null 2>&1; then \
	( cd "$(srcdir)" && git log --pretty=format:"%ad %s" --date=iso ) > "$(distdir)/ChangeLog"; \
	fi

#timezonedconfigdir=$(sysconfdir)

#timezonedconfig_DATA=timezoned.ini
sysconf_DATA = timezoned.ini
