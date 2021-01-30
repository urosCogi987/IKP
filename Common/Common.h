#pragma once

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <shellapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <cstdbool>
#include <limits.h> 
#include <winapifamily.h>
#include <math.h>


//#define BUFFER_SIZE 256
//#define ARRAY_SIZE 10
//#define DEFAULT_SERVER_PORT "27016"
//#define DEFAULT_CLIENT_PORT 27016


// client
#define SERVER_IP_ADDRESS "127.0.0.1"
#define SERVER_PORT 27016
#define SERVER_PORT_DVA 27017
#define BUFFER_SIZE 256


// server
#define MAX_CLIENTS 10
#define DEFAULT_SERVER_PORT "27016"
#define CLIENT_PORT 27016

// matrix
#define MAX_ORDER 5

#define MAX_WORKERS 125

// Matrix
typedef struct Matrix {
	int order; // number of rows = number of columns	
	int data[MAX_ORDER * MAX_ORDER]; // a pointer to an array of n_rows pointers to rows
} Matrix;


// Koji klijent je pokrenuo koje workere
typedef struct clientWorkerStruct {
	int idClient;
	int idWorkers[MAX_WORKERS];
	int numOfWorkers;
	SOCKET clientSocket;
	int det;
	bool ready;
	int counter;	// zbog provere pristiglih tredova
} clientWorkerStruct;


// Cvor liste
typedef struct node {
	clientWorkerStruct* clientWorker;
	struct node * next;
} node_t;


// Lista
typedef struct listStruct {
	node_t* head;
	CRITICAL_SECTION critSec;
} listStruct;


// Parametri za Client receive thread
typedef struct paramsClientRecv {
	SOCKET* clientSockets;
	listStruct* clientWorkerList;
} paramsClientRecv;


// Parametri za Worker thread
typedef struct paramsWorkerRecv {
	SOCKET* workerSockets;
	listStruct* clientWorkerList;
} paramsWorkerRecv;


// Parametri za Client send thread
typedef struct paramsClientSend {
	listStruct* clientWorkerList;
} paramsClientSend;


// Parametri za Server receive thread (na klijentu)
typedef struct paramsServerRecv {
	SOCKET* connectSocket;
} paramsServerRecv;