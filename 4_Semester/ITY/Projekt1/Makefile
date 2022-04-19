FNAME=proj1

all: pdf clean

pdf:
	latex ${FNAME}.tex
	latex ${FNAME}.tex
	dvips -t a4 ${FNAME}.dvi
	ps2pdf ${FNAME}.ps

clean:
	rm -f ${FNAME}.dvi ${FNAME}.ps ${FNAME}.aux ${FNAME}.log ${FNAME}.out

pack:
	zip xpysik00-fit.zip ${FNAME}.tex Makefile
