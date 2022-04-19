// Druhy IOS Projekt - The Faneuil Hall Problem
// Prace se synchronizaci procesu, sdilenou pameti a semafory
// Autor: Michal Pysik (xpysik00)
// Vysledne reseni inspirovano knihou The Little Book of Semaphores od Allena B. Downeyho

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include "proj2.h"

#define MAPMEM(pointer) {(pointer) = mmap(NULL, sizeof(*(pointer)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);}
#define UNMAPMEM(pointer) {munmap((pointer), sizeof((pointer)));}


FILE *outfile; //ukazatel na vystupni soubor

int PI, IG, JG, IT, JT; //promenne predane jako argumenty

sem_t *soubafor = NULL; //semafory
sem_t *bouncerafor = NULL; //securitaci co stoji pred klubem a pousteji lidi dovnitr se jmenuji bounceri, proto tento skvely nazev
sem_t *checkedafor = NULL;
sem_t *allafor = NULL;
sem_t *confirmafor = NULL;
sem_t *procafor = NULL;

int *actionCount = NULL; //sdilene promenne
int *imCount = NULL;
int *NE = NULL;
int *NC = NULL;
int *NB = NULL;
int *inDaBuildin = NULL;
int *procCount = NULL;


int init()
{
	MAPMEM(actionCount);
	MAPMEM(imCount);
	MAPMEM(NE);
	MAPMEM(NC);
	MAPMEM(NB);
	MAPMEM(inDaBuildin);
	MAPMEM(procCount);
	if ((soubafor = sem_open("/xpysik00.ios2.semafor1", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) return 1;
	if ((bouncerafor = sem_open("/xpysik00.ios2.semafor2", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) return 1;
	if ((checkedafor = sem_open("/xpysik00.ios2.semafor3", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) return 1;
	if ((allafor = sem_open("/xpysik00.ios2.semafor4", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED) return 1; //POZOR! TENTO SEMAFOR MA UMYSLNE DEFAULT VALUE 0
	if ((confirmafor = sem_open("/xpysik00.ios2.semafor5", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED) return 1; //POZOR! A TENTO TAKY
	if ((procafor = sem_open("/xpysik00.ios2.semafor6", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) return 1; // tento semafor pouze hlida ze vsechny child procesy skoncily


	return 0;
}


void cleanAll()
{
	UNMAPMEM(actionCount);
	UNMAPMEM(imCount);
	UNMAPMEM(NE);
	UNMAPMEM(NC);
	UNMAPMEM(NB);
	UNMAPMEM(inDaBuildin);
	UNMAPMEM(procCount);
	sem_close(soubafor);
	sem_close(bouncerafor);
	sem_close(checkedafor);
	sem_close(allafor);
	sem_close(confirmafor);
	sem_close(procafor);
	sem_unlink("/xpysik00.ios2.semafor1");
	sem_unlink("/xpysik00.ios2.semafor2");
	sem_unlink("/xpysik00.ios2.semafor3");
	sem_unlink("/xpysik00.ios2.semafor4");
	sem_unlink("/xpysik00.ios2.semafor5");
	sem_unlink("/xpysik00.ios2.semafor6");
}


void process_judge(int JG, int JT, int PI)
{
	int done = 0;
	int tmp = 0; //pomocna pro confirmation, ktera hodnoty vynuluje
	(*inDaBuildin) = 0;


	do{
		usleep(1000 * (rand() % (JG + 1)));
		sem_wait(soubafor);
		fprintf(outfile,"%-4d		: JUDGE 	: wants to enter\n", ++(*actionCount));
		sem_post(soubafor);

		sem_wait(bouncerafor);
		sem_wait(checkedafor);
		(*inDaBuildin) = 1;
		sem_wait(soubafor);
		fprintf(outfile,"%-4d		: JUDGE 	: %-22s : %d	: %d	: %d\n", ++(*actionCount),"enters", (*NE), (*NC), (*NB) );
		sem_post(soubafor);

		if( (*NE) > (*NC) )
		{
			sem_post(checkedafor);
			sem_wait(soubafor);
			fprintf(outfile,"%-4d		: JUDGE 	: %-22s : %d	: %d	: %d\n", ++(*actionCount),"waits for imm", (*NE), (*NC), (*NB) );
			sem_post(soubafor);
			sem_wait(allafor);
			sem_wait(checkedafor);
		}

		sem_wait(soubafor);
		fprintf(outfile,"%-4d		: JUDGE 	: %-22s : %d	: %d	: %d\n", ++(*actionCount),"starts confirmation", (*NE), (*NC), (*NB) );
		sem_post(soubafor);

		usleep(1000 * (rand() % (JT + 1)));


		sem_wait(soubafor);
		done += (*NC);
		tmp = (*NC);
		fprintf(outfile,"%-4d		: JUDGE 	: %-22s : %d	: %d	: %d\n", ++(*actionCount),"ends confirmation", (*NE) = 0, (*NC) = 0, (*NB) );
		sem_post(soubafor);

		for(int i = 0; i < tmp; i++) sem_post(confirmafor);




		usleep(1000 * (rand() % (JT + 1)));
		sem_wait(soubafor);
		fprintf(outfile,"%-4d		: JUDGE 	: %-22s : %d	: %d	: %d\n", ++(*actionCount),"leaves", (*NE), (*NC), (*NB) );
		sem_post(soubafor);

		(*inDaBuildin) = 0;
		sem_post(checkedafor);
		sem_post(bouncerafor);

	}while (done < PI);

	sem_wait(soubafor);
	fprintf(outfile,"%-4d		: JUDGE 	: finishes\n", ++(*actionCount));
	sem_post(soubafor);
}


void process_imigrant(int IT, int ID)
{
	sem_wait(soubafor);
	fprintf(outfile, "%-4d		: IMM %d 	: starts\n", ++(*actionCount), ID);
	sem_post(soubafor);

	sem_wait(bouncerafor);
	sem_wait(soubafor);
	fprintf(outfile,"%-4d		: IMM %d 	: %-22s : %d	: %d	: %d\n", ++(*actionCount), ID,"enters", ++(*NE), (*NC), ++(*NB) );
	sem_post(soubafor);
	sem_post(bouncerafor);

	sem_wait(checkedafor);
	sem_wait(soubafor);
	fprintf(outfile,"%-4d		: IMM %d 	: %-22s : %d	: %d	: %d\n", ++(*actionCount), ID,"checks", (*NE), ++(*NC), (*NB) );
	sem_post(soubafor);

	if( (*inDaBuildin) && (*NE) == (*NC) )
	{
		sem_post(allafor);
		sem_post(checkedafor);
	}
	else 	sem_post(checkedafor);

	sem_wait(confirmafor);
	sem_wait(soubafor);
	fprintf(outfile,"%-4d		: IMM %d 	: %-22s : %d	: %d	: %d\n", ++(*actionCount), ID,"wants certificate", (*NE), (*NC), (*NB) );
	sem_post(soubafor);
	usleep(1000 * (rand() % (IT + 1)));
	sem_wait(soubafor);
	fprintf(outfile,"%-4d		: IMM %d 	: %-22s : %d	: %d	: %d\n", ++(*actionCount), ID,"got certificate", (*NE), (*NC), (*NB) );
	sem_post(soubafor);

	sem_wait(bouncerafor);
	sem_wait(soubafor);
	fprintf(outfile,"%-4d		: IMM %d 	: %-22s : %d	: %d	: %d\n", ++(*actionCount), ID,"leaves", (*NE), (*NC), --(*NB) );
	sem_post(soubafor);
	sem_post(bouncerafor);


	--(*procCount);
	if( (*procCount) <= 0 ) sem_post(procafor);

	exit(0);
}


void process_imgenerator(int PI, int IG, int IT)
{
	pid_t ImFork;
	for(int i = 0; i < PI; i++)
	{
		usleep(1000 * (rand() % (IG + 1)));
		ImFork = fork();
		if(ImFork == 0)
		{
			process_imigrant(IT, ++(*imCount));
		}
	}

	waitpid(ImFork,NULL,0); //cekani nez se vsichni imigranti vrati z vyletu
	
	(*procCount) -= 1;
	if( (*procCount) <= 0) sem_post(procafor);

	exit(0);
}




int main(int argc, char **argv)
{
	if(argc != 6)
	{
		fprintf(stderr, "Zadan neplatny pocet parametru, zadejte prosim prave 5 parametru\n");
		return 1;
	}


	PI = atoi(argv[1]);
	if(PI < 1)
	{
		fprintf(stderr, "Pocet imigrantu (1. argument) musi byt roven nebo vetsi nez 1\n");
		return 1;
	}
	IG = atoi(argv[2]);
	if(IG < 0 || IG > 2000)
	{
		fprintf(stderr, "Max doba po ktere je generovan imigrant (2. argument) musi byt v rozmezi <0,2000>\n");
		return 1;
	}
	JG = atoi(argv[3]);
	if(JG < 0 || JG > 2000)
	{
		fprintf(stderr, "Max doba po ktere soudce vstoupi do budovy (3. argument) musi byt v rozmezi <0,2000>\n");
		return 1;
	}
	IT = atoi(argv[4]);
	if(IT < 0 || IT > 2000)
	{
		fprintf(stderr, "Max doba simulujici vyzvedavani certifikatu (4. argument) musi byt v rozmezi <0,2000>\n");
		return 1;
	}
	JT = atoi(argv[5]);
	if(JT < 0 || JT > 2000)
	{
		fprintf(stderr, "Max doba simulujici trvani vydavani rozhodnuti soudcem (5. argument) musi byt v rozmezi <0,2000>\n");
		return 1;
	}

	srand(time(NULL)); //setup nahodneho generovani




	if((outfile = fopen("proj2.out", "w")) == NULL)
	{
		fprintf(stderr,"Soubor se nepodarilo otevrit, ujistete se ze je v adresari soubor proj2.out\n");
		return 1;
	}

	setbuf(outfile, NULL); //musime zarucit spravne poradi zapisu do souboru

	if(init()) //kdyz selze init
	{
		cleanAll();
		fclose(outfile);
		fprintf(stderr,"Nastala chyba pri inicializaci semaforu\n");
		return 1;
	}

	(*actionCount) = 0; //inicializace sdilenych promennych
	(*imCount) = 0;
	(*NE) = 0;
	(*NC) = 0;
	(*NB) = 0;
	(*inDaBuildin) = 0;
	(*procCount) = PI + 1;

	sem_wait(procafor);

	pid_t JudgeFork = fork();
	if(JudgeFork == 0)
	{
		process_judge(JG, JT, PI);
	}
	else
	{
		pid_t GenerFork = fork();
		if(GenerFork == 0)
		{
			process_imgenerator(PI, IG, IT);
		}
		else waitpid(GenerFork, NULL, 0); //cekani nez se generator ukonci
	}

	sem_wait(procafor);
	sem_post(procafor);

	sem_wait(soubafor);
	fclose(outfile);
	sem_post(soubafor);

	cleanAll();
	exit(0);

	return 0;
}
