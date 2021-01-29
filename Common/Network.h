#include "Common.h"
#include "List.h"

// Initialize windows socket lib
bool InitializeWindowsSockets();

// Client side connection
bool InitializeAndConnect(char *argv, SOCKET* connectSocket, unsigned short port);

// Shutting down client side connection
void shutdownClientConnection(SOCKET* connectSocket);

// Send matrix
bool SendMatrix(SOCKET socket, Matrix m, int length);

// Server side connection
bool InitializeAndListen(SOCKET* listenSocket, unsigned short port);

// Accept client sockets
bool AcceptSockets(SOCKET* clientSocket, SOCKET* listenSocket, sockaddr_in *clientAddr, int *clientAddrSize);

// Shutting down server side connection
void ShutdownServerConnection();

// Receive matrix
bool ReceiveMatrix(SOCKET* sockets, int currClient, int* lastIndex, char* dataBuffer, int length);


bool AcceptWorkerSockets(SOCKET* clientSocket, SOCKET* listenSocket);


void ResultFromServer(SOCKET socket);