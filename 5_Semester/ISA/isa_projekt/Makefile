#	ISA Projekt
#	Varianta 2: Přenos souboru skrz skrytý kanál
#	Autor: Michal Pyšík (login: xpysik00)

all:
	g++ -std=c++17 -o secret secret.cpp secret.hpp -lssl -lcrypto

pack:
	tar -cvf xpysik00.tar secret.cpp secret.hpp Makefile secret.1 manual.pdf

clean:
	rm -f secret xpysik00.tar
