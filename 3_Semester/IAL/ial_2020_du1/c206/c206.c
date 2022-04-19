
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2020
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu,
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem,
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

// VYPRACOVAL: Michal Pysik (xpysik00)

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
	if(L == NULL) return; //testuju zda je ukazatel na seznam platny
	
	L->First = NULL; //seznam ma 0 prvku, vsechny ukazatele dame na NULL stejne jak u normal seznamu, akorat je tu extra ukazatel na posledni prvek
	L->Last = NULL;
	L->Act = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
	if(L == NULL) return;
	
	L->Last = NULL; // tyto 2 nebudu potrebovat, projdu to od zacatku do konce
	L->Act = NULL;

	tDLElemPtr tmp = L->First;

	while(tmp != NULL)
	{
		L->First = tmp->rptr;
		tmp->rptr = NULL;
		tmp->lptr = NULL; //nastavime oba pointery z prvku co uvolnujeme na NULL
		free(tmp);
		tmp = L->First; //L->First bude eventuelne NULL, tim pozname ze jsme na konci
	}	
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if(L == NULL) return; //v teto 3. uloze jsem si oblibil testovani zda seznam vubec existuje
	
	tDLElemPtr tmp = malloc(sizeof(struct tDLElem));	
	if(tmp == NULL)
	{
		DLError(); //pokud funkce alloc selze tak vrati NULL
		return;
	}	
	
	tmp->data = val;
	if(L->First == NULL)
	{
		L->First = L->Last = tmp; //pokud vkladame prvni prvek
		tmp->lptr = tmp->rptr = NULL;
	}
	else
	{
		L->First->lptr = tmp; //novy prvek dosadime pred prvni prvek seznamu
		tmp->lptr = NULL;
		tmp->rptr = L->First;
		L->First = tmp; //pote nastavime ukazatel prvniho prvku na tento novy prvek
	}
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
	if(L == NULL) return;

	tDLElemPtr tmp = malloc(sizeof(struct tDLElem));
	if(tmp == NULL)
	{
		DLError();
		return;
	}

	tmp->data = val;  //cela funkce je symetricka k te minule
	if(L->Last == NULL)
	{
		L->Last = L->First = tmp;
		tmp->lptr = tmp->rptr = NULL;
	}
	else
	{
		L->Last->rptr = tmp;
		tmp->rptr = NULL;
		tmp->lptr = L->Last;
		L->Last = tmp;
	}
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->First; //nebudu zde strkat kontrolu zda neni L == NULL, podle zadani to neni treba	
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->Last; //doslova to stejne jako posledni funkce, akorat pro Last
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if(L->First == NULL) DLError(); //slo by otestovat i ostatni ukazatele, ale neni to potreba, pokud First ukazuje na prvek tak seznam prazdny logicky neni
	else *val = L->First->data;
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if(L->Last == NULL) DLError(); //to stejne jak u First, tentokrat kontroluji jen Last a staci mi to
	else *val = L->Last->data;
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
	if(L->First == NULL) return; //kdyz je prazdny tak nic nedelam

	if(L->First == L->Act) L->Act = NULL; //pro jistotu nastavim Act na NULL pokud je prvni prvek aktivni, i kdyz by se to asi stalo automaticky

	tDLElemPtr tmp = L->First;
	if(tmp->rptr != NULL)
	{
		tmp->rptr->lptr = NULL; //pokud nebyl seznam jednoprvkovy, tak se postara o novy zacatek seznamu
		L->First = tmp->rptr;
	}
	else
	{
		L->First = NULL; //pokud byl prvek jediny tak vyNULLuje ukazatele
		L->Last = NULL; //v tomto pripade je ukazatel Act uz predem NULL, diky if statementu na zacatku teto fce
	}
	tmp->rptr = NULL;
	tmp->lptr = NULL; //nastavim ukazatele mazaneho prvku na NULL a uvolnim ho
	free(tmp);
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L.
** Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se neděje.
**/ 
	if(L->Last == NULL) return; //celou funkci resim symetricky k te minule, u testu prazdnosti seznamu je jedno zda testuju ukazatel First nebo Last (pokud je tedy vse spravne osetreno)

	if(L->Last == L->Act) L->Act = NULL;

	tDLElemPtr tmp = L->Last;
	if(tmp->lptr != NULL)
	{
		tmp->lptr->rptr = NULL;
		L->Last = tmp->lptr;
	}
	else
	{
		L->First = NULL; //opet pokud byl prvek jediny v seznamu
		L->Last = NULL;
	}
	tmp->rptr = NULL;
	tmp->lptr = NULL;
	free(tmp);
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
	if(L == NULL) return; //muj klasicky test

	if(!DLActive(L) || L->Act == L->Last) return; //test podminek ze zadani
	
	tDLElemPtr tmp = L->Act->rptr;
	if(tmp->rptr == NULL) L->Last = L->Act; //pokud je mazany prvek posledni, tak aktivni prvek dostane take roli posledniho prvku
	else tmp->rptr->lptr = L->Act; //tato akce se provede jen kdyz mazany prvek neni posledni
	L->Act->rptr = tmp->rptr; //i kdyby byla tato hodnota NULL tak se preda, neni treba specialni osetreni
	tmp->rptr = NULL;
	tmp->lptr = NULL; //vyNULLovani ukazatelu a uvolneni tmp
	free(tmp);
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
	if(L == NULL) return;

	if(!DLActive(L) || L->Act == L->First) return;

	tDLElemPtr tmp = L->Act->lptr; //celou funkci delam symetricky k te minule
	if(tmp->lptr == NULL) L->First = L->Act;
	else tmp->lptr->rptr = L->Act;
	L->Act->lptr = tmp->lptr;
	tmp->rptr = NULL;
	tmp->lptr = NULL;
	free(tmp);
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if(L == NULL) return; //test zda seznam existuje
	if(!DLActive(L)) return; //test zda je aktivni

	tDLElemPtr tmp = malloc(sizeof(struct tDLElem));
	if(tmp == NULL) //test zda byla alokace uspesna
	{
		DLError();
		return;
	}

	tmp->data = val;
	tmp->lptr = L->Act;
	tmp->rptr = L->Act->rptr; //kdyby byl Aktivni prvek posledni tak preda hodnotu NULL, pohoda
	if(L->Act->rptr != NULL) L->Act->rptr->lptr = tmp; //pokud vkladame novy prvek mezi 2 existujici, a ne na konec
	L->Act->rptr = tmp;
	if(L->Act == L->Last) L->Last = tmp; //pokud byl aktivni posledni
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if(L == NULL) return;
	if(!DLActive(L)) return;
				//cela funkce je symetricka s tou minulou

	tDLElemPtr tmp = malloc(sizeof(struct tDLElem));
	if(tmp == NULL)
	{
		DLError();
		return;
	}

	tmp->data = val;
	tmp->rptr = L->Act;
	tmp->lptr = L->Act->lptr;
	if(L->Act->lptr != NULL) L->Act->lptr->rptr = tmp;
	L->Act->lptr = tmp;
	if(L->Act == L->First) L->First = tmp;
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
	if(L == NULL) return;	

	if(DLActive(L)) //check zda je aktivni, kdyz ano vrati hodnotu skrze pointer *val
	{
		*val = L->Act->data;
	}
	else DLError();	
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
	
	if(L == NULL) return;

	if(DLActive(L)) //dela presny opak minule funkce
	{
		L->Act->data = val;
	}	
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	if(DLActive(L))
	{
		L->Act = L->Act->rptr; //pokud je rptr NULL, stane se seznam neaktivnim
	}	
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
	if(DLActive(L))
	{
		L->Act = L->Act->lptr; //symetricke k minule funkci
	}
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	return L->Act != NULL; //pokud je aktivni vraci 1, pokud neni tak 0 (coz by znamenalo ze ukazatel Act je NULL)
}

/* Konec c206.c*/
