//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     White Box - Tests suite
//
// $NoKeywords: $ivs_project_1 $white_box_code.cpp
// $Author:     Michal Pysik <xpysik00@stud.fit.vutbr.cz>
// $Date:       $2020-02-25
//============================================================================//
/**
 * @file white_box_tests.cpp
 * @author Michal Pysik
 * 
 * @brief Implementace testu prace s maticemi.
 */

#include "gtest/gtest.h"
#include "white_box_code.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy operaci nad maticemi. Cilem testovani je:
// 1. Dosahnout maximalniho pokryti kodu (white_box_code.cpp) testy.
// 2. Overit spravne chovani operaci nad maticemi v zavislosti na rozmerech 
//    matic.
//============================================================================//


class Matrix_1x1 : public ::testing::Test //nejmensi mozna matice (1x1 = 1 prvek)
{
	protected:
	Matrix *m1x1 = new Matrix();
};


class Matrix_5x5 : public ::testing::Test //pomerne velka matice (5x5 = 25 prvku)
{
	protected:
	Matrix *m5x5 = new Matrix(5, 5);
};

//Testy konstruktoru

TEST_F(Matrix_1x1, constructor)
{
	ASSERT_FALSE(m1x1 == NULL);
	EXPECT_TRUE(m1x1->get(0,0) == 0); //jediny prvek v matici ma mit hodnotu 0
	EXPECT_ANY_THROW(m1x1->get(1,1));
}

TEST_F(Matrix_5x5, constructor)
{
	ASSERT_FALSE(m5x5 == NULL);
	
	for(int i=0; i<5; ++i)
	{
		for(int j=0; j<5; ++j)
		{
			EXPECT_NO_THROW(m5x5->get(i,j)); //test ze zadny prvek zahadne nechybi
			EXPECT_TRUE(m5x5->get(i,j) == 0);
		}
	}
}


//Testy fce set => vkladani hodnot do matic (do male matice pomoci jednoho prvku a do vetsi pomoci 2D pole)
TEST_F(Matrix_1x1, set)
{
	ASSERT_FALSE(m1x1->set(0, 1, 3));
	ASSERT_FALSE(m1x1->set(1, 0, 4));
	ASSERT_TRUE(m1x1->set(0, 0, 3)); //vlozime cislo 3 jako jediny prvek
	
	EXPECT_EQ(m1x1->get(0,0), 3);
	
}

TEST_F(Matrix_5x5, set)
{
	std::vector<std::vector<double>> valuesFake{ { 1.2, 1.3},
						     { 0.2, 5.5, 0.99 } };
	
	EXPECT_FALSE(m5x5->set(valuesFake));	
	

	std::vector<std::vector<double>> values{ { 1.0, 2.0, 3.0, 4.0, 5.0 },  //2D pole ktere vkladame do matice
						{ 6.0, 7.0, 8.0, 9.0, 10.0 },
						{ 11.0, 12.0, 13.0, 14.0, 15.0 },
						{ 16.0, 17.0, 18.0, 19.0, 20.0 },
						{ 21.0, 22.0, 23.0, 24.0, 25.0 } };
						
	EXPECT_TRUE(m5x5->set(values));

	double cislo = 1.0;	
	
	for(int x=0; x < 5; ++x)
	{
		for(int y=0; y < 5; ++y)
		{
			EXPECT_EQ(m5x5->get(x, y), cislo);
			//std::cout << m5x5->get(x, y) << "\n"; //DEBUG
			cislo += 1.0;
		}
	}
}


//Testy fce get
TEST_F(Matrix_1x1, get)
{
	m1x1->set(0, 0, 9);
	EXPECT_NO_THROW(m1x1->get(0,0));
	EXPECT_EQ(m1x1->get(0,0), 9);
	EXPECT_NE(m1x1->get(0,0), 0);
	EXPECT_GT(m1x1->get(0,0), 8.9);
	
	EXPECT_ANY_THROW(m1x1->get(0,1));
	EXPECT_ANY_THROW(m1x1->get(89, 0));
}

TEST_F(Matrix_5x5, get)
{
	std::vector<std::vector<double>> values{ { 1.0, 2.0, 3.0, 4.0, 5.0 },  //vse jsem zkopiroval z testu set a jen pridal vice EXPECT(...) testu
						{ 6.0, 7.0, 8.0, 9.0, 10.0 },
						{ 11.0, 12.0, 13.0, 14.0, 15.0 },
						{ 16.0, 17.0, 18.0, 19.0, 20.0 },
						{ 21.0, 22.0, 23.0, 24.0, 25.0 } };
						
	ASSERT_TRUE(m5x5->set(values)); //ASSERT protoze bez splneni teto podminky nema smysl pokracovat

	double cislo = 1.0;	
	
	for(int x=0; x < 5; ++x)
	{
		for(int y=0; y < 5; ++y)
		{
			EXPECT_EQ(m5x5->get(x, y), cislo);

		
			EXPECT_NE(m5x5->get(x, y), 0);
			EXPECT_NO_THROW(m5x5->get(x, y));
			EXPECT_ANY_THROW(m5x5->get(x+5, y));
			EXPECT_ANY_THROW(m5x5->get(x, y+5));
			

			cislo += 1.0;
		}
	}
}

