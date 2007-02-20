-include ../../build/config.mk

ifneq ($(XSLTPROC),)
MAN_OBJECTS    = mcrl22lpe.1 lpe2lts.1 lpe2torx.1 lpebinary.1 lpeinfo.1 lpepp.1 \
	         lperewr.1 lpedataelm.1 lpeconstelm.1 lpeparelm.1 ltsconvert.1 \
	         ltsinfo.1 ltsgraph.1 ltsmin.1 pnml2mcrl2.1 tbf2lpe.1 sim.1 \
	         xsim.1 ltsgraph.1 tracepp.1 lpeconfcheck.1 lpedecluster.1 lpeinvelm.1 lpeformcheck.1 \
                 lpeuntime.1 lpesumelm.1 ltscompare.1 ltsview.1 \
	         librewrite.7 prover.7 bdd_prover.7 confluence_checker.7 \
	         disjointness_checker.7 formula_checker.7 invariant_checker.7 \
		 invariant_eliminator.7 \
                 bdd2dot.7 smt_solver.7 bdd_simplifier.7 smt_solver_ario.7 smt_solver_cvc_lite.7 \
                 bdd_path_eliminator.7 smt_lib_solver.7
endif

# For now build only man objects
OBJECTS             = $(MAN_OBJECTS)
MAN_OBJECT_SUFFIXES = $(patsubst .%,%, $(sort $(suffix $(MAN_OBJECTS))))
MAN_INSTALL_TARGETS = $(patsubst %,%.install,$(MAN_OBJECT_SUFFIXES))

.PHONY: all clean distclean install web

all: manifest.man

clean:
	rm -f $(addprefix *.,$(MAN_OBJECT_SUFFIXES)) manifest.man *.txt web/*.html

distclean: clean

install: all $(MAN_INSTALL_TARGETS)

documentation: all

manifest.man: *.xml
	@$(XSLTPROC) http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl index.xml
	@touch manifest.man

web: 
	@$(XSLTPROC) --stringparam base.dir web/ --stringparam html.stylesheet man.css http://docbook.sourceforge.net/release/xsl/current/html/chunk.xsl index.xml

%.install:
	@install -d $(mandir)/man$*
	@for f in $(filter %.$*,$(OBJECTS)); do\
	  if test -f $$f; then\
	    echo "Installing $(mandir)/man$*/$$f";\
	    install -m 644 $$f $(mandir)/man$*;\
	  fi;\
	done
