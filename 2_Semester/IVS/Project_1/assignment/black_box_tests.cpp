//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Red-Black Tree - public interface tests
//
// $NoKeywords: $ivs_project_1 $black_box_tests.cpp
// $Author:     Michal Pysik <xpysik00@stud.fit.vutbr.cz>
// $Date:       $2020-02-13
//============================================================================//
/**
 * @file black_box_tests.cpp
 * @author Michal Pysik
 * 
 * @brief Implementace testu binarniho stromu.
 */

#include <vector>

#include "gtest/gtest.h"

#include "red_black_tree.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy Red-Black Tree, testujte nasledujici:
// 1. Verejne rozhrani stromu
//    - InsertNode/DeleteNode a FindNode
//    - Chovani techto metod testuje pro prazdny i neprazdny strom.
// 2. Axiomy (tedy vzdy platne vlastnosti) Red-Black Tree:
//    - Vsechny listove uzly stromu jsou *VZDY* cerne.
//    - Kazdy cerveny uzel muze mit *POUZE* cerne potomky.
//    - Vsechny cesty od kazdeho listoveho uzlu ke koreni stromu obsahuji
//      *STEJNY* pocet cernych uzlu.
//===============================================BTFIND=============================//

class NonEmptyTree : public ::testing::Test
{
protected:
	BinaryTree *Plny; //zde volim cesky nazev protoze mi to usnadni rozeznat tridu od instance teto tridy, to stejne v EmptyTree a AxiomTree

	virtual void SetUp()
	{
		int NumOfNodes = 16;
		int Nodes[] = {1, 4, 9, 69, 56, 33, 23, 90, 63, 70, 32, 7, 83, 43, 31, 8};
		
		Plny = new BinaryTree;

		for(int i=0; i<NumOfNodes; ++i)
		{				
			Plny->InsertNode(Nodes[i]);
		}
	}

	virtual void TearDown()
	{
		delete Plny;
	}
};


class EmptyTree : public ::testing::Test
{
protected:
	BinaryTree *Prazdny;

	virtual void SetUp()
	{
		Prazdny = new BinaryTree;
	}

	virtual void TearDown()
	{
		delete Prazdny;
	}

};


