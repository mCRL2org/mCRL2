-include ../config

MAN_OBJECTS  = libgsrewrite.7 tbf2lpe.1 
TXT_OBJECTS  = libgsrewrite.txt tbf2lpe.txt
HTML_OBJECTS = libgsrewrite.html tbf2lpe.html

# For now build only man objects
OBJECTS         = $(MAN_OBJECTS)
MAN_OBJECT_SUFFIXES = $(patsubst .%,%, $(sort $(suffix $(MAN_OBJECTS))))
MAN_INSTALL_TARGETS = $(patsubst %,%.install,$(MAN_OBJECT_SUFFIXES))

.PHONY: all clean distclean install

ifdef USE_XMLTO
all: $(OBJECTS)

clean:
	$(RM) $(OBJECTS)

distclean: clean

install: all $(MAN_INSTALL_TARGETS)

else
all:

clean:

distclean:

install:
endif

%.install:
	$(INSTALL) -d $(mandir)/man$*
	@for f in $(filter %.$*,$(OBJECTS)); do\
	  if test -f $$f; then\
	    c="$(INSTALL) -m 444 $$f $(mandir)/man$*";\
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
