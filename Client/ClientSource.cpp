#include "../Common/Network.h"
#include "../Common/Matrix.h"

int main()
{		
	SOCKET connectSocket = INVALID_SOCKET;	// Socket used to communicate with server
	int iResult;							// Variable used to store function return value	
	char dataBuffer[BUFFER_SIZE];			// Buffer we will use to store message
	char address[10] = "127.0.0.1";			// server IP addresss


	if (!InitializeAndConnect(address, &connectSocket, SERVER_PORT))
	{
		// Error, shutting down client connection
		shutdownClientConnection(&connectSocket);
		printf("Client side connection failed: %ld\n", WSAGetLastError());
		return 1;
	}
	
	int answer = 0;
	int order = 0;

	while (true) 
	{
		printf("(1) Input matrix values by hand\n(2) Input randomly generated matrix values\nChoose: ");
		scanf_s("%d", &answer);

		printf("Enter the order of the matrix: \n");
		scanf_s("%d", &order);


		// Treba zastita unosa za ovo i za sve ostalo u sustini
		if (answer == 1) 
		{
			Matrix* mByHand;
			mByHand = EnterMatrixByHand(order);

			for (int i = 0; i < order * order; i++) 
			{
				printf("%d\t", mByHand->data[i]);
			}

			// Slanje strukture matrice									Matrix*   ???
			iResult = send(connectSocket, (char*)&mByHand, (int)sizeof(Matrix), 0);
			// Check result of send function
			if (iResult == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(connectSocket);
				WSACleanup();
				return 1;
			}

			break;
		}
		else if (answer == 2)
		{
			Matrix* mRandomly;
			mRandomly = EnterMatrixRandomly(order);


			for (int i = 0; i < order * order; i++)
			{
				printf("%d\t", mRandomly->data[i]);
			}
			/*
			// Slanje strukture matrice									Matrix*   ???
			iResult = send(connectSocket, (char*)&mRandomly, (int)sizeof(Matrix), 0);
			// Check result of send function
			if (iResult == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(connectSocket);
				WSACleanup();
				return 1;
			}
			*/

			break;
		}
		else
		{
			printf("input controll\n");
		}
	}




	//promenljiva tipa studentInfo cija ce se polja popunuti i cela struktira poslati u okviru jedne poruke
	studentInfo student;
	short poeni;

	while (true)
	{
		// Unos potrebnih podataka koji ce se poslati serveru
		printf("Unesite ime studenta: ");
		gets_s(student.ime, 15);

		printf("Unesite prezime studenta: ");
		gets_s(student.prezime, 20);

		printf("Unesite osvojene poene na testu: ");
		scanf("%d", &poeni);
		student.poeni = htons(poeni);  //obavezna funkcija htons() jer cemo slati podatak tipa short 
		getchar();    //pokupiti enter karakter iz bafera tastature


		// Slanje pripremljene poruke zapisane unutar strukture studentInfo
		//prosledjujemo adresu promenljive student u memoriji, jer se na toj adresi nalaze podaci koje saljemo
		//kao i velicinu te strukture (jer je to duzina poruke u bajtima)
		iResult = send(connectSocket, (char*)&student, (int)sizeof(studentInfo), 0);

		// Check result of send function
		if (iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}

		printf("Message successfully sent. Total bytes: %ld\n", iResult);

		printf("\nPress 'x' to exit or any other key to continue: ");
		if (_getch() == 'x')
			break;
	}

	// Shutdown the connection since we're done
	iResult = shutdown(connectSocket, SD_BOTH);

	// Check if connection is succesfully shut down.
	if (iResult == SOCKET_ERROR)
	{
		printf("Shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	Sleep(1000);

	// Close connected socket
	closesocket(connectSocket);

	// Deinitialize WSA library
	WSACleanup();

	return 0;
}