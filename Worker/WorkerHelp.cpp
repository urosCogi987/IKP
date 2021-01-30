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
			iResult = recv(connectSocket, recvBuff, sizeof(recvBuff), 0);
			if (iResult > 0)
			{
				int retVal = 0;

				Matrix m = *(Matrix *)recvBuff;

				if (m.order == 1) {
					retVal = m.data[0];
				}
				else if(m.order == 2)
				{
					int k = 0;
					int DIM = m.order;
					
					int** mat = (int**)calloc(DIM, sizeof(int*));
					for (k = 0; k < DIM; k++)
						mat[k] = (int*)calloc(DIM, sizeof(int));


					
					k = 0;
					int x, y;
					for (x = 0; x < DIM; x++)
					{
						for (y = 0; y < DIM; y++)
						{
							mat[x][y] = m.data[k];
							k++;
						}
					}
					
					

					retVal = ((mat[0][0] * mat[1][1]) - (mat[1][0] * mat[0][1]));
				}


				iResult = send(connectSocket, (char*)&retVal, sizeof(int), 0);	// (char*)&lastIndex
				if (iResult == SOCKET_ERROR)
				{
					printf("send failed with error: %d\n", WSAGetLastError());
					//closesocket(arrayOfWorkerSocks[i]);
					//WSACleanup();
					return false;
				}

				printf("Work done!\n");		
				break;

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