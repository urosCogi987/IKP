#include "../Common/Network.h"

// TCP server that use non-blocking sockets
int main()
{	
	SOCKET listenSocket = INVALID_SOCKET;	// Socket used for listening for new clients 	
	SOCKET clientSockets[MAX_CLIENTS];		// Sockets used for communication with client
	short lastIndex = 0;	
	int iResult;							// Variable used to store function return value	
	char dataBuffer[BUFFER_SIZE];			// Buffer used for storing incoming data
	unsigned long  mode = 1;

	if (!InitializeAndListen(&listenSocket, SERVER_PORT)) {
		
		return 1;
	}

	//initialise all client_socket[] to 0 so not checked
	memset(clientSockets, 0, MAX_CLIENTS * sizeof(SOCKET));
	

	// set of socket descriptors
	fd_set readfds;

	// timeout for select function
	timeval timeVal;
	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;


	studentInfo *student;
	Matrix* matrica;

	while (true)
	{
		// initialize socket set
		FD_ZERO(&readfds);

		// add server's socket and clients' sockets to set
		if (lastIndex != MAX_CLIENTS)
		{
			FD_SET(listenSocket, &readfds);
		}

		for (int i = 0; i < lastIndex; i++)
		{
			FD_SET(clientSockets[i], &readfds);
		}

		// wait for events on set
		int selectResult = select(0, &readfds, NULL, NULL, &timeVal);	/* NAPRAVI ZASTITU OD 
																		PREVISE KLIJENATA */

		if (selectResult == SOCKET_ERROR)
		{
			printf("Select failed with error: %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}
		else if (selectResult == 0) // timeout expired
		{			
			continue;
		}
		else if (FD_ISSET(listenSocket, &readfds))
		{
			// Struct for information about connected client
			sockaddr_in clientAddr;
			int clientAddrSize = sizeof(struct sockaddr_in);
			
			// Accept new client connection
			if (!AcceptSockets(&clientSockets[lastIndex], &listenSocket, &clientAddr, &clientAddrSize))
			{
				closesocket(listenSocket);
				WSACleanup();
				printf("Crashed in Recevier thread\n");
				return 1;
			}			
						

			if (clientSockets[lastIndex] == INVALID_SOCKET)
			{
				if (WSAGetLastError() == WSAECONNRESET)
				{
					printf("accept failed, because timeout for client request has expired.\n");
				}
				else
				{
					printf("accept failed with error: %d\n", WSAGetLastError());
				}
			}
			else
			{
				if (ioctlsocket(clientSockets[lastIndex], FIONBIO, &mode) != 0)
				{
					printf("ioctlsocket failed with error.");
					continue;
				}
				lastIndex++;
				printf("New client request accepted (%d). Client address: %s : %d\n", lastIndex, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
			}
		}
		else
		{

			// Check if new message is received from connected clients
			for (int i = 0; i < lastIndex; i++)
			{
				// Check if new message is received from client on position "i"
				if (FD_ISSET(clientSockets[i], &readfds))
				{
					iResult = recv(clientSockets[i], dataBuffer, BUFFER_SIZE, 0);

					if (iResult > 0)
					{
						dataBuffer[iResult] = '\0';
						printf("Message received from client (%d):\n", i + 1);

						//primljenoj poruci u memoriji pristupiti preko pokazivaca tipa (studentInfo *)
						//jer znamo format u kom je poruka poslata a to je struct studentInfo
						matrica = (Matrix*)dataBuffer;

						for (int i = 0; i < matrica->order * matrica->order; i++)
						{
							printf("%d ", matrica->data[i]);
						}
						/*printf("Ime i prezime: %s %s  \n", student->ime, student->prezime);

						printf("Poeni studenta: %d  \n", ntohs(student->poeni));*/
						printf("\n_______________________________  \n");


					}
					else if (iResult == 0)
					{
						// connection was closed gracefully
						printf("Connection with client (%d) closed.\n\n", i + 1);
						closesocket(clientSockets[i]);

						// sort array and clean last place
						for (int j = i; j < lastIndex - 1; j++)
						{
							clientSockets[j] = clientSockets[j + 1];
						}
						clientSockets[lastIndex - 1] = 0;

						lastIndex--;
					}
					else
					{
						// there was an error during recv
						printf("recv failed with error: %d\n", WSAGetLastError());
						closesocket(clientSockets[i]);

						// sort array and clean last place
						for (int j = i; j < lastIndex - 1; j++)
						{
							clientSockets[j] = clientSockets[j + 1];
						}
						clientSockets[lastIndex - 1] = 0;

						lastIndex--;
					}
				}
			}
		}
	}

	//Close listen and accepted sockets
	closesocket(listenSocket);

	// Deinitialize WSA library
	WSACleanup();

	return 0;
}
