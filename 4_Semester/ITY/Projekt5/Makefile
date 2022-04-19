FNAME=proj5


all:
	pdflatex $(FNAME).tex

clean:
	rm -f ${FNAME}.aux ${FNAME}.log ${FNAME}.nav ${FNAME}.out ${FNAME}.snm ${FNAME}.toc ${FNAME}.vrb ${FNAME}.pdf xpysik00-fit.zip

pack:
	zip xpysik00-fit.zip ${FNAME}.tex Makefile insert.png delete.png

