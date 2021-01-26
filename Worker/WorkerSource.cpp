#include "WorkerHelp.h"

int main() 
{
	printf("Pokrenut!!!");
	if (!ConnectToServer())
	{
		return 1;
	}


	getchar();

	return 0;
}