//Testy funkce operator== (ekvivalence)
TEST_F(Matrix_1x1, Equality)
{
	Matrix prazdna;
	EXPECT_TRUE(m1x1->operator==(prazdna));
	
	m1x1->set(0, 0, 2);
	EXPECT_FALSE(m1x1->operator==(prazdna));

	prazdna.set(0, 0, 2);
	EXPECT_TRUE(m1x1->operator==(prazdna));
}

TEST_F(Matrix_5x5, Equality)
{
	Matrix tempM(5, 5);
	EXPECT_TRUE(m5x5->operator==(tempM));

	std::vector<std::vector< double >> hodnoty13(5, std::vector<double>(5, 13.0)); //obe matice 5x5 zaplnime pouze hodnotou 13.0
	
	ASSERT_TRUE(m5x5->set(hodnoty13));
	ASSERT_TRUE(tempM.set(hodnoty13));
	EXPECT_TRUE(m5x5->operator==(tempM));

	tempM.set(4, 3, 8.0); //pozmenime jednu hodnotu v docasne matici
	EXPECT_FALSE(m5x5->operator==(tempM));

	ASSERT_TRUE(m5x5->set(4, 3, 8.0)); //stejnou hodnotu na stejne nastavime i v nove matici
	EXPECT_TRUE(m5x5->operator==(tempM));
}


//Testy scitani matic
TEST_F(Matrix_1x1, Addition)
{
	Matrix tempM; // pomocna matice
	Matrix tempM2; // zde bude vysledek ... stejneho schematu se drzim v nasledujicich testech
	ASSERT_TRUE(m1x1->operator==(tempM));
	ASSERT_TRUE(m1x1->operator==(tempM2));

	m1x1->set(0, 0, 1);
	tempM.set(0, 0, 4);

	EXPECT_NO_THROW(tempM2 = m1x1->operator+(tempM));
	
	EXPECT_EQ(tempM2.get(0, 0), (m1x1->get(0, 0)) + (tempM.get(0, 0)));
	//std::cout << tempM.get(0, 0) << "\n"; //DEBUG
}


TEST_F(Matrix_5x5, Addition)
{
	Matrix tempM(5, 5);
	Matrix tempM2(5, 5);
	ASSERT_TRUE(m5x5->operator==(tempM));
	ASSERT_TRUE(m5x5->operator==(tempM2));

	std::vector<std::vector<double>> values{ { 1.0, 2.0, 3.0, 4.0, 5.0 },  //opet zkopirovane, budeme scitat dve stejne matice
						{ 6.0, 7.0, 8.0, 9.0, 10.0 },
						{ 11.0, 12.0, 13.0, 14.0, 15.0 },
						{ 16.0, 17.0, 18.0, 19.0, 20.0 },
						{ 21.0, 22.0, 23.0, 24.0, 25.0 } };
						
	ASSERT_TRUE(m5x5->set(values));
	ASSERT_TRUE(tempM.set(values));
	
	EXPECT_NO_THROW(tempM2 = m5x5->operator+(tempM));
	
	
	for(int x=0; x < 5; ++x)
	{
		for(int y=0; y < 5; ++y)
		{
			EXPECT_EQ(tempM2.get(x, y), (m5x5->get(x, y)) + (tempM.get(x, y))); //konkretni prvek ve vysledne matici je soucet prvku v obou maticich na techto souradnicich
		}
	}	
}

//Testy nasobeni matic
TEST_F(Matrix_1x1, Multiplication_by_matrix)
{
	Matrix tempM;
	Matrix tempM2;
	ASSERT_TRUE(m1x1->operator==(tempM));
	ASSERT_TRUE(m1x1->operator==(tempM2));

	m1x1->set(0, 0, 9);
	tempM.set(0, 0, 5);

	EXPECT_NO_THROW(tempM2 = m1x1->operator*(tempM));
	
	EXPECT_EQ(tempM2.get(0, 0), (m1x1->get(0, 0)) * (tempM.get(0, 0)));
}

