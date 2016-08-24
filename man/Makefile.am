EXTRA_DIST = timezoned.asciidoc gettz.asciidoc

if HAVE_A2X
dir = $(top_srcdir)/man
destdir = .

man_MANS = timezoned.1 gettz.1

timezoned.1:
	$(A2X) -D $(destdir) -k -f manpage $(dir)/timezoned.asciidoc

timezoned.pdf: timezoned.1
	$(A2X) -D $(destdir) -f pdf $(dir)/timezoned.asciidoc -a revdate="`date`" -v -v

gettz.1:
	$(A2X) -D $(destdir) -k -f manpage $(dir)/gettz.asciidoc

gettz.pdf: gettz.1
	$(A2X) -D $(destdir) -f pdf $(dir)/gettz.asciidoc -a revdate="`date`" -v -v

CLEANFILES = timezoned.1 timezoned.pdf gettz.1 gettz.pdf

all-local: timezoned.pdf timezoned.1 gettz.1 gettz.pdf
clean-local:
	rm -rf $(dir)/timezoned.1
	rm -rf timezoned.pdf
	rm -rf gettz.1 gettz.pdf

endif
