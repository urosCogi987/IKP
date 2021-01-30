#include "WorkerHelper.h"

#include <process.h>


// Listening on client connectinos and messages
DWORD WINAPI ClientReceiver(LPVOID lpParam)
{
	paramsClientRecv* params = (paramsClientRecv*)lpParam;		
	SOCKET* arrayOfClientSocks = params->clientSockets;

	listStruct* clientWorkerList = params->clientWorkerList;
	bool* shutDown = params->shutDown;

	Matrix* matrica;

	char dataBuffer[BUFFER_SIZE];			// Buffer used for storing incoming data
	SOCKET listenSocket = INVALID_SOCKET;	// Socket used for listening for new clients 	
	int iResult;							// Multipurpose var
	int lastIndex = 0;						// Index of last connected client
	unsigned short port = 27016;
	int numOfThreads;

	int workerCounter = 0;

	DWORD funId;
	HANDLE handle;

	// set of socket descriptors
	fd_set readfds;

	// timeout for select function
	timeval timeVal;
	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;

	if (!InitializeAndListen(&listenSocket, port))
	{
		return 1;
	}

	Sleep(1);

	while (!*shutDown)
	{
		// initialize socket set
		FD_ZERO(&readfds);
		
		// add server's socket to set
		if (lastIndex != MAX_CLIENTS)
		{
			FD_SET(listenSocket, &readfds);
		}

		// add clients' sockets to set
		for (int i = 0; i < lastIndex; i++)
		{
			FD_SET(arrayOfClientSocks[i], &readfds);
		}


		// wait for events on set
		int selectResult = select(0, &readfds, NULL, NULL, &timeVal);
		if (selectResult == SOCKET_ERROR)
		{
			printf("Select failed with error: %d\n", WSAGetLastError());
			//closesocket(listenSocket);
			//WSACleanup();
			return 1;
		}
		else if (selectResult == 0) // timeout expired
		{
			continue;
		}
		else if (FD_ISSET(listenSocket, &readfds))	// Listen socket (client connecting)
		{
			// Struct for information about connected client
			sockaddr_in clientAddr;
			int clientAddrSize = sizeof(struct sockaddr_in);

			// Accept new client connection
			if (!AcceptSockets(&arrayOfClientSocks[lastIndex], &listenSocket, &clientAddr, &clientAddrSize))
			{
				//closesocket(listenSocket);
				//WSACleanup();
				printf("Crashed in Recevier thread\n");
				return 1;
			}

			lastIndex++;
			printf("New client request accepted (%d). Client address: %s : %d\n", lastIndex, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		}
		else										// Client socket (client sending a msg)
		{
			// Check if new message is received from connected clients
			for (int i = 0; i < lastIndex; i++)
			{
				// Check if new message is received from client on position "i"
				if (FD_ISSET(arrayOfClientSocks[i], &readfds))
				{
					/*if (!ReceiveMatrix(clientSockets, i, &lastIndex, dataBuffer, BUFFER_SIZE))
					{
						closesocket(clientSockets[i]);
						WSACleanup();
						printf("Crashed in Recevier thread\n");
						return 1;
					}*/

					// Receive matrix
					iResult = recv(arrayOfClientSocks[i], dataBuffer, BUFFER_SIZE, 0);

					if (iResult > 0)
					{
						dataBuffer[iResult] = '\0';
						printf("Message received from client (%d):\n", i + 1);

						matrica = (Matrix*)dataBuffer;
						int k = 0;						
						
						int DIM = matrica->order;

						int** mat = (int**)calloc(DIM, sizeof(int*));
						for (k = 0; k < DIM; k++)
							mat[k] = (int*)calloc(DIM, sizeof(int));

						
						k = 0;
						int x, y;
						for (x = 0; x < DIM; x++)
						{
							for (y = 0; y < DIM; y++)
							{
								mat[x][y] = matrica->data[k];
								k++;
							}
						}

						stampa_matrice(mat, DIM);

						int retVal = determinant(mat, DIM, clientWorkerList);

						iResult = send(arrayOfClientSocks[i], (char*)&retVal, sizeof(int), 0);	// (char*)&lastIndex
						if (iResult == SOCKET_ERROR)
						{
							printf("send failed with error: %d\n", WSAGetLastError());
							//closesocket(arrayOfWorkerSocks[i]);
							//WSACleanup();
							return false;
						}

						// DODAVANJE KLIJENTA U LISTU

						// Client thread
						/*DWORD clientSenderFunID;
						HANDLE clientSenderHandle;

						clientReceiverHandle = CreateThread(NULL, 0, &ClientReceiver, &paramsClient, 0, &clientReceiverFunID);*/

						printf("\n_______________________________  \n");
					}
					else if (iResult == 0)
					{
						// connection was closed gracefully
						printf("Connection with client (%d) closed.\n\n", i + 1);
						//closesocket(arrayOfClientSocks[i]);

						// sort array and clean last place
						for (int j = i; j < lastIndex - 1; j++)
						{
							arrayOfClientSocks[j] = arrayOfClientSocks[j + 1];
						}
						arrayOfClientSocks[lastIndex - 1] = 0;

						lastIndex--;
					}
					else
					{
						// there was an error during recv
						printf("recv failed with error: %d\n", WSAGetLastError());
						//closesocket(arrayOfClientSocks[i]);

						// sort array and clean last place
						for (int j = i; j < lastIndex - 1; j++)
						{
							arrayOfClientSocks[j] = arrayOfClientSocks[j + 1];
						}
						arrayOfClientSocks[lastIndex - 1] = 0;

						lastIndex--;
					}
				}
			}
		}
	}

	
	
	for (int i = 0; i < lastIndex; i++)
	{
		closesocket(arrayOfClientSocks[i]);
	}

	
	
	return 0;
}


DWORD WINAPI WorkerReceiver(LPVOID lpParam)
{
	paramsWorkerRecv* params = (paramsWorkerRecv*)lpParam;
	SOCKET* arrayOfWorkerSocks = params->workerSockets;
	
	listStruct* clientWorkerList = params->clientWorkerList;
	bool* shutDown = params->shutDown;

	char dataBuffer[BUFFER_SIZE];			// Buffer used for storing incoming data
	SOCKET listenSocket2 = INVALID_SOCKET;	// Socket used for listening for new clients 	
	int iResult;							// Multipurpose var
	int lastIndex = 0;						// Index of last connected worker	
	unsigned short port = 27017;	
	int threadId;

	int msgForClient;

	// set of socket descriptors
	fd_set readfds;
	fd_set writefds;

	// timeout for select function
	timeval timeVal;
	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;

	if (!InitializeAndListen(&listenSocket2, port))
	{
		return 1;
	}
	
	Sleep(1);

	while (!*shutDown)
	{

		// initialize socket set
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);

		// add server's socket to set
		if (lastIndex != MAX_WORKERS)
		{
			FD_SET(listenSocket2, &readfds);
		}

		// add worker' sockets to set
		for (int i = 0; i < lastIndex; i++)
		{
			FD_SET(arrayOfWorkerSocks[i], &readfds);
		}

		// add worker' sockets to set
		for (int i = 0; i < lastIndex; i++)
		{
			FD_SET(arrayOfWorkerSocks[i], &writefds);
		}

		// wait for events on set
		int selectResult = select(0, &readfds, NULL, NULL, &timeVal);
		if (selectResult == SOCKET_ERROR)
		{
			printf("Select failed with error: %d\n", WSAGetLastError());			
			return 1;
		}
		else if (selectResult == 0) // timeout expired
		{
			continue;
		}
		else if (FD_ISSET(listenSocket2, &readfds))	// Listen socket (worker connecting)
		{
			// Struct for information about connected client
			sockaddr_in clientAddr;
			int clientAddrSize = sizeof(struct sockaddr_in);

			// Accept new client connection
			if (!AcceptSockets(&arrayOfWorkerSocks[lastIndex], &listenSocket2, &clientAddr, &clientAddrSize))
			{				
				printf("Crashed in Recevier thread\n");
				return 1;
			}
			
			clientWorkerStruct* worker = (clientWorkerStruct *)malloc(sizeof(clientWorkerStruct));
			worker->clientSocket = arrayOfWorkerSocks[lastIndex];

			lastIndex++;
			AddOnEnd(clientWorkerList, worker);
			printf("New WORKER request accepted . Worker address: %s : %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		}
		else										// Client socket (client sending a msg)
		{
			

		}
				
	}
	

	for (int i = 0; i < lastIndex; i++)
	{
		closesocket(arrayOfWorkerSocks[i]);
	}

	

	return 0;
}

void FreeList(listStruct* list) {
	char message[] = "Shutdown";

	printf("Waiting for all workers to finish...\n");
	
	printf("All workers finished!\n");

	while (!IsListEmpty(list))
	{

		clientWorkerStruct* cwStruct = RemoveFirstElement(list);

		if (cwStruct == NULL) {
			printf("worker is null\n");
			continue;
		}		

		free(cwStruct);
	}
	printf("All workers destroyed! Continuing shutdown!\n");
}