TEST_F(Matrix_5x5, Multiplication_by_matrix)
{
	Matrix tempM(5, 5);
	Matrix tempM2(5, 5);

	std::vector<std::vector<double>> values{ { 1.0, 2.0, 3.0, 4.0, 5.0 },
						{ 6.0, 7.0, 8.0, 9.0, 10.0 },
						{ 11.0, 12.0, 13.0, 14.0, 15.0 },
						{ 16.0, 17.0, 18.0, 19.0, 20.0 },
						{ 21.0, 22.0, 23.0, 24.0, 25.0 } };

	ASSERT_TRUE(m5x5->set(values));
	ASSERT_TRUE(tempM.set(values));

	double vysledek, x2, y2;
	vysledek = x2 = y2 = 0.0;

	EXPECT_NO_THROW(tempM2 = m5x5->operator*(tempM));
	
	for(int x=0; x < 5; ++x)
	{
		for(int y=0; y < 5; ++y) //vzhledem ke strukture 2D pole pomoci std::vector jsou zde osy prohozene oproti tradici (x je zde vertikalni a y horizontalni)
		{
			while(x2 < 5 && y2 < 5)
			{
				vysledek += ((m5x5->get(x, y2)) * (tempM.get(x2, y)));
				++x2;
				++y2;
			}
			EXPECT_EQ(tempM2.get(x, y), vysledek); //Hlavni test
			
			//std::cout << vysledek << " je vysledek a hodnota v matici je " << tempM2.get(x, y) << "\n"; //DEBUG
			vysledek = x2 = y2 = 0.0;
			
		}
	}	
}

//Testy nasobeni skalarem
TEST_F(Matrix_1x1, Multiplication_by_scalar)
{
	Matrix tempM; //tentokrat bude vysledek v tempM a ne tempM2
	ASSERT_TRUE(m1x1->operator==(tempM));

	const double scalar1 = 13.37;
	const double scalar2 = 42.0;

	m1x1->set(0, 0, 6.9);

	EXPECT_NO_THROW(tempM = m1x1->operator*(scalar1)); // vynasobime prvnim skalarem
	
	EXPECT_EQ(tempM.get(0, 0), (m1x1->get(0, 0)) * scalar1);

	EXPECT_NO_THROW(tempM = tempM.operator*(scalar2)); // ulozeny vysledek zkusime vynasobit jeste druhym skalarem
	
	EXPECT_EQ(tempM.get(0, 0), (m1x1->get(0, 0)) * scalar1 * scalar2);
}

TEST_F(Matrix_5x5, Multiplication_by_scalar)
{
	Matrix tempM(5, 5);
	ASSERT_TRUE(m5x5->operator==(tempM));

	const double scalar = 4.42;

	std::vector<std::vector<double>> values{ { 1.0, 2.0, 3.0, 4.0, 5.0 },  //opet zkopirovane, budeme scitat dve stejne matice
						{ 6.0, 7.0, 8.0, 9.0, 10.0 },
						{ 11.0, 12.0, 13.0, 14.0, 15.0 },
						{ 16.0, 17.0, 18.0, 19.0, 20.0 },
						{ 21.0, 22.0, 23.0, 24.0, 25.0 } };
						
	ASSERT_TRUE(m5x5->set(values));
	
	EXPECT_NO_THROW(tempM = m5x5->operator*(scalar));
	
	
	for(int x=0; x < 5; ++x)
	{
		for(int y=0; y < 5; ++y)
		{
			EXPECT_EQ(tempM.get(x, y), m5x5->get(x, y) * scalar);
		}
	}	
}


//Testy reseni rovnic
TEST_F(Matrix_1x1, solveEquation)
{
	std::vector<double> vysledek;

	m1x1->set(0, 0, 5);

	std::vector<double> rightS = { 10 };
	
	EXPECT_NO_THROW(vysledek = m1x1->solveEquation(rightS));
	
	EXPECT_EQ(vysledek[0], rightS[0]/(m1x1->get(0, 0)));
	//std::cout << "Realny vysledek je " << rightS[0]/(m1x1->get(0, 0)) << "\n"; //DEBUG
}

TEST_F(Matrix_5x5, solveEquation)
{
	std::vector<double> vysledek; // Tuto rovnici jsem nasel na strance chegg.com a vypocital pomoci maticove kalkulacky na strankymatrixcalc.org
	std::vector<double> rightS = { 14, 0, 8, 5, 0 };

	std::vector<double> spravnyVys = { 5, -12, 1, 7, -5 };


	std::vector<std::vector<double>> values{ { 6, 2, 1, 1, 0 },
						{ 5, 4, 1, 1, -3 },
						{ 2, 2, 3, 2, -1 },
						{ 1, 1, 1, 3, 2 },
						{ 3, -1, 5, -1, 5 } };
						
	ASSERT_TRUE(m5x5->set(values));
	
	EXPECT_NO_THROW(vysledek = m5x5->solveEquation(rightS));

	EXPECT_EQ(vysledek, spravnyVys);
}


/*** Konec souboru white_box_tests.cpp ***/
