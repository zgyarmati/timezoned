EXTRA_DIST = timezoned.asciidoc gettz.asciidoc

if HAVE_A2X
dir = $(top_srcdir)/man

man_MANS = timezoned.1 gettz.1

timezoned.1:
	$(A2X) -k -f manpage timezoned.asciidoc

timezoned.pdf: timezoned.1
	$(A2X) -f pdf timezoned.asciidoc -a revdate="`date`" -v -v

gettz.1:
	$(A2X) -k -f manpage gettz.asciidoc

gettz.pdf: gettz.1
	$(A2X) -f pdf gettz.asciidoc -a revdate="`date`" -v -v

CLEANFILES = timezoned.1 timezoned.pdf gettz.1 gettz.pdf

all-local: timezoned.pdf timezoned.1 gettz.1 gettz.pdf
clean-local:
	rm -rf $(dir)/timezoned.1
	rm -rf timezoned.pdf
	rm -rf gettz.1 gettz.pdf

endif
