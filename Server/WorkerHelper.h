#include "../Common/Network.h"
#include "../Common/DeterminantCalculator.h"


DWORD WINAPI ClientReceiver(LPVOID lpParam);

DWORD WINAPI WorkerCaller(LPVOID lpParam);

DWORD WINAPI WorkerReceiver(LPVOID lpParam);

DWORD WINAPI ClientSender(LPVOID lpParam);