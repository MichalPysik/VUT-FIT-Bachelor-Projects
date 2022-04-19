FNAME=proj2


all:
	latex ${FNAME}.tex
	latex ${FNAME}.tex
	dvips -t a4 ${FNAME}.dvi
	ps2pdf ${FNAME}.ps

clean:
	rm -f ${FNAME}.dvi ${FNAME}.ps ${FNAME}.aux ${FNAME}.log ${FNAME}.out ${FNAME}.pdf xpysik00-fit.zip

pack:
	zip xpysik00-fit.zip ${FNAME}.tex Makefile
