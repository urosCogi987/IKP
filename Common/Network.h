#include "Common.h"

// Initialize windows socket lib
bool InitializeWindowsSockets();

// Client side connection
bool InitializeAndConnect(char *argv, SOCKET* connectSocket, unsigned short port);

// Shutting down client side connection
void shutdownClientConnection(SOCKET* connectSocket);

