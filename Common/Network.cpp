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
		WSACleanup();
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
		closesocket(*connectSocket);
		WSACleanup();
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
		closesocket(*connectSocket);
		WSACleanup();
		return;
	}

	closesocket(*connectSocket);
	WSACleanup();
}
