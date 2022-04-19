//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - priority queue code
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     Michal Pysik <xpysik00@stud.fit.vutbr.cz>
// $Date:       $2020-02-19
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author Michal Pysik
 * 
 * @brief Implementace metod tridy prioritni fronty.
 */

#include <stdlib.h>
#include <stdio.h>

#include "tdd_code.h"

//============================================================================//
// ** ZDE DOPLNTE IMPLEMENTACI **
//
// Zde doplnte implementaci verejneho rozhrani prioritni fronty (Priority Queue)
// 1. Verejne rozhrani fronty specifikovane v: tdd_code.h (sekce "public:")
//    - Konstruktor (PriorityQueue()), Destruktor (~PriorityQueue())
//    - Metody Insert/Remove/Find a GetHead
//    - Pripadne vase metody definovane v tdd_code.h (sekce "protected:")
//
// Cilem je dosahnout plne funkcni implementace prioritni fronty implementovane
// pomoci tzv. "double-linked list", ktera bude splnovat dodane testy 
// (tdd_tests.cpp).
//============================================================================//

PriorityQueue::PriorityQueue()
{
	head = NULL;
}

PriorityQueue::~PriorityQueue()
{
	if(GetHead() == NULL) return;
	PriorityQueue::Element_t *temp = GetHead();
	
	while(temp->pNext != NULL)
	{
		temp = temp->pNext;
	 	delete temp->pPrev;
	}
	
	if(temp->pNext == NULL && temp->pPrev == NULL)
	{
		delete temp;
	}
}

void PriorityQueue::Insert(int value)
{
	PriorityQueue::Element_t *temp = GetHead();
	PriorityQueue::Element_t *hodnota = new PriorityQueue::Element_t;
	hodnota->pPrev = NULL;
	hodnota->pNext = NULL;
	hodnota->value = value;
	
	//if(head != NULL) printf("insert %d\n",head->value); //...DEBUG
	//else printf("insert NULL\n"); //...DEBUG
	
	if(temp == NULL) //kdyz je fronta prazdna
	{
		head = hodnota;
		return;
	}

	else if(temp->pPrev == NULL && value < temp->value)
	{
		hodnota->pNext = temp;		
		temp->pPrev = hodnota;
		head = hodnota;
		return;
	}
	else
	{
		while(temp->pNext != NULL)
		{
			
			temp = temp->pNext;

			if(value < temp->value) //prvek nekde ve fronte (jinde nez na zacatku a konci)
			{
				hodnota->pPrev = temp->pPrev;
				temp->pPrev->pNext = hodnota;
				hodnota->pNext = temp;
				temp->pPrev = hodnota;
				return;
			}
		}

		temp->pNext = hodnota; //zarazeni na konec
		hodnota->pPrev = temp;
		hodnota->pNext = NULL;
		return;
	}
}

bool PriorityQueue::Remove(int value)
{
	//if(head != NULL) printf("remove %d\n",head->value); //DEBUG
	//else printf("remove NULL\n"); //DEBUG

	if(GetHead() == NULL) return false;  //fronta je prazdna

	PriorityQueue::Element_t *temp = GetHead();
	
	
	if((temp->value == value) && (temp->pNext == NULL))
	{
		delete temp;
		head = NULL; //kdyz je fronta jednoprvkova a zadana hodnota odpovida
		return true;
	}
		
	while(temp->value != value)
	{
		if(temp->pNext == NULL) return false; //kdyz prvek neni nalezen
		temp = temp->pNext;
	}
	
	if(temp->pPrev == NULL && temp->pNext != NULL)
	{
		temp->pNext->pPrev = NULL;
		head = temp->pNext;
		delete temp;
		return true;
	}


	else if(temp->pPrev != NULL && temp->pNext == NULL)
	{
		temp->pPrev->pNext = NULL;
		delete temp;
		return true;
	}

	else if(temp->pPrev != NULL && temp->pNext != NULL)
	{
		temp->pPrev->pNext = temp->pNext;
		temp->pNext->pPrev = temp->pPrev;
		delete temp;
		return true;
	}
	
	
	return false; //kdyby se neco nepovedlo (ale nenapada me co)
		
	
}

PriorityQueue::Element_t *PriorityQueue::Find(int value)
{
	if(GetHead() == NULL) return NULL; //pokud je fronta prazdna, bez tohoto bychom dostali segmentation fault
	
	PriorityQueue::Element_t *temp = GetHead();
	
	if(temp->value == value) return temp; //kdyz je prvek head

	while(temp->pNext != NULL) //cestou na konec hleda prvek
	{
		temp = temp->pNext;
		if(temp->value == value) return temp;
	}
    	return NULL; //pokud nenajde tak vrati NULL
}

PriorityQueue::Element_t *PriorityQueue::GetHead()
{
	//printf("debug head\n"); //debug
	return head; // kdyz bude fronta prazdna vrati NULL
}

/*** Konec souboru tdd_code.cpp ***/
