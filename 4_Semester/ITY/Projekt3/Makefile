FNAME=proj3


all:
	latex ${FNAME}.tex
	latex ${FNAME}.tex
	dvips -t a4 ${FNAME}.dvi
	ps2pdf -sPAPERSIZE=a4 ${FNAME}.ps

clean:
	rm -f ${FNAME}.dvi ${FNAME}.ps ${FNAME}.aux ${FNAME}.log ${FNAME}.out ${FNAME}.pdf xpysik00-fit.zip

pack:
	zip xpysik00-fit.zip ${FNAME}.tex Makefile algorithm2e.sty etiopan.eps oniisan.eps oniisan2.eps

