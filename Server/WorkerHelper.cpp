#include "WorkerHelper.h"

#include <process.h>

DWORD WINAPI WorkerCaller(LPVOID lpParam)
{
	LPCWSTR modee = L"open";	
		
	ShellExecute(NULL, "open", "..\\Debug\\Worker.exe", NULL, NULL, SW_SHOWDEFAULT);
	printf("***********************\n*********\n***********\n********");
	
	return 0;
}