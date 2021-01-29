#include "WorkerHelper.h"

#include <process.h>

DWORD WINAPI WorkerCaller(LPVOID lpParam)
{
	LPCWSTR modee = L"open";	
		
	ShellExecute(NULL, "open", "..\\Debug\\Worker.exe", NULL, NULL, SW_SHOWDEFAULT);
	
	
	return 0;
}

// Listening on client connectinos and messages
DWORD WINAPI ClientReceiver(LPVOID lpParam)
{
	paramsClientRecv* params = (paramsClientRecv*)lpParam;		
	SOCKET* arrayOfClientSocks = params->clientSockets;

	listStruct* clientWorkerList = params->clientWorkerList;

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

	while (true)
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

						for (int i = 0; i < matrica->order * matrica->order; i++)
						{
							if ((i != 0) && (i % matrica->order == 0))
								printf("\n");
							printf("%d ", matrica->data[i]);
						}												

						/* OVDE TREBA NEKA F-JA, ZA RACUNANJE MINORA, */
						if ((matrica->order == 1) || (matrica->order == 2) || (matrica->order == 3))
							numOfThreads = 1;
						else if (matrica->order == 4)
							numOfThreads = 4;
						else if (matrica->order == 5)
							numOfThreads = 5;

						// Opening new workers
						LPCWSTR modee = L"open";						

						for (int i = 0; i < numOfThreads; i++)
						{
							//callerHandle = CreateThread(NULL, 0, &WorkerCaller, NULL, 0, &callerID);
							ShellExecute(NULL, "open", "..\\Debug\\Worker.exe", NULL, NULL, SW_SHOWDEFAULT);							
						}

						// DODAVANJE KLIJENTA U LISTU						
						clientWorkerStruct* cwStr = (clientWorkerStruct *)malloc(sizeof(clientWorkerStruct));
						cwStr->idClient = i;
						cwStr->numOfWorkers = numOfThreads;
						cwStr->clientSocket = arrayOfClientSocks[i];
						cwStr->det = 0;
						cwStr->counter = 0;
						cwStr->ready = false;
						cwStr->clientSocket = arrayOfClientSocks[i];			// dal je potrebno uopste?
						for (int j = 0; j < numOfThreads; j++)
						{
							cwStr->idWorkers[j] = workerCounter + j;							
						}

						workerCounter += numOfThreads;
						
						AddOnEnd(clientWorkerList, cwStr);
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


	//closesocket(listenSocket);
	for (int i = 0; i < lastIndex; i++)
	{
		//closesocket(arrayOfClientSocks[i]);
	}

	//WSACleanup();
	return 0;
}


DWORD WINAPI WorkerReceiver(LPVOID lpParam)
{
	paramsWorkerRecv* params = (paramsWorkerRecv*)lpParam;
	SOCKET* arrayOfWorkerSocks = params->workerSockets;
	
	listStruct* clientWorkerList = params->clientWorkerList;

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

	while (true)
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
			//closesocket(listenSocket2);
			//WSACleanup();
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
				//closesocket(listenSocket2);
				//WSACleanup();
				printf("Crashed in Recevier thread\n");
				return 1;
			}
			
			
			iResult = send(arrayOfWorkerSocks[lastIndex], (char*)&lastIndex, sizeof(lastIndex),0);	// (char*)&lastIndex
			if (iResult == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				//closesocket(arrayOfWorkerSocks[i]);
				//WSACleanup();
				return false;
			}
			printf("Poruka jebeno poslata\n");
									

			lastIndex++;
			printf("New WORKER request accepted (%d). Worker address: %s : %d\n", lastIndex, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		}
		else 
		{	/* PRIMANJE PORUKA OD WORKERA	*/
			// Check if new message is received from connected clients
			for (int i = 0; i < lastIndex; i++)
			{
				// Check if new message is received from client on position "i"
				if (FD_ISSET(arrayOfWorkerSocks[i], &readfds))
				{
					iResult = recv(arrayOfWorkerSocks[i], dataBuffer, 4, 0);
					if (iResult > 0)
					{
						printf("\n\nVRACENO NAZAD BRE SIPU RACKU BOGA TI MEBJE:\n\n%d\n\n", *(int*)dataBuffer);

						threadId = *(int*)dataBuffer;
						/* NE ZNAM DAL ISPOD MENJA STVARI U LISTI IL SAMO LOKALNO, LUGI-SAAAAN */

						for (node_t** current = &clientWorkerList->head; *current; current = &(*current)->next)	// ako u listi postoji thread sa ovim ID
						{
							for (int j = 0; j < (*current)->clientWorker->numOfWorkers; j++)	// za svaki worker
							{
								if ((*current)->clientWorker->idWorkers[j] == threadId)	// ako je njegov ID jednak ID koji je poslao deo resenja
								{
									// NAPRAVI UPDATE NODE
									(*current)->clientWorker->det += *(int*)dataBuffer;
									(*current)->clientWorker->counter++;

									if ((*current)->clientWorker->counter == (*current)->clientWorker->numOfWorkers)	// Ako su svi threadovi vratili vred
									{
										(*current)->clientWorker->ready = true;
									}
									//msgForClient = *(int*)dataBuffer;														
								}
							}							
						}

						PrintList(clientWorkerList);


						for (node_t** current = &clientWorkerList->head; *current; current = &(*current)->next)
						{
							if ((*current)->clientWorker->ready == true)
							{
								iResult = send((*current)->clientWorker->clientSocket, (char*)&threadId, sizeof(threadId), 0);
								if (iResult == SOCKET_ERROR)
								{
									printf("send failed with error: %d\n", WSAGetLastError());
									//closesocket(arrayOfWorkerSocks[i]);
									//WSACleanup();
									return false;
								}

								printf("Primljena poruka vracena Klijentu.\n");
							}							
						}

						
					}
					else if (iResult == 0)
					{
						// connection was closed gracefully
						printf("Connection with client (%d) closed.\n\n", i + 1);
						//closesocket(arrayOfClientSocks[i]);

						// sort array and clean last place
						for (int j = i; j < lastIndex - 1; j++)
						{
							arrayOfWorkerSocks[j] = arrayOfWorkerSocks[j + 1];
						}
						arrayOfWorkerSocks[lastIndex - 1] = 0;

						lastIndex--;
					}
					else
					{
						// there was an error during recv
						printf("WORKER recv failed with error: %d\n", WSAGetLastError());
						//closesocket(arrayOfClientSocks[i]);

						// sort array and clean last place
						for (int j = i; j < lastIndex - 1; j++)
						{
							arrayOfWorkerSocks[j] = arrayOfWorkerSocks[j + 1];
						}
						arrayOfWorkerSocks[lastIndex - 1] = 0;

						lastIndex--;
					}
				}
			}
		}
	}


	return 0;
}

DWORD WINAPI ClientSender(LPVOID lpParam)
{


	return 0;
}