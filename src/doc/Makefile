# Relative path to the root of the source tree
TREE_ROOT = ../../

-include ../../config

SOURCE_OBJECTS = $(wildcard %.xml)

MAN_OBJECTS    = mcrl22lpe.1 lpe2lts.1 lpe2torx.1 lpeinfo.1 lpepp.1 \
	         lperewr.1 lpedataelm.1 lpeconstelm.1 lpeparelm.1 ltsconvert.1 \
	         ltsinfo.1 ltsmin.1 pnml2mcrl2.1 tbf2lpe.1 sim.1 \
	         svc2fsm.1 xsim.1 tracepp.1 lpeconfcheck.1 lpeinvelm.1 lpeformcheck.1 \
	         librewrite_c.7 librewrite.7 prover.7 bdd_prover.7 confluence_checker.7 \
	         disjointness_checker.7 formula_checker.7 invariant_checker.7 \
		 invariant_eliminator.7

TXT_OBJECTS    = $(patsubst %.xml,%.txt,$(SOURCE_OBJECTS))
HTML_OBJECTS   = $(patsubst %.xml,%.html,$(SOURCE_OBJECTS))

# For now build only man objects
OBJECTS             = $(MAN_OBJECTS)
MAN_OBJECT_SUFFIXES = $(patsubst .%,%, $(sort $(suffix $(MAN_OBJECTS))))
MAN_INSTALL_TARGETS = $(patsubst %,%.install,$(MAN_OBJECT_SUFFIXES))

.PHONY: all clean distclean install xmlto_available

all: $(USE_XMLTO)

clean:
	$(RM) $(addprefix *.,$(MAN_OBJECT_SUFFIXES)) *.txt *.html

distclean: clean

xmlto_available: $(MAN_OBJECTS)

install: all $(MAN_INSTALL_TARGETS)

documentation: all

%.install:
	@$(INSTALL) -d $(mandir)/man$*
	@for f in $(filter %.$*,$(OBJECTS)); do\
	  if test -f $$f; then\
	    c="$(INSTALL) -m 644 $$f $(mandir)/man$*";\
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
