EXTRA_DIST = timezoned.asciidoc

if HAVE_A2X
dir = $(top_srcdir)/man

man_MANS = timezoned.1

timezoned.1:
	$(A2X) -f manpage timezoned.asciidoc

timezoned.pdf:
	$(A2X) -f pdf timezoned.asciidoc -a revdate="`date`" -v -v

CLEANFILES = timezoned.1 timezoned.pdf

all-local: timezoned.pdf timezoned.1
clean-local:
	rm -rf $(dir)/timezoned.1
	rm -rf timezoned.pdf

endif
