#include "WorkerHelper.h"

// TCP server that use non-blocking sockets
int main()
{		
	int lastIndex = 0;						// Index of last connected client
	int iResult;							// Variable used to store function return value	
	char dataBuffer[BUFFER_SIZE];			// Buffer used for storing incoming data
	unsigned long  mode = 1;				// For unblocking mode (ioctlsocket)
	bool* clientShutdown = (bool*)malloc(sizeof(bool));
	*clientShutdown = false;


	SOCKET clientSockets[MAX_CLIENTS];		// Sockets used for communication with clients
	SOCKET workerSockets[MAX_WORKERS];		// Sockets used for communication with workers
	
	memset(clientSockets, 0, MAX_CLIENTS * sizeof(SOCKET));
	memset(workerSockets, 0, MAX_WORKERS * sizeof(SOCKET));
	
	
	listStruct* pClientWorkerList = InitList();


	// Client receiver thread
	DWORD clientReceiverFunID;
	HANDLE clientReceiverHandle;

	paramsClientRecv paramsClient;
	paramsClient.clientSockets = clientSockets;
	paramsClient.clientWorkerList = pClientWorkerList;
	paramsClient.shutDown = clientShutdown;
	clientReceiverHandle = CreateThread(NULL, 0, &ClientReceiver, &paramsClient, 0, &clientReceiverFunID);

	// Worker receiver thread
	DWORD workerReceiverFunID;
	HANDLE workerReceiverHandle;

	paramsWorkerRecv paramsWorker;
	paramsWorker.workerSockets = workerSockets;
	paramsWorker.clientWorkerList = pClientWorkerList;
	paramsWorker.shutDown = clientShutdown;
	workerReceiverHandle = CreateThread(NULL, 0, &WorkerReceiver, &paramsWorker, 0, &workerReceiverFunID);	
	
	getchar();

	*clientShutdown = true;
	CloseHandle(clientReceiverHandle);
	CloseHandle(workerReceiverHandle);
	FreeList(pClientWorkerList);	
	free(pClientWorkerList);

	getchar();

	// Deinitialize WSA library
	WSACleanup();

	free(&clientShutdown);

	// FREE MEMSET BUFFERS
	return 0;
}

