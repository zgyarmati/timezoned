#!/bin/sh

#dependecies
#apt-get install asciidoc dblatex
#edit /etc/asciidoc/dblatex/asciidoc-dblatex.xsl:

#<xsl:param name="doc.publisher.show">0</xsl:param>



a2x -fpdf timezoned.asciidoc -a revdate="`date`" -v -v
a2x -f manpage timezoned.asciidoc -v -v
#a2x -f docbook timezoned.asciidoc -v -v
#a2x -f xhtml timezoned.asciidoc -v -v

a2x -fpdf gettz.asciidoc -a revdate="`date`" -v -v
a2x -f manpage gettz.asciidoc -v -v

