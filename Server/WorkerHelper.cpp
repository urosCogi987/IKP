#include "WorkerHelper.h"

#include <process.h>

DWORD WINAPI WorkerCaller(LPVOID lpParam)
{
	LPCWSTR modee = L"open";	
		
	ShellExecute(NULL, "open", "..\\Debug\\Worker.exe", NULL, NULL, SW_SHOWDEFAULT);
	printf("***********************\n*********\n***********\n********");
	
	return 0;
}

// Listening on client connectinos and messages
DWORD WINAPI ClientReceiver(LPVOID lpParam)
{
	paramsClientRecv* params = (paramsClientRecv*)lpParam;		
	SOCKET* arrayOfClientSocks = params->clientSockets;

	Matrix* matrica;

	char dataBuffer[BUFFER_SIZE];			// Buffer used for storing incoming data
	SOCKET listenSocket = INVALID_SOCKET;	// Socket used for listening for new clients 	
	int iResult;							// Multipurpose var
	int lastIndex = 0;						// Index of last connected client
	unsigned short port = 27016;

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
			closesocket(listenSocket);
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
				closesocket(listenSocket);
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

						//// mozda vise tredova.
						//DWORD callerID;
						//HANDLE callerHandle;
						///* OVDE TREBA NEKA F-JA, ZA RACUNANJE MINORA, */


						//for (int i = 0; i < 3; i++)
						//{
						//	callerHandle = CreateThread(NULL, 0, &WorkerCaller, NULL, 0, &callerID);
						//}

						printf("\n_______________________________  \n");
					}
					else if (iResult == 0)
					{
						// connection was closed gracefully
						printf("Connection with client (%d) closed.\n\n", i + 1);
						closesocket(arrayOfClientSocks[i]);

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
						closesocket(arrayOfClientSocks[i]);

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


	closesocket(listenSocket);
	for (int i = 0; i < lastIndex; i++)
	{
		closesocket(arrayOfClientSocks[i]);
	}

	WSACleanup();
	return 0;
}


DWORD WINAPI WorkerReceiver(LPVOID lpParam)
{
	paramsWorkerRecv* params = (paramsWorkerRecv*)lpParam;
	SOCKET* arrayOfWorkerSocks = params->workerSockets;
	

	char dataBuffer[BUFFER_SIZE];			// Buffer used for storing incoming data
	SOCKET listenSocket = INVALID_SOCKET;	// Socket used for listening for new clients 	
	int iResult;							// Multipurpose var
	int lastIndex = 0;						// Index of last connected worker	
	unsigned short port = 27017;

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
		if (lastIndex != MAX_WORKERS)
		{
			FD_SET(listenSocket, &readfds);
		}

		// add worker' sockets to set
		for (int i = 0; i < lastIndex; i++)
		{
			FD_SET(arrayOfWorkerSocks[i], &readfds);
		}		

		// wait for events on set
		int selectResult = select(0, &readfds, NULL, NULL, &timeVal);
		if (selectResult == SOCKET_ERROR)
		{
			printf("Select failed with error: %d\n", WSAGetLastError());
			closesocket(listenSocket);
			//WSACleanup();
			return 1;
		}
		else if (selectResult == 0) // timeout expired
		{
			continue;
		}
		else if (FD_ISSET(listenSocket, &readfds))	// Listen socket (worker connecting)
		{
			// Struct for information about connected client
			sockaddr_in clientAddr;
			int clientAddrSize = sizeof(struct sockaddr_in);

			// Accept new client connection
			if (!AcceptSockets(&arrayOfWorkerSocks[lastIndex], &listenSocket, &clientAddr, &clientAddrSize))
			{
				closesocket(listenSocket);
				//WSACleanup();
				printf("Crashed in Recevier thread\n");
				return 1;
			}

			lastIndex++;
			printf("New client request accepted (%d). Client address: %s : %d\n", lastIndex, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		}
		else
		{

		}
	}


	return 0;
}