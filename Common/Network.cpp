#include "Network.h"

// Initialize windows socket lib
bool InitializeWindowsSockets()
{
	// WSADATA data structure that is to receive details of the Windows Sockets implementation
	WSADATA wsaData;

	// Initialize windows sockets library for this process
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

//client side connection
bool InitializeAndConnect(char *argv, SOCKET* connectSocket, unsigned short port)
{
	if (InitializeWindowsSockets() == false)
	{
		return false;
	}

	// create a socket
	*connectSocket = socket(AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP);

	if (*connectSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		//WSACleanup();
		return false;
	}

	// create and initialize address structure
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;								// IPv4 protocol
	inet_pton(AF_INET, argv, (PVOID*)(&serverAddress.sin_addr.s_addr));	// ip address of server
	serverAddress.sin_port = htons(port);					// server port	

	// Connect to server specified in serverAddress and socket connectSocket
	if (connect(*connectSocket, (SOCKADDR*)&serverAddress, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		printf("Unable to connect to server.\n");
		//closesocket(*connectSocket);
		//WSACleanup();
		return false;
	}	

	return true;
}

// Shutting down client side connection
void shutdownClientConnection(SOCKET* connectSocket)
{

	int iResult = shutdown(*connectSocket, SD_BOTH);

	// Check if connection is succesfully shut down.
	if (iResult == SOCKET_ERROR)
	{
		printf("Shutdown failed with error: %d\n", WSAGetLastError());
		//closesocket(*connectSocket);
		//WSACleanup();
		return;
	}

	//closesocket(*connectSocket);
	//WSACleanup();
}

// Send Matrix
bool SendMatrix(SOCKET socket, Matrix m, int length)
{
	int iResult;

	timeval timeVal;
	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;

	fd_set writeFds;
	
	FD_ZERO(&writeFds);
	FD_SET(socket, &writeFds);

	iResult = select(0, NULL, &writeFds, NULL, &timeVal);

	if (iResult == SOCKET_ERROR)
	{
		printf("Select failed with error: %d\n", WSAGetLastError());
		//closesocket(socket);
		//WSACleanup();
		return false;
	}
	else if (iResult == 0) {
		printf("time out expired.\n");
	}
	else if (FD_ISSET(socket, &writeFds))
	{			
		iResult = send(socket, (char*)&m, (int)sizeof(Matrix), 0);
		if (iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			//closesocket(socket);
			//WSACleanup();
			return false;
		}			
	}	

	return true;
}

bool InitializeAndListen(SOCKET* listenSocket, unsigned short port)
{
	int iResult;

	if (InitializeWindowsSockets() == false)
	{
		return false;
	}

	addrinfo* resultingAddress = NULL;
	addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;       // IPv4 address
	hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
	hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
	hints.ai_flags = AI_PASSIVE;     // 

	char* sPort = (char*)malloc(128);
	snprintf(sPort, 128, "%u", port);

	  // Resolve the server address and port
	iResult = getaddrinfo(NULL, sPort, &hints, &resultingAddress);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return false;
	}

	// Create a SOCKET for connecting to server
	*listenSocket = socket(AF_INET,      // IPv4 address family
		SOCK_STREAM,  // Stream socket
		IPPROTO_TCP); // TCP protocol

	// Check if socket is successfully created
	if (*listenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(resultingAddress);
		//WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket - bind port number and local address to socket
	iResult = bind(*listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);

	// Check if socket is successfully binded to address and port from sockaddr_in structure
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(resultingAddress);
		//closesocket(*listenSocket);
		//WSACleanup();
		return 1;
	}

	//// All connections are by default accepted by protocol stek if socket is in listening mode.
	//// With SO_CONDITIONAL_ACCEPT parameter set to true, connections will not be accepted by default
	bool bOptVal = true;
	int bOptLen = sizeof(bool);
	iResult = setsockopt(*listenSocket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, (char *)&bOptVal, bOptLen);
	if (iResult == SOCKET_ERROR) {
		printf("setsockopt for SO_CONDITIONAL_ACCEPT failed with error: %u\n", WSAGetLastError());
	}

	unsigned long  mode = 1;
	if (ioctlsocket(*listenSocket, FIONBIO, &mode) != 0)
		printf("ioctlsocket failed with error.");

	// Set listenSocket in listening mode
	iResult = listen(*listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		//closesocket(*listenSocket);
		//WSACleanup();
		return 1;
	}

	printf("Server socket is set to listening mode. Waiting for new connection requests.\n");

	return true;
}

// Accept client connections
bool AcceptSockets(SOCKET* clientSocket, SOCKET* listenSocket, sockaddr_in *clientAddr, int *clientAddrSize)
{	
	do {
		fd_set readfds;
		FD_ZERO(&readfds);

		timeval timeVal;
		timeVal.tv_sec = 1;
		timeVal.tv_usec = 0;

		FD_SET(*listenSocket, &readfds);

		int result = select(0, &readfds, NULL, NULL, &timeVal);
		if (result == 0)
		{
			// vreme za cekanje je isteklo
		}
		else if (result == SOCKET_ERROR)
		{
			//desila se greska prilikom poziva funkcije
		}
		else
		{
			if (FD_ISSET(*listenSocket, &readfds))
			{	
				// New connection request is received. Add new socket in array on first free position.
				*clientSocket = accept(*listenSocket, (struct sockaddr *)clientAddr, clientAddrSize);

				if (*clientSocket == INVALID_SOCKET) {
					printf("accept failed with error: %d\n", WSAGetLastError());
					//closesocket(*listenSocket);
					//WSACleanup();
					return false;

				}
				else
				{
					unsigned long  mode = 1;
					if (ioctlsocket(*clientSocket, FIONBIO, &mode) != 0)
						printf("ioctlsocket failed with error.");
				}				

				printf("Connection accepted\n");
				return true;
			}
		}
	} while (1);
	

	return true;
}

// Shutting down server side connection
void ShutdownServerConnection()
{

}

// Receive matrix
bool ReceiveMatrix(SOCKET* sockets, int currClient, int* lastIndex, char* dataBuffer, int length)
{
	Matrix* matrica;
	int iResult;
	iResult = recv(sockets[currClient], dataBuffer, length, 0);

	// Uspesno primljena poruka
	if (iResult > 0)
	{
		dataBuffer[iResult] = '\0';
		printf("Message received from client (%d):\n", currClient + 1);
		
		matrica = (Matrix*)dataBuffer;

		for (int i = 0; i < matrica->order * matrica->order; i++)
		{
			printf("%d ", matrica->data[i]);
		}
		
		printf("\n_______________________________  \n");
	}
	// Klijent se normalno diskonektovao
	else if (iResult == 0)
	{
		// connection was closed gracefully
		printf("Connection with client (%d) closed.\n\n", currClient + 1);
		//closesocket(sockets[currClient]);

		// sort array and clean last place
		for (int j = currClient; j < *lastIndex - 1; j++)
		{
			sockets[j] = sockets[j + 1];
		}
		sockets[*lastIndex - 1] = 0;

		*lastIndex--;
	}
	else
	{
		// there was an error during recv
		printf("recv failed with error: %d\n", WSAGetLastError());
		//closesocket(sockets[currClient]);

		// sort array and clean last place
		for (int j = currClient; j < *lastIndex - 1; j++)
		{
			sockets[j] = sockets[j + 1];
		}
		sockets[*lastIndex - 1] = 0;

		*lastIndex--;
	}

	return true;
}

void ResultFromServer(SOCKET socket)
{
	int iResult;
	int bytesReceived = 0;
	fd_set readfds;

	char buffer[BUFFER_SIZE];
	timeval timeVal;
	timeVal.tv_sec = 0;
	timeVal.tv_usec = 50;
	int size = 0;
	int rezultat;

	do {

		FD_ZERO(&readfds);
		FD_SET(socket, &readfds);

		iResult = select(0, &readfds, NULL, NULL, &timeVal);

		if (iResult == SOCKET_ERROR)
		{
			printf("Select failed with error: %d\n", WSAGetLastError());
			//closesocket(socket);			
		}
		else if (iResult == 0)
		{		
		}
		else if (FD_ISSET(socket, &readfds))
		{
			iResult = recv(socket, buffer, 4, 0);

			if (iResult > 0) 
			{
				rezultat = *(int*)buffer;
				
				printf("\n***Determinanta matrice je: %d***\n", rezultat);				
			}
			else if (iResult == 0) 
			{

				printf("Connection  is closed. [SERVER SHUTDOWN INITATED]\n");				
				break;
			}
			else
			{
				// WSAWouldBlock error 
				if (WSAGetLastError() == 10035) {
					continue;
				}

				printf("recv failed with error: %d\n", WSAGetLastError());
				//closesocket(socket);
			}
		}

	} while (true);
}

bool SendMatrixToWorker(SOCKET socket, int** matrix, int length, int size)
{
	int iResult;

	timeval timeVal;
	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;

	fd_set writeFds;

	FD_ZERO(&writeFds);
	FD_SET(socket, &writeFds);

	iResult = select(0, NULL, &writeFds, NULL, &timeVal);

	if (iResult == SOCKET_ERROR)
	{
		printf("Select failed with error: %d\n", WSAGetLastError());
		//closesocket(socket);
		//WSACleanup();
		return false;
	}
	else if (iResult == 0) {
		printf("time out expired.\n");
	}
	else if (FD_ISSET(socket, &writeFds))
	{

		iResult = send(socket, (char*)matrix, (int)sizeof(matrix), 0);
		if (iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			//closesocket(socket);
			//WSACleanup();
			return false;
		}
	}

	return true;
}




int ReceiveValueFromWorker(SOCKET socket)
{

	int iResult;
	int bytesReceived = 0;
	fd_set readfds;
	bool shouldWork = true;

	char buffer[BUFFER_SIZE];

	timeval timeVal;
	timeVal.tv_sec = 0;
	timeVal.tv_usec = 50;

	do {


		FD_ZERO(&readfds);
		FD_SET(socket, &readfds);


		iResult = select(0, &readfds, NULL, NULL, &timeVal);

		if (iResult == SOCKET_ERROR)
		{
			printf("ReceiveWhenWorkDone: Select failed with error: %d\n", WSAGetLastError());
			closesocket(socket);

			return -1;
		}
		else if (iResult == 0) {

		}
		else if (FD_ISSET(socket, &readfds))
		{
			iResult = recv(socket, buffer, 4, 0);
			if (iResult > 0) {

				printf("Returned value of determinant: %d\n", *(int *)buffer);
				return *(int *)buffer;
					
			}
			else if (iResult == 0) {

				printf("ReceiveWhenWorkDone: Connection closed.\n");
				closesocket(socket);

				return 0;
			}
			else
			{
				// WSAWouldBlock error 
				if (WSAGetLastError() == 10035) {
					continue;
				}

				printf("ReceiveWhenWorkDone: recv failed with error: %d\n", WSAGetLastError());
				closesocket(socket);

				return -1;
			}
		}
		//clear
		FD_CLR(socket, &readfds);

	} while (true);

}