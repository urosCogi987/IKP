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


//#define BUFFER_SIZE 256
//#define ARRAY_SIZE 10
//#define DEFAULT_SERVER_PORT "27016"
//#define DEFAULT_CLIENT_PORT 27016


// client
#define SERVER_IP_ADDRESS "127.0.0.1"
#define SERVER_PORT 27016
#define BUFFER_SIZE 256


// server
#define MAX_CLIENTS 10
#define DEFAULT_SERVER_PORT "27016"

// matrix
#define MAX_ORDER 5

struct studentInfo {
	char ime[15];
	char prezime[20];
	short poeni;
};

typedef struct Matrix {
	int order; // number of rows = number of columns	
	int data[MAX_ORDER * MAX_ORDER]; // a pointer to an array of n_rows pointers to rows
} Matrix;

