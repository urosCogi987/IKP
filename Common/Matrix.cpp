#include "Matrix.h"
#include <time.h>

Matrix EnterMatrixByHand(int order)
{	
	Matrix m;
	m.order = order;		

	// unosenje elemenata u matricu
	printf("\nEnter the elements of the matrix one by one: \n");
	for (int i = 0; i < order * order; i++)
	{
		scanf_s("%d", &m.data[i]);
	}	

	return m;
}

Matrix EnterMatrixRandomly(int order)
{
	int randNum;
	Matrix m;	
	m.order = order;

	// Use current time as seed for random generator
	srand(time(0));

	// unosenje elemenata u matricu
	printf("\nElements will be randomly generated: \n");
	
	for (int i = 0; i < order * order; i++)
	{
		randNum = (rand() % (100 - 1 + 1) + 1);
		m.data[i] = randNum;
	}	

	return m;
}