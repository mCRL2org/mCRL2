#Make rules
all     : 

clean   :
	rm -f *.dvi *.log *.aux *.toc *.out *.blg *.bbl *.ps

#LaTeX rules
%.dvi   : %.tex
	texi2dvi $^

%.ps    : %.dvi
	dvips $^

%.pdf   : %.ps
	ps2pdf $^

%.pdf   : %.tex
	pdflatex $^
