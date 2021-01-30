#include "WorkerHelp.h"

int main() 
{
	printf("Pokrenut!!!");
	if (!ConnectToServer())
	{
		return 1;
	}


	//getchar();
	Sleep(2000);

	return 0;
}