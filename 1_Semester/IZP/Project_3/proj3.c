#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//Autor: Michal Pysik (xpysik00)


typedef struct {
	int rows;
	int cols;
	unsigned char *cells;
} Map;


//fce testfile testuje zda je soubor kompatibilni, je volana take automaticky pred kazdym pruchodem bludistem
int testfile(FILE *f)
{
	bool validate = false;	
	int a, b;
	
	if(fscanf(f, "%d %d ", &a, &b) == 2)
	{
		if(a > 0 && b > 0)
		{
			int soucin = a*b;
			validate = true;
			//recyklujeme promenne - do "a" nacitame znaky a "b" slouzi jako pocitadlo
			b = 0;

			while(fscanf(f, "%d", &a) == 1)
			{
				b++;
				if(a < 0 || a > 7)
				{
					validate = false;
				}
			}

			//pocet prvku pole se musi rovnat soucinu radku a sloupcu
			if(b != soucin)
			{
				validate = false;
			}
		}
	}
	
	rewind(f);

	if(validate)
	{
		return 0;
	}
	else
	{
		return 1;
	}
	
}


void argerr()
{
	fprintf(stderr, "Program spusten s neplatnymi argumenty, pro napovedu spustte programa s jedinym argumentem: --help\n");
	return;
}


void help()
{
	printf("-Pro napovedu spustte program s jedinym argumentem: --help\n\n"); 
	printf("-Pro otestovani souboru zadejte jako 1. argument: --test a jako 2. argument nazev souboru i s koncovkou .txt\n\n");
	printf("-Pro pruchod bludistem pravidlem prave/leve ruky zadejte: --rpath , nebo: --lpath jako 1. argument, cislo vstupniho radku jako 2. argument, a cislo vstupniho sloupce jako 3. argument a nazev souboru (s koncovkou .txt) jako 4. argument\n");
	return;
}





void initMap(Map *map)
{
	map->rows = 0;
	map->cols = 0;
	map->cells = NULL;
	
	return;

}


int loadMap (Map *map, char *filename)
{
	FILE *f;
	f = fopen(filename, "r");
	if(f == NULL)
	{
		fprintf(stderr, "soubor se nepodarilo otevrit\n");
		return 1;
	}
	
	if(testfile(f) == 1)
	{
		fprintf(stderr, "Vybrany soubor je ve spatnem formatu\n");
		 return 1;
	}
	
	fscanf(f, "%d %d\n", &map->rows, &map->cols);	
	
	int soucin = (map->rows)*(map->cols);

	map->cells = malloc(sizeof(unsigned char) * soucin);
	
	int tmp;	
	
	for(int i=0; i<soucin; i++)
	{
		fscanf(f, "%d", &tmp);
		map->cells[i] = tmp;
	}

	fclose(f);


	/* TEST PRVKU (DEBUGOVACI FCE)
	for(int i=0; i<soucin; i++)
	{	
		if(map->cells[i] < 8)
		{fprintf(stderr, "%d <--- %d prvek\n", map->cells[i], i+1); }
	}
	*/
	
	return 0;
	
}


void freedom(Map *map)
{
	free(map->cells);
	map->cells = NULL;
	free(map);
	map = NULL;
	return;
}


// border = 1 -> leva stena; border = 2 -> prava stena; border = 3 -> horni/dolni stena
bool isborder(Map *map, int r, int c, int border)
{

	unsigned char number = map->cells[((r-1) * map->cols) + c - 1];
	
	if(border == 1)
	{
		if(number % 2 == 1)  return true;
	}
	if(border == 2)
	{
		if(number == 2 || number == 3 || number == 6 || number == 7) return true;
	}
	if(border == 3)
	{
		if(number/4 >= 1) return true;
	}
	return false;

}


//fce vraci jekou ze 3 hran vstupniho trojuhelniku ma vyzkouset jako prvni, plati stejna symbolika sten jako ve fci isborder
int start_border(Map *map, int r, int c, int leftright)
{

	if(leftright == 01)
	{
		if(c == 1 && (r % 2 == 1)) return 2;
		else if(c == 1 && (r % 2 == 0)) return 3;
		else if(c == map->cols && (r+c) % 2 == 0) return 3;
		else if(c == map->cols && (r+c) % 2 == 1) return 1;
		else if(r == 1) return 1;
		else if(r == map->rows) return 2;
		
	}

	if(leftright == 10)
	{
		if(c == 1 && (r % 2 == 1)) return 3;
		else if(c == 1 && (r % 2 == 0)) return 2;
		else if(c == map->cols && (r+c) % 2 == 0) return 1;
		else if(c == map->cols && (r+c) % 2 == 1) return 3;
		else if(r == 1) return 2;
		else if(r == map->rows) return 1; 
	}
	
	return 0;
}






