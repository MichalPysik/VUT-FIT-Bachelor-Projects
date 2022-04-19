#ifndef PROJ2_H
#define PROJ2_H

int init(); //inicializuje sdilenou pamet a semafory

void cleanAll(); //vycisti sdilenou pamet a semafory

void process_judge(int JG, int JT, int PI); //proces soudce

void process_imigrant(int IT, int ID); //proces imigranta

void process_imgenerator(int PI, int IG, int IT); //generator imigrantu




#endif
