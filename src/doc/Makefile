# Relative path to the root of the source tree
TREE_ROOT = ../../

-include ../../config

MAN_OBJECTS  = librewrite_c.7 librewrite.7 tbf2lpe.1 lpeinfo.1 \
               lpeconstelm.1 lpeparelm.1 pnml2mcrl2.1 lpepp.1 \
               mcrl22lpe.1
TXT_OBJECTS  = librewrite_c.txt librewrite.txt tbf2lpe.txt lpeinfo.txt \
	       lpeconstelm.txt lpeparelm.txt pnml2mcrl2.txt lpepp.txt \
               mcrl22lpe.txt
HTML_OBJECTS = librewrite_c.html librewrite.html tbf2lpe.html lpeinfo.html \
	       lpeconstelm.html lpeparelm.html pnml2mcrl2.html lpepp.html \
               mcrl22lpe.html

# For now build only man objects
OBJECTS         = $(MAN_OBJECTS)
MAN_OBJECT_SUFFIXES = $(patsubst .%,%, $(sort $(suffix $(MAN_OBJECTS))))
MAN_INSTALL_TARGETS = $(patsubst %,%.install,$(MAN_OBJECT_SUFFIXES))

.PHONY: all clean distclean install

all: $(OBJECTS)

clean:
	$(RM) $(OBJECTS)

distclean: clean

install: all $(MAN_INSTALL_TARGETS)

documentation: all

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
