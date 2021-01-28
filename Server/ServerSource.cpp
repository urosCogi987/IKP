#include "WorkerHelper.h"

// TCP server that use non-blocking sockets
int main()
{	
	// CLIENT
	SOCKET listenSocket = INVALID_SOCKET;	// Socket used for listening for new clients 	
	SOCKET listenSocket2 = INVALID_SOCKET;

	int lastIndex = 0;						// Index of last connected client
	int iResult;							// Variable used to store function return value	
	char dataBuffer[BUFFER_SIZE];			// Buffer used for storing incoming data
	unsigned long  mode = 1;				// For unblocking mode (ioctlsocket)



	SOCKET clientSockets[MAX_CLIENTS];		// Sockets used for communication with clients
	SOCKET workerSockets[MAX_WORKERS];		// Sockets used for communication with workers
	
	memset(clientSockets, 0, MAX_CLIENTS * sizeof(SOCKET));
	memset(workerSockets, 0, MAX_WORKERS * sizeof(SOCKET));
	
	
	listStruct* pClientWorkerList = InitList();


	// Client thread
	DWORD clientReceiverFunID;
	HANDLE clientReceiverHandle;

	paramsClientRecv paramsClient;
	paramsClient.clientSockets = clientSockets;
	paramsClient.clientWorkerList = pClientWorkerList;
	clientReceiverHandle = CreateThread(NULL, 0, &ClientReceiver, &paramsClient, 0, &clientReceiverFunID);

	// Worker thread
	DWORD workerReceiverFunID;
	HANDLE workerReceiverHandle;

	paramsWorkerRecv paramsWorker;
	paramsWorker.workerSockets = workerSockets;
	paramsWorker.clientWorkerList = pClientWorkerList;
	workerReceiverHandle = CreateThread(NULL, 0, &WorkerReceiver, &paramsWorker, 0, &workerReceiverFunID);


	
	getchar();

	//Close listen and accepted sockets
	closesocket(listenSocket);

	// Deinitialize WSA library
	WSACleanup();

	return 0;
}
