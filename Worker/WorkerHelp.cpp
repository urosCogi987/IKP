#include "WorkerHelp.h"

bool ConnectToServer() 
{
	SOCKET connectSocket = INVALID_SOCKET;

	char address[10] = "127.0.0.1";
	//unsigned short port = 27017;
	char *recvBuff = NULL;
	
	if (!InitializeAndConnect(address, &connectSocket, SERVER_PORT)) 
	{		
		// Error, shutting down client connection
		shutdownClientConnection(&connectSocket);
		printf("Client side connection failed: %ld\n", WSAGetLastError());
		return 1;		
	}

	return true;
}