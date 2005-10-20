-include ../config

MAN_OBJECTS  = libgsrewrite.7 
TXT_OBJECTS  = libgsrewrite.txt
HTML_OBJECTS = libgsrewrite.html

# For now build only man objects
OBJECTS         = $(MAN_OBJECTS)
MAN_OBJECT_SUFFIXES = $(patsubst .%,%, $(sort $(suffix $(MAN_OBJECTS))))
MAN_INSTALL_TARGETS = $(patsubst %,%.install,$(MAN_OBJECT_SUFFIXES))

.PHONY: all clean install

ifdef USE_XMLTO
all: $(OBJECTS)

clean:
	$(RM) $(OBJECTS)

install: all $(MAN_INSTALL_TARGETS)

else
all:

clean:

install:
endif

%.install:
	$(INSTALL) -d $(mandir)/man$*
	@for f in $(filter %.$*,$(OBJECTS)); do\
	  if test -f $$f; then\
	    c="$(INSTALL) $$f $(mandir)/man$*";\
	    echo $$c;\
	    $$c;\
	  fi;\
	done

%.1 %.2 %.3 %.4 %.5 %.6 %.7 %.8 %.9: %.xml
	xmlto man $<

%.html: %.xml
	xmlto xhtml-nochunks $<

%.txt: %.xml
	xmlto txt $<
