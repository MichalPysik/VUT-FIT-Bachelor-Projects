#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define I_0 0.000000000001
#define U_T 0.0258563

//Autor: Michal Pysik (login: xpysik00)
//Ladici prikazy v kodu vzdy umyslne nechavam, jsou oznaceny jako debug

double diode(double U_0, double R, double eps)
{
	double U_p, U_p1, U_p2, I_p, I_R;

	U_p1 = 0;
	U_p2 = U_0;
	
	//metoda puleni intervalu pro vzpocet U_p
	do {
		//debug printf("delam dalsi pokus \n");
		
		U_p = (U_p1 + U_p2)/2;
		
		
		I_R = (U_0 - U_p)/R;
		I_p = I_0 * (exp(U_p/U_T) - 1);
		
		//podminka pro velice male epsilon
		if(U_p <= U_p1 || U_p >= U_p2)
		{
		  return U_p;
		}

		if((I_p - I_R) > 0)
		{
			U_p2 = U_p;
		}
		else
		{
			U_p1 = U_p;
		}

	} while(fabs(I_p - I_R) > eps);
	
	return U_p;
}






int main(int argc, char *argv[])
{
	if(argc != 4)
	{
		fprintf(stderr, "error: please enter exactly 3 arguments\n");
		return 1;
	}

	double U_0, R, eps, I_p, U_p;
	
	//prevod argumentu na typ double
	U_0 = atof(argv[1]);
	R = atof(argv[2]);	
	eps = atof(argv[3]);
	
	//podminky spravne zadanych argumentu
	if(eps <= 0 || R <= 0 || U_0 <= 0)
	{
		fprintf(stderr, "error: invalid arguments\n");
		return 1;
	}
	//debug printf("%g  %g  %g \n", U_0, R, eps);

	
	U_p = diode(U_0, R, eps);
	I_p = I_0 * (exp(U_p/U_T) - 1);

	
	
	printf("Up = %g V\nIp = %g A\n", U_p, I_p);
	





	return 0;
}