class TreeAxioms : public ::testing::Test
{
protected:
	BinaryTree *Axiomy;
	virtual void SetUp()
	{
		int NumOfNodes = 15;
		int Nodes[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; //1+2+4+8=15, takze strom by mel byt vyvazeny
		
		Axiomy = new BinaryTree;

		for(int i=0; i<NumOfNodes; ++i)
		{				
			Axiomy->InsertNode(Nodes[i]);
		}
	}

	virtual void TearDown()
	{
		delete Axiomy;
	}
};



//Testy funkce InsertNode na prazdnem a neprazdnem stromu

TEST_F(EmptyTree, InsertNode)
{
	ASSERT_TRUE(Prazdny->GetRoot() == NULL);
	//vlozime node 4, ktery by se mel stat korenem stromu
	EXPECT_FALSE(Prazdny->FindNode(4));
	std::pair <bool, BinaryTree::Node_t*> temp = Prazdny->InsertNode(4);
	EXPECT_TRUE(temp.first);
	EXPECT_EQ(Prazdny->GetRoot(), temp.second);
	EXPECT_TRUE(Prazdny->FindNode(4));
	EXPECT_EQ(Prazdny->FindNode(4), temp.second);
	//mame koren, ale zkusme vlozit dalsi node, navic s mensi hodnotou
	temp = Prazdny->InsertNode(1);
	EXPECT_TRUE(temp.first);
	EXPECT_TRUE(Prazdny->GetRoot() != temp.second); //kontrola ze zustal zachovan puvodni koren
	EXPECT_TRUE(Prazdny->FindNode(1));
	//Zkusime vlozit oba nody podruhe
	temp = Prazdny->InsertNode(1);
	EXPECT_FALSE(temp.first);
	temp = Prazdny->InsertNode(4);
	EXPECT_FALSE(temp.first);
	EXPECT_EQ(Prazdny->GetRoot(), temp.second); //test zda jsme dostali ukazatel na jiz obsazeny prvek (nyni dokonce koren)
}


TEST_F(NonEmptyTree, InsertNode)
{
	ASSERT_FALSE(Plny->GetRoot() == NULL);
	//vlozeni jiz existujiciho nodu
	std::pair <bool, BinaryTree::Node_t*> temp = Plny->InsertNode(32); // prvek 32 jiz ve stromu je
	EXPECT_FALSE(temp.first);
	EXPECT_EQ(Plny->FindNode(32), temp.second);
	//vlozeni noveho nodu
	temp = Plny->InsertNode(420);
	EXPECT_TRUE(temp.first);
	EXPECT_FALSE(NULL == temp.second);
	EXPECT_EQ(Plny->FindNode(420), temp.second);
}


//Testy funkce DeleteNode na prazdnem i neprazdnem stromu

TEST_F(EmptyTree, DeleteNode)
{
	EXPECT_TRUE(Prazdny->GetRoot() == NULL); //prazdny strom by nemel mit ani koren
	ASSERT_FALSE(Prazdny->DeleteNode(1));
	Prazdny->InsertNode(1); //vlozime uzel, nyni by strom uz mel najit koren
	EXPECT_TRUE(Prazdny->GetRoot() != NULL);
	EXPECT_TRUE(Prazdny->DeleteNode(1));
	EXPECT_TRUE(Prazdny->GetRoot() == NULL); //koren by mel zaniknout, navrat do puvodniho stavu
}

TEST_F(NonEmptyTree, DeleteNode)
{
	EXPECT_FALSE(Plny->DeleteNode(1337));
	EXPECT_TRUE(Plny->FindNode(56) != NULL);
	EXPECT_TRUE(Plny->DeleteNode(56)); //po odstraneni by prvek s ID 56 nemel byt k nalezeni
	EXPECT_TRUE(Plny->FindNode(56) == NULL);
	EXPECT_FALSE(Plny->DeleteNode(56)); //jiz odstraneny prvek
}

//Testy funkce FindNode na prazdnem i neprazdnem stromu

TEST_F(EmptyTree, FindNode)
{
	EXPECT_TRUE(Prazdny->FindNode(9) == NULL);
	ASSERT_TRUE(Prazdny->FindNode(2) == NULL);
	Prazdny->InsertNode(2);
	EXPECT_TRUE(Prazdny->FindNode(9) == NULL);
	ASSERT_TRUE(Prazdny->FindNode(2) != NULL); //nyni by mel byt node k nalezeni a navic byt korenem
	EXPECT_EQ(Prazdny->GetRoot(), Prazdny->FindNode(2)); //fce FindNode by zde mela vratit stejny ukazatel jako fce GetRoot
	Prazdny->InsertNode(9);
	EXPECT_TRUE(Prazdny->FindNode(9) != NULL);
	EXPECT_FALSE(Prazdny->GetRoot() == Prazdny->FindNode(9));
}

TEST_F(NonEmptyTree, FindNode)
{
	ASSERT_TRUE(Plny->FindNode(0) == NULL);
	ASSERT_TRUE(Plny->FindNode(69) != NULL); //prvek byl predem ve stromu
	Plny->DeleteNode(69);
	EXPECT_TRUE(Plny->FindNode(69) == NULL); //po odebrani by jiz nemel jit najit
	Plny->InsertNode(0);
	EXPECT_FALSE(Plny->FindNode(0) == NULL); //tento prvek byl vlozen a naopak by nyni mel byt k nalezeni
}

//Test prvniho axiomu

TEST_F(TreeAxioms, Axiom1)
{
	std::vector<BinaryTree::Node_t*> listy;
	Axiomy->GetLeafNodes(listy); //nacteme listy do pole pomoci vhodne funkce

	for(int i = 0; i < listy.size(); ++i)
	{
		ASSERT_TRUE(listy[i]->color == 1); //Cerna barva ma cislo 1
		EXPECT_FALSE(listy[i] == NULL);
	}
	
}

//Test druheho axiomu

TEST_F(TreeAxioms, Axiom2)
{
	std::vector<BinaryTree::Node_t*> uzly; 
	Axiomy->GetNonLeafNodes(uzly);	//nacteme vsechny uzly (krome listu, ty nas zde nezajimaji) do jednoho pole
	
	for(int i = 0; i < uzly.size(); ++i)
	{
		EXPECT_FALSE(uzly[i] == NULL);
		if(uzly[i]->color == 0) //pokud je uzel cerveny...
		{
			ASSERT_TRUE(uzly[i]->pLeft->color == 1); //...tak musi levy i pravy potomek byt cerny
			ASSERT_TRUE(uzly[i]->pRight->color == 1);
		}
	}
	
}

//Test tretiho axiomu

TEST_F(TreeAxioms, Axiom3)
{
	
	std::vector<BinaryTree::Node_t*> listy;
	Axiomy->GetLeafNodes(listy);
	BinaryTree::Node_t *temp;
	int cerne, mincerne;
	
	for(int i = 0; i < listy.size(); ++i)
	{
		ASSERT_FALSE(listy[i] == NULL);
		temp = listy[i]; //nacte vsechny uzly do pole
		ASSERT_FALSE(temp == NULL);

		
		cerne = 0;
		
		while(temp != Axiomy->GetRoot()) //dokud nejsme u korene tak se posunujeme dal
		{
			EXPECT_FALSE(temp == NULL);
			if(temp->color == 1) cerne++;	//secte cerne uzly
			temp = temp->pParent;
		}
		
		if(i == 0) mincerne = cerne;

		EXPECT_EQ(mincerne, cerne); //srovnani poctu cernych s jejich prvnim poctem - hlavni cast tohoto testu
	}
}











/*** Konec souboru black_box_tests.cpp ***/
