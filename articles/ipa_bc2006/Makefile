#Make rules
all     : 

clean   :
	rm -f *.tbf *.aut *.svc *.fsm *.bcg *.dlk *.dot *.dvi *.log *.aux *.toc *.out *.blg *.bbl *.ps *.o evaluator *~ core core.*

#mCRL rules
%.tbf   : %.mcrl
	mcrl -regular $^

%.svc   : %.tbf
	instantiator -i -svc-term -o $@ $^

%.aut   : %.svc
	svc2aut -o $@ $^

%.bcg   : %.aut
	bcg_io $^ $@
	bcg_min -branching $@

%.fsm   : %.svc
	svc2fsm $^ > $@

%.dot   : %.aut
	lts2dot -o $@ $^

%.dot   : %.svc
	lts2dot -o $@ $^

%.png   : %.dot
	dot -Tpng $^ > $@

%.ps    : %.dot
	dot -Tps $^ > $@

#LaTeX rules
%.dvi   : %.tex
	texi2dvi $^

%.ps    : %.dvi
	dvips -o $@ -t a4 $^

%.pdf   : %.ps
	ps2pdf14 -sPAPERSIZE=a4 $^

%.pdf   : %.tex
	pdflatex $^
