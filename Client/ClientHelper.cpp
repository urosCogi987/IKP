#include "ClientHelper.h"


DWORD WINAPI ServerReceiver(LPVOID lpParam)
{
	paramsServerRecv* params = (paramsServerRecv*)lpParam;
	SOCKET connectSocket = *params->connectSocket;

	ResultFromServer(connectSocket);

	return 0;	
}