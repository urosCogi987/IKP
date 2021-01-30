#include "DeterminantCalculator.h"
#include "List.h"
#include "Network.h"

int determinant(int** mat, int size, listStruct *workerList)
{
	if (size == 1)
	{
		// Opening new workers
		LPCWSTR mode = L"open";
		ShellExecute(NULL, mode, L"..\\Debug\\Worker.exe", NULL, NULL, SW_SHOWDEFAULT);

		while (true) {

			while (!IsListEmpty(workerList))
			{
				clientWorkerStruct* worker = RemoveFirstElement(workerList);

				if (worker == NULL)
				{
					printf("Worker is null!\n");
					continue;
				}

				Matrix m;
				int k = 0;
				m.data[0] = mat[0][0];
				m.order = 1;

				if (!SendMatrix(worker->clientSocket, m, sizeof(m))) {
					printf("Send failed when distributing the matrix on worker!\n");
					return 1;

				}


				int retVal = ReceiveValueFromWorker(worker->clientSocket);

				free(worker);

				return retVal;
			}
		}
	}
	if (size == 2) {

		// Opening new workers
		LPCWSTR mode = L"open";	
		ShellExecute(NULL, mode, L"..\\Debug\\Worker.exe", NULL, NULL, SW_SHOWDEFAULT);
	
		while (true) {

			while (!IsListEmpty(workerList))
			{
				clientWorkerStruct* worker = RemoveFirstElement(workerList);

				if (worker == NULL)
				{
					printf("Worker is null!\n");
					continue;
				}

				Matrix m;
				int k = 0;
				for (int i = 0; i < 2; i++) {
					for (int j = 0; j < 2; j++) {
						m.data[k] = mat[i][j];
						k++;
					}
				}
				m.order = 2;

				if (!SendMatrix(worker->clientSocket, m, sizeof(m))) {
					printf("Send failed when distributing the matrix on worker!\n");
					return 1;

				}


				int retVal = ReceiveValueFromWorker(worker->clientSocket);

				free(worker);

				return retVal;
			}
		}
	}


	int i;
	int sum = 0;
	for (i = 0; i < size; i++)
	{
		int** min = minore(mat, i, 0, size);
		sum += (pow(-1, i + 0))*mat[i][0] * determinant(min, size - 1, workerList);
	}

	return sum;
}

int** minore(int **mat, int i, int j, int dim) {
	int s;
	int t;
	int** min = (int**)calloc(dim - 1, sizeof(int*));
	for (s = 0; s < dim - 1; s++)
		min[s] = (int*)calloc(dim - 1, sizeof(int));
	int a = 0, b = 0;
	for (s = 0; s < dim; s++)
	{
		b = 0;
		for (t = 0; t < dim; t++)
		{
			if (!(s == i || t == j))
			{
				min[a][b] = mat[s][t];
				b++;
			}
		}
		if (!(s == i || t == j))
			a++;
	}

	return min;
}

void stampa_matrice(int **mat, int dim)
{
	int i, j;
	for (i = 0; i < dim; i++)
	{
		for (j = 0; j < dim; j++) {
			printf("%d ", mat[i][j]);
		}
		printf("\n");
	}
	printf("\n\n");

}
