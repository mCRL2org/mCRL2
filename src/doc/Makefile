ifeq ($(findstring clean,$(MAKECMDGOALS)),)
 include ../config
endif

XMLTO        = xmlto
MAN_OBJECTS  = libgsrewrite.7 
TXT_OBJECTS  = libgsrewrite.txt
HTML_OBJECTS = libgsrewrite.html

# For now build only man objects
OBJECTS     := $(MAN_OBJECTS)

.PHONY: all clean install
.SECONDARY: $(patsubst %,$(mandir)/man%,1 2 3 4 5 6 7 8 9)

all: $(OBJECTS)

clean:
	$(RM) $(OBJECTS)

install: all $(patsubst %,%.install,$(OBJECTS))

$(mandir)/man%:
	$(INSTALL) -d $@

%.1.install: $(mandir)/man1
	@if test -f $*.1; then \
	  $(INSTALL) $*.1 $<; \
	fi
%.2.install: $(mandir)/man2
	@if test -f $*.2; then \
	  $(INSTALL) $*.2 $<; \
	fi
%.3.install: $(mandir)/man3
	@if test -f $*.3; then \
	  $(INSTALL) $*.3 $<; \
	fi
%.4.install: $(mandir)/man4
	@if test -f $*.4; then \
	  $(INSTALL) $*.4 $<; \
	fi
%.5.install: $(mandir)/man5
	@if test -f $*.5; then \
	  $(INSTALL) $*.5 $<; \
	fi
%.6.install: $(mandir)/man6
	@if test -f $*.6; then \
	  $(INSTALL) $*.6 $<; \
	fi
%.7.install: $(mandir)/man7
	@if test -f $*.7; then \
	  $(INSTALL) $*.7 $<; \
	fi
%.8.install: $(mandir)/man8
	@if test -f $*.8; then \
	  $(INSTALL) $*.8 $<; \
	fi
%.9.install: $(mandir)/man9
	@if test -f $*.9; then \
	  $(INSTALL) $*.9 $<; \
	fi

%.1 %.2 %.3 %.4 %.5 %.6 %.7 %.8 %.9: %.xml
	xmlto man $<

%.html: %.xml
	xmlto xhtml-nochunks $<

%.txt: %.xml
	xmlto txt $<
