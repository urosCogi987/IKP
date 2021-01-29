#include "WorkerHelp.h"

bool ConnectToServer() 
{
	SOCKET connectSocket = INVALID_SOCKET;

	char address[10] = "127.0.0.1";
	//unsigned short port = 27017;
	char recvBuff[BUFFER_SIZE];
	int iResult;
	int sendBack;
	int lastIndex;
	
	if (!InitializeAndConnect(address, &connectSocket, SERVER_PORT_DVA)) 
	{		
		// Error, shutting down client connection
		shutdownClientConnection(&connectSocket);
		printf("Client side connection failed: %ld\n", WSAGetLastError());
		return 1;		
	}

	fd_set readfds;

	timeval timeVal;
	timeVal.tv_sec = 5;
	timeVal.tv_usec = 0;

	while (true)
	{
		FD_ZERO(&readfds);
		FD_SET(connectSocket, &readfds);

		iResult = select(0, &readfds, NULL, NULL, &timeVal);
		if (iResult == SOCKET_ERROR)
		{
			printf("Select failed with error: %d\n", WSAGetLastError());
			//closesocket(connectSocket);
			//WSACleanup();
			return false;
		}
		else if (iResult == 0) {

		}
		else if (FD_ISSET(connectSocket, &readfds))
		{
			iResult = recv(connectSocket, recvBuff, 4, 0);
			if (iResult > 0)
			{
				printf("%d", *(int*)recvBuff);

				lastIndex = *(int*)recvBuff;

				iResult = send(connectSocket, (char*)&lastIndex, sizeof(lastIndex), 0);	// (char*)&lastIndex
				if (iResult == SOCKET_ERROR)
				{
					printf("send failed with error: %d\n", WSAGetLastError());
					//closesocket(arrayOfWorkerSocks[i]);
					//WSACleanup();
					return false;
				}

				printf("Primljena poruka vracena nazad\n");						break;
			}
			else if (iResult == SOCKET_ERROR) 
			{
				printf("Error during receive of size");
				//closesocket(connectSocket);
				//WSACleanup();
				return false;
			}
			else
			{
				// WSAWouldBlock error 
				if (WSAGetLastError() == 10035) {
					continue;
				}

				printf("recv failed with error: %d\n", WSAGetLastError());
				//closesocket(connectSocket);
				//WSACleanup();
				return false;
			}
		}
	}
	//iResult = recv();

	return true;
}