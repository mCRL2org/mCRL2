# Relative path to the root of the source tree
TREE_ROOT = ../../

-include ../../config

MAN_OBJECTS  = mcrl22lpe.1 lpe2lts.1 lpe2torx.1 lpeinfo.1 lpepp.1 \
               lperewr.1 lpedataelm.1 lpeconstelm.1 lpeparelm.1 ltsconvert.1 \
               ltsinfo.1 ltsmin.1 pnml2mcrl2.1 tbf2lpe.1 sim.1 \
               svc2fsm.1 xsim.1 tracepp.1 librewrite_c.7 librewrite.7 \
               lpeconfcheck.1 lpeinvelm.1 lpeformcheck.1 \
               prover.7 bdd_prover.7 confluence_checker.7 disjointness_checker.7 \
               formula_checker.7 invariant_checker.7 invariant_eliminator.7

TXT_OBJECTS  = mcrl22lpe.txt lpe2lts.txt lpe2torx.txt lpeinfo.txt \
               lperewr.txt lpedataelm.txt lpeconstelm.txt lpeparelm.txt \
               ltsconvert.txt ltsinfo.txt ltsmin.txt ltspp.txt pnml2mcrl2.txt \
               tbf2lpe.txt sim.txt svc2fsm.txt xsim.txt tracepp.txt \
               librewrite_c.txt librewrite.txt \
               lpeconfcheck.txt lpeinvelm.txt lpeformcheck.txt \
               prover.txt bdd_prover.txt confluence_checker.txt disjointness_checker.txt \
               formula_checker.txt invariant_checker.txt invariant_eliminator.txt

HTML_OBJECTS = mcrl22lpe.html lpe2lts.html lpe2torx.html lpeinfo.html \
               lpepp.html lperewr.html lpedataelm.html lpeconstelm.html \
               lpeparelm.html ltsconvert.html ltsinfo.html ltsmin.html \
               pnml2mcrl2.html tbf2lpe.html sim.html svc2fsm.html xsim.html \
               tracepp.html librewrite_c.html librewrite.html \
               lpeconfcheck.html lpeinvelm.html lpeformcheck.html \
               prover.html bdd_prover.html confluence_checker.html disjointness_checker.html \
               formula_checker.html invariant_checker.html invariant_eliminator.html


# For now build only man objects
OBJECTS             = $(MAN_OBJECTS)
MAN_OBJECT_SUFFIXES = $(patsubst .%,%, $(sort $(suffix $(MAN_OBJECTS))))
MAN_INSTALL_TARGETS = $(patsubst %,%.install,$(MAN_OBJECT_SUFFIXES))

.PHONY: all clean distclean install xmlto_available

all: $(OBJECTS)

clean:
	$(RM) $(OBJECTS)

distclean: clean

xmlto_available: $(MAN_INSTALL_TARGETS)

install: all $(USE_XMLTO)

documentation: all

%.install:
	$(INSTALL) -d $(mandir)/man$*
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
