
/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                              Radek Hranický, 2014-2018
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

// Vypracoval: Michal Pysik (xpysik00)

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void htInit ( tHTable* ptrht ) {

	if(ptrht == NULL) return; //tabulka neexistuje

	for(int i = 0; i < HTSIZE; i++) //vsechny prvky v tabulce nastavim na NULL
	{
		(*ptrht)[i] = NULL;
	}

}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není,
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {

	if(ptrht == NULL) return NULL; //tabulka neexistuje

	int hash = hashCode(key); //vypocitam hash daneho klice

	tHTItem *tmp = (*ptrht)[hash];

	while(tmp != NULL)
	{
		if(strcmp(key, tmp->key) == 0) return tmp; //pokud najde vrati ukazatel
		else tmp = tmp->ptrnext;
	}
	return NULL; //kdyz ne tak NULL


}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {

	if(ptrht == NULL) return; //tabulka neexistuje

	int hash = hashCode(key); //vypocitam hash daneho klice


	tHTItem *tmp = (*ptrht)[hash];
	while (tmp != NULL) //dokud neni prvek NULL (kdyz vkladam prvni tak se cyklus preskoci)
	{
		if(strcmp(key, tmp->key) == 0) //pokud nasel prvek se stejnym klicem, prepise jen data
		{
			tmp->data = data;
			return;
		}
		tmp = tmp->ptrnext;
	}
	tmp = malloc(sizeof(tHTItem));  //pokud nenasel, vlozi novy prvek na zacatek seznamu
	tmp->data = data;
	tmp->key = malloc((strlen(key)+1)*sizeof(char));
	strcpy(tmp->key, key);
	tmp->ptrnext = (*ptrht)[hash];
	(*ptrht)[hash] = tmp;

}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {

	tHTItem *tmp = htSearch(ptrht,key);

	if(tmp != NULL) return &(tmp->data);
	else return NULL;
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {

	if(ptrht == NULL) return; //tabulka neexistuje

	int hash = hashCode(key); //vypocitam hash daneho klice

	tHTItem *tmp1, *tmp2;
	tmp1 = (*ptrht)[hash];
	int cnt = 0;

	while(tmp1 != NULL) //dokud nenarazi na konec seznamu (nebo je prazdny)
	{
		if(strcmp(key, tmp1->key) == 0) //nasel prvek s klicem
		{
			if (tmp1->ptrnext != NULL) //mazani prvku s naslednikem (ve skutecnosti prebere vsechny data a ukazatele naslednika, a toho smaze)
			{
				tmp2 = tmp1->ptrnext;
				tmp1->ptrnext = tmp2->ptrnext;
				tmp1->data = tmp2->data;
				free(tmp1->key);
				tmp1->key = tmp2->key;
				tmp2->key = NULL;
				free(tmp2); //uvolnime do neznama nasledujici nevinny prvek, pote co jsme ho okradli o vsechna data, zivot je nefer
				return;
			}
			else //mazany prvek nema potomka
			{
				free(tmp1->key);
				free(tmp1);
				tmp1 = NULL;
				if (cnt == 0) (*ptrht)[hash] = NULL; //pokud to byl jediny prvek s danym hashem, je seznam nyni prazdny
				else tmp2->ptrnext = NULL;
				return;
			}

		}
		tmp2 = tmp1; //uchovame prvek, hodi se pri mazani prvku bez potomka
		tmp1 = tmp1->ptrnext;
		cnt++;
	}

}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht ) {

	if (ptrht == NULL) return; //kdyz tabulka neexistuje

	tHTItem *tmp1, *tmp2; //pomocne pointery

	for (int i = 0; i < HTSIZE; i++)
	{
		tmp1 = (*ptrht)[i];
		(*ptrht)[i] = NULL;

		while(tmp1 != NULL)
		{
			tmp2 = tmp1->ptrnext;
			tmp1->ptrnext = NULL;
			free(tmp1->key);
			free(tmp1);
			tmp1 = tmp2;
		}
	}
}
