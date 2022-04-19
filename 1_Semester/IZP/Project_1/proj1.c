#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Autor: Michal Pysik (xpysik00)
// v pripade ze dany komentar zacina slovem "debug", jednĂĄ se o ladici prikaz pouzivany k debugovani a testovani programu



// funkce pro prirazovani znaku cislum
int value(char bruh1, char bruh2)
{
if(bruh1=='1') {
	if(bruh2=='1') {
		return 0;} }
if(bruh1=='2') {
	if(bruh2=='2'||bruh2=='a'||bruh2=='b'||bruh2=='c'||bruh2=='A'||bruh2=='B'||bruh2=='C') {
		return 0;} }
if(bruh1=='3') {
	if(bruh2=='3'||bruh2=='d'||bruh2=='e'||bruh2=='f'||bruh2=='D'||bruh2=='E'||bruh2=='F') {
		return 0;} }
if(bruh1=='4') {
	if(bruh2=='4'||bruh2=='g'||bruh2=='h'||bruh2=='i'||bruh2=='G'||bruh2=='H'||bruh2=='I') {
		return 0;} }
if(bruh1=='5') {
	if(bruh2=='5'||bruh2=='j'||bruh2=='k'||bruh2=='l'||bruh2=='J'||bruh2=='K'||bruh2=='L') {
		return 0;} }
if(bruh1=='6') {
	if(bruh2=='6'||bruh2=='m'||bruh2=='n'||bruh2=='o'||bruh2=='M'||bruh2=='N'||bruh2=='O') {
		return 0;} }
if(bruh1=='7') {
	if(bruh2=='7'||bruh2=='p'||bruh2=='q'||bruh2=='r'||bruh2=='s'||bruh2=='P'||bruh2=='Q'||bruh2=='R'||bruh2=='S') {
		return 0;} }
if(bruh1=='8') {
	if(bruh2=='8'||bruh2=='t'||bruh2=='u'||bruh2=='v'||bruh2=='T'||bruh2=='U'||bruh2=='V') {
		return 0;} }
if(bruh1=='9') {
	if(bruh2=='9'||bruh2=='w'||bruh2=='x'||bruh2=='y'||bruh2=='z'||bruh2=='W'||bruh2=='X'||bruh2=='Y'||bruh2=='Z') {
		return 0;} }
if(bruh1=='0') {
	if(bruh2=='0'||bruh2=='+') {
		return 0;} }
return 1;
}


int main(int argc, char *argv[])
{
	
if(argc>2) {
	fprintf(stderr, "Error: Bylo zadano vice nez 1 argument \n");
	return 1; }

if(argc==2){
	char line1[100];
	char line2[100];
	char c;
	// count,i,p => pocitadla; lenl,lena => delky retezcu
	// sum je pocet nalezenych kontaktu (momentalne ho vyuziva jen jedna podminka, ale muze byt vytisknut, coz vsak v zadani nebylo)
	int count,i,p,sum,lenl,lena;
	//boolovske promenne (uspornejsi vuci pameti nez pouziti integeru s hodnotou 1/0)
	bool kontrola;
	bool legitcheck = false;
	sum = 0;
	
	// nacteni jmena osoby
	do {
		count = 0;
		c = getc(stdin);
		if(c==EOF) {
			if(legitcheck==false) {
				fprintf(stderr, "Seznam je prazdny \n");
				return 1;}
			break;}
		legitcheck = true;
		while(c != '\n') {
			line1[count] = c;
			count++;
			c = getc(stdin);}
		line1[count] = '\0';
		// debug printf("%s \n", line1);
	
		// nacteni tel. cisla osoby	
		count = 0;
		c = getc(stdin);
		if(c==EOF) {
			if(sum==0) {
				printf("Not found \n");}
			fprintf(stderr, "Seznam je ve spatnem formatu (nejspise ke jmenu chybi tel. clislo), ukoncuji program \n");
			return 1;}
		while(c != '\n') {
			line2[count] = c;
			count++;
			c = getc(stdin);}
		line2[count] = '\0';
		// debug printf("%s \n", line2);
	
		// test shody argumentu s jmenem
		kontrola = false;
		lenl = strlen(line1);
		lena = strlen(argv[1]);
		// debug printf("delka jmena je %d \n", lenl);	
		for(i=0;i<lenl;i++) {
			int g=i;
			p=0;
			while(value(argv[1][p],line1[g])==0 && kontrola==false) {
				p++;
				g++;
				if(p==lena) {
					kontrola = true;
					sum++;
					printf("%s, %s \n", line1, line2);} } }
	
		// test shody argumentu s tel. cislem
		lenl = strlen(line2);
		// radek pro debug printf("delka cisla je %d \n", lenl);	
		for(i=0;i<lenl;i++) {
			int g=i;
			p=0;
			// zde pouziti funkce value neni nutna, avsak hodi se pro pripad ze telefonni cislo obsahuje znak "+"
			while(value(argv[1][p],line2[g])==0 && kontrola==false) {
				p++;
				g++;
				if(p==lena) {
					kontrola = true;
					sum++;
					printf("%s, %s \n", line1, line2);} } } } while(1); //cyklus obsahuje podminky pro ukonceni uvnitr
	if(sum==0 && legitcheck) {
		printf("Not found \n");} }










if(argc<2) {
	char fullseznam[2048];
	int c = getc(stdin);
	if(c == EOF) {
		fprintf(stderr, "Vybrany seznam je prazdny \n");
		return 1;}	
	int i=0;
	while(c != EOF)	{
		fullseznam[i]=c;
		i++;
		c = getc(stdin);}
	printf("%s", fullseznam); }					



// debug printf("\n argument je %s \n",argv[1]);


return 0;
}

