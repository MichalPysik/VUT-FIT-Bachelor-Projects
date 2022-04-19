#	IPK Projekt 2
#	Varianta ZETA: Sniffer paketů
#	Autor: Michal Pyšík (login: xpysik00)

all:
	g++ -std=c++17 -o ipk-sniffer ipk-sniffer.cpp ipk-sniffer.hpp -lpcap

pack:
	tar -cf xpysik00.tar ipk-sniffer.cpp ipk-sniffer.hpp Makefile README manual.pdf

clean:
	rm -f ipk-sniffer xpysik00.tar
