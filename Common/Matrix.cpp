#include "Matrix.h"
#include <time.h>

Matrix* EnterMatrixByHand(int order) 
{
	int i;	

	// alokacija memorije za matricu/povratnu vrednost
	Matrix* m = (Matrix*)malloc(sizeof(Matrix));
	m->order = order;
	m->data = (int*)malloc(sizeof(double)*order*order);

	// unosenje elemenata u matricu
	printf("\nEnter the elements of the matrix one by one: \n");
	for (int i = 0; i < order * order; i++)
	{
		scanf_s("%d", &m->data[i]);
	}			

	printf("*******succ*********\n");

	return m;
}

Matrix* EnterMatrixRandomly(int order)
{	
	int i, randNum;

	// alokacija memorije za matricu/povratnu vrednost
	Matrix* m = (Matrix*)malloc(sizeof(Matrix));
	m->order = order;
	m->data = (int*)malloc(sizeof(double)*order*order);

	// Use current time as seed for random generator
	srand(time(0));		

	// unosenje elemenata u matricu
	printf("\nElements will be randomly generated: \n");
	
	printf("\nEnter the elements of the matrix one by one: \n");
	for (int i = 0; i < order * order; i++)
	{
		randNum = (rand() % (100 - 1 + 1) + 1);
		m->data[i] = randNum;		
	}

	printf("*******succ*********\n");

	return m;
}