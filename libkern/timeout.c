#include <windows.h>
#include <stdio.h>

int hz = 1000;

void timeout(void(*ftn) (void *), void *arg, register int ticks)
{
	HANDLE hTimer = NULL;
	HANDLE hTimerQueue = CreateTimerQueue();

	if (NULL == hTimerQueue)
	{
		printf("CreateTimerQueue failed (%d)\n", GetLastError());
		return;
	}

	if (!CreateTimerQueueTimer(&hTimer, hTimerQueue,
		(WAITORTIMERCALLBACK)ftn, arg, ticks, 0, 0))
	{
		printf("CreateTimerQueueTimer failed (%d)\n", GetLastError());
	}
}