int main(int argc, char *argv[])
{

	if(argc == 2)
	{
		if(strcmp(argv[1], "--help") == 0)
		{
			help();
		}
		else
		{
			argerr();
			return 1;
		}
	}

	else if(argc == 3)
	{
		if(strcmp(argv[1], "--test") == 0)
		{
			
			FILE *f;
			f = fopen(argv[2], "r");
			if(f == NULL)
			{
				fprintf(stderr, "soubor se nepodarilo otevrit\n");
				return 1;
			}
			if(testfile(f) == 0)
			{
				printf("valid\n");
			}
			else
			{
				printf("invalid\n");
			}
			fclose(f);
		}
		else
		{
			argerr();
			return 1;
		}
	}

	else if(argc == 5)
	{
		if((strcmp(argv[1], "--lpath") == 0) || (strcmp(argv[1], "--rpath") == 0))
		{

			int radek = atoi(argv[2]);
			int sloupec = atoi(argv[3]);
			int ruka;
			
			// pozice jednicky v cislici hodnoty promenne ruka znaci smer pravidla (10 - leva, 01 - prava)
			if(strcmp(argv[1], "--lpath") == 0)
			{
				ruka = 10;
			}
			else
			{
				ruka = 01;
			}
							

			Map *bludiste;
			bludiste = (Map *)malloc(sizeof(Map ));
			//printf("pred initem\n");
			initMap(bludiste);
			//printf("po initu pred loadem\n");
			if(loadMap(bludiste, argv[4])==1)
			{
				return 1;
			}
			
			if((radek > bludiste->rows) || (sloupec > bludiste->cols))
			{
				fprintf(stderr, "Error: Presahli jste velikost zvoleneho bludiste\n");
				return 1;
			}

			

			int hranice = start_border(bludiste, radek, sloupec, ruka);
			
			if(hranice == 0)
			{
				fprintf(stderr, "Error: vybrany bod neni hranici bludiste\n");
				return 1;
			}
		
		//reseni pomoci prave ruky	
			if(ruka == 01)
			{
				while(radek > 0 && sloupec > 0 && (radek <= bludiste->rows) && (sloupec <= bludiste->cols))
				{
					if(isborder(bludiste, radek, sloupec, hranice))
					{
						if((radek+sloupec) % 2 == 1)
						{	
							hranice--;
							if(hranice < 1) hranice = 3; 
						}
						else
						{
							hranice++;
							if(hranice > 3) hranice = 1;
						}
					}
					else
					{
						if(hranice == 1)
						{
							printf("%d,%d\n", radek, sloupec);
							sloupec--;
							if((radek+sloupec) % 2 == 0) hranice = 3;					
						}
						else if(hranice == 2)
						{
							printf("%d,%d\n", radek, sloupec);
							sloupec++;
							if((radek+sloupec) % 2 == 1) hranice = 3;
						}
						else if(hranice == 3)
						{
							if((radek+sloupec) % 2 == 1)
							{
								printf("%d,%d\n", radek, sloupec);
								radek++;
								hranice = 1;
							}
							else
							{
								printf("%d,%d\n", radek, sloupec);
								radek--;
								hranice = 2;
							}
						}
					}
				} 
			}


		//reseni pomoci leve ruky
			if(ruka == 10)
			{
				while(radek > 0 && sloupec > 0 && (radek <= bludiste->rows) && (sloupec <= bludiste->cols))
				{
					if(isborder(bludiste, radek, sloupec, hranice))
					{
						if((radek+sloupec) % 2 == 1)
						{	
							hranice++;
							if(hranice > 3) hranice = 1;
						}
						else
						{
							hranice--;
							if(hranice < 1) hranice = 3;
						}
					}
					else
					{
						if(hranice == 1)
						{
							printf("%d,%d\n", radek, sloupec);
							sloupec--;
							if((radek+sloupec) % 2 == 1) hranice = 3;					
						}
						else if(hranice == 2)
						{
							printf("%d,%d\n", radek, sloupec);
							sloupec++;
							if((radek+sloupec) % 2 == 0) hranice = 3;
						}
						else if(hranice == 3)
						{
							if((radek+sloupec) % 2 == 1)
							{
								printf("%d,%d\n", radek, sloupec);
								radek++;
								hranice = 2;
							}
							else
							{
								printf("%d,%d\n", radek, sloupec);
								radek--;
								hranice = 1;
							}
						}
					}
				} 
			}
			
			
	
			freedom(bludiste);
			//printf("uvolneno\n");


			
			


		}
		else
		{
		argerr();
		return 1;
		}
	}
	
	else
	{
		argerr();
		return 1;
	}





	return 0;
}

