#include "../Common/Network.h"
#include "../Common/DeterminantCalculator.h"


DWORD WINAPI ClientReceiver(LPVOID lpParam);

DWORD WINAPI WorkerReceiver(LPVOID lpParam);

void FreeList(listStruct* list);