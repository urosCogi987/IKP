#pragma once
#include "Common.h"


int determinant(int** mat, int size, listStruct *list);
int** minore(int **mat, int i, int j, int dim);
void stampa_matrice(int **mat, int dim);