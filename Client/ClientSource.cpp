#include "../Common/Network.h"
#include "../Common/Matrix.h"

int main()
{		
	SOCKET connectSocket = INVALID_SOCKET;	// Socket used to communicate with server
//	int iResult;							// Variable used to store function return value	
//	char dataBuffer[BUFFER_SIZE];			// Buffer we will use to store message
	char address[10] = "127.0.0.1";			// server IP addresss


	if (!InitializeAndConnect(address, &connectSocket, SERVER_PORT))
	{
		// Error, shutting down client connection
		shutdownClientConnection(&connectSocket);
		printf("Client side connection failed: %ld\n", WSAGetLastError());
		return 1;
	}
	
	int answer = 0;
	int order = 0;

	while (true) 
	{
		printf("(1) Input matrix values by hand\n(2) Input randomly generated matrix values\n(3) Exit\nChoose: ");
		scanf_s("%d", &answer);	

		// Treba zastita unosa za ovo i za sve ostalo u sustini
		if (answer == 1) 
		{
			printf("Enter the order of the matrix: \n");
			scanf_s("%d", &order);

			Matrix mByHand;
			mByHand = EnterMatrixByHand(order);			

			printf("\nMatrix you entered: \n");
			for (int i = 0; i < order * order; i++)		// I  i=1	
			{
				if ((i != 0) &&(i % order == 0))
					printf("\n");
				printf("%d\t", mByHand.data[i]);
			}

			printf("\n\n");

			if (!SendMatrix(connectSocket, mByHand, sizeof(mByHand)))
			{
				break;
			}						
		}
		else if (answer == 2)
		{
			printf("Enter the order of the matrix: \n");
			scanf_s("%d", &order);

			Matrix mRandomly;
			mRandomly = EnterMatrixRandomly(order);			


			for (int i = 0; i < order * order; i++)
			{
				if ((i != 0) && (i % order == 0))
					printf("\n");				
				printf("%d\t", mRandomly.data[i]);
			}

			printf("\n\n");
			
			if (!SendMatrix(connectSocket, mRandomly, sizeof(mRandomly)))
			{
				break;
			}			
		}		
		else if (answer == 3)
		{
			break;
		}
		else
		{
			printf("input controll\n");
		}		
	}	

	shutdownClientConnection(&connectSocket);

	return 0;
}