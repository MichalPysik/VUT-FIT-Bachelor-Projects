
/* c201.c *********************************************************************}
{* Téma: Jednosměrný lineární seznam
**
**                     Návrh a referenční implementace: Petr Přikryl, říjen 1994
**                                          Úpravy: Andrea Němcová listopad 1996
**                                                   Petr Přikryl, listopad 1997
**                                Přepracované zadání: Petr Přikryl, březen 1998
**                                  Přepis do jazyka C: Martin Tuček, říjen 2004
**                                              Úpravy: Kamil Jeřábek, září 2020
**
** Implementujte abstraktní datový typ jednosměrný lineární seznam.
** Užitečným obsahem prvku seznamu je celé číslo typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou typu tList.
** Definici konstant a typů naleznete v hlavičkovém souboru c201.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ tList:
**
**      DisposeList ... zrušení všech prvků seznamu,
**      InitList ...... inicializace seznamu před prvním použitím,
**      InsertFirst ... vložení prvku na začátek seznamu,
**      First ......... nastavení aktivity na první prvek,
**      CopyFirst ..... vrací hodnotu prvního prvku,
**      DeleteFirst ... zruší první prvek seznamu,
**      PostDelete .... ruší prvek za aktivním prvkem,
**      PostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      Copy .......... vrací hodnotu aktivního prvku,
**      Actualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      Succ .......... posune aktivitu na další prvek seznamu,
**      Active ........ zjišťuje aktivitu seznamu.
**
** Při implementaci funkcí nevolejte žádnou z funkcí implementovaných v rámci
** tohoto příkladu, není-li u dané funkce explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam předá
** někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

// VYPRACOVAL: Michal Pysik (xpysik00)

#include "c201.h"

int errflg;
int solved;

void Error() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;                      /* globální proměnná -- příznak chyby */
}

void InitList (tList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/

 //jen nastavime oba ukazatele na NULL, jelikoz je v seznamu 0 prvku
 L->Act = NULL;
 L->First = NULL; 

}

void DisposeList (tList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam L do stavu, v jakém se nacházel
** po inicializaci. Veškerá paměť používaná prvky seznamu L bude korektně
** uvolněna voláním operace free.
***/

 L->Act = NULL;
 
 tElemPtr tmp = L->First; //budeme rusit prvky smerem od prvniho, pouzijeme jen ukazatel First + pomocny ukazatel

 while(tmp != NULL)
 {
	 L->First = tmp->ptr;
	 tmp->ptr = NULL;
	 free(tmp);
	 tmp = L->First; //pri uvolneni posledniho prvku se tmp nastavi na First, ktery se rovna NULL, tim se krasne ukonci cyklus a vse je uvolneno
 }

}

void InsertFirst (tList *L, int val) {
/*
** Vloží prvek s hodnotou val na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci Error().
**/
 
 tElemPtr tmp = malloc(sizeof(struct tElem));
 if(tmp == NULL) //pokud selze funkce malloc, tak vraci NULL
 {
 	Error();	 
 	return;
 }

 tmp->data = val;
 if(L->First != NULL) tmp->ptr = L->First; //pokud seznam neni prazdny, nalinkujou se prvky
 else tmp->ptr = NULL; //pokud je prazdny bude ukazatel z jedineho prvku NULL
 L->First = tmp; 

}

void First (tList *L) {
/*
** Nastaví aktivitu seznamu L na jeho první prvek.
** Funkci implementujte jako jediný příkaz, aniž byste testovali,
** zda je seznam L prázdný.
**/
	
 L->Act = L->First; //tohle je opravdu obvious

}

void CopyFirst (tList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci Error().
**/

 if(L->First != NULL) *val = L->First->data; //pokud je ukazatel na prvni prvek NULL, je seznam logicky prazdny
 else Error(); 

}

void DeleteFirst (tList *L) {
/*
** Zruší první prvek seznamu L a uvolní jím používanou paměť.
** Pokud byl rušený prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se neděje.
**/
 if(L->First != NULL)
 {
	 if(L->First == L->Act) L->Act = NULL; //asi by se to udelalo automaticky ale jistota je jistota
	 tElemPtr tmp = L->First;
	 L->First = tmp->ptr;
	 tmp->ptr = NULL;
	 free(tmp); //funkce free automaticky nastavuje tmp na NULL takze pohoda
 }	 

}	

void PostDelete (tList *L) {
/* 
** Zruší prvek seznamu L za aktivním prvkem a uvolní jím používanou paměť.
** Pokud není seznam L aktivní nebo pokud je aktivní poslední prvek seznamu L,
** nic se neděje.
**/

 if(Active(L))
 {
 	tElemPtr tmp = L->Act->ptr;
	if(tmp != NULL)
	{
		L->Act->ptr = tmp->ptr; //melo by fungovat i v pripade ze mazany prvek je posledni v seznamu, jednoduse se preda hodnota NULL
		tmp->ptr = NULL;
		free(tmp);
	}
 }	 
	
}

void PostInsert (tList *L, int val) {
/*
** Vloží prvek s hodnotou val za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje!
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** zavolá funkci Error().
**/
 
 if(Active(L))
 {
	 tElemPtr tmp = malloc(sizeof(struct tElem));
	 if(tmp == NULL)
	 {
		 Error();
		 return;
	 }

	 tmp->data = val;
	 tmp->ptr = L->Act->ptr; //opet by melo fungovat i na konci seznamu
	 L->Act->ptr = tmp;
 }	 
	
}

void Copy (tList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam není aktivní, zavolá funkci Error().
**/

 if(Active(L))
 {
	 *val = L->Act->data;
 }
 else Error(); 
	
}

void Actualize (tList *L, int val) {
/*
** Přepíše data aktivního prvku seznamu L hodnotou val.
** Pokud seznam L není aktivní, nedělá nic!
**/
	
 if(Active(L))
 {
 	L->Act->data = val;
 }	 

}

void Succ (tList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Všimněte si, že touto operací se může aktivní seznam stát neaktivním.
** Pokud není předaný seznam L aktivní, nedělá funkce nic.
**/
 
 if(Active(L))
 {
	 L->Act = L->Act->ptr;
 }
	
}

int Active (tList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Tuto funkci je vhodné implementovat jedním příkazem return. 
**/
 
 return (L->Act != NULL); //vraci 1 pro aktivni nebo 0 pro neaktivni	
	
}

/* Konec c201.c */
